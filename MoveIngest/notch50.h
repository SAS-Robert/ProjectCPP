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

// Begin header file, notch50.h

#ifndef NOTCH50_H_ // Include guards
#define NOTCH50_H_

/*
Generated code is based on the following filter design:
<micro.DSP.FilterDocument sampleFrequency="#1" arithmetic="float" biquads="Direct1" classname="notch50" inputMax="#1" inputShift="#15" >
  <micro.DSP.ParksMcClellanDesigner N="#21" firtype="+" symmetry="+" evenodd="#0" >
    <micro.DSP.FilterStructure coefficientBits="#0" variableBits="#0" accumulatorBits="#0" biquads="Direct1" >
      <micro.DSP.FilterSection form="Fir" historyType="Double" accumulatorBits="#0" variableBits="#0" coefficientBits="#0" />
    </micro.DSP.FilterStructure>
    <micro.DSP.PoleOrZeroContainer >
      <micro.DSP.ReciprocalZero i="#0.26960037911894014" r="#0.8576866683132702" isPoint="#true" isPole="#false" isZero="#true" symmetry="i" N="#1" cascade="#0" />
      <micro.DSP.ReciprocalZero i="#0.6853908963954424" r="#0.4888697598451247" isPoint="#true" isPole="#false" isZero="#true" symmetry="i" N="#1" cascade="#0" />
      <micro.DSP.ReciprocalZero i="#0.8441109581081048" r="#-0.001889684114326545" isPoint="#true" isPole="#false" isZero="#true" symmetry="i" N="#1" cascade="#0" />
      <micro.DSP.ReciprocalZero i="#0.6896469558442839" r="#-0.4945319566274209" isPoint="#true" isPole="#false" isZero="#true" symmetry="i" N="#1" cascade="#0" />
      <micro.DSP.ReciprocalZero i="#0.2649796467946514" r="#-0.8103381936921593" isPoint="#true" isPole="#false" isZero="#true" symmetry="i" N="#1" cascade="#0" />
    </micro.DSP.PoleOrZeroContainer>
    <micro.DSP.GenericC.CodeGenerator generateTestCases="#false" />
    <micro.DSP.BandStopSpecification bandType="s" N="#27" f1="#0.047000000000000014" f2="#0.053" stopRipple="#0.051000000000000004" passRipple="#0.06900000000000003" transition="#0.019000000000000003" passGain="#1" stopGain="#0" >
      <micro.DSP.ControlPoint start="#0" ripple="#0.06900000000000003" gain="#1" interpolation="linear" />
      <micro.DSP.ControlPoint start="#0.02800000000000001" ripple="#0.3" gain="#1" interpolation="cosine" />
      <micro.DSP.ControlPoint start="#0.047000000000000014" ripple="#0.051000000000000004" gain="#0" interpolation="linear" />
      <micro.DSP.ControlPoint start="#0.053" ripple="#0" gain="#0" interpolation="cosine" />
      <micro.DSP.ControlPoint start="#0.07200000000000001" ripple="#0.06900000000000003" gain="#1" interpolation="linear" />
      <micro.DSP.ControlPoint start="#0.5" ripple="#0" gain="#1" interpolation="linear" />
    </micro.DSP.BandStopSpecification>
  </micro.DSP.ParksMcClellanDesigner>
</micro.DSP.FilterDocument>

*/

static const int notch50_length = 24;
extern float notch50_coefficients[24];

typedef struct
{
	float * pointer;
	float state[48];
	float output;
} notch50Type;


notch50Type *notch50_create( void );
void notch50_destroy( notch50Type *pObject );
 void notch50_init( notch50Type * pThis );
 void notch50_reset( notch50Type * pThis );
#define notch50_writeInput( pThis, input )  \
	notch50_filterBlock( pThis, &(input), &(pThis)->output, 1 );

#define notch50_readOutput( pThis )  \
	(pThis)->output

 int notch50_filterBlock( notch50Type * pThis, float * pInput, float * pOutput, unsigned int count );
#define notch50_outputToFloat( output )  \
	(output)

#define notch50_inputFromFloat( input )  \
	(input)

 void notch50_dotProduct( float * pInput, float * pKernel, float * pAccumulator, short count );
#endif // NOTCH50_H_
