#pragma once
#include <JuceHeader.h>

class RvrsrLookAndFeel : public juce::LookAndFeel_V4 {
public:
    RvrsrLookAndFeel() {
        setColour(ColourScheme::defaultText,
            juce::Colour(0.08f, 0.18f, 0.42f, 1.0f));
        setColour(ColourScheme::defaultFill,
            juce::Colour(0.08f, 0.18f, 0.42f, 1.0f));

        setColour(juce::Label::textColourId,
            juce::Colour(0.08f, 0.18f, 0.50f, 1.0f));
        setColour(juce::DocumentWindow::backgroundColourId,
            juce::Colour(0.08f, 0.18f, 0.13f, 1.0f));
        setColour(juce::Slider::ColourIds::thumbColourId,
            juce::Colour(0.08f, 0.18f, 0.80f, 1.0f));
        setColour(juce::Slider::ColourIds::rotarySliderFillColourId,
            juce::Colour(0.08f, 0.18f, 0.42f, 1.0f));
        setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId,
            juce::Colour(0.08f, 0.18f, 0.42f, 1.0f));
        setColour(juce::Slider::ColourIds::backgroundColourId,
            juce::Colour(0.08f, 0.18f, 0.20f, 1.0f));
        setColour(juce::Slider::ColourIds::trackColourId,
            juce::Colour(0.08f, 0.18f, 0.45f, 1.0f));
        setColour(juce::Slider::ColourIds::textBoxBackgroundColourId,
            juce::Colour(0.08f, 0.18f, 0.23f, 1.0f));
        setColour(juce::Slider::ColourIds::textBoxHighlightColourId,
            juce::Colour(0.08f, 0.18f, 0.35f, 1.0f));
        setColour(juce::Slider::ColourIds::textBoxOutlineColourId,
            juce::Colour(0.08f, 0.18f, 0.42f, 1.0f));
        setColour(juce::Slider::ColourIds::textBoxTextColourId,
            juce::Colour(0.08f, 0.18f, 0.75f, 1.0f));
    }

    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos,
        const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider &) override {
        float radius = (float) juce::jmin(width / 2, height / 2) - 12.0f;
        float centreX = (float) x + (float) width * 0.5f;
        float centreY = (float) y + (float) height * 0.5f;
        float rx = centreX - radius;
        float ry = centreY - radius;
        float rw = radius * 2.0f;
        float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // Fill
        g.setGradientFill(juce::ColourGradient(
            findColour(juce::Slider::ColourIds::rotarySliderOutlineColourId),
            juce::Point<float>(rx, ry),
            findColour(juce::Slider::ColourIds::rotarySliderOutlineColourId).darker(0.25f),
            juce::Point<float>(rw, rw),
            false)
        );
        g.fillEllipse(rx, ry, rw, rw);

        // Inset
        float inset = radius * 0.2f;
        g.setGradientFill(juce::ColourGradient(
            findColour(juce::Slider::ColourIds::rotarySliderFillColourId).darker(0.25f),
            juce::Point<float>(rx, ry),
            findColour(juce::Slider::ColourIds::rotarySliderFillColourId),
            juce::Point<float>(rw, rw),
            false)
        );
        g.fillEllipse(rx + inset, ry + inset, rw - 2 * inset, rw - 2 * inset);

        // Pointer
        juce::Path p;
        float pointerLength = inset;
        float pointerThickness = 3.0f;
        p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
        g.setColour(findColour(juce::Slider::ColourIds::thumbColourId));
        g.fillPath(p);
    }
};
