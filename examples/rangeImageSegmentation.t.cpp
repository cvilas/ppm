//======================================================================== 
// Project: ECE893 - Machine Vision - Range Image Segmentation 
// ---------------------------------------------------------------------- 
// Package: 
// Authors: Vilas Kumar Chitrakaran
// Start Date: Sun Dec 1 06:46:12 edt 2002
// Compiler: GNU GCC 2.96 
// Operating System: QNX RTP, GNU/Linux 
// ----------------------------------------------------------------------  
// File: rangeImageSegmentation.t.cpp
// Range Image Segmentation example program for the class PPM.
//========================================================================  
 

// ----- Standard Libaries -----
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

// ----- Project Includes -----
#include "PPM.hpp"
#include "ConvolutionMask.hpp"
 
using namespace std;

// pre-declarations

// Camera specific range image to cartesian co-ords conversion
void convertOdeticsRangeToCartesian( PPM &rangeImage, 
								vector<double> position[3] );

// Cross-product between two 3-dimensional vectors.
void crossProduct( double *firstVector, double *secondVector, 
									 double *product );


//======================================================================== 
// main
//========================================================================  
// steps : 1. Mask the range image to remove background.
//					 2. Convert range data to cartesian co-ordinates.
//					 3. Compute surface normals for pixels in the image.
//				 	 4. Obtain the parameter space for the normals
//					 5. Segment the image based on local peaks in the parameter
//							space.
//
//========================================================================


