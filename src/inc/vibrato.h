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
#include <iostream>
#include "MUSI8903Config.h"


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
    
    void convertTimeToSamples(float paramValue, int& param);
    
    
public:
    //initialisation and reset functions
    Error_t init(float modFreq, float delayWidthInSecs, float modAmpSecs);
    Error_t reset();
    
    static Error_t create(CVibrato*& pCKortIf, long int sample_rate, int num_channels);
    static Error_t destroy(CVibrato*& pCKortIf);
    
    
    //set parameter functions
    Error_t setDelayWidth(float delay_width_secs);
    void setModAmp(float modAmpSecs);
    void setModFreq(float modFreq);
    
    //get parameter functions
    void getDelayWidth(int& delayWidth);
    void getModAmp(int& modAmp);
    void getModFreq(float& modFreq);
    
    //process function
    Error_t process(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames);
};

CVibrato::CVibrato(float maxwidth_secs, int num_channels, long int sample_rate)
{
    CRingBuffer<float> **fRingBuff;
    
    ppcLFO = new LFO_Niu(0.F);
    
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
void CVibrato::convertTimeToSamples(float paramValue, int& param)
{
    param = static_cast<int>(round(paramValue * sampleRate));
}

//set the modAmp and check error
void CVibrato::setModAmp(float delay_width_secs)
{
    int temp = 0;
    convertTimeToSamples(delay_width_secs, temp);
    
    
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
}


//set the modFreq
void CVibrato::setModFreq(float thisModFreq)
{
    
    LFOFreq = thisModFreq;
    ppcLFO->setFreq(LFOFreq);
}

void CVibrato::getDelayWidth(int &delayWidth)
{
    delayWidth = width;
}

void CVibrato::getModAmp(int &modAmp)
{
    modAmp = LFOAmp;
}

void CVibrato::getModFreq(float &modFreq)
{
    modFreq = LFOFreq;
}

//Error_t CVibrato::process(float **input_buffer, float **outputBuffer, int numberOfFrames)
//{
//    float temp = 0;
//    double tap = 0;
//    
//    for (int i = 0; i < numberOfFrames; i++)
//    {
//        for (int j = 0; j < numChannels; j++)
//        {
//            fRingBuff[j]->putPostInc(input_buffer[j][i]);
//            temp = fRingBuff[j]->getPostInc();
//            float time = processedSamples / static_cast<float>(sampleRate);
//            tap = 1 + width + LFOAmp * ppcLFO->getSine(time);
//            tap = fRingBuff[j]->getWriteIdx() - tap - fRingBuff[j]->getReadIdx();
//            outputBuffer[j][i] = fRingBuff[j]->get(tap);
//        }
//        processedSamples++;
//    }
//    return kNoError;
//}


Error_t CVibrato::process (float **input_buffer, float **output_buffer, int number_of_frames)
{
    float temp = 0;
    double tap = 0;
    
    for (int data_id = 0; data_id < number_of_frames; data_id++)
    {
        for (int channel_id = 0; channel_id < numChannels; channel_id++)
        {
            fRingBuff[channel_id]->putPostInc(input_buffer[channel_id][data_id]);
            temp = fRingBuff[channel_id]->getPostInc();
            double time = processedSamples/static_cast<double>(sampleRate);
            tap  = 1 + width + LFOAmp * ppcLFO->getSine(time);
            tap = fRingBuff[channel_id]->getWriteIdx() - tap - fRingBuff[channel_id]->getReadIdx();
            output_buffer[channel_id][data_id] = fRingBuff[channel_id]->get(tap);
        }
        processedSamples++;
    }
    return kNoError;
}



#endif /* Vibrato_h */
