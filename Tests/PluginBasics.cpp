#include <PluginProcessor.h>
#include <catch2/catch_all.hpp>
#include "Helpers.h"
#include "Matchers.h"
#include "ImageProcessing.h"

AudioPluginAudioProcessor *testPluginProcessor = nullptr;

TEST_CASE("one is equal to one", "[dummy]")
{
    REQUIRE(1 == 1);
}

TEST_CASE("AudioBuffer equals itself", "[dummy]")
{
    juce::AudioBuffer<float> *buffer = Helpers::generateAudioSampleBuffer();
    CHECK_THAT(*buffer,
               AudioBuffersMatch(*buffer));
}

TEST_CASE("AudioBuffer is not higher than itself", "[dummy]")
{
    juce::AudioBuffer<float> *buffer = Helpers::generateAudioSampleBuffer();
    CHECK_THAT(*buffer,
               !AudioBufferHigherEnergy(*buffer));
}

TEST_CASE("Draw Sine Sweep", "[dummy]")
{
    juce::MemoryMappedAudioFormatReader *reader = Helpers::readSineSweep();
    juce::AudioBuffer<float> *buffer = new juce::AudioBuffer<float>(reader->numChannels, reader->lengthInSamples);
    reader->read(buffer, reader->numChannels, reader->lengthInSamples, 0, true, true);
    ImageProcessing::drawAudioBufferImage(buffer, "SineSweep1");
}

TEST_CASE("Read and write buffer", "[dummy]")
{
    juce::AudioBuffer<float> *buffer = Helpers::generateAudioSampleBuffer();
    Helpers::writeBufferToFile(buffer, "test_file");
    juce::AudioBuffer<float> *readBuffer = Helpers::readBufferFromFile("test_file");
    CHECK_THAT(*buffer,
               AudioBuffersMatch(*readBuffer));
    juce::File test_file ("test_file");
    test_file.deleteFile();
}

TEST_CASE("Plugin instance name", "[name]")
{
    testPluginProcessor = new AudioPluginAudioProcessor();
    CHECK_THAT(testPluginProcessor->getName().toStdString(),
               Catch::Matchers::Equals("Filter Unit Test"));
    delete testPluginProcessor;
}

TEST_CASE("Wet Parameter", "[parameters]")
{
    testPluginProcessor = new AudioPluginAudioProcessor();
    juce::AudioBuffer<float> *buffer = Helpers::generateAudioSampleBuffer();
    juce::AudioBuffer<float> originalBuffer(2, buffer->getNumSamples());
    ImageProcessing::drawAudioBufferImage(buffer, "RandomWet");

    for (int ch = 0; ch < buffer->getNumChannels(); ++ch)
        originalBuffer.copyFrom (ch, 0, *buffer, ch, 0, buffer->getNumSamples());

    juce::MidiBuffer midiBuffer;

    testPluginProcessor->prepareToPlay(44100, 4096);
    testPluginProcessor->processBlock(*buffer, midiBuffer);

    CHECK_THAT(*buffer,
               !AudioBuffersMatch(originalBuffer));

    buffer = Helpers::generateAudioSampleBuffer();
    auto *parameters = testPluginProcessor->getParameters();
    juce::RangedAudioParameter* pParam = parameters->getParameter ( "WET"  );
    pParam->setValueNotifyingHost( 0.0f );

    for (int ch = 0; ch < buffer->getNumChannels(); ++ch)
        originalBuffer.copyFrom (ch, 0, *buffer, ch, 0, buffer->getNumSamples());
    testPluginProcessor->processBlock(*buffer, midiBuffer);

    CHECK_THAT(*buffer,
               AudioBuffersMatch(originalBuffer));

    buffer = Helpers::generateAudioSampleBuffer();
    pParam->setValueNotifyingHost( 1.0f );


    for (int ch = 0; ch < buffer->getNumChannels(); ++ch)
        originalBuffer.copyFrom (ch, 0, *buffer, ch, 0, buffer->getNumSamples());
    testPluginProcessor->processBlock(*buffer, midiBuffer);

    CHECK_THAT(*buffer,
               !AudioBuffersMatch(originalBuffer));

    ImageProcessing::drawAudioBufferImage(buffer, "RandomWet1");

    delete testPluginProcessor;
}


