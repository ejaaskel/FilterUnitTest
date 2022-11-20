#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_formats/juce_audio_formats.h>

#include <catch2/catch_all.hpp>

class Helpers {
public:
    static juce::AudioBuffer<float>* generateAudioSampleBuffer(int channels = 2, int samples = 4096);
    static juce::AudioBuffer<float>* generateIncreasingAudioSampleBuffer(int channels = 2, int samples = 4096);
    static juce::MemoryMappedAudioFormatReader* readSineSweep();
    static void writeBufferToFile(juce::AudioBuffer<float>* buffer, juce::String path);
    static juce::AudioBuffer<float>* readBufferFromFile(juce::String path);

};
