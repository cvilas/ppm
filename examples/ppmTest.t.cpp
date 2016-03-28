//======================================================================== 
// Project: 
// ---------------------------------------------------------------------- 
// Package: 
// Authors: Vilas Kumar Chitrakaran
// Start Date: Fri Sep 21 06:46:12 edt 2001
// Compiler: GNU GCC 2.96 
// Operating System: QNX RTP, GNU/Linux 
// ----------------------------------------------------------------------  
// File: ppmTest.t.cpp
// Example program for the class PPM.
//========================================================================  
 

// ----- Standard Libaries -----
#include <iostream>
#include <stdio.h>
#include <math.h>

// ----- Project Includes -----
#include "PPM.hpp"
#include "ConvolutionMask.hpp"

using namespace std; 
 
int main(int argc, char *argv[])
{
	
	if (argc != 2)
	{
		cout << "Usage : ppmTest.t [ppm image]" << endl;
		std::exit(0);
	}
	// open original image
	PPM originalImage(argv[1]);
	vector<int> tresholdedPixels;
	
	char option;
	
	do
	{
		cout << endl << "What do you want to do?" << endl
		 << "[g] Histogram" << endl     
     << "[t] Simple Threshold" << endl
     << "[h] Hysteresis Threshold" << endl
     << "[s] Smoothen" << endl
     << "[m] Median Filtering" << endl
     << "[e] Sobel edge detection" << endl
     << "[i] Invert image" << endl
     << "[p] Paint fill" << endl
     << "[l] Thinning" << endl
     << "[w] Write to file" << endl
     << "[q] Quit program" << endl;
					
		cin >> option;

		
		switch(option)
		{
			case 'g' :
			{
			 	vector<int> histogram;
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

				cout << endl << "Histogram written." << endl;
				
				break;
			}

			
			case 't' :
			{
				int upperThreshold, lowerThreshold;
				cout << endl << "Please select lower threshold :";
				cin >> lowerThreshold;
				cout << endl << "Please select higher threshold :";
				cin >> upperThreshold;
				
				originalImage.doSimpleThreshold( tresholdedPixels, lowerThreshold, upperThreshold );
				
				cout << endl << "Simple Thresholding performed on the image." << endl;
				
				break;
			}

			case 'h' :
			{
				int upperThreshold, lowerThreshold;
				cout << endl << "Please select lower threshold :";
				cin >> lowerThreshold;
				cout << endl << "Please select higher threshold :";
				cin >> upperThreshold;
				
				originalImage.doHysteresisThreshold( lowerThreshold, upperThreshold );
				
				cout << endl << "Hysteresis Thresholding performed on the image." << endl;
				
				break;
			}

			case 's' :
			{
				originalImage.doMeanFiltering<3>();
				cout << endl << "Smoothing performed on the image." << endl;
				break;
			}
			
			case 'm' :
			{
				originalImage.doMedianFiltering<3>();
				cout << endl << "Median filtering performed on the image." << endl;
				break;
			}
		
			case 'e' :
			{
				originalImage.doSobelFiltering();
				cout << endl << "Sobel filtering performed on the image." << endl;
				break;
			}
			
			case 'i' :
			{
				originalImage.invertImage();
				cout << endl << "Image has been inverted." << endl;
				break;
			}

			case 'p' :
			{
				int seedRow, seedColumn, paintOverLabel, newLabel, area;
				cout << endl << "Please select seed row :";
				cin >> seedRow;
				cout << endl << "Please select seed column :";
				cin >> seedColumn;
				cout << endl << "Please select paint-over-label :";
				cin >> paintOverLabel;
				cout << endl << "Please select new label :";
				cin >> newLabel;
				
				vector<int> junk;
				area = originalImage.paintFill( seedRow, seedColumn, paintOverLabel, newLabel, junk );
				
				cout << endl << "Painted " << area << " number of pixels."<< endl;
				
				break;
			}

			
			case 'l' :
			{
				originalImage.doThinning();
				cout << endl << "Image has been thinned." << endl;
				break;
			}
			
			case 'w' :
			{
				originalImage.writeToFile("convolvedImage.t.ppm");
				cout << endl << "File written." << endl;
				break;
			}
			
			case 'q' :
			{
				return 0;
			}
	
			default :
			{
				cout << endl << "Illegal choice. Please try again." << endl;
			}
		}
	}while ( option != 'q' );
	 
	return 0;
}
