//======================================================================== 
// Project: ECE893 - Machine Vision - Optical Character Recognition 
// ---------------------------------------------------------------------- 
// Package: 
// Authors: Vilas Kumar Chitrakaran
// Start Date: Sun Oct 21 06:46:12 edt 2001
// Compiler: GNU GCC 2.96 
// Operating System: QNX RTP, GNU/Linux 
// ----------------------------------------------------------------------  
// File: ocr.t.cpp
// Optical Character Recognition example program for the class PPM.
//========================================================================  
 

// ----- Standard Libaries -----
#include <iostream>
#include <stdio.h>
#include <math.h>

// ----- Project Includes -----
#include "PPM.hpp"
#include "ConvolutionMask.hpp"

using namespace std;
//======================================================================== 
// main
//========================================================================  
// idea : 1. Compute histogram to look at brightness levels.
//					2. Use thresholding to eliminate background and isolate text 
//						 clearly.
//					3. Use template matching to obtain positives.
//					4. Differentiate between true and false positives using some
//						 region/edge feature of the characters.
//========================================================================
 

// pre-declaration of functions.

int analyzeGroundTruth( vector<int> matchPointsVector, 
												vector<int> groundTruthVector, 
												int rowSize, int columnSize );

int generateRocData( PPM &image, PPM &templateImage, vector<int> gtVector, 
										int lowerErrorLevel, int upperErrorLevel, 
										int maxFalsePositives, char *ROCFilename);



