#include <PluginProcessor.h>
#include <catch2/catch_all.hpp>
#include "Helpers.h"
#include "Matchers.h"
#include "ImageProcessing.h"

TEST_CASE("Plugin Instance Name Demo", "[presentation]")
{
//    AudioPluginAudioProcessor *testPluginProcessor = new AudioPluginAudioProcessor();

//    CHECK_THAT(testPluginProcessor->getName().toStdString(),
//               Catch::Matchers::Equals("Filter Unit Test"));

}

TEST_CASE("Filter Processor Demo", "[presentation]")
{
//     int samplesPerBlock = 4096;
//     int sampleRate = 44100;

//     AudioPluginAudioProcessor *testPluginProcessor = new AudioPluginAudioProcessor();
//     juce::MemoryMappedAudioFormatReader *reader = Helpers::readSineSweep();
//     juce::AudioBuffer<float> *buffer = new juce::AudioBuffer<float>(reader->numChannels, reader->lengthInSamples);
//     reader->read(buffer->getArrayOfWritePointers(), 1, 0, reader->lengthInSamples);
    
//     juce::AudioBuffer<float> originalBuffer(*buffer);

//     //Dismiss the partial chunk for now
//     int chunkAmount = buffer->getNumSamples() / samplesPerBlock;

//     juce::MidiBuffer midiBuffer;

//     testPluginProcessor->prepareToPlay(sampleRate, samplesPerBlock);

//     //Process the sine sweep, one chunk at a time
//     for (int i = 0; i < chunkAmount; i++) {
//         juce::AudioBuffer<float> processBuffer(buffer->getNumChannels(), samplesPerBlock);
//         for (int ch = 0; ch < buffer->getNumChannels(); ++ch) {
//             processBuffer.copyFrom(ch, 0, *buffer, ch, i * samplesPerBlock, samplesPerBlock);
//         }
//
//         testPluginProcessor->processBlock(processBuffer, midiBuffer);
//
//         for (int ch = 0; ch < buffer->getNumChannels(); ++ch) {
//             buffer->copyFrom(ch, i * samplesPerBlock, processBuffer, ch, 0, samplesPerBlock);
//         }
//     }

//     //Check that originalBuffer has higher total energy
//     CHECK_THAT(originalBuffer,
//                !AudioBufferHigherEnergy(*buffer));

//     juce::Array<float> maxEnergies;
//     for (int i = 0; i < fft_size / 2; i++) {
//         //Set the threshold for the lowest 32 frequency bands
//         if (i < 32) {
//             maxEnergies.set(i, 100);
//         }
//         //Skip the rest
//         else {
//             maxEnergies.set(i, -1);
//         }
//     }

//     //Check that lower end frequencies are within limits
//     CHECK_THAT(*buffer,
//                AudioBufferCheckMaxEnergy(maxEnergies));

}

TEST_CASE("Wet Parameter Demo", "[presentation]")
{
//     AudioPluginAudioProcessor *testPluginProcessor = new AudioPluginAudioProcessor();
//     juce::AudioBuffer<float> *buffer = Helpers::generateAudioSampleBuffer();
//     juce::AudioBuffer<float> originalBuffer(*buffer);

//     juce::MidiBuffer midiBuffer;
 
//     testPluginProcessor->prepareToPlay(44100, 4096);
//     testPluginProcessor->processBlock(*buffer, midiBuffer);

//     //Check that initial value is not zero, i.e. filtering happens
//     CHECK_THAT(*buffer,
//                !AudioBuffersMatch(originalBuffer));

//     buffer = Helpers::generateAudioSampleBuffer();

//     auto *parameters = testPluginProcessor->getParameters();
//     juce::RangedAudioParameter* pParam = parameters->getParameter ( "WET"  );
//     pParam->setValueNotifyingHost( 0.0f );

//     for (int ch = 0; ch < buffer->getNumChannels(); ++ch)
//         originalBuffer.copyFrom (ch, 0, *buffer, ch, 0, buffer->getNumSamples());
//     testPluginProcessor->processBlock(*buffer, midiBuffer);

//     //Check that filter now doesnt affect the audio signal
//     CHECK_THAT(*buffer,
//                AudioBuffersMatch(originalBuffer));

//     buffer = Helpers::generateAudioSampleBuffer();
//     pParam->setValueNotifyingHost( 1.0f );

//     for (int ch = 0; ch < buffer->getNumChannels(); ++ch)
//         originalBuffer.copyFrom (ch, 0, *buffer, ch, 0, buffer->getNumSamples());
//     testPluginProcessor->processBlock(*buffer, midiBuffer);

//     //Finally, check that with max wet the signal is again affected
//     CHECK_THAT(*buffer,
//                !AudioBuffersMatch(originalBuffer));

}

TEST_CASE("Draw Noise Buffer Demo", "[presentation]")
{
//     juce::AudioBuffer<float> *buffer = Helpers::generateBigAudioSampleBuffer();
//     ImageProcessing::drawAudioBufferImage(buffer, "Noise");

}

TEST_CASE("Processblock Benchmark Demo", "[presentation]")
{
//     AudioPluginAudioProcessor *testPluginProcessor = new AudioPluginAudioProcessor();
//     juce::AudioBuffer<float> *buffer = Helpers::generateAudioSampleBuffer();

//     juce::MidiBuffer midiBuffer;

//     testPluginProcessor->prepareToPlay(44100, 4096);

//     //Example of a simple benchmark
//     BENCHMARK("Plugin Processor Processblock") {
//         return testPluginProcessor->processBlock(*buffer, midiBuffer);
//     };

}

