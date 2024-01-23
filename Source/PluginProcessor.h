#pragma once
#include <JuceHeader.h>
#include <JucePluginDefines.h>

class RvrsrAudioProcessor :
    public juce::AudioProcessor,
    public juce::AudioProcessorParameter::Listener {

public:
    RvrsrAudioProcessor() :
        juce::AudioProcessor(
        BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
        ) {
        addParameter(dry_in_parameter = new juce::AudioParameterFloat(
            juce::ParameterID("Dry In", 4), "Dry In",
            dry_in_minimum, dry_in_maximum, dry_in_default));
        dry_in_parameter->addListener(this);
        addParameter(dry_out_parameter = new juce::AudioParameterFloat(
            juce::ParameterID("Dry Out", 4), "Dry Out",
            dry_out_minimum, dry_out_maximum, dry_out_default));
        dry_out_parameter->addListener(this);
        addParameter(wet_out_parameter = new juce::AudioParameterFloat(
            juce::ParameterID("Wet Out", 4), "Wet Out",
            wet_out_minimum, wet_out_maximum, wet_out_default));
        wet_out_parameter->addListener(this);
        addParameter(delay_parameter = new juce::AudioParameterInt(
            juce::ParameterID("delay", 4), "Delay",
            delay_minimum, delay_maximum, delay_default));
        delay_parameter->addListener(this);
        addParameter(muting_parameter = new juce::AudioParameterFloat(
            juce::ParameterID("muting", 5), "Muting",
            muting_minimum, muting_maximum, muting_default));
        muting_parameter->addListener(this);
        addParameter(stereo_parameter = new juce::AudioParameterFloat(
            juce::ParameterID("stereo", 5), "Stereo",
            stereo_minimum, stereo_maximum, stereo_default));
        stereo_parameter->addListener(this);

        delay_parameter->addListener(this);

        cache.clear();
        cache.setSize(2, delay_maximum);
        cache_length = delay_maximum;
    };

    ~RvrsrAudioProcessor() override {
    };

    void prepareToPlay(double sampleRate, int samplesPerBlock) override {
    };

    void releaseResources() override {
        cache.clear();
    };

    void reset() override {
        cache.clear();
    }

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout &layouts) const override {
#if JucePlugin_IsMidiEffect
        juce::ignoreUnused(layouts);
        return true;
#else
        if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
            && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
            return false;
#if ! JucePlugin_IsSynth
        if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
            return false;
#endif
        return true;
#endif
    };
