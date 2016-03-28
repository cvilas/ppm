//======================================================================== 
// Project: ECE893 - Machine Vision - Target finding 
// ---------------------------------------------------------------------- 
// Package: 
// Authors: Vilas Kumar Chitrakaran
// Start Date: Fri Sep 21 06:46:12 edt 2001
// Compiler: GNU GCC 2.96 
// Operating System: QNX RTP, GNU/Linux 
// ----------------------------------------------------------------------  
// File: PPM.hpp
// Interface of the class PPM.
//========================================================================  
 

#ifndef INCLUDED_PPM_hpp
#define INCLUDED_PPM_hpp
#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>

#include "ConvolutionMask.hpp"

using namespace std;

//======================================================================== 
// class PPM
// ----------------------------------------------------------------------
// \brief
// This class provides a few methods to manipulate images in PPM (portable 
// pixmap) format.
//
// This file provides a description of the interface to the PPM class. The 
// PPM class can be used to do simple image processing operations on pixmaps.
// This version supports only greyscale images with a resolution of 8bits 
// per pixel.
//
// <b>Example Program:</b>
//
// \include ppmTest.t.cpp
//========================================================================  
 
template< int numRows, int numColumns, class T> class ConvolutionMask;

class PPM
{
 public:
 
	// ----- Constructors and Destructor -----
	PPM ();
		// This constructor just creates a PPM type object. No memory
		// is allocated unless the created object is set to an image using
		// openImageFile() function.
		
	PPM (char *sourceFilename);
		// This constructor opens up the specified image file.
		// If the file is not present, the application exits
		// with an error message.

	PPM (int rows, int columns, int pixelValue = 0);
		// This constructor creates an empty pixmap of specified 
		// dimensions. The created pixmap will be 8bpp greyscale
		// with all pixels having a value specified by \a pixelValue.
		// The application exits with an error message if number
		// of rows and columns are not specified.

	~PPM ();
		// The default destructor. Does nothing constructive!
	
	// ----- Accessors to image elements -----

	inline int getNumRows() const;
		//  return		Number of rows in the image.
		
	inline int getNumColumns() const;
		//  return		Number of columns in the image.
	
	inline char *getImageHeader() const;
		//  return		Type of the image (P3, P5, etc).
	
	inline int getMaximumPixelValue() const;
		//  return		Maximum allowed pixel value
		//			for eg. 255 for 8bpp greyscale.
		
	inline unsigned char &operator()(int index);
		// This function overloads the \c () operator to access
 		// the pixel value at the \a index position of the image.
 		// Legal index range is (0) to (totalElements-1). 
		// The operator can be intuitively used to assign a value to an
		// element in the image such as
 		// \code myImage(2)=255; \endcode or
 		// \code cout << (int)myImage(2) << endl; \endcode
		//  index		Position of a pixel in the image.
		//  return		Stored value of the property of the
		//						image (intensity, etc) at the specified 
		//						index.
						
	inline unsigned char &operator()(int row, int column);
		// This function overloads the \c () operator to access
 		// the pixel value at the \a row, \a column position of the image.
 		// Legal index range is (0,0) to (totalRows-1, totalColumns-1). 
		// The operator can be intuitively used to assign a value to an
		// element in the image such as
 		// \code myImage(2,3)=255; \endcode or
 		// \code cout << (int)myImage(2,3) << endl; \endcode
		//  row, column		Position of a pixel in the image.
		//  return					Stored value of the property of the
		//									image (intensity, etc) at the specified 
		//									row and column.

	inline PPM &operator=(const PPM &ppmImage);
		// This function overloads the assignment operator 
		// to assign a \a PPM type to another \a PPM type.

	int openImageFile(char *targetFilename);
		// Opens up a PPM image and sets the object up with image data.
		//  targetFilename		Name of the image file.
		//		return			0 : operation successful
		//						non-zero : read operation failed.								
		
	int writeToFile(char *targetFilename);
		// This function writes the PPM image to disk.
		//  targetFilename		Name of the target image file.
		//		return						0 : operation successful
		//									non-zero : write operation failed.								
	
	// ----- Basic Image processing operations -----
	
	inline double distance(int row1, int column1, int row2, int column2);
		// Calculates distance in pixel units between two pixels
		// in an image
		//  return	Distance between two pixels in the image.
	
