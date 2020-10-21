/******************************* SOURCE LICENSE *********************************
Copyright (c) 2020 MicroModeler.

A non-exclusive, nontransferable, perpetual, royalty-free license is granted to the Licensee to
use the following Information for academic, non-profit, or government-sponsored research purposes.
Use of the following Information under this License is restricted to NON-COMMERCIAL PURPOSES ONLY.
Commercial use of the following Information requires a separately executed written license agreement.

This Information is distributed WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

******************************* END OF LICENSE *********************************/

// A commercial license for MicroModeler DSP can be obtained at http://www.micromodeler.com/launch.jsp

#include "notch50.h"

#include <stdlib.h> // For malloc/free
#include <string.h> // For memset

float notch50_coefficients[24] =
{
	0.0000000, 0.0000000, 0.0000000, 0.18893098, 0.0051157465, 0.020595396,
	0.0045800620, -0.0039396882, -0.010367115, -0.031745897, -0.047299005, -0.048787232,
	-0.053407557, 0.94144521, -0.053407557, -0.048787232, -0.047299005, -0.031745897,
	-0.010367115, -0.0039396882, 0.0045800620, 0.020595396, 0.0051157465, 0.18893098
};


notch50Type *notch50_create( void )
{
	notch50Type *result = (notch50Type *)malloc( sizeof( notch50Type ) );	// Allocate memory for the object
	notch50_init( result );											// Initialize it
	return result;																// Return the result
}

void notch50_destroy( notch50Type *pObject )
{
	free( pObject );
}

 void notch50_init( notch50Type * pThis )
{
	notch50_reset( pThis );

}

 void notch50_reset( notch50Type * pThis )
{
	memset( &pThis->state, 0, sizeof( pThis->state ) ); // Reset state to 0
	pThis->pointer = pThis->state;						// History buffer points to start of state buffer
	pThis->output = 0;									// Reset output

}

 int notch50_filterBlock( notch50Type * pThis, float * pInput, float * pOutput, unsigned int count )
{
	float *pOriginalOutput = pOutput;	// Save original output so we can track the number of samples processed
	float accumulator;

 	for( ;count; --count )
 	{
 		pThis->pointer[notch50_length] = *pInput;						// Copy sample to top of history buffer
 		*(pThis->pointer++) = *(pInput++);										// Copy sample to bottom of history buffer

		if( pThis->pointer >= pThis->state + notch50_length )				// Handle wrap-around
			pThis->pointer -= notch50_length;

		accumulator = 0;
		notch50_dotProduct( pThis->pointer, notch50_coefficients, &accumulator, notch50_length );
		*(pOutput++) = accumulator;	// Store the result
 	}

	return pOutput - pOriginalOutput;

}

 void notch50_dotProduct( float * pInput, float * pKernel, float * pAccumulator, short count )
{
 	float accumulator = *pAccumulator;
	while( count-- )
		accumulator += ((float)*(pKernel++)) * *(pInput++);
	*pAccumulator = accumulator;

}
