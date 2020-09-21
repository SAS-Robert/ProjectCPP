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

// Begin header file, bandStop.h

#ifndef BANDSTOP_H_ // Include guards
#define BANDSTOP_H_

static const int bandStop_length = 24;
extern float bandStop_coefficients[24];

typedef struct
{
	float* pointer;
	float state[48];
	float output;
} bandStopType;


bandStopType* bandStop_create(void);
void bandStop_destroy(bandStopType* pObject);
void bandStop_init(bandStopType* pThis);
void bandStop_reset(bandStopType* pThis);
#define bandStop_writeInput( pThis, input )  \
	bandStop_filterBlock( pThis, &(input), &(pThis)->output, 1 );

#define bandStop_readOutput( pThis )  \
	(pThis)->output

int bandStop_filterBlock(bandStopType* pThis, float* pInput, float* pOutput, unsigned int count);
#define bandStop_outputToFloat( output )  \
	(output)

#define bandStop_inputFromFloat( input )  \
	(input)

void bandStop_dotProduct(float* pInput, float* pKernel, float* pAccumulator, short count);
#endif // BANDSTOP_H_

