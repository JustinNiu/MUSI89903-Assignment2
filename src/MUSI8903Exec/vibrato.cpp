//
//  CVibrato.cpp
//  MUSI8903
//
//  Created by Siyuan Niu on 2/10/16.
//
//
#include <iostream>
#include <stdio.h>
#include "MUSI8903Config.h"
#include "ErrorDef.h"
#include "RingBuffer.h"
#include "LFO.h"
#include "vibrato.h"

using namespace std;


CVibrato::CVibrato(float maxwidth_secs, int num_channels, long int sample_rate)
{
	CRingBuffer<float> **fRingBuff;
	LFO_Niu* ppcLFO;
	sampleRate = sample_rate;
	numChannels = num_channels;
	processedSamples = 0;
	convertTimeToSamples(maxwidth_secs, entireDelay);
	ppcLFO = new LFO_Niu(0.F);
	fRingBuff = new CRingBuffer<float>*[num_channels];

	for (int i = 0; i < numChannels; i++) {
		fRingBuff[i] = new CRingBuffer<float>(entireDelay);
	}
	reset();
}


CVibrato::~CVibrato() {}




 Error_t CVibrato::create(CVibrato*& pCVibrato, long int sample_rate, int num_channels)
{
	float maxwidth_sec = 1.0F;
	pCVibrato = new CVibrato(maxwidth_sec, num_channels, sample_rate);

	if (!pCVibrato)
		return kUnknownError;

	return kNoError;
}

Error_t CVibrato::destroy(CVibrato*& pCVibrato)
{
	if (!pCVibrato)
		return kUnknownError;

	pCVibrato->reset();

	delete pCVibrato->ppcLFO;
	for (int i = 0; i < pCVibrato->numChannels; i++) {
		delete pCVibrato->fRingBuff[i];
	}
	delete[] pCVibrato->fRingBuff;
	delete pCVibrato;
	pCVibrato = 0;

	return kNoError;
}

Error_t CVibrato::init(float thisModFreq, float thisDelayWidthInSecs, float thisModAmpSecs)
{
	reset();

	// initialise the parameters
	LFOFreq = thisModFreq;
	convertTimeToSamples(thisDelayWidthInSecs, width);
	convertTimeToSamples(thisModAmpSecs, LFOAmp);
	ppcLFO->setFreq(thisModFreq);

	// parameter validity check
    if (LFOAmp > width)
        return kFunctionInvalidArgsError;
    
	if (width * 2 > entireDelay)
		return kFunctionInvalidArgsError;

	if (LFOAmp < 0 || width < 0)
		return kFunctionInvalidArgsError;

	if (width + LFOAmp > entireDelay)
		return kFunctionInvalidArgsError;

	// set ring buffer
	int delay_length = width + LFOAmp;
	for (int i = 0; i < numChannels; i++) {

		fRingBuff[i]->setWriteIdx(fRingBuff[i]->getReadIdx() + delay_length + 1);
	}

	return kNoError;
}

Error_t CVibrato::reset()
{
	// reset buffers and variables to default values
	LFOFreq = 0.F;
	width = 0;
	LFOAmp = 0;
	processedSamples = 0;
	ppcLFO->setFreq(0);
	for (int i = 0; i < numChannels; i++) {
		fRingBuff[i]->reset();
	}
	return kNoError;
}


//convert the timeInSec to timeInSamples
Error_t CVibrato::convertTimeToSamples(float paramValue, int& param)
{
	param = static_cast<int>(round(paramValue * sampleRate));
	return kNoError;
}

//set the modAmp and check error
Error_t CVibrato::setModAmp(float delay_width_secs)
{
	int temp = 0;
	convertTimeToSamples(delay_width_secs, temp);
	if (temp > width || temp < 0)
		return kFunctionInvalidArgsError;

	int num_additional_taps = temp - LFOAmp;
	if (num_additional_taps < 0)
	{
		for (int i = 0; i < numChannels; i++)
		{
			fRingBuff[i]->setWriteIdx(temp + LFOAmp + fRingBuff[i]->getReadIdx() + 1);
		}
	}
	else
	{

		for (int i = 0; i < numChannels; i++)
		{
			for (int j = 0; j < num_additional_taps; j++)
			{
				fRingBuff[i]->putPostInc(0.F);
			}
		}
	}

	LFOAmp = temp;
	return kNoError;
}


//set the modFreq
Error_t CVibrato::setModFreq(float mod_freq)
{
	if (mod_freq < 0)
		return kFunctionInvalidArgsError;
	LFOFreq = mod_freq;
	ppcLFO->setFreq(LFOFreq);
	return kNoError;
}

Error_t CVibrato::getDelayWidth(int &delay_width)
{
	delay_width = width;
	return kNoError;
}

Error_t CVibrato::getModAmp(int &mod_amp)
{
	mod_amp = LFOAmp;
	return kNoError;
}

Error_t CVibrato::getModFreq(float &mod_freq)
{
	mod_freq = LFOFreq;
	return kNoError;
}

Error_t CVibrato::process(float **input_buffer, float **output_buffer, int number_of_frames)
{
	float temp = 0;
	double tap = 0;

	for (int data_id = 0; data_id < number_of_frames; data_id++)
	{
		for (int channel_id = 0; channel_id < numChannels; channel_id++)
		{
			fRingBuff[channel_id]->putPostInc(input_buffer[channel_id][data_id]);
			//cout << "Write Index: " << fRingBuff[channel_id]->getWriteIdx() << endl;
			temp = fRingBuff[channel_id]->getPostInc();
			//cout << "Read Index: " << fRingBuff[channel_id]->getReadIdx() << endl;
			float time = processedSamples / static_cast<float>(sampleRate);
			tap = 1 + width + LFOAmp * ppcLFO->getSine(time);
			tap = fRingBuff[channel_id]->getWriteIdx() - tap - fRingBuff[channel_id]->getReadIdx();
			output_buffer[channel_id][data_id] = fRingBuff[channel_id]->get(tap);
			//cout << output_buffer[channel_id][data_id]<< " " << tap <<endl;
		}
		processedSamples++;
	}
	return kNoError;
}


