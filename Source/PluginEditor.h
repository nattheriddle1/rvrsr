#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "PluginLookAndFeel.h"
#include <string.h>

class RvrsrAudioProcessorEditor :
    public juce::AudioProcessorEditor,
    public juce::Slider::Listener,
    public juce::Timer {

public:
    RvrsrAudioProcessorEditor(RvrsrAudioProcessor &p) :
        AudioProcessorEditor(&p), audioProcessor(p) {
        startTimerHz(30);
        setLookAndFeel(&lookAndFeel);
        setSize(515, 215);

        addAndMakeVisible(dry_in_slider);
        dry_in_slider.setRange(p.dry_in_minimum, p.dry_in_maximum, p.dry_in_interval);
        dry_in_slider.addListener(this);
        dry_in_slider.setName("Input");
        dry_in_slider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
        dry_in_slider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 60, 20);
        dry_in_slider.setDoubleClickReturnValue(true, p.dry_in_default);
        dry_in_label.setText(dry_in_slider.getName(), juce::NotificationType::dontSendNotification);
        dry_in_label.setJustificationType(juce::Justification::horizontallyCentred);
        dry_in_label.attachToComponent(&dry_in_slider, false);

        addAndMakeVisible(dry_out_slider);
        dry_out_slider.setRange(p.dry_out_minimum, p.dry_out_maximum, p.dry_out_interval);
        dry_out_slider.addListener(this);
        dry_out_slider.setName("Dry");
        dry_out_slider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
        dry_out_slider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 60, 20);
        dry_out_slider.setDoubleClickReturnValue(true, p.dry_out_default);
        dry_out_label.setText(dry_out_slider.getName(), juce::NotificationType::dontSendNotification);
        dry_out_label.setJustificationType(juce::Justification::horizontallyCentred);
        dry_out_label.attachToComponent(&dry_out_slider, false);

        addAndMakeVisible(wet_out_slider);
        wet_out_slider.setRange(p.wet_out_minimum, p.wet_out_maximum, p.wet_out_interval);
        wet_out_slider.addListener(this);
        wet_out_slider.setName("Wet");
        wet_out_slider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
        wet_out_slider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 60, 20);
        wet_out_slider.setDoubleClickReturnValue(true, p.wet_out_default);
        wet_out_label.setText(wet_out_slider.getName(), juce::NotificationType::dontSendNotification);
        wet_out_label.setJustificationType(juce::Justification::horizontallyCentred);
        wet_out_label.attachToComponent(&wet_out_slider, false);

        addAndMakeVisible(delay_slider);
        delay_slider.valueFromTextFunction = [&p](juce::String s) {
            double v = p.getSampleRate();
            std::stringstream ss = {};
            for (auto c : s) {
                if (('0' <= c && c <= '9') || (c == '.') || (c == '-'))
                    ss << (char) c;
                else if (c == 'm')
                    v /= 1000;
            }
            return v * stod(ss.str());
        };
        delay_slider.textFromValueFunction = [&p](double v) {
            v /= p.getSampleRate();
            return v < 1 ? juce::String(v * 1000) + "ms" : juce::String(v) + "s";
        };
        delay_slider.setSkewFactor(0.33);
        delay_slider.setRange(p.delay_minimum, p.delay_maximum, p.delay_interval);
        delay_slider.addListener(this);
        delay_slider.setName(p.delay_parameter->getName(20));
        delay_slider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
        delay_slider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 60, 20);
        delay_slider.setDoubleClickReturnValue(true, p.delay_default);

        delay_label.setText(delay_slider.getName(), juce::NotificationType::dontSendNotification);
        delay_label.setJustificationType(juce::Justification::horizontallyCentred);
        delay_label.attachToComponent(&delay_slider, false);

        addAndMakeVisible(muting_slider);
        muting_slider.setRange(p.muting_minimum, p.muting_maximum, p.muting_interval);
        muting_slider.addListener(this);
        muting_slider.setName(p.muting_parameter->getName(20));
        muting_slider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
        muting_slider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 60, 20);
        muting_slider.setDoubleClickReturnValue(true, p.muting_default);
        muting_label.setText(muting_slider.getName(), juce::NotificationType::dontSendNotification);
        muting_label.setJustificationType(juce::Justification::horizontallyCentred);
        muting_label.attachToComponent(&muting_slider, false);

        addAndMakeVisible(stereo_slider);
        stereo_slider.valueFromTextFunction = [&p](juce::String s) {
            double v = 1;
            std::stringstream ss = {};
            for (auto c : s) {
                switch (c) {
                case 'L':
                    v *= -1;
                    break;
                case '1': case '2': case '3':
                case '4': case '5': case '6':
                case '7': case '8': case '9':
                case '0': case '.':
                    ss << (char) c;
                };
            };
            return v * stod(ss.str());
        };
        stereo_slider.textFromValueFunction = [&p](double v) {
            v = round(v * 100) / 100;
            return v > 0 ? juce::String(v) + "R" : juce::String(-v) + "L";
        };
        stereo_slider.setRange(p.stereo_minimum, p.stereo_maximum, p.stereo_interval);
        stereo_slider.addListener(this);
        stereo_slider.setName(p.stereo_parameter->getName(20));
        stereo_slider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
        stereo_slider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 60, 20);
        stereo_slider.setDoubleClickReturnValue(true, p.stereo_default);
        stereo_label.setText(stereo_slider.getName(), juce::NotificationType::dontSendNotification);
        stereo_label.setJustificationType(juce::Justification::horizontallyCentred);
        stereo_label.attachToComponent(&stereo_slider, false);
    };

    ~RvrsrAudioProcessorEditor() override {
        setLookAndFeel(nullptr);
    };

    void paint(juce::Graphics &g) override {
        g.setGradientFill(juce::ColourGradient(
            findColour(juce::DocumentWindow::backgroundColourId),
            juce::Point<float>(getWidth() * (3.0f / 4.0f), 0),
            findColour(juce::DocumentWindow::backgroundColourId).darker(0.25f),
            juce::Point<float>(getWidth() * (1.0f / 4.0f), getHeight()),
            false)
        );
        g.fillAll();
        g.setFont(juce::Font(30, juce::Font::FontStyleFlags::bold));
        g.setColour(findColour(RvrsrLookAndFeel::ColourScheme::defaultText));
        g.drawFittedText("RVRSR", 30, 15, getWidth() - 50, 30, juce::Justification::right | juce::Justification::top, 1, 1.0f);
        g.setColour(findColour(RvrsrLookAndFeel::ColourScheme::defaultFill));
        g.fillRect(20, 30, getWidth() - 140, 2);
        g.setColour(findColour(juce::DocumentWindow::backgroundColourId).darker(0.5));
        g.fillRect(74, 60, 2, 127);
        g.fillRect(404, 60, 2, 127);
    };

    void resized() override {
        dry_in_slider.setBounds(16, 77, 40, 119);
        delay_slider.setBounds(84, 77, 110, 110);
        muting_slider.setBounds(184, 77, 110, 110);
        stereo_slider.setBounds(284, 77, 110, 110);
        wet_out_slider.setBounds(427, 77, 30, 119);
        dry_out_slider.setBounds(464, 77, 30, 119);
    };

    void sliderValueChanged(juce::Slider *slider) override {
        if (slider == &wet_out_slider) {
            audioProcessor.wet_out_parameter->setValueNotifyingHost(
                audioProcessor.wet_out_parameter->convertTo0to1(slider->getValue()));
        } else if (slider == &delay_slider) {
            audioProcessor.delay_parameter->setValueNotifyingHost(
                audioProcessor.delay_parameter->convertTo0to1(slider->getValue()));
        } else if (slider == &muting_slider) {
            audioProcessor.muting_parameter->setValueNotifyingHost(
                audioProcessor.muting_parameter->convertTo0to1(slider->getValue()));
        } else if (slider == &stereo_slider) {
            audioProcessor.stereo_parameter->setValueNotifyingHost(
                audioProcessor.stereo_parameter->convertTo0to1(slider->getValue()));
        } else if (slider == &dry_in_slider) {
            audioProcessor.dry_in_parameter->setValueNotifyingHost(
                audioProcessor.dry_in_parameter->convertTo0to1(slider->getValue()));
        } else if (slider == &dry_out_slider) {
            audioProcessor.dry_out_parameter->setValueNotifyingHost(
                audioProcessor.dry_out_parameter->convertTo0to1(slider->getValue()));
        }
    };

    void timerCallback() override {
        dry_in_slider.setValue(audioProcessor.dry_in_parameter->get(),
            juce::NotificationType::dontSendNotification);
        dry_out_slider.setValue(audioProcessor.dry_out_parameter->get(),
            juce::NotificationType::dontSendNotification);
        wet_out_slider.setValue(audioProcessor.wet_out_parameter->get(),
            juce::NotificationType::dontSendNotification);
        delay_slider.setValue(audioProcessor.delay_parameter->get(),
            juce::NotificationType::dontSendNotification);
        muting_slider.setValue(audioProcessor.muting_parameter->get(),
            juce::NotificationType::dontSendNotification);
        stereo_slider.setValue(audioProcessor.stereo_parameter->get(),
            juce::NotificationType::dontSendNotification);
    }

private:
    RvrsrLookAndFeel lookAndFeel;
    juce::Slider dry_in_slider;
    juce::Label dry_in_label;
    juce::Slider dry_out_slider;
    juce::Label dry_out_label;
    juce::Slider wet_out_slider;
    juce::Label wet_out_label;
    juce::Slider delay_slider;
    juce::Label delay_label;
    juce::Slider muting_slider;
    juce::Label muting_label;
    juce::Slider stereo_slider;
    juce::Label stereo_label;
    RvrsrAudioProcessor &audioProcessor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RvrsrAudioProcessorEditor)
};
