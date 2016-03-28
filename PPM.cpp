//======================================================================== 
// Project: ECE893 - Machine Vision - Target finding 
// ---------------------------------------------------------------------- 
// Package: 
// Authors: Vilas Kumar Chitrakaran
// Start Date: Fri Sep 21 06:46:12 edt 2001
// Compiler: GNU GCC 2.96
// Operating System: QNX RTP, GNU/Linux 
// ----------------------------------------------------------------------  
// File: PPM.cpp
// Implementation of the class PPM.
// See PPM.hpp for more details.
//========================================================================  
 

// ----- Standard Libaries -----
#include <stdio.h>
#include <math.h>

// ----- Project Includes -----
#include "PPM.hpp"


//------------- Static Data Members--------------

int PPM::s_paintFillQueueLength = 10000;

//======================================================================== 
// PPM::PPM: Constructor of the PPM class
//========================================================================  
 
PPM::PPM ()
{
	d_pixel = 0;
	d_numRows = 0;
	d_numColumns = 0;
	d_maximumPixelValue = 0;
}

PPM::PPM (char *sourceFilename)
{
	d_pixel = 0;
	FILE *source;
	if ( (source = fopen(sourceFilename, "rb")) == NULL )
	{
	 	//specified file not present. 
		cout << endl 
				 << "ERROR : [PPM::PPM (char *sourceFilename)]: " 
				 << "The file - " << sourceFilename << " - could not be opened."
			 	 << endl;
		return;
	}
	else
	{
		//read the file and get all the parameters
		char header[80];
		header[0] = fgetc(source);
		header[1] = fgetc(source);		
		header[2] = fgetc(source);				
		header[2] = '\0';
		
				if ( std::strcmp(header, "P5")	!= 0 )
				{
					cout << endl
							 <<"ERROR : [PPM::PPM (char *sourceFilename)]: " 
							 << "Image not greyscale. Exiting program...."
							 << endl;
					return; 
				}
				else
				{
					d_header = "P5";
					
					// get the remaining parameters
					d_maximumPixelValue = d_numRows = d_numColumns = -1;
					while( d_maximumPixelValue == -1 )
					{
						fscanf( source, "%s", header);
						if ( header[0] == '#' )
						{
							while( (header[0] = fgetc(source)) != '\n' );
							continue;
						}	
						if ( d_numColumns == -1 ) d_numColumns = atoi ( header );
						else if ( d_numRows == -1 ) d_numRows = atoi ( header);
						else d_maximumPixelValue = atoi ( header );
					}
					
					if ( d_maximumPixelValue != 255 )
					{
						cout << endl 
	 							 <<"ERROR : [PPM::PPM (char *sourceFilename)]: " 
								 << "Color resolution not 8 bits."
								 << endl;
						return; 
					}
				}
			
			// read the pixel value into the array
			header[0] = fgetc(source); //reading whitespace right after header
			if ( ( d_pixel = (unsigned char *) calloc(d_numRows * d_numColumns, 1) ) == NULL )
			{
				cout << endl 
						 <<"ERROR : [PPM::PPM (char *sourceFilename)]: " 
						 << "Unable to allocate enough memory for the image."
						 << " Exiting program....."
						 << endl;
				std::exit(0);
			}
			if ( ( fread( d_pixel, 1, d_numRows * d_numColumns, source ) ) != d_numRows * d_numColumns )
			{
				cout << endl 
						 <<"ERROR : [PPM::PPM (char *sourceFilename)]: " 
						 << "Error reading image data. Exiting program..."
						 << endl;
					
				//free allocated memory if any
				if ( d_pixel != NULL)
					free(d_pixel);

				std::exit(0);
			}
	}
	fclose(source);
}


PPM::PPM (int rows, int columns, int pixelValue)
{
	d_pixel = 0;
	if ( rows == 0 || columns == 0 )
	{
		cout << endl
				 << "ERROR : [PPM::PPM (int rows, int columns)]: "
				 << "Number of rows or columns specified for the"
		     << " empty image are not valid. Exiting program...."
			   << endl;
		std::exit(0);
	}
	else
	{
		d_header = "P5";
		d_numRows = rows;
		d_numColumns = columns;
		d_maximumPixelValue = 255;
	
		// initialize the pixel values;
		if ( ( d_pixel = (unsigned char *) calloc(d_numRows * d_numColumns, 1) ) == NULL )
		{
			cout << endl 
 					 << "ERROR : [PPM::PPM (int rows, int columns)]: "					 
					 << "Unable to allocate enough memory for the image."
					 << " Exiting program....."
					 << endl;
			std::exit(0);
		}
		
		for ( int row = 0; row < d_numRows; row++ )
		{
			for( int column = 0; column < d_numColumns; column++ )
			{
				d_pixel[row * d_numColumns + column] = pixelValue;
			}
		}
	}
}


