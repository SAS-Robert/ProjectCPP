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

// Begin header file, E100.h

#ifndef E100_H_ // Include guards
#define E100_H_

/*
Generated code is based on the following filter design:
<micro.DSP.FilterDocument sampleFrequency="#1000" arithmetic="float" biquads="Direct1" classname="E100" inputMax="#1" inputShift="#15" >
  <micro.DSP.IirEllipticFilter N="#1" bandType="s" warp="#true" w1="#0.098" w2="#0.102" stopbandRipple="#0.05" passbandRipple="#0.05" transitionRatio="#0.05" >
    <micro.DSP.FilterStructure coefficientBits="#0" variableBits="#0" accumulatorBits="#0" biquads="Direct1" >
      <micro.DSP.FilterSection form="Direct1" historyType="WriteBack" accumulatorBits="#0" variableBits="#0" coefficientBits="#0" />
    </micro.DSP.FilterStructure>
    <micro.DSP.PoleOrZeroContainer >
      <micro.DSP.PoleOrZero i="#0.5876973167299436" r="#0.8090808759996893" isPoint="#true" isPole="#false" isZero="#true" symmetry="c" N="#1" cascade="#0" />
      <micro.DSP.PoleOrZero i="#0.585282644167602" r="#0.8057762248385318" isPoint="#true" isPole="#true" isZero="#false" symmetry="c" N="#1" cascade="#0" />
    </micro.DSP.PoleOrZeroContainer>
    <micro.DSP.GenericC.CodeGenerator generateTestCases="#false" />
    <micro.DSP.GainControl magnitude="#1" frequency="#0" peak="#true" />
  </micro.DSP.IirEllipticFilter>
</micro.DSP.FilterDocument>

*/

static const int E100_numStages = 1;
static const int E100_coefficientLength = 5;
extern float E100_coefficients[5];

typedef struct
{
	float state[4];
	float output;
} E100Type;

typedef struct
{
	float *pInput;
	float *pOutput;
	float *pState;
	float *pCoefficients;
	short count;
} E100_executionState;


E100Type *E100_create( void );
void E100_destroy( E100Type *pObject );
 void E100_init( E100Type * pThis );
 void E100_reset( E100Type * pThis );
#define E100_writeInput( pThis, input )  \
	E100_filterBlock( pThis, &(input), &(pThis)->output, 1 );

#define E100_readOutput( pThis )  \
	(pThis)->output

 int E100_filterBlock( E100Type * pThis, float * pInput, float * pOutput, unsigned int count );
#define E100_outputToFloat( output )  \
	(output)

#define E100_inputFromFloat( input )  \
	(input)

 void E100_filterBiquad( E100_executionState * pExecState );
#endif // E100_H_
