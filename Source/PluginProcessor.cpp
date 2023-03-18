/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DelayAudioProcessor::DelayAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
params(*this, nullptr, "PARAMETERS", createParameterLayout())

#endif
{
    params.addParameterListener(Delay_timeID, this);
    params.addParameterListener(Delay_WetnessID, this);
    params.addParameterListener(Delay_FeedbackID, this);
}

DelayAudioProcessor::~DelayAudioProcessor()
{
    params.removeParameterListener(Delay_timeID, this);
    params.removeParameterListener(Delay_WetnessID, this);
    params.removeParameterListener(Delay_FeedbackID, this);
}

juce::AudioProcessorValueTreeState::ParameterLayout DelayAudioProcessor::createParameterLayout()

{
    std::vector <std::unique_ptr<juce::RangedAudioParameter>> UserParams;
    
    
    
    auto paramTime = std::make_unique<juce::AudioParameterFloat>(Delay_timeID, Delay_timeName, 0.0f, 2000.0f, 500.0f);
    auto paramWet = std::make_unique<juce::AudioParameterFloat>(Delay_WetnessID, Delay_WetnessName, juce::NormalisableRange<float>(0.00, 1.0),
                                                                0.5,
                                                                juce::String(),
                                                                juce::AudioProcessorParameter::genericParameter,
                                                                [](float value, int maxStringLength) {return static_cast<juce::String>(round(value * 100.f * 100.f) / 100.f); },
                                                                [](const juce::String& text) {return text.getFloatValue() / 100.f; });
    auto paramFBack = std::make_unique<juce::AudioParameterFloat>(Delay_FeedbackID, Delay_FeedbackName, juce::NormalisableRange<float>(-0.99, 0.99),
                                                                  0.0,
                                                                  juce::String(),
                                                                  juce::AudioProcessorParameter::genericParameter,
                                                                  [](float value, int maxStringLength) {return static_cast<juce::String>(round(value * 100.f * 100.f) / 100.f); },
                                                                  [](const juce::String& text) {return text.getFloatValue() / 100.f; }
                                                                  );
    
    auto paramDamp = std::make_unique<juce::AudioParameterFloat>(Reverb_DampID, Reverb_DampName, juce::NormalisableRange<float>(-0.99, 0.99),
                                                                  0.0,
                                                                  juce::String(),
                                                                  juce::AudioProcessorParameter::genericParameter,
                                                                  [](float value, int maxStringLength) {return static_cast<juce::String>(round(value * 100.f * 100.f) / 100.f); },
                                                                  [](const juce::String& text) {return text.getFloatValue() / 100.f; }
                                                                  );
    
    auto paramWidth = std::make_unique<juce::AudioParameterFloat>(Reverb_WidthID, Reverb_WidthName, juce::NormalisableRange<float>(-0.99, 0.99),
                                                                  0.0,
                                                                  juce::String(),
                                                                  juce::AudioProcessorParameter::genericParameter,
                                                                  [](float value, int maxStringLength) {return static_cast<juce::String>(round(value * 100.f * 100.f) / 100.f); },
                                                                  [](const juce::String& text) {return text.getFloatValue() / 100.f; }
                                                                  );
    
    auto paramMix = std::make_unique<juce::AudioParameterFloat>(Reverb_MixID, Reverb_MixName, juce::NormalisableRange<float>(-0.99, 0.99),
                                                                  0.0,
                                                                  juce::String(),
                                                                  juce::AudioProcessorParameter::genericParameter,
                                                                  [](float value, int maxStringLength) {return static_cast<juce::String>(round(value * 100.f * 100.f) / 100.f); },
                                                                  [](const juce::String& text) {return text.getFloatValue() / 100.f; }
                                                                  );
    
    auto paramSize = std::make_unique<juce::AudioParameterFloat>(Reverb_SizeID, Reverb_SizeName, juce::NormalisableRange<float>(-0.99, 0.99),
                                                                  0.0,
                                                                  juce::String(),
                                                                  juce::AudioProcessorParameter::genericParameter,
                                                                  [](float value, int maxStringLength) {return static_cast<juce::String>(round(value * 100.f * 100.f) / 100.f); },
                                                                  [](const juce::String& text) {return text.getFloatValue() / 100.f; }
                                                                  );
    
    
    UserParams.push_back(std::move(paramTime));
    UserParams.push_back(std::move(paramWet));
    UserParams.push_back(std::move(paramFBack));
    
    UserParams.push_back(std::move(paramDamp));
    UserParams.push_back(std::move(paramWidth));
    UserParams.push_back(std::move(paramMix));
    UserParams.push_back(std::move(paramSize));
    
    
    return {UserParams.begin(), UserParams.end()};
}

void DelayAudioProcessor::parameterChanged(const juce::String &parameterID, float newValue)

{
    updateParameters();
    
}

void DelayAudioProcessor::updateParameters()
{

    
    reverbParams.roomSize   = (params.getRawParameterValue(Reverb_DampID)->load());
    reverbParams.damping    = (params.getRawParameterValue(Reverb_WidthID)->load());
    reverbParams.width      = (params.getRawParameterValue(Reverb_MixID)->load());
    reverbParams.wetLevel   = (params.getRawParameterValue(Reverb_SizeID)->load());

    myReverb.setParameters (reverbParams);
        
    myDelay.setDelayTime(params.getRawParameterValue(Delay_timeID) -> load());
    myDelay.setMix(params.getRawParameterValue(Delay_WetnessID) -> load());
    myDelay.setFeedBack(params.getRawParameterValue(Delay_FeedbackID) -> load());
    
}

//==============================================================================
const juce::String DelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DelayAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DelayAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DelayAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DelayAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DelayAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DelayAudioProcessor::getProgramName (int index)
{
    return {};
}

void DelayAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();
    
    myReverb.prepare(spec);
    myDelay.prepare(spec);
    
    updateParameters();
    
    
}

void DelayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif


void DelayAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    
    juce::ScopedNoDenormals noDenormals;
    
    const int numSamples = buffer.getNumSamples();
    
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
        juce::dsp::AudioBlock<float> block (buffer);
        juce::dsp::ProcessContextReplacing<float> ctx (block);
        myReverb.process (ctx);
        myDelay.process(buffer);
    
}

//==============================================================================
bool DelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DelayAudioProcessor::createEditor()
{
    return new DelayAudioProcessorEditor (*this);
}

//==============================================================================
void DelayAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DelayAudioProcessor();
}