int main(int argc, char *argv[])
{
	if (argc != 2 )
	{
		cout << "Usage : rangeImageSegmentation.t [ppm range image]" << endl;
		std::exit(0);
	}
	
	//----------------------------------------------------------------------
	//--------------------------- information ------------------------------
	//----------------------------------------------------------------------
	
	cout << "-------------------------------------------------------" << endl
			 << "--------- ECE 893 - MACHINE VISION - PROJECT 3 --------" << endl
			 << "---------------- Range Image Segmenation --------------" << endl
			 << "-------------------------------------------------------" << endl;
	

	//----------------------------------------------------------------------
	//-------------------- Define program parameters -----------------------
	//----------------------------------------------------------------------

	// Parameters for creating the mask image to remove background in
	// the range image.
	int cutOffRange = 127;

	// Surface normals are obtained by cross-product of two vectors along
	// a surface in the image. The vector lengths are defined by an offset
	// in number of pixel units.
	int offset = 3;
	
	//----------------------------------------------------------------------
	//-------------------------- open images -------------------------------
	//----------------------------------------------------------------------

	cout << "Opening input range image : " << argv[1] << endl;
	
	PPM rangeImage(argv[1]);
	// range image provided to us.
	
	int imageNumRows = rangeImage.getNumRows();
	int imageNumColumns = rangeImage.getNumColumns();
	

	//----------------------------------------------------------------------
	//---------------- compute the histogram of the image ------------------
	//----------------------------------------------------------------------

 	vector<int> histogram;
 	histogram = rangeImage.getHistogram();
	
	// write the histogram to a file.
	FILE *histogramDataFile;
	histogramDataFile = fopen("histogram.dat", "w+");
	for (int index = 0; index < 256; index++ )
	{
		int value = histogram[index];
		fprintf(histogramDataFile,"%d\n",value);
	}
 	fclose(histogramDataFile);

	cout << "Histogram of the range image written to file 'histogram.dat'."
			 << endl;

	//----------------------------------------------------------------------
	//----------------------- create the masked image ----------------------
	//----------------------------------------------------------------------

	// First make a mask. This is done by:-
	// 	a) Threshold the range image at an appropriate level.
	// 	b) Do median filtering to remove noisy pixels. 
	// Now create the masked image:-
	// Add (merge) the original range image with the 'mask' to get a 
	// 'masked' image.
	
	PPM maskImage(imageNumRows, imageNumColumns);
	vector<int> tresholdedPixels;
	
	maskImage = rangeImage;
	maskImage.doSimpleThreshold( tresholdedPixels, cutOffRange, 255 );
	
	// noise removal
	maskImage.doMedianFiltering<3>();
	
	// now we have a mask. Write it to file.
 	maskImage.writeToFile("mask.t.ppm");

	cout << "The mask has been written to 'mask.t.ppm'." << endl;
	cout << "Opening 'mask.t.ppm'.......";
	system("xview mask.t.ppm&");
	cout << "done!" << endl;
	
	// convert the mask to the masked image.
	maskImage.addImage( rangeImage );
 	maskImage.writeToFile("maskedImage.t.ppm");

	cout << "The masked image has been written to 'maskedImage.t.ppm'." 
			 << endl;
	cout << "Opening 'maskedImage.t.ppm'.......";
	system("xview maskedImage.t.ppm&");
	cout << "done!" << endl;


	//----------------------------------------------------------------------
	//----- conversion of range image data to cartesian co-ordinates -------
	//----------------------------------------------------------------------

	vector<double> cartesianCoords[3];
	convertOdeticsRangeToCartesian( maskImage, cartesianCoords );
	
	cout << "Conversion of range image data to cartesian co-ords " 
	     << "is complete." 
			 << endl;


	//----------------------------------------------------------------------
	//--------------------- computation of surface normals -----------------
	//----------------------------------------------------------------------

	double product[3];
	vector<double> surfaceNormals[4];
	// This vector holds surface normal vector x.i + y.j + z.k in the format 
	// {index, x, y, z}
	
	for ( int index = 0; index < imageNumRows * imageNumColumns; index++ )
	{
		if ( (index + offset) < imageNumRows * imageNumColumns &&
				 (index + offset * imageNumColumns) < imageNumRows * imageNumColumns 
				 && maskImage(index) != 255 &&
				 maskImage(index + offset) != 255 && 
				 maskImage(index + offset * imageNumColumns) != 255 )
		{
			double firstVector[3] = 
			{ cartesianCoords[0][index + offset] - cartesianCoords[0][index], 
				cartesianCoords[1][index + offset] - cartesianCoords[1][index], 
				cartesianCoords[2][index + offset] - cartesianCoords[2][index] 
			};
			
			double secondVector[3] = 
			
			{ cartesianCoords[0][index + offset * imageNumColumns] 
																	- cartesianCoords[0][index], 
				cartesianCoords[1][index + offset * imageNumColumns] 
																	- cartesianCoords[1][index], 
				cartesianCoords[2][index + offset * imageNumColumns] 
																	- cartesianCoords[2][index] 
			};
		
			// The normal vector points out of the surface.
			crossProduct( firstVector, secondVector, product );
	
			surfaceNormals[0].push_back( index );
			surfaceNormals[1].push_back( product[0] );
			surfaceNormals[2].push_back( product[1] );
			surfaceNormals[3].push_back( product[2] );
		}
	}
	
	cout << "Surface normal computation over." << endl;


	//----------------------------------------------------------------------
	//---------- description of parameter space for surface normals --------
	//----------------------------------------------------------------------

	// The 3D cartesian surface normal vectors can be converted to an 
	// equivalent representation in spherical co-ordinate system. 
	// (x,y,z) -> (phi, theta, r). Since we aren't bothered about the length
	// of the surface normals, 'r' can be omitted, giving us a parameter
	// space of two parameters :-
	// phi: angle between the x axis and projection of surface normal on
	//				x-y plane.
	// theta: angle between the normal vector and the z axis. 
	// Both these angles have a range (0 - 2.pi)
	
	vector<double> parameters[3];
	// This vector holds the sherical co-ord parameter values for each of 
	// the surface normal vectors.

	// need to write the parameters into a file so that we can make a hough 
	// transform plot later.
	FILE *parameterDataFile;
	parameterDataFile = fopen("parameters.dat", "w+");
	fprintf( parameterDataFile, "%s\n%s\n", 
					 "%parameters for hough transform plot",
					 "%pixel-index theta phi" );
	
	double x, y, z; // some locally used variables. 
	for ( int index = 0; index < surfaceNormals[0].size(); index++ )
	{
		
		x = surfaceNormals[1][index];
		y = surfaceNormals[2][index];
		z = surfaceNormals[3][index];
		
		// first parameter just holds the index in the image grid so that
		// we know whom we are talking about
		parameters[0].push_back( surfaceNormals[0][index] );
		
		// theta = acos ( z / r )
		parameters[1].push_back( acos( z / sqrt(x * x + y * y + z * z) ) );
		
		// phi = acos( x / x^2 + y^2 )
		parameters[2].push_back( acos( x / sqrt( x * x + y * y) ) ); 


		fprintf( parameterDataFile, "%d %f %f\n", 
						 (int)parameters[0][index],
				 		 parameters[1][index],
				 		 parameters[2][index] );
	}

	fclose(parameterDataFile);
	
	cout << "Parameters of surface normals written to file 'parameters.dat'." 
			 << endl;


	//----------------------------------------------------------------------
	//---------- segmentation based on clustering in parameter space -------
	//----------------------------------------------------------------------
	
	// Local peaks in the parameter (hough) space were discovered by hand.
	// In what follows, the image is segmented into surfaces using this
	// information.

	// create a new image. surfaces will be generated over it.
	PPM segmentedImage(imageNumRows, imageNumColumns, 255);
	
	for ( int index = 0; index < parameters[0].size(); index++ )
	{
		// surface (1)
		if ( parameters[1][index] >= 1.5 && parameters[1][index] <= 2.5 
				&& parameters[2][index] >= 2.5 && parameters[2][index] <= 3.14 )
		{
			segmentedImage( parameters[0][index] ) = 180;
		}

		// surface (2)
		if ( parameters[1][index] >= 1.5 && parameters[1][index] <= 2.5 
				&& parameters[2][index] >= 1 && parameters[2][index] <= 2.5 )
		{
			segmentedImage( parameters[0][index] ) = 0;
		}
	
		// surface (3)
		if ( parameters[1][index] >= 1.5 && parameters[1][index] <= 3.14 
				&& parameters[2][index] >= 0 && parameters[2][index] <= 1 )
		{
			segmentedImage( parameters[0][index] ) = 100;
		}
	}

	segmentedImage.writeToFile("segmentedImage.t.ppm");

	cout << "Final segmented image written to 'segmentedImage.t.ppm'." 
			 << endl;
	
	cout << "Opening 'segmentedImage.t.ppm'.......";
	system("xview segmentedImage.t.ppm&");
	cout << "done!" << endl;

	return 0;
}