//======================================================================== 
// PPM::~PPM: Destructor of the PPM class
//========================================================================  
 
//------------------------------------------------------------------------
// PROGRAMMER NOTE : Remember that the destructor does not get called when
// program is terminated by calling std::exit() or when unforseen crashes
// happen. In these cases, all that the destructor is supposed to do never
// happens - most importantly the memory allocated to the image by the
// constructor is not EXPLICITLY de-allocated.
//------------------------------------------------------------------------

PPM::~PPM ()
{
	//free allocated memory
	if ( d_pixel != NULL)
		free(d_pixel);
}

//======================================================================== 
// PPM::openImageFile
//========================================================================  

int PPM::openImageFile (char *sourceFilename)
{
	// Check if this object already allocated image memory. If so,
	// deallocate and start all over.
	if( (d_numColumns + d_numRows) != 0 )
		free(d_pixel);
	
	d_pixel = 0;
	FILE *source;
	if ( (source = fopen(sourceFilename, "rb")) == NULL )
	{
	 	//specified file not present. 
		cout << endl 
				 << "ERROR : [PPM::openImageFile]: " 
				 << "The file - " << sourceFilename << " - could not be opened."
			 	 << endl;
		return -1;
	}
	else
	{
		//read the file and get all the parameters
		char header[80];
		header[0] = fgetc(source);
		header[1] = fgetc(source);		
		header[2] = fgetc(source);				
		header[2] = '\0';
		
				if ( std::strcmp(header, "P5")	!= 0 )
				{
					cout << endl
							 <<"ERROR : [PPM::openImageFile]: " 
							 << "Image not greyscale."
							 << endl;
					return -1; 
				}
				else
				{
					d_header = "P5";
					
					// get the remaining parameters
					d_maximumPixelValue = d_numRows = d_numColumns = -1;
					while( d_maximumPixelValue == -1 )
					{
						fscanf( source, "%s", header);
						if ( header[0] == '#' )
						{
							while( (header[0] = fgetc(source)) != '\n' );
							continue;
						}	
						if ( d_numColumns == -1 ) d_numColumns = atoi ( header );
						else if ( d_numRows == -1 ) d_numRows = atoi ( header);
						else d_maximumPixelValue = atoi ( header );
					}
					
					if ( d_maximumPixelValue != 255 )
					{
						cout << endl 
	 							 <<"ERROR : [PPM::openImageFile]: " 
								 << "Color resolution not 8 bits."
								 << endl;
						return -1; 
					}
				}
			
			// read the pixel value into the array
			header[0] = fgetc(source); //reading whitespace right after header
			if ( ( d_pixel = (unsigned char *) calloc(d_numRows * d_numColumns, 1) ) == NULL )
			{
				cout << endl 
						 <<"ERROR : [PPM::openImageFile]: " 
						 << "Unable to allocate enough memory for the image."
						 << " Exiting program....."
						 << endl;
				std::exit(0);
			}
			if ( ( fread( d_pixel, 1, d_numRows * d_numColumns, source ) ) != d_numRows * d_numColumns )
			{
				cout << endl 
						 <<"ERROR : [PPM::openImageFile]: " 
						 << "Error reading image data. Exiting program..."
						 << endl;
					
				//free allocated memory if any
				if ( d_pixel != NULL)
					free(d_pixel);

				std::exit(0);
			}
	}
	fclose(source);
	return 0;
}

//======================================================================== 
// PPM::writeToFile
//========================================================================  
 
int PPM::writeToFile(char *targetFilename)
{
	FILE *destinationFile;
	
	destinationFile = fopen(targetFilename, "w+");

	if ( destinationFile == NULL)
	{
		cout << endl 
 				 << "ERROR : [PPM::writeToFile]: "					 
				 << "Unable to write to specified image file."
				 << endl;
		return -1;
	}
	
	fprintf(destinationFile, "%s %d %d %d\n", d_header, d_numColumns, d_numRows, 
								d_maximumPixelValue);
	
	if ( ( fwrite ( d_pixel, 1, d_numRows * d_numColumns, destinationFile ) ) 
								!= d_numRows * d_numColumns )
	{
		cout << endl 
				 <<"ERROR : [PPM::writeToFile]: " 
				 << "Error writing image data."
				 << endl;
		return -1;
	}
	
	fclose(destinationFile);
	return 0;
}

//======================================================================== 
// PPM::getHistogram
//========================================================================  
 
vector<int> PPM::getHistogram()
{
	vector<int> histogram;
	for (int index = 0; index <= d_maximumPixelValue; index++)
		histogram.push_back(0);
	
	for (int pixelPosition = 0; pixelPosition < d_numRows * d_numColumns; pixelPosition++)
		histogram[ d_pixel[pixelPosition] ] ++;

	return histogram;
}


