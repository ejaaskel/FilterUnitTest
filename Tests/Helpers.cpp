#include "Helpers.h"

juce::AudioBuffer<float>* Helpers::generateAudioSampleBuffer() {
    int channels = 2;
    int samples = 4096;
    juce::AudioBuffer<float> *buffer = new juce::AudioBuffer<float>(channels, samples);

    //Fill with random values ranging from -1 to 1
    for (int i = 0; i < channels; i++) {
        auto* writePointer = buffer->getWritePointer(i);
        for (int j = 0; j < samples; j++) {
            //writePointer[i] = (-1.0f + juce::Random::getSystemRandom().nextFloat() * 2.0f);
            buffer->setSample(i, j, -1.0f + juce::Random::getSystemRandom().nextFloat() * 2.0f);
        }
    }

    return buffer;
}
juce::AudioBuffer<float>* Helpers::generateMaxAudioSampleBuffer() {
    int channels = 2;
    int samples = 4096;
    juce::AudioBuffer<float> *buffer = new juce::AudioBuffer<float>(channels, samples);

    //Fill with random values ranging from -1 to 1
    for (int i = 0; i < channels; i++) {
        auto* writePointer = buffer->getWritePointer(i);
        for (int j = 0; j < samples; j++) {
            //writePointer[i] = (-1.0f + juce::Random::getSystemRandom().nextFloat() * 2.0f);
            buffer->setSample(i, j, juce::jmap((float)(j % 1001), 0.0f, 1000.0f, -1.0f, 1.0f));
        }
    }

    return buffer;
}

