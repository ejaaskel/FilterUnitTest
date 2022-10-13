#include "Helpers.h"

juce::AudioBuffer<float>* Helpers::generateAudioSampleBuffer() {
    int channels = 2;
    int samples = 4096;
    juce::AudioBuffer<float> *buffer = new juce::AudioBuffer<float>(channels, samples);

    //Fill with random values ranging from -1 to 1
    for (int i = 0; i < channels; i++) {
        auto* writePointer = buffer->getWritePointer(i);
        for (int j = 0; j < samples; j++) {
            buffer->setSample(i, j, -1.0f + juce::Random::getSystemRandom().nextFloat() * 2.0f);
        }
    }

    return buffer;
}

juce::AudioBuffer<float>* Helpers::generateBigAudioSampleBuffer() {
    int channels = 2;
    int samples = 4096 * 256;
    juce::AudioBuffer<float> *buffer = new juce::AudioBuffer<float>(channels, samples);

    //Fill with random values ranging from -1 to 1
    for (int i = 0; i < channels; i++) {
        auto* writePointer = buffer->getWritePointer(i);
        for (int j = 0; j < samples; j++) {
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
            buffer->setSample(i, j, juce::jmap((float)(j % 1001), 0.0f, 1000.0f, -1.0f, 1.0f));
        }
    }

    return buffer;
}

juce::MemoryMappedAudioFormatReader* Helpers::readSineSweep() {
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();
    juce::AudioFormat *audioFormat = formatManager.getDefaultFormat();

    juce::MemoryMappedAudioFormatReader *reader = audioFormat->createMemoryMappedReader(juce::File("samples/CSC_sweep_20-20k_amped.wav"));
    reader->mapEntireFile();

    return reader;
}

void Helpers::writeBufferToFile(juce::AudioBuffer<float>* buffer, juce::String path) {
    juce::File bufferFile = juce::File(path);
    juce::FileOutputStream output (bufferFile);
 
    output.setNewLineString("\n");

    output.writeInt(buffer->getNumChannels());
    output.writeInt(buffer->getNumSamples());

    for (int i = 0; i < buffer->getNumChannels(); i++) {
        for (int j = 0; j < buffer->getNumSamples(); j++) {
            output.writeFloat(buffer->getSample(i,j));
        }
    }

    output.flush(); // (called explicitly to force an fsync on posix)

    /*if (output.getStatus().failed())
    {
        WARN ("An error occurred in the FileOutputStream");
        // ... some other error handling
    }*/
}

juce::AudioBuffer<float>* Helpers::readBufferFromFile(juce::String path) {
    juce::File bufferFile = juce::File(path);

    juce::FileInputStream input (bufferFile);

    /*if (! input.openedOk())
    {
        DBG("Failed to open file");
        // ... Error handling here
    }*/
    
    int numChannels = input.readInt();
    int numSamples = input.readInt();

    juce::AudioBuffer<float> *buffer = new juce::AudioBuffer<float>(numChannels, numSamples);
    for (int i = 0; i < buffer->getNumChannels(); i++) {
        for (int j = 0; j < buffer->getNumSamples(); j++) {
            buffer->setSample(i, j, input.readFloat());
        }
    }
    return buffer;
}

