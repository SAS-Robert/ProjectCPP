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

// Begin header file, E50.h

#ifndef E50_H_ // Include guards
#define E50_H_

/*
Generated code is based on the following filter design:
<micro.DSP.FilterDocument sampleFrequency="#1000" arithmetic="float" biquads="Direct1" classname="E50" inputMax="#1" inputShift="#15" >
  <micro.DSP.IirEllipticFilter N="#2" bandType="s" warp="#true" w1="#0.048" w2="#0.052" stopbandRipple="#0.05" passbandRipple="#0.05" transitionRatio="#0.25239924860212376" >
	<micro.DSP.FilterStructure coefficientBits="#0" variableBits="#0" accumulatorBits="#0" biquads="Direct1" >
	  <micro.DSP.FilterSection form="Direct1" historyType="WriteBack" accumulatorBits="#0" variableBits="#0" coefficientBits="#0" />
	  <micro.DSP.FilterSection form="Direct1" historyType="WriteBack" accumulatorBits="#0" variableBits="#0" coefficientBits="#0" />
	</micro.DSP.FilterStructure>
	<micro.DSP.PoleOrZeroContainer >
	  <micro.DSP.PoleOrZero i="#0.3066417319688929" r="#0.9518250092402056" isPoint="#true" isPole="#false" isZero="#true" symmetry="c" N="#1" cascade="#1" />
	  <micro.DSP.PoleOrZero i="#0.31094321494771293" r="#0.9504284913016763" isPoint="#true" isPole="#false" isZero="#true" symmetry="c" N="#1" cascade="#0" />
	  <micro.DSP.PoleOrZero i="#0.29947105808724583" r="#0.9484211361132452" isPoint="#true" isPole="#true" isZero="#false" symmetry="c" N="#1" cascade="#1" />
	  <micro.DSP.PoleOrZero i="#0.31474559696758236" r="#0.9431697537613467" isPoint="#true" isPole="#true" isZero="#false" symmetry="c" N="#1" cascade="#0" />
	</micro.DSP.PoleOrZeroContainer>
	<micro.DSP.GenericC.CodeGenerator generateTestCases="#false" />
	<micro.DSP.GainControl magnitude="#1" frequency="#0.052734375" peak="#true" />
  </micro.DSP.IirEllipticFilter>
</micro.DSP.FilterDocument>

*/

static const int E50_numStages = 2;
static const int E50_coefficientLength = 10;
extern float E50_coefficients[10];

typedef struct
{
	float state[8];
	float output;
} E50Type;

typedef struct
{
	float* pInput;
	float* pOutput;
	float* pState;
	float* pCoefficients;
	short count;
} E50_executionState;


E50Type* E50_create(void);
void E50_destroy(E50Type* pObject);
void E50_init(E50Type* pThis);
void E50_reset(E50Type* pThis);
#define E50_writeInput( pThis, input )  \
	E50_filterBlock( pThis, &(input), &(pThis)->output, 1 );

#define E50_readOutput( pThis )  \
	(pThis)->output

int E50_filterBlock(E50Type* pThis, float* pInput, float* pOutput, unsigned int count);
#define E50_outputToFloat( output )  \
	(output)

#define E50_inputFromFloat( input )  \
	(input)

void E50_filterBiquad(E50_executionState* pExecState);
#endif // E50_H_