	inline double distance(int index1, int index2);
		// Calculates distance in pixel units between two pixels
		// in an image
		//  return	Distance between two pixels in the image.

	vector<int> getHistogram();
		// This function computes the histogram.
		//  return		A vector of size equal to the 
		//						total number of intensity levels
		//						with value at a particular index
		//						corresponding to the number of pixels
		//						having intensity corresponding to the 
		//						index.
	
	PPM &doSimpleThreshold(vector<int> &pixelIndicesVector, 
					int lowerCutOffValue = 126, int upperCutOffValue = 255);
		// This function returns a binary image that is a result of 
		// simple tresholding.
		//  lowerCutOffValue, upperCutOffValue  Cutoff levels
		//  pixelIndicesVector	Vector of indices of pixels painted 0xff

	PPM &doHysteresisThreshold(int lowerCutOffValue = 0, int upperCutOffValue = 255);
		// This function returns a binary image that is a result of 
		// hysteresis tresholding.
		//  lowerCutOffValue, upperCutOffValue  Hysteresis cutoff levels
	
	PPM &invertImage();
		// Invert the pixel values in the image.
	
	PPM &fillColor(int color);
		// Completely clear the image with one value
		//	color	The color to fill the image with.
		
	PPM &addImage ( PPM &withThisImage );
		// Does a pointwise addition of pixel values between the two images.
		//  withThisImage  Image whose pixel values are added to the target image.
		//  return		Image havinf pixel values that are a sum of corresponding
		//						pixel values in both images.
	
	template< int maskNumRows, int maskNumColumns, class T >
	PPM &convolve( ConvolutionMask< maskNumRows, maskNumColumns, T > &mask );
	 	// This function performs a convolution of the image with the given
		// convolution mask.
		//  image		The image on which convolution is to be performed
		//  mask		The mask matrix with which the image is convolved with.
		//					The mask must have odd dimensions for length of the edges.
		//  return		Image after convolution with the mask.
		 	
	template< int filterWindowSize >
	PPM &doMeanFiltering();
		// This function performs mean filtering (smoothing) using a 
		// square mask of specified size. For example, to filter using
		// a mask of size 3x3, we do the following:
		// \code PPM myImage("imageFile.ppm");
		//				 myImage.doMeanFiltering<3>();
		// \endcode
		//  filterWindowSize		Dimensions of the mask (must be odd value).
		//		return		Smoothened image.

	template< int filterWindowSize >
	PPM &doMedianFiltering();
		// This function performs median filtering using a 
		// square mask of specified size. The filter can be used to remove
		// "salt & pepper noise".For example, to filter using
		// a mask of size 3x3, we do the following:
		// \code PPM myImage("imageFile.ppm");
		//				 myImage.doMedianFiltering<3>();
		// \endcode
		//  filterWindowSize		Dimensions of the mask (must be odd value).
		//  return		Image after median filtering.

	PPM &doSobelFiltering( int direction = 3 );
		// This function performs sobel filtering using square mask of 
		// size 3x3. The filter is used to detect edges in the direction
		// specified by the \a direction parameter. For example, to find edges
		// of an image in the horizonatal direction, we do the 
		// following:
		// \code PPM myImage("imageFile.ppm");
		//				 myImage.doSobelFiltering(0);
		// \endcode
		//  direction		This specifies the direction in which the edge needs to
		//								be detected. Valid values are
		//								0 : horizontal
		//								1 : vertical
		//								any other : both horizontal and vertical 
		//  return		Image after sobel filtering.

 PPM &doThinning();
 		// This function thins regions in a BINARY image (0s and 255s) to 
 		// skeleton regions. 
 		//  return  skeletal BINARY image after thinning.

	// ----- Region property computation -----

	// NOTES : Region property functions must strictly be used only on 
	// segmented images.

 int paintFill( int row, int column, int paintOverLabel, 
								int newLabel, vector<int> &indicesOfPixelsPainted );
		// Fills up a contiguous area in an image with the specified new label.
		// This function can be used for region segmentation on a thresholded image.
		//  row, column		Pixel to paint from.
		//  paintOverLabel		image label to paint over
		//  newLabel		image label for painting
		//  indicesOfPixelsPainted		indices of pixels painted over
		//	  return		count of pixels painted, -1 in case of error.