//======================================================================== 
// PPM::doSimpleThreshold
//========================================================================  
 
PPM &PPM::doSimpleThreshold(vector<int> &pixelIndicesVector, 
							int lowerCutOffValue, int upperCutOffValue)
{
	pixelIndicesVector.clear();
		
	if ( lowerCutOffValue > upperCutOffValue || 
			 lowerCutOffValue < 0 || upperCutOffValue < 0 )
	{
		cout << endl << "ERROR : [PPM::doSImpleTreshold] :"
				 << " Both cutoff values must be positive integers and"
				 << " the lowerCutOffValue should be lower than upperCutOffValue. "
				 << "Exiting function.."
				 << endl;
		return *this;	
	}
	
	for ( int index = 0; index < d_numRows*d_numColumns; index ++)
	{
		if ( d_pixel[index] >= lowerCutOffValue && d_pixel[index] <= upperCutOffValue )
		{
			d_pixel[index] = 255;
			pixelIndicesVector.push_back(index);
		}
		else
			d_pixel[index] = 0; 
	}
	return *this;	
}


//======================================================================== 
// PPM::doHysteresisTreshold
//========================================================================  
 
//------------------------------------------------------------------------
// PROGRAMMER NOTE : Currently this function takes too much time to 
// execute. This is in urgent need for optimization. 
//------------------------------------------------------------------------


PPM &PPM::doHysteresisThreshold(int lowerCutOffValue, int upperCutOffValue)
{
	if ( lowerCutOffValue > upperCutOffValue || 
			 lowerCutOffValue < 0 || upperCutOffValue < 0 )
	{
		cout << endl << "ERROR : [PPM::doHysteresisTreshold] :"
				 << " Both cutoff values must be positive integers and"
				 << " the lowerCutOffValue should be lower than upperCutOffValue."
				 << "Exiting function."
				 << endl;
		
		return *this;
	}
	
	vector<int> brightPixelVector; // vector of pixels painted bright.
	int totalElements = d_numRows * d_numColumns; // total pixels in the image.
		
	// if pixel value greater than upper cutoff, it is definitely an edge
	// and if lower than lower cutoff then definitely not an edge.
	for ( int index = 0; index < totalElements; index++ )
	{
		if ( d_pixel[index] >= upperCutOffValue )
		{
			d_pixel[index] = 255;
			brightPixelVector.push_back(index);		
		}
		if ( d_pixel[index] < lowerCutOffValue )
			d_pixel[index] = 0;
	}
	
	// only pixels that are not painted now are the ones in the range
	// lowerCutOff <= pixelValue < upperCutOff. Paint them if they have a
	// neighbour at 255.
	 	
	int currentPixelIndex; // pixel index in the loop
	 
	while ( brightPixelVector.size() != 0 )
	{

		// execute until this brightPixelVector size falls to zero. 
		currentPixelIndex = brightPixelVector[0];

		// check all eight neighbours. if not painted, paint them, and
		// add the index to the vector.
		
		if ( d_pixel[ currentPixelIndex - d_numColumns - 1 ] != 255 &&
				 d_pixel[ currentPixelIndex - d_numColumns - 1 ] != 0 &&
				 currentPixelIndex - d_numColumns - 1 < totalElements &&
				 currentPixelIndex - d_numColumns - 1 >= 0 )
		{
				 d_pixel[ currentPixelIndex - d_numColumns - 1 ] = 255;
				 brightPixelVector.push_back( currentPixelIndex - d_numColumns - 1 );
		}

		if ( d_pixel[ currentPixelIndex - d_numColumns ] != 255 &&
				 d_pixel[ currentPixelIndex - d_numColumns ] != 0 &&
				 currentPixelIndex - d_numColumns < totalElements &&
				 currentPixelIndex - d_numColumns >= 0 )
		{
			 d_pixel[ currentPixelIndex - d_numColumns ] = 255;
				 brightPixelVector.push_back( currentPixelIndex - d_numColumns );
		}

		if ( d_pixel[ currentPixelIndex - d_numColumns + 1 ] != 255 &&
  				 d_pixel[ currentPixelIndex - d_numColumns + 1 ] != 0 &&
				 currentPixelIndex - d_numColumns + 1 < totalElements &&
				 currentPixelIndex - d_numColumns + 1 >= 0 )
		{
				 d_pixel[ currentPixelIndex - d_numColumns + 1 ] = 255;
				 brightPixelVector.push_back( currentPixelIndex - d_numColumns + 1 );
		}

		if ( d_pixel[ currentPixelIndex - 1 ] != 255 &&
				 d_pixel[ currentPixelIndex - 1 ] != 0 &&
				 currentPixelIndex - 1 < totalElements &&
				 currentPixelIndex - 1 >= 0 )
		{
				 d_pixel[ currentPixelIndex - 1 ] = 255;
				 brightPixelVector.push_back( currentPixelIndex - 1 );
		}

		if ( d_pixel[ currentPixelIndex + 1 ] != 255 &&
				 d_pixel[ currentPixelIndex + 1 ] != 0 &&
				 currentPixelIndex + 1 < totalElements &&
				 currentPixelIndex + 1 >= 0 )
		{
				 d_pixel[ currentPixelIndex + 1 ] = 255;
				 brightPixelVector.push_back( currentPixelIndex + 1 );
		}

		if ( d_pixel[ currentPixelIndex + d_numColumns - 1 ] != 255 &&
				 d_pixel[ currentPixelIndex + d_numColumns - 1 ] != 0 &&
				 currentPixelIndex + d_numColumns - 1 < totalElements &&
				 currentPixelIndex + d_numColumns - 1 >= 0 )
		{
				 d_pixel[ currentPixelIndex + d_numColumns - 1 ] = 255;
				 brightPixelVector.push_back( currentPixelIndex + d_numColumns - 1 );
		}

		if ( d_pixel[ currentPixelIndex + d_numColumns ] != 255 &&
				 d_pixel[ currentPixelIndex + d_numColumns ] != 0 &&
				 currentPixelIndex + d_numColumns < totalElements &&
				 currentPixelIndex + d_numColumns >= 0 )
		{
				 d_pixel[ currentPixelIndex + d_numColumns ] = 255;
				 brightPixelVector.push_back( currentPixelIndex + d_numColumns );
		}

		if ( d_pixel[ currentPixelIndex + d_numColumns + 1 ] != 255 &&
				 d_pixel[ currentPixelIndex + d_numColumns + 1 ] != 0 &&
				 currentPixelIndex + d_numColumns + 1 < totalElements &&
				 currentPixelIndex + d_numColumns + 1 >= 0 )
		{
				 d_pixel[ currentPixelIndex + d_numColumns + 1 ] = 255;
				 brightPixelVector.push_back( currentPixelIndex + d_numColumns + 1 );
		}

		// all neighbours of this pixel checked and painted. remove from list.					 
		brightPixelVector.erase( brightPixelVector.begin() );
		

	}

	// any pixels unpainted are surrounded by dark regions only. paint them dark	
	for ( int index = 0; index < totalElements; index++ )
	{
		if ( d_pixel[index] != 255 && d_pixel[index] != 0 )
			d_pixel[index] = 0;
	}

	return *this; 
}


