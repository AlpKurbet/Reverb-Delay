/*
  ==============================================================================

    Delay.h
    Created: 17 Mar 2023 11:00:58pm
    Author:  Alperen Kurbetci

  ==============================================================================
*/

#pragma once

#include "./Parameters/Global.h"
#include <JuceHeader.h>
#include "DelayLine.h"



class Delayer
{
public:
    
    Delayer();
    ~Delayer();
    
    //Default functions
    void prepare(juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);
    void delayReset();
    void delayUpdate();
    
    //For interpolation
    float linearInterp(const float& y0, const float& yp1, const float& frac);
    

    
    
    // To get the parameters from TreeState
    void setDelayTime(float newTime);
    void setMix(float newWet);
    void setFeedBack(float newFB);
    
private:
    
    
    //Initial state of SR
    float sampleRate = 44100.0f;
    
    //To handle from the parameters from TreeState
    juce::SmoothedValue<float> _time;
    juce::SmoothedValue<float> _wetness;
    juce::SmoothedValue<float> _FB;
    
    
    //User inputs for stereo
    std::array<juce::SmoothedValue<float>, 2> _timeStereo;
    std::array<juce::SmoothedValue<float>, 2> _wetnessStereo;
    std::array<juce::SmoothedValue<float>, 2> _FBStereo;
    std::array<DelayLine   <float>, 2> delayLines;
    
    
    //Variables for the DelayLine During DSP
    float lastTime = 0.f;
    int samplesPerBlock = 512;
    int delayBufferLen = 0;
    int numChannels = 2;
    

};