#endif

    void processBlock(juce::AudioBuffer<float> &audio_buffer, juce::MidiBuffer &midi_buffer) override {
        juce::AudioPlayHead *playHead = getPlayHead();
        for (int i = 0; i < audio_buffer.getNumSamples(); i++) {
            juce::AudioPlayHead::PositionInfo info = playHead->getPosition()
                .orFallback(juce::AudioPlayHead::PositionInfo{});
            int should = info.getTimeInSamples().orFallback(0) % cache_length;
            if (info.getIsPlaying() || info.getIsRecording()) {
                if (should != previous) {
                    if (previous == -1)
                        cache.clear();
                    if (cache_position != should)
                        cache_position = should;
                }
                previous = should;
            } else {
                previous = -1;
            }
            for (int channel = 0; channel < audio_buffer.getNumChannels(); channel++) {
                int channel_stereo = 0;
                if (*stereo_parameter > 0 && channel % 2 == 1)
                    channel_stereo = audio_buffer.getNumSamples() * *stereo_parameter;
                else if (*stereo_parameter < 0 && channel % 2 == 0)
                    channel_stereo = audio_buffer.getNumSamples() * -*stereo_parameter;

                float *buffer_pointer = audio_buffer.getWritePointer(channel);
                float *cache_pointer = cache.getWritePointer(channel);

                cache_pointer[cache_length - (cache_position % cache_length)] /= powf(*muting_parameter + 1, 6);
                cache_pointer[cache_length - (cache_position % cache_length)] += buffer_pointer[i] * *dry_in_parameter;
                buffer_pointer[i] *= *dry_out_parameter;
                buffer_pointer[i] += cache_pointer[
                    (cache_position + audio_buffer.getNumSamples() + channel_stereo)
                        % cache_length] * *wet_out_parameter;
            }
            cache_position = (cache_position + 1) % cache_length;
        }
    };

    juce::AudioProcessorEditor *createEditor() override;

    bool hasEditor() const override {
        return true;
    };
    const juce::String getName() const override {
        return JucePlugin_Name;
    };
    bool acceptsMidi() const override {
#if JucePlugin_WantsMidiInput
        return true;
#else
        return false;
#endif
    };
    bool producesMidi() const override {
#if JucePlugin_ProducesMidiOutput
        return true;
#else
        return false;
#endif
    };
    bool isMidiEffect() const override {
#if JucePlugin_IsMidiEffect
        return true;
#else
        return false;
#endif
    };
    double getTailLengthSeconds() const override {
        return 0.0;
    };
    int getNumPrograms() override {
        return 1;
    };
    int getCurrentProgram() override {
        return 0;
    };
    void setCurrentProgram(int index) override {
    };
    const juce::String getProgramName(int index) override {
        return {};
    };
    void changeProgramName(int index, const juce::String &newName) override {
    };

    void getStateInformation(juce::MemoryBlock &destData) override {
        juce::MemoryOutputStream stream(destData, true);
        stream.writeFloat(dry_in_parameter->get());
        stream.writeInt(delay_parameter->get());
        stream.writeFloat(muting_parameter->get());
        stream.writeFloat(stereo_parameter->get());
        stream.writeFloat(wet_out_parameter->get());
        stream.writeFloat(dry_out_parameter->get());
        stream.flush();
    };
    void setStateInformation(const void *data, int sizeInBytes) override {
        juce::MemoryInputStream stream(data, static_cast<size_t> (sizeInBytes), false);
        *dry_in_parameter = stream.readFloat();
        *delay_parameter = stream.readInt();
        *muting_parameter = stream.readFloat();
        *stereo_parameter = stream.readFloat();
        *wet_out_parameter = stream.readFloat();
        *dry_out_parameter = stream.readFloat();
    };

    void parameterValueChanged(int parameterIndex, float newValue) override {
        if (delay_parameter->getParameterIndex() == parameterIndex)
            cache_length = 2 * delay_parameter->convertFrom0to1(newValue);
    };
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {
    };

    juce::AudioParameterFloat *dry_in_parameter;
    const float dry_in_minimum = 0.0f;
    const float dry_in_maximum = 1.0f;
    const float dry_in_interval = 0.01f;
    const float dry_in_default = 1.0f;

    juce::AudioParameterFloat *dry_out_parameter;
    const float dry_out_minimum = 0.0f;
    const float dry_out_maximum = 1.0f;
    const float dry_out_interval = 0.01f;
    const float dry_out_default = 1.0f;

    juce::AudioParameterFloat *wet_out_parameter;
    const float wet_out_minimum = 0.0f;
    const float wet_out_maximum = 1.0f;
    const float wet_out_interval = 0.01f;
    const float wet_out_default = 0.5f;

    juce::AudioParameterInt *delay_parameter;
    const int delay_minimum = 50;
    const int delay_maximum = 500000;
    const int delay_interval = 1;
    const int delay_default = 50000;

    juce::AudioParameterFloat *muting_parameter;
    const float muting_minimum = 0.0f;
    const float muting_maximum = 1.0f;
    const float muting_interval = 0.01f;
    const float muting_default = 0.25f;

    juce::AudioParameterFloat *stereo_parameter;
    const float stereo_minimum = -1.0f;
    const float stereo_maximum = 1.0f;
    const float stereo_interval = 0.01f;
    const float stereo_default = 0.0f;

private:
    juce::AudioBuffer<float> cache;
    unsigned int cache_position = 0;
    unsigned int cache_length = 0;
    int previous = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RvrsrAudioProcessor)
};