int main(int argc, char *argv[])
{
	
	if (argc != 3)
	{
		cout << "Usage : ocr.t [ppm text image] [ppm template image]" << endl;
		std::exit(0);
	}
	
	//----------------------------------------------------------------------
	//--------------------------- information ------------------------------
	//----------------------------------------------------------------------
	
	cout << "-------------------------------------------------------" << endl
			 << "--------- ECE 893 - MACHINE VISION - PROJECT 2 --------" << endl
			 << "------------ Optical Character Recognition ------------" << endl
			 << "-------------------------------------------------------" << endl;
	

	//----------------------------------------------------------------------
	//-------------------- Define program parameters -----------------------
	//----------------------------------------------------------------------

	// Parameters for thresholding the image of text.
	int iLowerThreshold = 130;
	int iUpperThreshold = 255;
	
	// Parameters for thresholding the template of character.
	int tLowerThreshold = 180;
	int tUpperThreshold = 255;

	// Acceptable error level in template matching using RMSE criterion
	int errorLevelT = 128;	// for matching using thresholded images.
	int errorLevelUT = 67; // for matching using un-tresholded images.
	
	// use 100 and 35 when a template of single sample of 'o' is used
	// use 128 and 67 when a template of average of many templates of 'o' 
	// is used.
	
	// Verification of the recognized character - acceptable range for
	// compactness and area.
	int lowerCompactness = 20;
	int upperCompactness = 33;
	
	int characterArea = 32;
	
	// Error levels for which ROC data is generated.
	int rocLowerErrorLevel = 10;
	int rocUpperErrorLevel = 140;
	
	//----------------------------------------------------------------------
	//-------------------------- open images -------------------------------
	//----------------------------------------------------------------------
	
	cout << endl	 << "Opening input text image : " << argv[1] << endl;
	
	PPM textImage(argv[1]);
	// image of the text.
	
	int imageNumRows = textImage.getNumRows();
	int imageNumColumns = textImage.getNumColumns();
	
	
	cout << endl	 << "Reading the template into the program.." << endl;

	PPM templateImage(argv[2]);
	// un-thresholded template of the character.
	
	int templateNumRows = templateImage.getNumRows();
	int templateNumColumns = templateImage.getNumColumns();
	

	// make a copy of the text image and the template.
	PPM step1Image( imageNumRows, imageNumColumns );
	PPM step2Image( imageNumRows, imageNumColumns );
	PPM step3Image( imageNumRows, imageNumColumns );

	step1Image = textImage;
	step3Image = textImage;
	// will initialize step2Image later.
	
	PPM templateImageCopy( templateNumRows, templateNumColumns );
	templateImageCopy = templateImage;
	

	//----------------------------------------------------------------------
	//---------------- compute the histogram of the image ------------------
	//----------------------------------------------------------------------

	cout << endl	 << "Computing histogram of the image ...." << endl;

 	vector<int> histogram;
 	histogram = textImage.getHistogram();
	
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
	//---------------------- open the Ground Truth table -------------------
	//----------------------------------------------------------------------

	// we have a Ground Truth (GT) table thanks to Phil Carns. Compare the 
	// matched points with values in the table to find success rate of 
	// recognition.
	
	FILE *gtTable;
	gtTable = fopen( "GT.txt", "r" );

	if ( gtTable == NULL)
	{
		cout << endl 
				 << "Unable to open the Ground Truth file."
				 << endl;
	}

	char data[10]; // data from the file
	int numInstances; // number of instances of the letter
	vector<int> gtVector;
	int row, column;
	
	fscanf( gtTable, "%c", data );
	if ( data[0] == '#' )
	{
		while( (data[0] = fgetc(gtTable)) != '\n' );
	} 
		
	fscanf( gtTable, "%c %d", data, &numInstances );
	
  int count = 0;
	while ( count != numInstances )
	{
		fscanf( gtTable, "%d %d", &column, &row );
		gtVector.push_back( row * imageNumColumns + column);
		count++;
	}

	fclose( gtTable );


	//----------------------------------------------------------------------
	//----- OCR on thresholded image of text with thresholded template -----
	//----------------------------------------------------------------------

	cout << "------------------------------------------------------" << endl
			 << "--- OCR with thresholded text image and template -----" << endl
			 << "------------------------------------------------------" << endl;

	cout << endl	 << "Performing thresholding on the text image...." << endl;
	
	vector<int> tPixels;

	step1Image.doSimpleThreshold( tPixels, iLowerThreshold, iUpperThreshold );
	templateImage.doSimpleThreshold( tPixels, tLowerThreshold, tUpperThreshold );

	
	// do template maching using "root mean squared error" as the criterion.
	
	vector<int> matchPointsT; // vector of points matched.
	matchPointsT = step1Image.doRMSETemplateMatching( templateImage,
																											errorLevelT );
	
	// just scale colors in the image. we will put bright spots at the
	// locations where character was identified. 
		
	for ( int index = 0; index < imageNumRows * imageNumColumns - 1; index++ )
		step1Image(index) = (int)floor( 0.5*step1Image(index) );

	// save this image for later when region properties will be applied
	step2Image = step1Image;


	// lay a dot at all matched points
	for ( int index = 0; index < matchPointsT.size(); index++ )
		step1Image( matchPointsT[index] ) = 255;


	//======================================================================
	// using region properties, the matched points are further
	// tested and a dot placed at points that pass the test.
	//======================================================================


	vector<int> regionVector;
	int centroidX, centroidY;
	double compactness = 0;
	vector<int> matchedPointTestedT;
	
	// locate characters recognised as a positive
	
	for ( int index = 0; index < matchPointsT.size(); index++ )
	{
		for ( int i = matchPointsT[index] - 4; i < matchPointsT[index] + 4; i++ )
		{
			if ( i < 0 || i > imageNumRows * imageNumColumns - 1 )
				continue;
			
			if ( step2Image(i) == 0 )
			{
				step2Image.paintFill ( i / imageNumColumns, i % imageNumColumns, 0, 1, 
																															regionVector );
				
				
				// check if the region is really near the point we are looking at
				step2Image.calculateCentroid( regionVector, (double &)centroidX, (double &)centroidY );
				
				if ( std::abs(centroidY - matchPointsT[index] / imageNumColumns) <= 4
				&& std::abs(centroidX - matchPointsT[index] % imageNumColumns) <= 4 ) 
				{
					// do a compactness test
					compactness = step2Image.calculateCompactness( regionVector );
					
					// put a dot at the locations that matched the template if the 
					// compactness test passed
					
					if ( compactness >= lowerCompactness 
								&& compactness <= upperCompactness 
								&& regionVector.size() <= characterArea)
					{
						step2Image( matchPointsT[index] ) = 255;
						matchedPointTestedT.push_back( matchPointsT[index] );
					}
					// if you dont clear the regionVector, indices keep getting
					// adding up in the vector.
					regionVector.clear();
				}
				break;
			}
		}
	}

	cout << endl << "Matched points located on the text image." << endl;


	//----------------------------------------------------------------------
	//-- OCR on un-thresholded image of text with un-thresholded template --
	//----------------------------------------------------------------------

	cout << "------------------------------------------------------" << endl
			 << "-- OCR with un-thresholded text image and template ---" << endl
			 << "------------------------------------------------------" << endl;

	
	// do template maching using "root mean square error" as the criterion.
	
	vector<int> matchPointsUT; // vector of points matched.
	matchPointsUT = step3Image.doRMSETemplateMatching( templateImageCopy, 
																												errorLevelUT );
	
	// put a dot at the locations that matched the template.
	
	for ( int index = 0; index < imageNumRows * imageNumColumns - 1; index++ )
		step3Image(index) = (int)floor( 0.5*step3Image(index) );


	for ( int index = 0; index < matchPointsUT.size(); index++ )
		step3Image( matchPointsUT[index] ) = 255;

	cout << endl << "Matched points located on the text image." << endl;




	//----------------------------------------------------------------------
	//--------------- analyze matched points with GT table -----------------
	//----------------------------------------------------------------------

	cout << "-------------------------------------------------------" << endl
	     << "-------- analyzing matched points with GT table -------" << endl
			 << "-------------------------------------------------------" << endl;

		 
	int truePositives, falsePositives; 

	// analysis for OCR method using thresholded images
	
	truePositives = analyzeGroundTruth( matchPointsT, gtVector, 
									imageNumRows, imageNumColumns );
	
	falsePositives = matchPointsT.size() - truePositives;
	
	cout << endl << "OCR method using thresholded images: "
			 << endl << "TP : " << truePositives << " FP : " 
		   << falsePositives << endl;
	
	
	
	// analysis for OCR method using thresholded images and region properties
	
	truePositives = analyzeGroundTruth( matchedPointTestedT, gtVector, 
									imageNumRows, imageNumColumns );
	
	falsePositives = matchedPointTestedT.size() - truePositives;

	cout << endl << "OCR method using thresholded images and region properties: "
			 << endl << "TP : " << truePositives << " FP : " 
			 << falsePositives << endl;


	// analysis for OCR method using un-thresholded images
	
	truePositives = analyzeGroundTruth( matchPointsUT, gtVector, 
									imageNumRows, imageNumColumns );
	
	falsePositives = matchPointsUT.size() - truePositives;

	cout << endl << "OCR method using un-thresholded images: "
			 << endl << "TP : " << truePositives << " FP : " 
			 << falsePositives << endl;



	//----------------------------------------------------------------------
	//------------- write back the image containing the results ------------
	//----------------------------------------------------------------------

	cout << endl << "Writing output images after character recognition..." 
			 << endl;

 	// write the result to file
 	step1Image.writeToFile("thresholdedTextMatchingResult.t.ppm");
 	step2Image.writeToFile("thresholdedTextMatchingTestedResult.t.ppm");
 	step3Image.writeToFile("unthresholdedTextMatchingResult.t.ppm");




	//======================================================================
	// In what follows, the mere effect of change in error levels in template 
	// matching is examined using the ground truth tables. Besides the
	// GT table no other method has been used to differentiate the true 
	// positives from the false positives.
	//======================================================================


	//----------------------------------------------------------------------
	//------------------ generate ROC data for MATLAB plot -----------------
	//----------------------------------------------------------------------

	cout << endl << "Generating ROC data for MATLAB plot. This can " << endl
			 << "take a crazy amount of time." << endl;


	// generate ROC data for the case with un-thresholded images.

	generateRocData( textImage, templateImageCopy, gtVector,
									 rocLowerErrorLevel, rocUpperErrorLevel,
									 500, "ROCdataUnthresholdedImages.txt" );

	// generate ROC data for the case with thresholded images.

	textImage.doSimpleThreshold( tPixels, iLowerThreshold, iUpperThreshold );

	generateRocData( textImage, templateImage, gtVector,
									 rocLowerErrorLevel, rocUpperErrorLevel,
									 500, "ROCdataThresholdedImages.txt" );


	return 0;
}	



