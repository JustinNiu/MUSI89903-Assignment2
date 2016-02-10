//
//  vibrato.h
//  MUSI8903
//
//  Created by Siyuan Niu on 2/9/16.
//
//

#ifndef vibrato_h
#define vibrato_h

#include <stdio.h>
#include "RingBuffer.h"
#include "LFO.h"
#include "ErrorDef.h"

class vibrato{
    
    CRingBuffer<float> **fRingDelayLine;
    LFO_Niu* ppcSine;
    float freq;
    int width;
    int delay;
    int numVar;
    
    
public:
    void process(float **ppfInBuff,float **ppfOutbuff, int iNumOfFrames);
    
    
};


//class Vibrato{
//private:
//    CRingBuffer<float> **ppfRingBuff;
//    LFO ppcLFO;
//    
//    enum VibratoParam {
//        kModFreq,
//        kWidth,
//        kDelay,
//        kNumParams
//    };
//    
//    float fVibParam[kNumParams];//ModFreq,Width,
//    float fVibParamRange[kNumParams][3];
//    float SamplingRate;
//    int iNumChannels;   //Width in seconds,ModFreq in Hz
//    int iFramesProcessed;
//    
//public:
//    void process(float **ppfInBuff,float **ppfOutbuff, int iNumOfFrames);
//    
//    void setVibratoParam(VibratoParam eVibParam, float fParamVal);
//    float getVibratoParam(VibratoParam eVibParam) const;
//    
//    
//protected:
//    Vibrato(float fVParam[3], int UserNumChannels,float UserSamplingRate, int iMaxDelayInSec);
//    virtual ~Vibrato();
//};


#endif /* vibrato_h */


