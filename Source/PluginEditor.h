/* Copyright 2021 Yegor Suslin
 *
 * This file is part of Pulverizer.
 *
 * Pulverizer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Pulverizer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Pulverizer.  If not, see <http://www.gnu.org/licenses/>.
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