 void calculateCentroid( vector<int> &areaPixelIndicesVector, double &centroidX, double &centroidY );
 		// This function can be used to find the centroid of a region
 		// defined by the pixel indices vector. Note that the centroid could
 		// be anywhere in the image or even outside it depending on the shape
 		// of the region.
 		//  areaPixelIndicesVector		Vector of all indices of the pixels
 		//														that form a region
 		//  centroidX, centroidY		return value of centroid of the region.

 int calculatePerimeter( vector<int> &areaPixelIndicesVector );
 		// This function can be used to find the perimeter of a region
 		// defined by the pixel indices vector. Actually what this function
 		// does is to find the number of pixels along the edge of a region
 		// and this isn't necessarily the perimeter. For example, if this
 		// function is applied on a region like an annular ring, it will 
 		// return the number of pixels lying on its inner and outer
 		// circumference. So beware of this limitation while using it.
 		//  areaPixelIndicesVector		Vector of all indices of the pixels
 		//														that form a region
 		//  return		Perimeter of the region.

 double calculateCompactness( vector<int> &areaPixelIndicesVector );
 		// calculates the compactness of a region as (perimeter^2)/area
 		//  areaPixelIndicesVector		The vector of pixel (indices) forming a 
 		//														unique region in a segmented image.
 		//  return		The compactness factor.
 		
 int calculateMoment(int j, int k, vector<int> &areaPixelIndicesVector );
 		// moment mjk  = E E [x^j * y^k * f(x,y) ], x = 0 to maxRows,
 		// y = 0 to maxColumns, f(x,y) is 1 for all pixels specified 
 		// by the vector of indices forming a region and zero otherwise. 
 		// ( E is summation symbol.)
 		// centroids : [ m01/m00 , m10,m00 ]
 		// area : m00
 		// principal axes : theta = (1/2) * atan [ 2 * m11 / (m20 - m02) ]
 		//  areaPixelIndicesVector		The vector of pixel (indices) forming a 
 		//														unique region in a segmented image.
 		//  return		The moment mjk.

 int calculatePrincipalAxis( vector<int> &areaPixelIndicesVector );
 		// calculate the angle (in radians) of the principal axis
 		// with respect to the base.
 		//  areaPixelIndicesVector		The vector of pixel (indices) forming a 
 		//														unique region in a segmented image.
 		//  return  angle in radians, of the principal axis.
 	
 int calculateElongation( vector<int> &areaPixelIndicesVector );
 		// calculate the elongation of the region of interest. 		
 		//  areaPixelIndicesVector		The vector of pixel (indices) forming a 
 		//														unique region in a segmented image.
 		//  return  elongation of the region.

	// ----- Object Recognition -----
	
	vector<int> doMAETemplateMatching( PPM &templateImage, int acceptableError );
		// Given a template of an object, this function will return an 
		// image that is the result of Mean Absolute Error (M.A.E) in 
		// template matching. The points of best match will seem bright.
		//  templateImage		The template of the object being looked for in
		//										the source image.
		//  acceptableError	The max. value of MAE error acceptable for matching.
		//  return						A vector with points (centroids) on the image that match 
		//										with template.

	vector<int> doRMSETemplateMatching( PPM &templateImage, int acceptableError );
		// Given a template of an object, this function will return an 
		// image that is the result of Root Mean Square Error (R.M.S.E) in 
		// template matching. The points of best match will seem bright.
		//  templateImage		The template of the object being looked for in
		//										the source image.
		//  acceptableError	The max. value of RMSE error acceptable for matching.
		//  return						A vector with points (centroids) on the image that match 
		//										with template.

 
 public:
		 
	// ----- public Data Members -----
	
 	static int s_paintFillQueueLength;
		// This variable specifies the size of the array holding the indices
		// of all pixels painted over in a paintfill operation. This value needs to
		// be large for larger images.
			
 protected:
		 
	// ----- Protected Data Members -----
		 	
	// ----- Protected Methods -----


	// ========== END OF INTERFACE ==========
 private:
		 
	// ----- Private Data Members -----
	
	char *d_header;
		// Specifies the type of image (P3, P5, etc)
	
