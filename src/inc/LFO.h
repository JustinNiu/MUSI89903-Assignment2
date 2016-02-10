//
//  LFO.h
//  MUSI8903
//
//  Created by Siyuan Niu on 2/10/16.
//
//

#ifndef LFO_h
#define LFO_h

#define PI 3.14159

//declare the LFO class

class LFO_Niu{
    
    float freq;
//    float amp;
    
public:
    
    LFO_Niu();
    ~LFO_Niu(){
        freq = 0;
    }
    void setFreq( float);
//    void setAmp();
    
    float getSine( float);
};



void LFO_Niu::setFreq(float thisFreq){
    freq = thisFreq;
//    amp = thisAmp;
}

float LFO_Niu::getSine(float TimeInSec){
    return sin(2 * PI *freq * TimeInSec);
}

#endif /* LFO_h */