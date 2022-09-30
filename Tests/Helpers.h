#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_formats/juce_audio_formats.h>

#include <catch2/catch_all.hpp>

class Helpers {
    public:
        static juce::AudioBuffer<float>* generateAudioSampleBuffer();
        static juce::AudioBuffer<float>* generateBigAudioSampleBuffer();
        static juce::AudioBuffer<float>* generateMaxAudioSampleBuffer();
        static juce::MemoryMappedAudioFormatReader* readSineSweep();
};
