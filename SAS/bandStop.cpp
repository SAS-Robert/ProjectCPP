/******************************* SOURCE LICENSE *********************************
Copyright (c) 2019 MicroModeler.

A non-exclusive, nontransferable, perpetual, royalty-free license is granted to the Licensee to
use the following Information for academic, non-profit, or government-sponsored research purposes.
Use of the following Information under this License is restricted to NON-COMMERCIAL PURPOSES ONLY.
Commercial use of the following Information requires a separately executed written license agreement.

This Information is distributed WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

******************************* END OF LICENSE *********************************/

// A commercial license for MicroModeler DSP can be obtained at http://www.micromodeler.com/launch.jsp

#include "bandStop.h"

#include <stdlib.h> // For malloc/free
#include <string.h> // For memset

float bandStop_coefficients[24] =
{
	0.0000000, 0.0000000, 0.0000000, -0.28356239, 0.022875806, 0.0024638786,
	0.031303166, 0.048535634, 0.061353804, 0.095377349, 0.12114225, 0.12565384,
	0.13479592, 0.14379263, 0.13479592, 0.12565384, 0.12114225, 0.095377349,
	0.061353804, 0.048535634, 0.031303166, 0.0024638786, 0.022875806, -0.28356239
};


bandStopType* bandStop_create(void)
{
	bandStopType* result = (bandStopType*)malloc(sizeof(bandStopType));	// Allocate memory for the object
	bandStop_init(result);											// Initialize it
	return result;																// Return the result
}

void bandStop_destroy(bandStopType* pObject)
{
	free(pObject);
}

void bandStop_init(bandStopType* pThis)
{
	bandStop_reset(pThis);

}

void bandStop_reset(bandStopType* pThis)
{
	memset(&pThis->state, 0, sizeof(pThis->state)); // Reset state to 0
	pThis->pointer = pThis->state;						// History buffer points to start of state buffer
	pThis->output = 0;									// Reset output

}

int bandStop_filterBlock(bandStopType* pThis, float* pInput, float* pOutput, unsigned int count)
{
	float* pOriginalOutput = pOutput;	// Save original output so we can track the number of samples processed
	float accumulator;

	for (; count; --count)
	{
		pThis->pointer[bandStop_length] = *pInput;						// Copy sample to top of history buffer
		*(pThis->pointer++) = *(pInput++);										// Copy sample to bottom of history buffer

		if (pThis->pointer >= pThis->state + bandStop_length)				// Handle wrap-around
			pThis->pointer -= bandStop_length;

		accumulator = 0;
		bandStop_dotProduct(pThis->pointer, bandStop_coefficients, &accumulator, bandStop_length);
		*(pOutput++) = accumulator;	// Store the result
	}

	return pOutput - pOriginalOutput;

}

void bandStop_dotProduct(float* pInput, float* pKernel, float* pAccumulator, short count)
{
	float accumulator = *pAccumulator;
	while (count--)
		accumulator += ((float)*(pKernel++)) * *(pInput++);
	*pAccumulator = accumulator;

}


