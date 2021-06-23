/* Copyright 2021 Yegor Suslin
 *
 * This file is part of Pulverizer.
 *
 * Kraps is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Kraps is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Pulverizer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PulverizerAudioProcessor::PulverizerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
    parameters(*this, nullptr, juce::Identifier("Pulverizer"), {
            std::make_unique<juce::AudioParameterInt>("amount", "Amount", 0, 100, 8),
            std::make_unique<juce::AudioParameterFloat>("frequency", "Frequency", 50.0f, 8000.0f, 400.0f),
            std::make_unique<juce::AudioParameterFloat>("q_fac", "Q", 0.5f, 16.f, 0.5f),
            std::make_unique<juce::AudioParameterBool>("is_x2", "Perform x2", false),
            
        })
#endif
{
    proc.plug(&out_k, 0);
    out_p.plug(&proc, 0, 0);
}

PulverizerAudioProcessor::~PulverizerAudioProcessor()
{
    proc.unplug();
}

//==============================================================================
const juce::String PulverizerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PulverizerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PulverizerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PulverizerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PulverizerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PulverizerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PulverizerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PulverizerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PulverizerAudioProcessor::getProgramName (int index)
{
    return {};
}

void PulverizerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PulverizerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    proc.set_SR(sampleRate);
}

void PulverizerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PulverizerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else

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

void PulverizerAudioProcessor::preprocess_vals()
{
    auto& pt = proc.get_parameter_table();

    pt.set_value("frequency", parameters.getRawParameterValue("frequency")->load());
    pt.set_value("res", parameters.getRawParameterValue("q_fac")->load());
    pt.set_value("amount", parameters.getRawParameterValue("amount")->load());
    pt.set_value("is_x2", parameters.getRawParameterValue("is_x2")->load());


}

void PulverizerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
   
    const float** i_buf = buffer.getArrayOfReadPointers();
    float** o_buf = buffer.getArrayOfWritePointers();

    preprocess_vals();

    float d[2];

    for (int s = 0; s < buffer.getNumSamples(); ++s)
    {
        d[0] = i_buf[0][s];
        d[1] = i_buf[1][s];

        out_k.val = kraps::float8::loadu(d, 2);

        proc.process();
        out_p.process();

        out_p.get_sample().storeu(d, 2);

        o_buf[0][s] = d[0];
        o_buf[1][s] = d[1];
    }

}

//==============================================================================
bool PulverizerAudioProcessor::hasEditor() const
{
    return true; 
}

juce::AudioProcessorEditor* PulverizerAudioProcessor::createEditor()
{
    return new PulverizerPluginEditor(*this, parameters);
}

//==============================================================================
void PulverizerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void PulverizerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PulverizerAudioProcessor();
}