	int d_maximumPixelValue;
		// Maximum value allowed for a pixel (255 for 8bpp).
		
	int d_numRows;
		// Number of pixels in a row.
	
	int d_numColumns;
		// Number of pixels in a column
	
	unsigned char *d_pixel;
		// The image matrix with pixel values stored
		//	 in a one dimensional array.
	
	// ----- Private Methods -----
	

};


// IMPLEMENTATION OF INLINE FUNCTIONS FOLLOW.

//======================================================================== 
// PPM::getNumRows
//========================================================================  

int PPM::getNumRows() const 
{ 
	return d_numRows; 
}


//======================================================================== 
// PPM::getNumColumns
//========================================================================  

int PPM::getNumColumns() const 
{ 
	return d_numColumns; 
}

//======================================================================== 
// PPM::getImageHeader
//========================================================================  
		
char *PPM::getImageHeader() const 
{
	return d_header;
}

	
//======================================================================== 
// PPM::getMaximumPixelValue
//========================================================================  

int PPM::getMaximumPixelValue() const 
{ 
	return d_maximumPixelValue; 
}


//======================================================================== 
// PPM::operator()
//========================================================================  
		
unsigned char &PPM::operator()(int index)
{
	if ( index < 0 || index >= d_numRows * d_numColumns )
	{
		cout << endl << "ERROR : [PPM::operator()(int index)]: "
				 << "Co-ordinates out of bound of the image. Exiting program..." 
				 << endl;
		
		//free allocated memory if any
		if ( d_pixel != NULL)
			free(d_pixel);
		
		std::exit(0);  
	}
	else
	{
		return d_pixel[index];	
	}
}

unsigned char &PPM::operator()(int row, int column)
{
	if ( row < 0 || row >= d_numRows || 
												column < 0 || column >= d_numColumns )
	{
		cout << endl << "ERROR : [PPM::operator()(int row, int column)]: "
				 << "Co-ordinates out of bound of the image. Exiting program..." 
				 << endl;
	  
	  //free allocated memory if any
		if ( d_pixel != NULL)
			free(d_pixel);
		
		std::exit(0);  
	}
	else
		return d_pixel[row * d_numColumns + column];	
}


//======================================================================== 
// PPM::operator=
//========================================================================  

PPM &PPM::operator=(const PPM &ppmImage)
{
	int maxRows;
	int maxColumns;
	int secondImageNumColumns = ppmImage.getNumColumns();
	int secondImageNumRows = ppmImage.getNumRows();

	
	( secondImageNumRows < d_numRows ) ? 
				( maxRows = secondImageNumRows)
			: ( maxRows = d_numRows );
	
	( secondImageNumColumns < d_numColumns ) ? 
				( maxColumns = secondImageNumColumns)
			: ( maxColumns = d_numColumns );
	
	for ( int row = 0; row < maxRows; row++ )
	{
		for ( int column = 0; column < maxColumns; column++ )
		{
			d_pixel[row * d_numColumns + column] = 
				ppmImage.d_pixel[row * secondImageNumColumns + column];
		}
	}
	return *this;
}


//======================================================================== 
// PPM::distance
//========================================================================  
double PPM::distance(int row1, int column1, int row2, int column2)
{
	return( sqrt((double)(row1-row2) * (row1-row2) + 
	(column1-column2) * (column1-column2)) );
}


double PPM::distance(int index1, int index2)
{
	return( sqrt((double)(index1/d_numColumns-index2/d_numColumns) * 
						(index1/d_numColumns-index2/d_numColumns) + 
						(index1%d_numColumns-index2%d_numColumns) * 
						(index1%d_numColumns-index2%d_numColumns)) );
}



// IMPLEMENTATION OF TEMPLATE FUNCTIONS FOLLOW.


//======================================================================== 
// PPM::convolve
//========================================================================  

//------------------------------------------------------------------------
// PROGRAMMER NOTE : This is an important function. Can this function be
// further optimized?
//------------------------------------------------------------------------

