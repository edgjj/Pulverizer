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


#include <JuceHeader.h>
#include "PluginEditor.h"

//==============================================================================
PulverizerPluginEditor::PulverizerPluginEditor(PulverizerAudioProcessor& p, juce::AudioProcessorValueTreeState& vts) 
    : AudioProcessorEditor(&p), audioProcessor (p), value_tree(vts)
{
    setLookAndFeel(&pulv_lnf);
    


    amt.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    amt.setTextBoxStyle(Slider::NoTextBox, false, 80, 20);
    amt.setPopupDisplayEnabled(true, true, this);
    amt.setColour(Slider::ColourIds::trackColourId, juce::Colours::darkmagenta);

    qfac.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    qfac.setTextBoxStyle(Slider::NoTextBox, false, 80, 20);
    qfac.setPopupDisplayEnabled(true, true, this);
    qfac.setColour(Slider::ColourIds::trackColourId, juce::Colours::darkmagenta);

    freq.setTextValueSuffix(" Hz");
    freq.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    freq.setTextBoxStyle(Slider::NoTextBox, false, 80, 20);
    freq.setPopupDisplayEnabled(true, true, this);
    freq.setColour(Slider::ColourIds::trackColourId, juce::Colours::darkmagenta);

    is_x2.setButtonText("Toggle x2 Amount Processing");
    is_x2.setColour(juce::ToggleButton::ColourIds::textColourId, juce::Colours::black);
    is_x2.setColour(juce::ToggleButton::ColourIds::tickDisabledColourId, juce::Colours::black);

    amt_attach.reset(new SliderAttachment(value_tree, "amount", amt));
    qfac_attach.reset(new SliderAttachment(value_tree, "q_fac", qfac));
    freq_attach.reset(new SliderAttachment(value_tree, "frequency", freq));
    x2_attach.reset(new ButtonAttachment(value_tree, "is_x2", is_x2));


    addAndMakeVisible(amt);
    addAndMakeVisible(freq);
    addAndMakeVisible(qfac);
    addAndMakeVisible(is_x2);

    setSize(300, 100);

}

PulverizerPluginEditor::~PulverizerPluginEditor()
{
    setLookAndFeel(nullptr);
}

void PulverizerPluginEditor::paint (juce::Graphics& g)
{
    g.setColour(juce::Colours::lightblue.withBrightness(4.f));
    g.fillRect(getLocalBounds());

    g.setColour(juce::Colours::black);

    g.drawText("Amount", juce::Rectangle<int>(amt.getX(), amt.getBottom(), amt.getWidth(), 10), juce::Justification::centred);
    g.drawText("Frequency", juce::Rectangle<int>(freq.getX(), freq.getBottom(), freq.getWidth(), 10), juce::Justification::centred);
    g.drawText("Q", juce::Rectangle<int>(qfac.getX(), qfac.getBottom(), qfac.getWidth(), 10), juce::Justification::centred);

}

void PulverizerPluginEditor::resized()
{
    juce::Grid grid;

    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;
    using Px = juce::Grid::Px;

    grid.templateColumns = { Track(Fr(1)),Track(Fr(1)),Track(Fr(1)),Track(Fr(1)) };
    grid.templateRows = { Track(Fr(1)) };
    grid.autoRows = Track(Fr(1));
    grid.autoColumns = Track(Fr(1));

    grid.items = { juce::GridItem(amt), juce::GridItem(freq),juce::GridItem(qfac),juce::GridItem(is_x2) };


    grid.setGap(Px(1));

    grid.performLayout(getLocalBounds().withHeight(getHeight() - 20));


}
