//======================================================================== 
// Project: ECE893 - Machine Vision - Target finding 
// ---------------------------------------------------------------------- 
// Package: 
// Authors: Vilas Kumar Chitrakaran
// Start Date: Fri Sep 21 06:46:12 edt 2001
// Compiler: GNU GCC 2.95.2 
// Operating System: QNX RTP, GNU/Linux 
// ----------------------------------------------------------------------  
// File: targetFinding.t.cpp
// Example program for the class PPM.
//========================================================================  
 

// ----- Standard Libaries -----
#include <iostream>
#include <stdio.h>
#include <math.h>

// ----- Project Includes -----
#include "PPM.hpp"

using namespace std;
 
//======================================================================== 
// main
//========================================================================  
// idea : 1. Compute histogram to look at brightness levels.
//					2. Use thresholding to remove as much unwanted information as 
//						 possible.
//					3. Segment the remaining regions.
//					4. Remove regions not within a range of area expected of the 
//						 targets.
//					5. Find the centroids of the regions not eliminated in the image.
//					6. Find eight regions that are clustered together. They are the
//						 targets.
//========================================================================

int main(int argc, char *argv[])
{
	
	if (argc != 3)
	{
		cout << "Usage : targetFinding.t [input ppm image] [output ppm image]" 
				 << endl;
		std::exit(0);
	}
	
	//----------------------------------------------------------------------
	//--------------------------- information ------------------------------
	//----------------------------------------------------------------------
	
	cout << "-------------------------------------------------------" << endl
			 << "--------- ECE 893 - MACHINE VISION - PROJECT I --------" << endl
			 << "----------------- Target Finding ----------------------" << endl
			 << "-------------------------------------------------------" << endl;
	
	//----------------------------------------------------------------------
	//-------------------- Define program parameters -----------------------
	//----------------------------------------------------------------------

	// first operation is the thresholding. These are the levels
	
	int lowerThreshold = 180;
	int upperThreshold = 255;
	
	// second operation is region isolation. These are the parameters
	
	int minimumRegionArea = 280;
	int maximumRegionArea = 620; 

	// third operation is target finding by the property of clustering.
	// maximum radius around which to look for neighbours ( in both rows 
	// and columns )

	int allowableRadius = 30;

	//----------------------------------------------------------------------
	//----------------------- open original image --------------------------
	//----------------------------------------------------------------------
	
	cout << endl	 << "Opening input image : " << argv[1] << endl;
	
	PPM originalImage(argv[1]);

	
	//----------------------------------------------------------------------
	//---------------- compute the histogram of the image ------------------
	//----------------------------------------------------------------------

	cout << endl	 << "Computing histogram of the image...." << endl;

 	vector<int> histogram;
 	vector<int> tresholdedPixels;
 	histogram = originalImage.getHistogram();
	
	// write the histogram to a file.
	FILE *histogramDataFile;
	histogramDataFile = fopen("histogram.dat", "w+");
	for (int index = 0; index < 256; index++ )
	{
		int value = histogram[index];
		fprintf(histogramDataFile,"%d\n",value);
	}
 	fclose(histogramDataFile);


	//----------------------------------------------------------------------
	//-------------- threshold the image to remove background --------------
	//----------------------------------------------------------------------

	cout << endl	 << "Performing thresholding on the image...." << endl;

	originalImage.doSimpleThreshold(tresholdedPixels, lowerThreshold, upperThreshold);
 	
 	// write the result to file
 	originalImage.writeToFile("thresholdedImage.t.ppm");


	//----------------------------------------------------------------------
	//------------ locate regions of a particular range in area ------------
	//----------------------------------------------------------------------

	cout << endl	 << "Locating regions and identifying targets...." << endl;
	
	int regionArea;	
	// a vector containing area of regions grown
	
	vector<int> regionAreaIndices; 
	// a vector containing indices to pixels painted 
	// over in a region during region growing.
	
	vector<int> regionCentroids;
	// a list of centroids of all regions located.
	
	int regionNumber = 1;	
	// a counter that will ultimately hold total number of regions
	
	int numRows = originalImage.getNumRows();	
	int numColumns = originalImage.getNumColumns();
	// get the dimensions of the image.
	
	int row, column; // other local variables
	
	for ( row = 0; row < numRows; row++ )
	{
		for ( column = 0; column < numColumns; column++ )
		{
			if ( originalImage(row, column) == 255 )
			{
				// check that total number of regions are less than the maximum 
				// possible.
				if ( regionNumber == 255 )
				{
					cout << endl << "Total number of regions exceed available number "
							 << "of labels. Exiting program...."
							 << endl;
					break;
				}
				
				regionArea = 0;
				regionAreaIndices.clear();
				regionArea = originalImage.paintFill(row, column, 255, 
															(255-regionNumber),  regionAreaIndices ) ;
				
				// check whether the region has area that is expected of the 
				// targets. If so, find the centroid of the region.
				
				if ( regionArea > 	minimumRegionArea 
						 && regionArea < maximumRegionArea)
				{
					// Find the centroid of the region.

					int centroidX = 0;
					int centroidY = 0;
					
					originalImage.calculateCentroid( regionAreaIndices, (double &)centroidX, (double &)centroidY );
					
					// note that centroidX is the column and centroidY is the 
					// row position.
					regionCentroids.push_back( centroidY * numColumns + centroidX );
					regionNumber++;
				}
				else
				{
					// region is not of interest. blank it out
					for ( int index = 0; index < regionArea; index++ )
						originalImage( regionAreaIndices[index] ) = 0;
				}
			}
		}
		if ( column < numColumns )
			break;
	}

	// print out all the regions that were detected.
	if ( regionCentroids.size() > 0 )
	{
		cout << endl
			 << "Detected " << regionCentroids.size()
			 << " regions comparable to the target area in " << endl
			 << "the original image. The location of their centroids " << endl
			 << "are..." <<  endl;
		for ( int offset = 0; offset < regionCentroids.size(); offset++ )
			cout << "(" << regionCentroids[offset] / numColumns << " ," 
					 << regionCentroids[offset] % numColumns << ")" << endl;
	}
	else
	{
		cout << endl << "No areas comparable to that of the targets were detected. "
			 << "Exiting program..."
			 << endl;
		std::exit(0);
	}

	
	//----------------------------------------------------------------------
	//--------- verify that correct targets have been identified -----------
	//----------------------------------------------------------------------
	
	cout << endl << "Verifying targets..." << endl;

	// The following is based on the fact that in the target cluster, each
	// target will have atleast 3 nearest neighbours. Any target that does 
	// not have 3 neighbours are eliminated.
	
	vector<int> targetCentroids;
	// vector holding all the final target centroids
	
	for ( int index = 0; index < regionCentroids.size(); index++ )
	{
		int currentRow = regionCentroids[index] / numColumns;
		int currentColumn = regionCentroids[index] % numColumns;
		int neighbours = 0;
		
		for ( int offset = 0; offset < regionCentroids.size(); offset++ )
		{
			if ( index == offset )
				continue;
			
			if ( std::abs(currentRow - regionCentroids[offset] / numColumns) <= allowableRadius
			|| std::abs(currentColumn - regionCentroids[offset] % numColumns) <= allowableRadius )
			{
				neighbours++;
			}
		}
		if ( neighbours >= 3 )
			targetCentroids.push_back( regionCentroids[index] );
		
	}

	if ( targetCentroids.size() > 0 )
	{
		cout << endl << "Successfully found " << targetCentroids.size() 
				 << " targets." << endl
				 << "Their centroids are the following..."
				 << endl;
		
		for ( int offset = 0; offset < targetCentroids.size(); offset++ )
		{
				cout << "(" << targetCentroids[offset] / numColumns << " ," 
						 << targetCentroids[offset] % numColumns << ")" << endl;
				
				// put a dark dot at the centroid of the targets.
				originalImage(targetCentroids[offset]) = 100;
		}
	}
	else
	{
		cout << endl << "Error detecting targets. "
			 << "Exiting program..."
			 << endl;
		std::exit(0);
	}	
	
	//----------------------------------------------------------------------
	//---- write back the image containing the targets and some crap -------
	//----------------------------------------------------------------------

	originalImage.writeToFile(argv[2]);

	cout << endl << "Writing output image : " << argv[2] << endl;

	return 0;
}
