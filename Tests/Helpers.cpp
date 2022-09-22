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

juce::MemoryMappedAudioFormatReader* Helpers::readSineSweep() {
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();
    juce::AudioFormat *audioFormat = formatManager.getDefaultFormat();

//Previous read
    /*juce::AudioFormatReader *reader = formatManager.createReaderFor(juce::File("samples/halfsweep.wav"));*/
//New read
    juce::MemoryMappedAudioFormatReader *reader = audioFormat->createMemoryMappedReader(juce::File("samples/CSC_sweep_20-20k.wav"));
    reader->mapEntireFile();
    //reader->mapSectionOfFile(juce::Range<juce::int64> (0, reader->sampleRate * 1));
//juce::AudioFormatReader *reader = formatManager.createReaderFor(juce::File("samples/minisweep.wav"));

    return reader;
    /*AudioSampleBuffer buffer;
    reader->read(buffer, 0, reader->lengthInSamples, 0, true, true);
    delete reader; */
}