//======================================================================== 
// PPM::invertImage
//========================================================================  

PPM &PPM::invertImage()
{
	for ( int index = 0; index < d_numRows * d_numColumns; index ++ )
		d_pixel[index] = 255 - d_pixel[index];

	return *this;	
}


//======================================================================== 
// PPM::invertImage
//========================================================================  

PPM &PPM::fillColor(int color)
{
	if(color > 255)
		color = 255;
	if(color < 0)
		color = 0;
		
	for ( int index = 0; index < d_numRows * d_numColumns; index ++ )
		d_pixel[index] = color;

	return *this;	
}

//======================================================================== 
// PPM::addImage
//========================================================================  

PPM &PPM::addImage( PPM &withThisImage )
{
	// define max limit of dimensions in rows and columns
	int maxRows;
	int maxColumns;
	int sum;
	
	( withThisImage.getNumRows() <= getNumRows() ) ? 
	( maxRows = withThisImage.getNumRows() ) :
	( maxRows = getNumRows() );
	
	( withThisImage.getNumColumns() <= getNumColumns() ) ? 
	( maxColumns = withThisImage.getNumColumns() ) :
	( maxColumns = getNumColumns() );

	for ( int row = 0; row < maxRows; row++ )
	{
		for ( int column = 0; column < maxColumns; column++ )
		{
			sum = 	d_pixel[ row * getNumColumns() + column ] + withThisImage(row, column); 
			if ( sum > 255)
				sum = 255;
			
			d_pixel[ row * getNumColumns() + column ] = sum;
		}
	} 

	return *this;	
}


//======================================================================== 
// PPM::doSobelFiltering
//========================================================================  

//------------------------------------------------------------------------
// PROGRAMMER NOTE : This function is important. Optimization will help.
//------------------------------------------------------------------------


