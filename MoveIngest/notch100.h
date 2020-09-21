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

// Begin header file, notch100.h

#ifndef NOTCH100_H_ // Include guards
#define NOTCH100_H_

/*
Generated code is based on the following filter design:
<micro.DSP.FilterDocument sampleFrequency="#1" arithmetic="float" biquads="Direct1" classname="notch100" inputMax="#1" inputShift="#15" >
  <micro.DSP.ParksMcClellanDesigner N="#21" firtype="2" symmetry="+" evenodd="#1" >
    <micro.DSP.FilterStructure coefficientBits="#0" variableBits="#0" accumulatorBits="#0" biquads="Direct1" >
      <micro.DSP.FilterSection form="Fir" historyType="Double" accumulatorBits="#0" variableBits="#0" coefficientBits="#0" />
    </micro.DSP.FilterStructure>
    <micro.DSP.PoleOrZeroContainer >
      <micro.DSP.ReciprocalZero i="#0" r="#0.8459456151750874" isPoint="#true" isPole="#false" isZero="#true" symmetry="i" N="#1" cascade="#0" />
      <micro.DSP.ReciprocalZero i="#0.5261476554239161" r="#0.7247159431603736" isPoint="#true" isPole="#false" isZero="#true" symmetry="i" N="#1" cascade="#0" />
      <micro.DSP.ReciprocalZero i="#0.8224778876369285" r="#0.264918048474805" isPoint="#true" isPole="#false" isZero="#true" symmetry="i" N="#1" cascade="#0" />
      <micro.DSP.ReciprocalZero i="#0.8183888890353047" r="#-0.26254917134074907" isPoint="#true" isPole="#false" isZero="#true" symmetry="i" N="#1" cascade="#0" />
      <micro.DSP.ReciprocalZero i="#0.5149843293108101" r="#-0.7021938249514348" isPoint="#true" isPole="#false" isZero="#true" symmetry="i" N="#1" cascade="#0" />
      <micro.DSP.ReciprocalZero i="#0" r="#-0.8720281373342077" isPoint="#true" isPole="#false" isZero="#true" symmetry="i" N="#1" cascade="#0" />
    </micro.DSP.PoleOrZeroContainer>
    <micro.DSP.GenericC.CodeGenerator generateTestCases="#false" />
    <micro.DSP.BandStopSpecification bandType="s" N="#27" f1="#0.097" f2="#0.10299999999999998" stopRipple="#0.051000000000000004" passRipple="#0.06900000000000003" transition="#0.019000000000000003" passGain="#1" stopGain="#0" >
      <micro.DSP.ControlPoint start="#0" ripple="#0.06900000000000003" gain="#1" interpolation="linear" />
      <micro.DSP.ControlPoint start="#0.078" ripple="#0" gain="#1" interpolation="cosine" />
      <micro.DSP.ControlPoint start="#0.097" ripple="#0.051000000000000004" gain="#0" interpolation="linear" />
      <micro.DSP.ControlPoint start="#0.10299999999999998" ripple="#0" gain="#0" interpolation="cosine" />
      <micro.DSP.ControlPoint start="#0.12199999999999998" ripple="#0.06900000000000003" gain="#1" interpolation="linear" />
      <micro.DSP.ControlPoint start="#0.5" ripple="#0" gain="#1" interpolation="linear" />
    </micro.DSP.BandStopSpecification>
  </micro.DSP.ParksMcClellanDesigner>
</micro.DSP.FilterDocument>

*/

static const int notch100_length = 24;
extern float notch100_coefficients[24];

typedef struct
{
	float * pointer;
	float state[48];
	float output;
} notch100Type;


notch100Type *notch100_create( void );
void notch100_destroy( notch100Type *pObject );
 void notch100_init( notch100Type * pThis );
 void notch100_reset( notch100Type * pThis );
#define notch100_writeInput( pThis, input )  \
	notch100_filterBlock( pThis, &(input), &(pThis)->output, 1 );

#define notch100_readOutput( pThis )  \
	(pThis)->output

 int notch100_filterBlock( notch100Type * pThis, float * pInput, float * pOutput, unsigned int count );
#define notch100_outputToFloat( output )  \
	(output)

#define notch100_inputFromFloat( input )  \
	(input)

 void notch100_dotProduct( float * pInput, float * pKernel, float * pAccumulator, short count );
#endif // NOTCH100_H_