TEST_CASE("Big Wet Parameter", "[parameters]")
{
    testPluginProcessor = new AudioPluginAudioProcessor();
    juce::AudioBuffer<float> *buffer = Helpers::generateBigAudioSampleBuffer();
    juce::AudioBuffer<float> originalBuffer(2, buffer->getNumSamples());
    ImageProcessing::drawAudioBufferImage(buffer, "RandomWet");

    for (int ch = 0; ch < buffer->getNumChannels(); ++ch)
        originalBuffer.copyFrom (ch, 0, *buffer, ch, 0, buffer->getNumSamples());

    juce::MidiBuffer midiBuffer;

    int blockCount = buffer->getNumSamples() / 4096;

    testPluginProcessor->prepareToPlay(44100, 4096);
    for (int i = 0; i < blockCount; i++) {
        juce::AudioBuffer<float> *processBuffer = new juce::AudioBuffer<float>(1, 4096);
        processBuffer->copyFrom(0, 0, *buffer, 0, i * 4096, 4096);
        testPluginProcessor->processBlock(*processBuffer, midiBuffer);
        buffer->copyFrom(0, i * 4096, *processBuffer, 0, 0, 4096);
    }
    ImageProcessing::drawAudioBufferImage(buffer, "RandomWet1");

    CHECK_THAT(*buffer,
               !AudioBuffersMatch(originalBuffer));

    buffer = Helpers::generateBigAudioSampleBuffer();
    auto *parameters = testPluginProcessor->getParameters();
    juce::RangedAudioParameter* pParam = parameters->getParameter ( "WET"  );
    pParam->setValueNotifyingHost( 0.0f );

    for (int ch = 0; ch < buffer->getNumChannels(); ++ch)
        originalBuffer.copyFrom (ch, 0, *buffer, ch, 0, buffer->getNumSamples());
    for (int i = 0; i < blockCount; i++) {
        juce::AudioBuffer<float> *processBuffer = new juce::AudioBuffer<float>(1, 4096);
        processBuffer->copyFrom(0, 0, *buffer, 0, i * 4096, 4096);
        testPluginProcessor->processBlock(*processBuffer, midiBuffer);
        buffer->copyFrom(0, i * 4096, *processBuffer, 0, 0, 4096);
    }
    ImageProcessing::drawAudioBufferImage(buffer, "RandomWet2");

    CHECK_THAT(*buffer,
               AudioBuffersMatch(originalBuffer));

    buffer = Helpers::generateBigAudioSampleBuffer();
    pParam->setValueNotifyingHost( 1.0f );


    for (int ch = 0; ch < buffer->getNumChannels(); ++ch)
        originalBuffer.copyFrom (ch, 0, *buffer, ch, 0, buffer->getNumSamples());
    for (int i = 0; i < blockCount; i++) {
        juce::AudioBuffer<float> *processBuffer = new juce::AudioBuffer<float>(1, 4096);
        processBuffer->copyFrom(0, 0, *buffer, 0, i * 4096, 4096);
        testPluginProcessor->processBlock(*processBuffer, midiBuffer);
        buffer->copyFrom(0, i * 4096, *processBuffer, 0, 0, 4096);
    }

    CHECK_THAT(*buffer,
               !AudioBuffersMatch(originalBuffer));

    ImageProcessing::drawAudioBufferImage(buffer, "RandomWet3");

    delete testPluginProcessor;
}




TEST_CASE("Filter Parameter", "[parameters]")
{
    testPluginProcessor = new AudioPluginAudioProcessor();
    juce::MemoryMappedAudioFormatReader *reader = Helpers::readSineSweep();
    juce::AudioBuffer<float> *buffer = new juce::AudioBuffer<float>(reader->numChannels, reader->lengthInSamples);
    juce::AudioBuffer<float> originalBuffer(buffer->getNumChannels(), buffer->getNumSamples());
    reader->read(buffer->getArrayOfWritePointers(), 1, 0, reader->lengthInSamples);
    int chunkAmount = buffer->getNumSamples() / 4096;

    ImageProcessing::drawAudioBufferImage(buffer, "RandomFilter");


    for (int ch = 0; ch < buffer->getNumChannels(); ++ch)
        originalBuffer.copyFrom (ch, 0, *buffer, ch, 0, buffer->getNumSamples());

    CHECK_THAT(*buffer,
               AudioBuffersMatch(originalBuffer));


    juce::MidiBuffer midiBuffer;

    testPluginProcessor->prepareToPlay(44100, 4096);
    for (int i = 0; i < chunkAmount; i++) {
        juce::AudioBuffer<float> *processBuffer = new juce::AudioBuffer<float>(1, 4096);
        processBuffer->copyFrom(0, 0, *buffer, 0, i * 4096, 4096);

        testPluginProcessor->processBlock(*processBuffer, midiBuffer);
        buffer->copyFrom(0, i * 4096, *processBuffer, 0, 0, 4096);
    }

    CHECK_THAT(originalBuffer,
               AudioBufferHigherEnergy(*buffer));

    ImageProcessing::drawAudioBufferImage(buffer, "RandomFilter1");

    delete testPluginProcessor;
}

TEST_CASE("Processblock Benchmark", "[benchmarking]")
{
    testPluginProcessor = new AudioPluginAudioProcessor();
    juce::AudioBuffer<float> *buffer = Helpers::generateAudioSampleBuffer();
    juce::AudioBuffer<float> originalBuffer(2, buffer->getNumSamples());

    for (int ch = 0; ch < buffer->getNumChannels(); ++ch)
        originalBuffer.copyFrom (ch, 0, *buffer, ch, 0, buffer->getNumSamples());

    juce::MidiBuffer midiBuffer;

    testPluginProcessor->prepareToPlay(44100, 4096);

    //Example of a simple benchmark
    BENCHMARK("Plugin Processor Processblock") {
        return testPluginProcessor->processBlock(*buffer, midiBuffer);
    };

    //Example of an advanced benchmark with varying random input
    BENCHMARK_ADVANCED("Plugin Processor Processblock ADVANCED")(Catch::Benchmark::Chronometer meter) {
        int run_amount = meter.runs();
        juce::Array<juce::AudioBuffer<float>> v;
        for (int j = 0; j < meter.runs(); j++) {
            v.add(*Helpers::generateAudioSampleBuffer());
        }
        meter.measure([&v, midiBuffer] (int i) mutable { return testPluginProcessor->processBlock(v.getReference(i), midiBuffer); });
    };


    delete testPluginProcessor;
}