PPM &PPM::doSobelFiltering(int direction)
{
	// This implementation of convolution may be computation intensive
	// in some cases.
	
	// define a local variable to hold results of convolution.
	PPM convolvedImage( getNumRows(), getNumColumns() ); 

	// define the convolution masks for x and y directions
	ConvolutionMask<3, 3, int> xMask;
	ConvolutionMask<3, 3, int> yMask;

	switch( direction )
	{
		case 0:
		{
			xMask = -1, 0, 1, -2, 0, 2, -1, 0, 1;
			yMask = 0;
			break;
		}
		case 1:
		{
			xMask = 0;
			yMask = 1, 2, 1, 0, 0, 0, -1, -2, -1;
			break;
		}
		default:
		{
			xMask = -1, 0, 1, -2, 0, 2, -1, 0, 1;
			yMask = 1, 2, 1, 0, 0, 0, -1, -2, -1;
		}
	}
	
	int rowRange = 1; 
	int columnRange = 1;

	for ( int row = 0; row < d_numRows; row++ )
	{
		for ( int column = 0; column < d_numColumns; column++ )
		{
			int xSum = 0;
			int ySum = 0;
			int sum = 0;
			
			for ( int dr = (-1) * rowRange; dr <= rowRange; dr++ )
			{
				for ( int dc = (-1) * columnRange; dc <= columnRange; dc++ )
				{
					if ( (row + dr) >= 0 && (row + dr) < d_numRows &&
							 (column + dc) >= 0 && (column + dc) < d_numColumns )
					{
						xSum += (int)d_pixel[(row + dr) * d_numColumns + (column + dc)]
											* xMask( (dr + rowRange) * 3 + (dc + columnRange) );
						ySum += (int)d_pixel[(row + dr) * d_numColumns + (column + dc)]
										* yMask( (dr + rowRange) * 3 + (dc + columnRange) );
					}
				}
			}
			
			sum = (int)sqrt( (double)(xSum * xSum + ySum * ySum) );

			// Max. pixel value is only 255. Truncate if convolution exceeds [0 255] range.
			if ( sum > getMaximumPixelValue() ) sum = getMaximumPixelValue();

			convolvedImage( row * convolvedImage.getNumColumns() + column ) = sum;
		}
	}
	
	*this = convolvedImage;
	return *this;
}


//======================================================================== 
// PPM::doThinning
//========================================================================  

//------------------------------------------------------------------------
// PROGRAMMER NOTE : The following code is only a slightly modified version
// of "thin.c" found at http://vision.ucsd.edu./classes/ece253b/course/thin.c
//------------------------------------------------------------------------

PPM &PPM::doThinning()
{
	int r, c, r2, c2; 
	int Transitions, EdgeNeighbors, ThinningFlag;
	
	unsigned char	*TempArray;

	int ncols = d_numColumns;
	int nrows = d_numRows;
	
	if ( ( TempArray = (unsigned char *) calloc( nrows * ncols, sizeof(unsigned char) ) ) == NULL )
	{
		cout << endl 
				 << "ERROR : [PPM::doThinning]: "					 
				 << "Unable to allocate enough memory for the operation."
				 << " Exiting function."
				 << endl;
		return *this;
	}

	do
	{
		for (r=0; r < nrows * ncols; r++)
  			TempArray[r] = d_pixel[r];

	 ThinningFlag = 0;

 	 for (r = 2; r < (nrows - 2); r++)
   {
			for (c = 2; c < (ncols - 2); c++)
   	   {
    				// look for number of background (0) to foreground (255) transitions as you 
    				// move around the pixel in clockwise direction.
    				if (TempArray[r * ncols + c] == 255)
					{
						Transitions = 0;
						if ( TempArray[ (r - 1) * ncols + c ] == 0  &&  TempArray[ (r - 1) * ncols + c + 1 ] == 255 )
		  					Transitions++;
						if ( TempArray[ (r-1) * ncols + c + 1 ] == 0  &&  TempArray[ r * ncols + c + 1 ] == 255)
		  					Transitions++;
						if (TempArray[ r * ncols + c + 1 ] == 0  &&  TempArray[ (r+1) * ncols + c + 1 ] == 255)
		  					Transitions++;
						if (TempArray[ (r+1) * ncols + c + 1 ] == 0  &&  TempArray[ (r+1) * ncols + c ] == 255)
		  					Transitions++;
						if (TempArray[ (r+1) * ncols + c ] == 0  &&  TempArray[ (r+1) * ncols + c - 1 ] == 255)
		  					Transitions++;
						if (TempArray[ (r+1) * ncols + c - 1 ] == 0  &&  TempArray[ r * ncols + c - 1 ] == 255)
		  					Transitions++;
						if (TempArray[ r * ncols + c - 1 ] == 0  &&  TempArray[ (r-1) * ncols + c - 1 ] == 255)
		  					Transitions++;
						if (TempArray[ (r-1) * ncols + c - 1 ] == 0  &&  TempArray[ (r-1) * ncols + c ] == 255)
		  					Transitions++;
						
						// The pixel in question is considered for thinning only if it has
						// one background to foreground transition and only if 
						// 3 to 7 pixels in its neighbourhood are edges (255)
						
						if (Transitions != 1)
		  					continue;	
						
						EdgeNeighbors = 0;
						for (r2 = -1; r2 <= 1; r2++)
		  					for (c2 = -1; c2 <= 1; c2++)
		    					if (TempArray[ (r+r2) * ncols + c + c2 ] == 255)
			  						EdgeNeighbors++;
						
						if (EdgeNeighbors < 3  ||  EdgeNeighbors > 7)
		  						continue;	
						
						// The pixel in question is reduced to zero if ONE OF the following 
						// conditions are met:
						// a) pixel to its top is 0
						// b) pixel to its right is 0
						// c) pixel to its left AND pixel to its bottom is zero.
						if ( TempArray[ (r-1) * ncols + c ] == 0  || 
								TempArray[ r * ncols + c + 1 ] == 0  ||
								( TempArray[ r * ncols + c - 1 ] == 0  &&
								TempArray[ (r+1) * ncols + c ] == 0 ) )
		  				{
		  					ThinningFlag = 1;
		  					d_pixel[ r * ncols + c ] = 0;
		  				}
					}
      		}
    		}
  		}while ( ThinningFlag == 1 );
	free( TempArray );
	
	return *this;
}


