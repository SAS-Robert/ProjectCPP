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

#include "notch100.h"

#include <stdlib.h> // For malloc/free
#include <string.h> // For memset

float notch100_coefficients[24] =
{
	0.0000000, 0.0000000, 0.0000000, -0.22746499, 0.0029429635, -0.0097635888,
	0.015294211, 0.025938695, 0.024790765, 0.031471370, 0.024032562, -0.0013859819,
	-0.016296117, 0.98236672, -0.016296117, -0.0013859819, 0.024032562, 0.031471370,
	0.024790765, 0.025938695, 0.015294211, -0.0097635888, 0.0029429635, -0.22746499
};


notch100Type *notch100_create( void )
{
	notch100Type *result = (notch100Type *)malloc( sizeof( notch100Type ) );	// Allocate memory for the object
	notch100_init( result );											// Initialize it
	return result;																// Return the result
}

void notch100_destroy( notch100Type *pObject )
{
	free( pObject );
}

 void notch100_init( notch100Type * pThis )
{
	notch100_reset( pThis );

}

 void notch100_reset( notch100Type * pThis )
{
	memset( &pThis->state, 0, sizeof( pThis->state ) ); // Reset state to 0
	pThis->pointer = pThis->state;						// History buffer points to start of state buffer
	pThis->output = 0;									// Reset output

}

 int notch100_filterBlock( notch100Type * pThis, float * pInput, float * pOutput, unsigned int count )
{
	float *pOriginalOutput = pOutput;	// Save original output so we can track the number of samples processed
	float accumulator;

 	for( ;count; --count )
 	{
 		pThis->pointer[notch100_length] = *pInput;						// Copy sample to top of history buffer
 		*(pThis->pointer++) = *(pInput++);										// Copy sample to bottom of history buffer

		if( pThis->pointer >= pThis->state + notch100_length )				// Handle wrap-around
			pThis->pointer -= notch100_length;

		accumulator = 0;
		notch100_dotProduct( pThis->pointer, notch100_coefficients, &accumulator, notch100_length );
		*(pOutput++) = accumulator;	// Store the result
 	}

	return pOutput - pOriginalOutput;

}

 void notch100_dotProduct( float * pInput, float * pKernel, float * pAccumulator, short count )
{
 	float accumulator = *pAccumulator;
	while( count-- )
		accumulator += ((float)*(pKernel++)) * *(pInput++);
	*pAccumulator = accumulator;

}