//======================================================================== 
// convertOdeticsRangeToCartesian
//========================================================================  
// The following code was shamelessly lifted from another source with 
// hardly any modification.	
// This routine converts the data in an Odetics range image into 3D
// cartesian coordinate data.  The range image is 8-bit, and comes
// already separated from the intensity image.  The cartesian
// coordinates are returned into the variable 'position' (x, y, z)
//========================================================================


void convertOdeticsRangeToCartesian( PPM &rangeImage, vector<double> position[3] )
{
	int	r,c; // something for 'row' and 'column'
	double	 cp[7];  // constant parameters
	double 	xAngle, yAngle, dist;
	double scanDirectionFlag,slantCorrection;
	int rows = rangeImage.getNumRows();
	int columns = rangeImage.getNumColumns();

	// define all camera specific constants. No idea what they mean.
	cp[0] = 1220.7;		// horizontal mirror angular velocity in rpm 
	cp[1] = 32.0;		  // scan time per single pixel in microseconds 
	cp[2] = ( columns / 2 ) - 0.5;		// middle value of columns 
	cp[3] = 1220.7 / 192.0;	  // vertical mirror angular velocity in rpm 
	cp[4] = 6.14;		      // scan time (with retrace) per line in milliseconds 
	cp[5] = ( rows / 2 ) - 0.5;		// middle value of rows 
	cp[6] = 10.0;		// standoff distance in range units (3.66cm per r.u.) 	
	
	cp[0] = cp[0] * 3.1415927 / 30.0;	// convert rpm to rad/sec 
	cp[3] = cp[3] * 3.1415927 / 30.0;	// convert rpm to rad/sec 
	cp[0] = 2.0 * cp[0];		  // beam ang. vel. is twice mirror ang. vel. 
	cp[3] = 2.0 * cp[3];		  // beam ang. vel. is twice mirror ang. vel. 
	cp[1] /= 1000000.0;		// units are microseconds : 10^-6 
	cp[4] /= 1000.0;			  // units are milliseconds : 10^-3 

 	// Odetics image -- scan direction downward (1); upward (-1).
  scanDirectionFlag=1;
  
	// start with semi-spherical coordinates from laser-range-finder:
	//			(r,c,rangeImage[r*COLS+c])		  
	// convert those to axis-independant spherical coordinates:	  
	//			(xAngle,yAngle,dist)			  
	// then convert the spherical coordinates to cartesian:           
	//			(position => X, Y, Z)			  

	for (r = 0; r < rows; r++)
  {
 	 for (c = 0; c < columns; c++)
  	 {
    slantCorrection = cp[3] * cp[1] * ( (double)c - cp[2] );
    xAngle = cp[0] * cp[1] * ( (double)c - cp[2] );
    yAngle = ( cp[3] * cp[4] * ( cp[5] - (double)r ) ) + 	// Standard Transform Part 
						 slantCorrection * scanDirectionFlag;	        // + slant correction 
    
    // set to some junk value out of range of camera if we are dealing 
    // with background
    if ( rangeImage( r * columns + c ) == 255 )
    {
    		position[0].push_back( -200 );
    		position[1].push_back( -200 );
    		position[2].push_back( -200 );
    }
    else
    {
      dist = (double)rangeImage( r * columns + c ) + cp[6];
   
	  		position[2].push_back( sqrt( (dist * dist) / (1.0 + ( tan(xAngle) * tan(xAngle) )
													 + ( tan(yAngle) * tan(yAngle) ) ) ) );
    		position[0].push_back( tan(xAngle) * position[2][r * columns + c] );
    		position[1].push_back( tan(yAngle) * position[2][r * columns + c] );
	 	}
	 }
 }
}


//======================================================================== 
// crossProduct
//========================================================================
// Computes the cross product between two 3D vectors passed in as an
// array of 3 elements (x, y, z).
//========================================================================  


void crossProduct( double *firstVector, double *secondVector, double *product )
{
	product[0] = firstVector[1] * secondVector[2] - firstVector[2] * secondVector[1];
	product[1] = firstVector[2] * secondVector[0] - firstVector[0] * secondVector[2];
	product[2] = firstVector[0] * secondVector[1] - firstVector[1] * secondVector[0];	
}