//======================================================================== 
// PPM::paintFill
//========================================================================  

//------------------------------------------------------------------------
// PROGRAMMER NOTE : The following code is only a slightly modified version
// of the one provided to me in my Machine Vision class. 
//------------------------------------------------------------------------

int PPM::paintFill( int row, int column, int paintOverLabel, 
					int newLabel, vector<int> &indicesOfPixelsPainted)
{
	int countOfPixelsPainted;		// count of pixels painted.
	int queue[s_paintFillQueueLength];		// queue of indices painted over
	int qTail, qHead;	// head and tail positions of the queue
	int currentRow, currentColumn;		// The index to pixel being operated upon
																	// during the looping.

	if ( d_pixel[ row * d_numColumns + column ] != paintOverLabel )
		return 0;
	
	d_pixel[ row * d_numColumns + column ] = newLabel;
	indicesOfPixelsPainted.push_back(row * d_numColumns + column);
	countOfPixelsPainted = 1;
	
	queue[0] = row * d_numColumns + column;
	qTail = 0;
	qHead = 1;
	

	while ( qTail != qHead )
	{
		for ( int rowOffset = -1; rowOffset <= 1; rowOffset++ )
			for ( int columnOffset = -1; columnOffset <= 1; columnOffset++ )
			{
				currentRow = queue[qTail] / d_numColumns + rowOffset;
				currentColumn = queue[qTail] % d_numColumns + columnOffset;
				
				// The particular pixel whose co-ordinates have been 
				// passed into the function has already been examined.
				// So skip that pixel now. (Only it's neighbours need to be examined)
				if ( rowOffset == 0 && columnOffset == 0 )
					continue;
				
				// Skip if indices are out of bounds of the image
				if ( currentRow < 0 || currentRow >= d_numRows ||
						 currentColumn < 0 || currentColumn >= d_numColumns )
					continue;
					
				// Skip if the pixel does not match the paintOverLabel
				if ( d_pixel[ currentRow * d_numColumns + currentColumn ] != paintOverLabel )
					continue;
				
				// If the function gets through until here, then the pixel is a part of the 
				// contiguous region that needs to be paint-filled
				d_pixel[ currentRow * d_numColumns + currentColumn ] = newLabel;
				
				// Note the index of the pixel painted over and increment the count 
				// of painted pixels.
				indicesOfPixelsPainted.push_back(currentRow * d_numColumns + currentColumn);
				countOfPixelsPainted++;
				
				// The head of the queue holds the index to the painted pixel. Move the 
				// head of the queue to the next position.
				queue[qHead] = currentRow * d_numColumns + currentColumn;
				qHead = ( qHead + 1 ) % s_paintFillQueueLength;
				
				// The queue is circular - the head of the queue can fall off the far 
				// end and come back in from the beginning of the queue. If the head comes
				// back to the tail position, the queue is not long enough to hold the 
				// indices of pixels painted over.
				if ( qHead == qTail )
				{
					cout << endl << "[PPM::paintFill]: "
							 << "Queue of indices to painted pixels have overflowed. Increase the "
							 << "value of the static variable s_paintFillQueueLength."
							 << endl;
					return -1;
				}
			}
			
			// All pixels in the neighbourhood of this pixel has been examined
			// Move over to the next pixel from qTail and repeat the process. If all
			// pixels forming a region has been found, qTail will get to qHead position,
			// at which point the while() loop is exited.
			qTail = ( qTail + 1 ) % s_paintFillQueueLength;
	}
	return countOfPixelsPainted;
}


