//
//  vibrato.h
//  MUSI8903
//
//  Created by Hua on 10/02/2016
//
//

#ifndef vibrato_h
#define vibrato_h

#include <stdio.h>
#include "RingBuffer.h"
#include "LFO.h"
#include "ErrorDef.h"

class CVibrato{

private:

    CRingBuffer<float> **fRingBuff;
    LFO_Niu* ppcLFO;
	CVibrato(float maxwidth_secs, int num_channels, long int sample_rate);
	virtual ~CVibrato();

	float LFOFreq;
	int width;
	int entireDelay;
	int LFOAmp;
	long int sampleRate;
	int numChannels;
	long long int processedSamples;

	Error_t convertTimeToSamples(float paramValue, int& param);
    
    
public:
	//initialisation and reset functions
	Error_t init(float modFreq, float delayWidthInSecs, float modAmpSecs);
	Error_t reset();

	static Error_t create(CVibrato*& pCKortIf, long int sample_rate, int num_channels);
	static Error_t destroy(CVibrato*& pCKortIf);


	//set parameter functions
	//Error_t setDelayWidth(float delay_width_secs);
	Error_t setModAmp(float modAmpSecs);
	Error_t setModFreq(float modFreq);

	//get parameter functions
	Error_t getDelayWidth(int& delayWidth);
	Error_t getModAmp(int& modAmp);
	Error_t getModFreq(float& modFreq);

	//process function
	Error_t process(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames);    
};

#endif /* Vibrato_h */