//======================================================================== 
// analyzeGroundTruth
//========================================================================  
// The following function generates the true positives, given the vectors
// for match points and the ground truth table for the character being
// matched.
//========================================================================

int analyzeGroundTruth( vector<int> matchPointsVector, 
												vector<int> groundTruthVector, 
												int rowSize, int columnSize )
{
	int numMatches = matchPointsVector.size();
	int truePositives = 0;
	int count = 0;

	for ( int offset = 0; offset < groundTruthVector.size(); offset++ )
	{
		for ( int index = 0; index <  numMatches; index++ )
		{
			// The character is roughly 16x8 pixels. Check if a matchpoint occurs
			// in this window around a ground truth.
			if ( std::abs( groundTruthVector[offset] / columnSize 
										 - matchPointsVector[index] / columnSize) <= 8
			&& std::abs( groundTruthVector[offset] % columnSize 
									   - matchPointsVector[index] % columnSize) <= 4 ) 
			{
				truePositives++;
				break; // doing this helps avoid the fact that sometimes more than 
							 // one positive gets registered at a point. So once you hit
							 // a positive at a region, more on to next region to avoid
							 // processing a double at the same region again.
			}
		}
	}
	return truePositives;
}


//======================================================================== 
// generateROCdata
//========================================================================  
// The following function generates a file that can be used to plot the
// ROC curve in MATLAB.
//========================================================================