template< int maskNumRows, int maskNumColumns, class T >
PPM &PPM::convolve( ConvolutionMask< maskNumRows, maskNumColumns, T > &mask )
{
	// This implementation of convolution may be computation intensive
	// in some cases.
	
	// check whether mask has odd dimensions. If not, return nothing.
	if ( mask.getNumRows() % 2 == 0 || mask.getNumColumns() % 2 == 0 )
	{
		cout << endl << "ERROR : [PPM::convolve]: "
				 << "The dimensions of the convolution mask must be odd. "
				 << "Exiting the function."
				 << endl;
		return *this;
	}
	
	// define a local variable to hold results of convolution.
	PPM convolvedImage( getNumRows(), getNumColumns() ); 

	int rowRange = mask.getNumRows() / 2; 
	int columnRange = mask.getNumColumns() / 2;

	for ( int row = 0; row < d_numRows; row++ )
	{
		for ( int column = 0; column < d_numColumns; column++ )
		{
			T sum = 0;
			
			for ( int dr = (-1) * rowRange; dr <= rowRange; dr++ )
			{
				for ( int dc = (-1) * columnRange; dc <= columnRange; dc++ )
				{
					if ( (row + dr) >= 0 && (row + dr) < d_numRows &&
							 (column + dc) >= 0 && (column + dc) < d_numColumns )
					{
						sum += (T)d_pixel[(row + dr) * d_numColumns + (column + dc)]
										* mask( (dr + rowRange) * mask.getNumColumns() + (dc + columnRange) );
					}
				}
			}
			
			// Truncate pixel value if convolution exceeds [ 0 getMaximumPixelValue() ] range.
			if ( sum > getMaximumPixelValue() ) sum = getMaximumPixelValue();
			if ( sum < 0 ) sum = 0;
			convolvedImage( row * convolvedImage.getNumColumns() + column ) = (int)sum;
		}
	}
	
	*this = convolvedImage;
	return *this;
}

//======================================================================== 
// PPM::doMeanFiltering
//========================================================================  

template< int filterWindowSize >
PPM &PPM::doMeanFiltering()
{
	ConvolutionMask< filterWindowSize, filterWindowSize, double > smoothingMask;
	smoothingMask = 1.0/(filterWindowSize * filterWindowSize);
	
	return convolve( smoothingMask );
}


//======================================================================== 
// PPM::doMedianFiltering
//========================================================================  
	
//------------------------------------------------------------------------
// PROGRAMMER NOTE : I need to look for ways to optimize the following 
// code. It takes a lot of execution time. I think sort() calls are to
// blame.
//------------------------------------------------------------------------

template< int filterWindowSize >
PPM &PPM::doMedianFiltering()
{
	// check whether mask has odd dimensions. If not, exit.
	if ( filterWindowSize % 2 == 0 )
	{
		cout << endl << "ERROR : [PPM::doMedianFiltering]: "
				 << "The dimensions of the convolution mask must be odd. "
				 << "Exiting the function."
				 << endl;
		return *this;
	}

	ConvolutionMask< filterWindowSize, filterWindowSize, double > medianFilterMask;
	medianFilterMask = 1.0;
	
	// define a local variable to hold results of convolution.
	PPM convolvedImage( getNumRows(), getNumColumns() ); 

	int rowRange = filterWindowSize / 2; 
	int columnRange = filterWindowSize / 2;

	// convolution process
	for ( int row = 0; row < d_numRows; row++ )
	{
		for ( int column = 0; column < d_numColumns; column++ )
		{
			vector<int> pixelValues; // array of pixel values for the mask window
		
			for ( int dr = (-1) * rowRange; dr <= rowRange; dr++ )
			{
				for ( int dc = (-1) * columnRange; dc <= columnRange; dc++ )
				{
					if ( (row + dr) >= 0 && (row + dr) < d_numRows &&
							 (column + dc) >= 0 && (column + dc) < d_numColumns )
					{
						pixelValues.push_back( d_pixel[(row + dr) * d_numColumns + (column + dc)] );
					}
				}
			}
			// find the median value in the window and replace the output pixel
			// with that value
			sort( pixelValues.begin(), pixelValues.end() );	// sorting in ascending order 
															// using STL sort algorithm.
			
			convolvedImage( row * convolvedImage.getNumColumns() + column ) 
			= pixelValues[ (filterWindowSize * filterWindowSize / 2) + 1 ];
		}
	}
	
	*this = convolvedImage;
	return *this;
}



#endif

