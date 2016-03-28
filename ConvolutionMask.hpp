//======================================================================== 
// Project: ECE893 - Machine Vision - Target finding 
// ---------------------------------------------------------------------- 
// Package: 
// Authors: Vilas Kumar Chitrakaran
// Start Date: Fri Sep 21 06:46:12 edt 2001
// Compiler: GNU GCC 2.96 
// Operating System: QNX RTP, GNU/Linux 
// ----------------------------------------------------------------------  
// File: ConvolutionMask.hpp
// Interface of the class ConvolutionMask.
//========================================================================  
 

#ifndef INCLUDED_ConvolutionMask_hpp
#define INCLUDED_ConvolutionMask_hpp
 
 
//======================================================================== 
// class ConvolutionMask
// ----------------------------------------------------------------------
// \brief
// Interface for the convolution mask to be used with PPM images.
//
// This file describes the convolution mask (matrix) that can be used to 
// convolve with images of type PPM.
//
//========================================================================  
 
template<class T> class MaskInitializer;

template< int numRows, int numColumns, class T = int>
class ConvolutionMask
{
 public:
 
	// ----- Constructors and Destructor -----
	ConvolutionMask () {}
		// The constructor for the mask.Doesn't do anything. 
	
	~ConvolutionMask () {}
		// The destructor. Doesn't do anything either.
		
	inline int getNumRows() const { return numRows; }
		//  return		Number of rows in the mask.
		
	inline int getNumColumns() const { return numColumns; }
		//  return		Number of columns in the mask.

	inline T &operator()(int index);
		// This function overloads the \c () operator to access
 		// the value at the \a index position of the mask.
 		// Legal index range is (0) to (totalElements-1). 
		// The operator can be intuitively used to assign a value to an
		// element in the image such as
 		// \code myMask(2)= -1; \endcode or
 		// \code cout << myMask(2) << endl; \endcode
		//  index		Position of a pixel in the image.
		//  return		Stored value of the property of the
		//						image (intensity, etc) at the specified 
		//						index.
						
	inline T &operator()(int row, int column);
		// This function overloads the \c () operator to access
 		// the value at the \a row, \a column position of the mask.
 		// Legal index range is (0,0) to (totalRows-1, totalColumns-1). 
		// The operator can be intuitively used to assign a value to an
		// element in the image such as
 		// \code myMask(2,3) = 0; \endcode or
 		// \code cout << myMask(2,3) << endl; \endcode
		//  row, column		Position of a pixel in the image.
		//  return					Stored value of the property of the
		//									image (intensity, etc) at the specified 
		//									row and column.
		 	

	inline MaskInitializer<T> operator=(const T &elementValue);
		// This function provides an overloaded assignment
		// operator for initializing a ConvolutionMask object.
		//  elementValue		This is the value to
		//				which all elements in the mask
		//				are initialized.
		// The initialization of the ConvolutionMask object can also
		// be done as a comma seperated list. For example:
		// \code ConvolutionMask<2,2,double> myMask;
		//	   myMask = -0.125, 0.125, 0.125, -0.125 ; \endcode


	// ========== END OF INTERFACE ==========

 private:
		 
	// ----- Private Data Members -----

	T d_mask[numRows * numColumns];
		// The mask matrix with values stored
		//	 in a one dimensional array.

		 	
	// ----- Private Methods -----

};


// IMPLEMENTATION OF INLINE FUNCTIONS FOLLOW.

//======================================================================== 
// ConvolutionMask::operator()
//========================================================================  
		
template< int numRows, int numColumns, class T >
T &ConvolutionMask< numRows, numColumns, T >::operator()(int index)
{
	if ( index < 0 || index > (numRows * numColumns - 1) )
	{
		cout << endl << "[ConvolutionMask::operator()(int index)]: "
				 << "Co-ordinates out of bound of the mask." 
				 << endl;
		std::exit(0);  
	}
	else
	{
		return d_mask[index];	
	}
}


template< int numRows, int numColumns, class T >
T &ConvolutionMask< numRows, numColumns, T >::operator()(int row, int column)
{
	if ( row < 0 || row > (numRows - 1) || 
												column < 0 || column > (numColumns - 1) )
	{
		cout << endl << "[ConvolutionMask::operator()(int row, int column)]: "
				 << "Co-ordinates out of bound of the image." 
				 << endl;
		std::exit(0);  
	}
	else
		return d_mask[row * numColumns + column];	
}


//======================================================================== 
// ConvolutionMask::operator=
//========================================================================  

template<int numRows, int numColumns, class T>
MaskInitializer<T> ConvolutionMask<numRows, numColumns, T>::operator=(const T &elementValue)
{
	for (int index = 0; index < numRows * numColumns; index++)
	{	
		d_mask[index] = elementValue;
	}
	MaskInitializer<T>  maskInitialize(numRows*numColumns, 1, d_mask);
	return maskInitialize;
}


//----- END OF DEFINITIONS FOR CLASS ConvolutionMask --------------------



//======================================================================== 
// class MaskInitializer
// ----------------------------------------------------------------------
// \brief
// The class \c MaskInitializer is used to initialize the ConvolutionMask
// object.
//
//========================================================================  


template<class T = int>
class MaskInitializer
{
 public:
 
	// ----- Constructors and Destructor -----
	inline MaskInitializer (int totalElements, int offset, T *firstElementPointer);
		// The default constructor.

	~MaskInitializer (){}
		// The default destructor.
	
		 	
	// ----- Public Data Members -----
		

	inline MaskInitializer<T>  operator,(const T &elementValue);
		// This function provides a method to
		// easily assign the elements of a mask.
		// A mask object is initialized in
		// the following manner:
		// \code ConvolutionMask<2,2,double> myMask;
		//	   myMask = -0.125, 0.125, 0.125, -0.125 ; \endcode
	 
	// ========== END OF INTERFACE ==========
 
private:
		 
	// ----- Private Data Members -----
		
	T d_numElements;
		// The total number of
		// elements in the non-scalar object.
	
	int d_positionCounter;
		// Holds the offset from the pointer
		// to first value in non-scalar object
		// to the current element being initialized.
	
	T *d_firstElementPointer; 
		// Pointer to the first element in 
		// the non-scalar object.	

};


//========================================================================
// MaskInitializer::MaskInitializer 
//========================================================================

template<class T>
MaskInitializer<T>::MaskInitializer (int totalElements, int offset, T *firstElementPointer)
// The default constructor.
{	
	d_numElements = totalElements;
	d_firstElementPointer = firstElementPointer;
	d_positionCounter = offset;
}

//========================================================================
// MaskInitializer::operator, 
//========================================================================

template<class T>
MaskInitializer<T>  MaskInitializer<T>::operator,(const T &elementValue)
{
	if (d_positionCounter > (d_numElements-1))
	{
		cout << endl << "[MaskInitializer::operator,()]: "
				 << "Incorrect number of elements specified."
				 << endl;
		std::exit(0);
	}
	else
	{
		*(d_firstElementPointer + d_positionCounter) = elementValue;
		d_positionCounter++;
		MaskInitializer<T>  maskInitialize(d_numElements,d_positionCounter, d_firstElementPointer);
		return maskInitialize;
	}
}











#endif

