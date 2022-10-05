#include "Helpers.h"

juce::AudioBuffer<float>* Helpers::generateAudioSampleBuffer() {
    int channels = 2;
    int samples = 4096;
    //int samples = 8192;
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

juce::AudioBuffer<float>* Helpers::generateBigAudioSampleBuffer() {
    int channels = 2;
    //int samples = 4096;
    int samples = 4096 * 256;
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
    juce::MemoryMappedAudioFormatReader *reader = audioFormat->createMemoryMappedReader(juce::File("samples/CSC_sweep_20-20k_amped.wav"));
    reader->mapEntireFile();
    //reader->mapSectionOfFile(juce::Range<juce::int64> (0, reader->sampleRate * 1));
//juce::AudioFormatReader *reader = formatManager.createReaderFor(juce::File("samples/minisweep.wav"));

    return reader;
    /*AudioSampleBuffer buffer;
    reader->read(buffer, 0, reader->lengthInSamples, 0, true, true);
    delete reader; */
}

void Helpers::writeBufferToFile(juce::AudioBuffer<float>* buffer, juce::String path) {
    juce::File bufferFile = juce::File(path);
juce::TemporaryFile tempFile (bufferFile);
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

if (output.getStatus().failed())
{
    WARN ("An error occurred in the FileOutputStream");
    // ... some other error handling
}


}

juce::AudioBuffer<float>* Helpers::readBufferFromFile(juce::String path) {
    juce::File bufferFile = juce::File(path);

juce::FileInputStream input (bufferFile);

if (! input.openedOk())
{
    DBG("Failed to open file");
    // ... Error handling here
}
    
/*bool readWholeFile = true;
if (readWholeFile)
{
    String content = input->readString();
    // ... Do your stuff. New lines are normally seperated by "\n"
}
else
{
    while (! input->isExhausted())
    {*/
        int numChannels = input.readInt();
int numSamples = input.readInt();
        // ... Do something with each line
/*    }
}*/


    juce::AudioBuffer<float> *buffer = new juce::AudioBuffer<float>(numChannels, numSamples);
for (int i = 0; i < buffer->getNumChannels(); i++) {
    for (int j = 0; j < buffer->getNumSamples(); j++) {
        buffer->setSample(i, j, input.readFloat());
    }
}
    return buffer;


}

