/*
  ==============================================================================

    Delay.cpp
    Created: 17 Mar 2023 11:00:52pm
    Author:  Alperen Kurbetci

  ==============================================================================
*/

#include "Delay.h"


Delayer::Delayer()
{
    // Emtyp constructor
}

Delayer::~Delayer()
{
    // Emtyp destructor
}

//Preparing for DSP
//Clearing the data pointer before any action
void Delayer::prepare(juce::dsp::ProcessSpec &spec)
{
   
    sampleRate = spec.sampleRate;
    samplesPerBlock = spec.maximumBlockSize;
    numChannels = spec.numChannels;
    delayBufferLen = 2 * (sampleRate + samplesPerBlock);

    // Prepare delay lines
    
    for (auto& dline : delayLines)
    {
        dline.delayResize(delayBufferLen);
        dline.delayClear();
    }
    
    //Setting up the parameters
    auto FeedB = _FB.getNextValue() / 100.f;
    auto WetScale = _wetness.getNextValue() / 100.f;
    auto Mix = 1 - WetScale;
    lastTime = _time.getNextValue();
    
    
    //Getting the variables into the channel
    for (int channel = 0; channel < 2; ++channel)
    {
        _timeStereo[channel].reset(samplesPerBlock);
        _wetnessStereo[channel].reset(samplesPerBlock);
        _timeStereo[channel].reset(samplesPerBlock);

        _timeStereo[channel].setCurrentAndTargetValue(FeedB);
        _wetnessStereo[channel].setCurrentAndTargetValue(WetScale);
        _timeStereo[channel].setCurrentAndTargetValue(Mix);
    }
}

//Reseting the parameters
void Delayer::delayReset()
{
    if(sampleRate <= 0) return;
    
    _time.reset(sampleRate, 0.02);
    _time.setTargetValue(0.0);
    
    _wetness.reset(sampleRate, 0.02);
    _wetness.setTargetValue(1.0);
    
    _FB.reset(sampleRate, 0.02);
    _FB.setTargetValue(0.0);
}


//Coppied from juce_Compressor.h

void Delayer::process(juce::AudioBuffer<float>& buffer)
{
    float theTime = _time.getNextValue();
    delayUpdate();

    // delay in fraction and integer
    float delayInSamplesFrac = sampleRate * (theTime / 1000.f);
    int delayInSamplesInt = static_cast<int> (delayInSamplesFrac);
    float frac = delayInSamplesFrac - delayInSamplesInt;

    // Add delay into buffer
    for (auto channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        float FeedB = _timeStereo[channel].getNextValue();
        float WetScale  = _wetnessStereo [channel].getNextValue();
        float Mix  = _timeStereo [channel].getNextValue();
      
        const float* input = buffer.getReadPointer(channel);
        float* output = buffer.getWritePointer(channel);

        for (auto sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float delayedSample = 0.f;

            // Fractal delay with liner interpolation if needed
            if (frac != 0)
            {
                float y0  = delayLines[channel].delayGet(delayInSamplesInt);
                float ym1 = delayLines[channel].delayGet(delayInSamplesInt + 1);
                delayedSample = linearInterp(y0, ym1, frac);
            }
            else
            {
                delayedSample = delayLines[channel].delayGet(delayInSamplesInt);
            }

            auto inputSample = input[sample];
            delayLines[channel].delayPush(inputSample + delayedSample * FeedB);
            //Mixing Outputs
            output[sample] = inputSample * Mix + delayedSample * WetScale;
        }
    }
    
}


//Updating the delay line
void Delayer::delayUpdate()
{
    auto FeedB   = _FB.getNextValue();
    auto WetScale    = _wetness.getNextValue();
    auto Mix    = 1 - WetScale;
    
    auto theTime = _time.getNextValue();

    // Clear delay buffers is time is changed and play only input
    // as otherwise will hear crackle
    if (theTime != lastTime)
    {
        WetScale = 0;
        Mix = 1;
       
        for (auto& dline : delayLines)
        {
            
            dline.delayClear();
        }
    
    }
    lastTime = theTime;

    //Feeding the variables into the channel
    for (int channel = 0; channel < 2; ++channel)
    {
        _timeStereo[channel].setCurrentAndTargetValue(FeedB);
        _wetnessStereo[channel].setCurrentAndTargetValue(WetScale);
        _timeStereo[channel].setCurrentAndTargetValue(Mix);
    }
}




//==============================================================================
// Linear interpolation used with fractal delays

float Delayer::linearInterp(const float& initialY, const float& lastY, const float& frac)
{
    return lastY * frac + initialY * (1 - frac);
}




//Setting the parameters
void Delayer::setDelayTime(float newTime)
{
    _time.setTargetValue(newTime);
}


void Delayer::setMix(float newWet)
{
    _wetness.setTargetValue(newWet);
}


void Delayer::setFeedBack(float newFB)
{
    _FB.setTargetValue(newFB);
}

