/*
  ==============================================================================

    CustomL&F.h
    Created: 23 Jun 2021 6:55:56pm
    Author:  killm

  ==============================================================================
*/

#pragma once
#define DEG2RAD(x) x * MathConstants<float>::pi / 180.0

#include <JuceHeader.h>
using namespace juce;

class CustomLookAndFeel : public LookAndFeel_V4 {
public:
    CustomLookAndFeel() {
        setColour(Slider::thumbColourId, Colours::blue);
        setColour(PopupMenu::backgroundColourId, Colour(221, 221, 221));
        setColour(PopupMenu::textColourId, Colours::black);
        setColour(ComboBox::textColourId, Colours::black);
    }
    ~CustomLookAndFeel() {
        ;
    }
    void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
        const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override
    {
        auto radius = jmin(width / 2, height / 2) - 16.0f;
        auto centreX = x + width * 0.5f;
        auto centreY = y + height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);


        auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(10);
        auto lineW = jmin(5.0f, radius * 0.5f); //first constant is actual width
        auto arcRadius = radius + lineW * 0.7f;
        auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        // fill
        g.setColour(Colour(64, 64, 64).withAlpha(0.6f));
        g.fillEllipse(rx, ry, rw, rw);



        // outline
        g.setColour(Colours::black);
        g.drawEllipse(rx, ry, rw, rw, 3.0f);


        g.setColour(Colours::white);
        int sob = 4; //second outline bias
        g.drawEllipse(rx + sob / 2, ry + sob / 2, rw - sob, rw - sob, 1.0f); // x&y bias = minus half added/subtracted width



        if (slider.isEnabled())
        {

            Path arcOutline;
            Point<float> centre(centreX, centreY);
            Point<float> circ1 = centre.getPointOnCircumference(radius + 1, rotaryStartAngle);
            Point<float> circ2 = centre.getPointOnCircumference(radius + 1, rotaryEndAngle);

            arcOutline.startNewSubPath(circ1.getX(), circ1.getY());
            arcOutline.addCentredArc(bounds.getCentreX(),
                bounds.getCentreY(),
                arcRadius + lineW / 2,
                arcRadius + lineW / 2,
                0.0f,
                rotaryStartAngle,
                rotaryEndAngle,
                false);
            arcOutline.lineTo(circ2.getX(), circ2.getY());
            Path arcTrack;
            arcTrack.addCentredArc(bounds.getCentreX(),
                bounds.getCentreY(),
                arcRadius,
                arcRadius,
                0.0f,
                rotaryStartAngle,
                rotaryEndAngle,
                true);

            Path valueArc;
            valueArc.addCentredArc(bounds.getCentreX(),
                bounds.getCentreY(),
                arcRadius,
                arcRadius,
                0.0f,
                rotaryStartAngle,
                toAngle,
                true);

            g.setColour(Colours::black);
            g.strokePath(arcOutline, PathStrokeType(lineW / 1.5, PathStrokeType::curved, PathStrokeType::butt));

            g.setColour(Colour(38, 38, 38));
            g.strokePath(arcTrack, PathStrokeType(lineW, PathStrokeType::curved, PathStrokeType::butt));

            if (slider.isColourSpecified(Slider::ColourIds::trackColourId)) {
                g.setColour(slider.findColour(Slider::ColourIds::trackColourId));
            }
            else g.setColour(Colours::darkred);

            g.strokePath(valueArc, PathStrokeType(lineW, PathStrokeType::curved, PathStrokeType::butt));

        }

        Path p;
        auto pointerLength = rw * 0.13f;
        auto pointerThickness = 2.0f;

        p.addRectangle(-pointerThickness * 0.5f, -radius + sob, pointerThickness, pointerLength);
        p.applyTransform(AffineTransform::rotation(angle).translated(centreX, centreY));



        // pointer
        g.setColour(Colours::lightgrey);
        g.fillPath(p);
    }

    void drawComboBox(Graphics& g, int width, int height, bool,
        int, int, int, int, ComboBox& box) override
    {
        auto cornerSize = box.findParentComponentOfClass<ChoicePropertyComponent>() != nullptr ? 0.0f : 8.0f; // 16.f = border-radius
        auto bounds = juce::Rectangle<int>(0, 0, width, height).toFloat().reduced(1, 1);
        juce::Rectangle<int> boxBounds(1, 0, bounds.getWidth(), bounds.getHeight());
        juce::Rectangle<int> glossBounds(1, 1, bounds.getWidth(), bounds.getHeight());

        //DropShadow(Colours::black.withAlpha(0.1f), 20, { 5,0 }).drawForRectangle(g, glossBounds.reduced(15,15));

        g.setColour(Colours::white);
        g.fillRoundedRectangle(boxBounds.toFloat(), cornerSize);


        g.setGradientFill(ColourGradient::vertical(Colours::white, 0, Colours::black.withAlpha(0.1f), glossBounds.getHeight()));
        g.fillRoundedRectangle(glossBounds.toFloat(), cornerSize);


    }
    void positionComboBoxText(ComboBox& box, Label& label) override
    {
        label.setBounds(0, -1,
            box.getWidth(),
            box.getHeight());
        label.setJustificationType(Justification::centred);
        label.setFont(getComboBoxFont(box));
    }
private:

};