//======================================================================== 
// PPM::calculateCentroid
//========================================================================  

void PPM::calculateCentroid( vector<int> &areaPixelIndicesVector, 
										double &centroidX, double &centroidY )
{
	int area = areaPixelIndicesVector.size();
	centroidX = 0;
	centroidY = 0;

	if ( area <= 0 )
	{
		cout << endl << "ERROR : [PPM::calculateCentroid]: "
				 << "The region has no size. Centroids set to (0,0)."
				 << endl;
	}
	
	else
	{
		for ( int index = 0; index < area; index++ )
		{
			if ( areaPixelIndicesVector[index] > d_numRows * d_numColumns )
			{
				cout << endl << "ERROR : [PPM::calculateCentroid]: "
						 << "Invalid index to a pixel found. Centroid calculation"
						 << " may be erronous." 
						 << endl;
			}
			else
			{
				centroidX += areaPixelIndicesVector[index] % d_numColumns;
				centroidY += areaPixelIndicesVector[index] / d_numColumns;
			}
		}
		centroidX = centroidX / (double)area;
		centroidY = centroidY / (double)area;
	}
}

//======================================================================== 
// PPM::calculatePerimeter
//========================================================================  

//------------------------------------------------------------------------
// PROGRAMMER NOTE : The following function calls the find() algorithm too
// many times. Can it be further optimized. Also, probably a nice way to 
//------------------------------------------------------------------------

int PPM::calculatePerimeter( vector<int> &areaPixelIndicesVector )
{
	// The funda is that if a pixel has atleast one neighbour not belonging
	// to the region, it is on the border of the region and hence part of 
	// the perimeter. This ofcourse works only for smooth regions with no 
	// bubbles or cavities inside.
	
	int perimeter = 0;
	vector<int>::iterator indexIterator;
	
	for ( int index = 0; index < areaPixelIndicesVector.size(); index++ )
	{
		// check all eight neighbours of the pixel. if atleast one neighbour
		// does not belong to the set of indices, it is a part of perimeter.
		
		if ( (indexIterator = find( areaPixelIndicesVector.begin(), 
													areaPixelIndicesVector.end(), 
													areaPixelIndicesVector[index] - d_numColumns - 1 ) )
													== areaPixelIndicesVector.end() )
		{
			perimeter++;
			continue;
		}
		
		if ( (indexIterator = find( areaPixelIndicesVector.begin(), 
													areaPixelIndicesVector.end(), 
													areaPixelIndicesVector[index] - d_numColumns ) )
													== areaPixelIndicesVector.end() )
		{
			perimeter++;
			continue;
		}
		
		if ( (indexIterator = find( areaPixelIndicesVector.begin(), 
													areaPixelIndicesVector.end(), 
													areaPixelIndicesVector[index] - d_numColumns + 1 ) )
													== areaPixelIndicesVector.end() )
		{
			perimeter++;
			continue;
		}
		
		if ( (indexIterator = find( areaPixelIndicesVector.begin(), 
													areaPixelIndicesVector.end(), 
													areaPixelIndicesVector[index] - 1 ) )
													== areaPixelIndicesVector.end() )
		{
			perimeter++;
			continue;
		}
		
		if ( (indexIterator = find( areaPixelIndicesVector.begin(), 
													areaPixelIndicesVector.end(), 
													areaPixelIndicesVector[index] + 1 ) )
													== areaPixelIndicesVector.end() )
		{
			perimeter++;
			continue;
		}

		if ( (indexIterator = find( areaPixelIndicesVector.begin(), 
													areaPixelIndicesVector.end(), 
													areaPixelIndicesVector[index] + d_numColumns - 1 ) )
													== areaPixelIndicesVector.end() )
		{
			perimeter++;
			continue;
		}

		if ( (indexIterator = find( areaPixelIndicesVector.begin(), 
													areaPixelIndicesVector.end(), 
													areaPixelIndicesVector[index] + d_numColumns ) )
													== areaPixelIndicesVector.end() )
		{
			perimeter++;
			continue;
		}

		if ( (indexIterator = find( areaPixelIndicesVector.begin(), 
													areaPixelIndicesVector.end(), 
													areaPixelIndicesVector[index] + d_numColumns + 1 ) )
													== areaPixelIndicesVector.end() )
		{
			perimeter++;
			continue;
		}
	}
	return perimeter;
}


//======================================================================== 
// PPM::calculateCompactness
//========================================================================  

