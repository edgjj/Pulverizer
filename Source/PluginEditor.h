/*
  ==============================================================================

    PluginEditor.h
    Created: 23 Jun 2021 6:51:05pm
    Author:  killm

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CustomL&F.h" 

//==============================================================================
/*
*/
class PulverizerPluginEditor : public juce::AudioProcessorEditor
{
public:
    PulverizerPluginEditor(PulverizerAudioProcessor&, juce::AudioProcessorValueTreeState& vts);
    ~PulverizerPluginEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    CustomLookAndFeel pulv_lnf;
    PulverizerAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& value_tree;

    juce::Slider amt, qfac, freq;
    juce::ToggleButton is_x2;


    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

    std::unique_ptr<SliderAttachment> amt_attach, qfac_attach, freq_attach;
    std::unique_ptr<ButtonAttachment> x2_attach;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PulverizerPluginEditor)
};
