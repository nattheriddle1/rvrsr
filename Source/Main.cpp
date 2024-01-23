#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::AudioProcessorEditor *RvrsrAudioProcessor::createEditor() {
    return new RvrsrAudioProcessorEditor(*this);
};

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
    return new RvrsrAudioProcessor();
}