double PPM::calculateCompactness( vector<int> &areaPixelIndicesVector )
{
	double perimeter = calculatePerimeter( areaPixelIndicesVector );
	double area = areaPixelIndicesVector.size();
	
	return ( perimeter * perimeter / area );
}


//======================================================================== 
// PPM::calculateMoment
//========================================================================  

int PPM::calculateMoment(int j, int k, vector<int> &areaPixelIndicesVector )
{
	int moment = 0;
	for ( int index = 0; index < areaPixelIndicesVector.size(); index++ )
	{
		if ( areaPixelIndicesVector[index] > d_numRows * d_numColumns )
		{
			cout << endl << "ERROR : [PPM::calculateMoment]: "
					 << "Invalid index to a pixel found. Moment calculation"
					 << " may be erronous." 
					 << endl;
		}
		else
		{
			moment += (int)
			( std::pow( (double) (areaPixelIndicesVector[index] / d_numColumns), j )
			* std::pow( (double) (areaPixelIndicesVector[index] % d_numColumns), k ) );
		}
	}
	return moment;
}


//======================================================================== 
// PPM::calculatePrincipalAxis
//========================================================================  

int PPM::calculatePrincipalAxis( vector<int> &areaPixelIndicesVector )
{

	double m11 = calculateMoment( 1, 1, areaPixelIndicesVector );
	double m20 = calculateMoment( 2, 0, areaPixelIndicesVector );
	double m02 = calculateMoment( 0, 2, areaPixelIndicesVector );
	
	return (int)( (0.5) * std::atan2( (2 * m11), (m20 - m02) ) );
	
}


//======================================================================== 
// PPM::calculateElongation
//========================================================================  

int PPM::calculateElongation( vector<int> &areaPixelIndicesVector )
{
	int elongation = 0;
	
	// no algorithm yet
	
	return elongation;
}


//======================================================================== 
// PPM::doMAETemplateMatching
//========================================================================  

vector<int> PPM::doMAETemplateMatching( PPM &templateImage, int acceptableError )
{

	// define a local variable to hold results.
	vector<int> matchingIndices; 

	// get the limits of the template image.
	int maxTRow = templateImage.getNumRows();
	int maxTColumn = templateImage.getNumColumns();
	
	
	for ( int iRow = 0; iRow <= d_numRows - maxTRow; iRow++ )
	{
		for ( int iColumn = 0; iColumn <= d_numColumns - maxTColumn; iColumn++ )
		{
			//declare a variable for mean absolute error
			double MAE = 0;

			for ( int tRow = 0; tRow < maxTRow; tRow++ )
			{
				for ( int tColumn = 0; tColumn < maxTColumn; tColumn++ )
				{
					MAE += std::abs( d_pixel[ (iRow + tRow) * d_numColumns + (iColumn + tColumn) ]
						- templateImage.d_pixel[tRow * maxTColumn + tColumn] );
				}
			}
			MAE = MAE / (double)(maxTRow * maxTColumn);
	
			// the position in the image grid with acceptable MAE value. 
			if ( MAE <= acceptableError )
			{
				matchingIndices.push_back( (iRow + maxTRow/2) * d_numColumns + (iColumn + maxTColumn/2) );
			}

		}
	}
			
	return matchingIndices;
}


//======================================================================== 
// PPM::doRMSETemplateMatching
//========================================================================  

vector<int> PPM::doRMSETemplateMatching( PPM &templateImage, int acceptableError )
{

	// define a local variable to hold results.
	vector<int> matchingIndices; 

	// get the limits of the template image.
	int maxTRow = templateImage.getNumRows();
	int maxTColumn = templateImage.getNumColumns();
	
	
	for ( int iRow = 0; iRow <= d_numRows - maxTRow; iRow++ )
	{
		for ( int iColumn = 0; iColumn <= d_numColumns - maxTColumn; iColumn++ )
		{
			//declare a variable for mean absolute error
			double RMSE = 0;

			for ( int tRow = 0; tRow < maxTRow; tRow++ )
			{
				for ( int tColumn = 0; tColumn < maxTColumn; tColumn++ )
				{
					RMSE += std::pow( (double)std::abs( d_pixel[ (iRow + tRow) * d_numColumns + (iColumn + tColumn) ]
						- templateImage.d_pixel[tRow * maxTColumn + tColumn] ) , 2 );
				}
			}
			
			RMSE = std::sqrt( RMSE / (double)(maxTRow * maxTColumn) );
	
			// the position in the image grid with acceptable RMSE value. 
			if ( RMSE <= acceptableError )
			{
				matchingIndices.push_back( (iRow + maxTRow/2) * d_numColumns + (iColumn + maxTColumn/2) );
			}

		}
	}
			
	return matchingIndices;
}



//================ END OF FUNCTION DEFINITIONS ============================