int generateRocData( PPM &image, PPM &templateImage, vector<int> gtVector, 
										int lowerErrorLevel, int upperErrorLevel, 
										int maxFalsePositives, char *ROCFilename)
{
	
	// set up a file to hold all the data
	FILE *ROCdata;
	ROCdata = fopen( ROCFilename, "w+" );
	if ( ROCdata == NULL)
	{
		cout << endl 
				 << "Unable to open file to hold ROC data."
				 << endl;
		return -1;
	}

	int imageNumRows = image.getNumRows();
	int imageNumColumns = image.getNumColumns();
	int truePositives;
	int falsePositives;
	int numMatches;
	int count = 1; // counter to number of data points obtained
	vector<int> matchPoints; // vector of points matched.

	
	// write the header
	fprintf( ROCdata, "%s\n", "% TP FP totalHits errorLevel" );


	for ( int errorLevel = lowerErrorLevel; errorLevel <= upperErrorLevel; 	
																										errorLevel += 2 )
	{
		matchPoints.clear(); // clear the vector;
		matchPoints = image.doRMSETemplateMatching( templateImage, errorLevel );
		
		truePositives = analyzeGroundTruth( matchPoints, gtVector, 
										imageNumRows, imageNumColumns );

		numMatches = matchPoints.size();
		falsePositives = numMatches - truePositives;
		
		// exit if false positives exceed limit
		if ( falsePositives > maxFalsePositives ) return count;

		fprintf( ROCdata, "%s%d%s%d%s %s%d%s%d%s %s%d%s%d%s %s%d%s%d%s\n", 
						 "TP(", count, ")=", truePositives, ";", 
						 "FP(", count, ")=", falsePositives, ";", 
						 "totalMatches(", count, ")=", numMatches, ";", 
						 "errorLevel(", count, ")=", errorLevel, ";" );
		count++;
	}
	
	fclose( ROCdata );

	return count;
}
