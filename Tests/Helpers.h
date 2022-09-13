#include <juce_audio_processors/juce_audio_processors.h>

#include <catch2/catch_all.hpp>

class Helpers {
    public:
        static juce::AudioBuffer<float>* generateAudioSampleBuffer();
        static juce::AudioBuffer<float>* generateMaxAudioSampleBuffer();

};
