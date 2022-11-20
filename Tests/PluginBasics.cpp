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
    juce::AudioBuffer<float> originalBuffer(*buffer);

    juce::MidiBuffer midiBuffer;

    testPluginProcessor->prepareToPlay(44100, 4096);
    testPluginProcessor->processBlock(*buffer, midiBuffer);

    //Check that initial value is not zero, i.e. filtering happens
    CHECK_THAT(*buffer,
               !AudioBuffersMatch(originalBuffer));

    delete buffer;

    buffer = Helpers::generateAudioSampleBuffer();
    auto *parameters = testPluginProcessor->getParameters();
    juce::RangedAudioParameter* pParam = parameters->getParameter ( "WET"  );
    pParam->setValueNotifyingHost( 0.0f );

    for (int ch = 0; ch < buffer->getNumChannels(); ++ch)
        originalBuffer.copyFrom (ch, 0, *buffer, ch, 0, buffer->getNumSamples());
    testPluginProcessor->processBlock(*buffer, midiBuffer);

    //Check that filter now doesnt affect the audio signal
    CHECK_THAT(*buffer,
               AudioBuffersMatch(originalBuffer));

    delete buffer;

    buffer = Helpers::generateAudioSampleBuffer();
    pParam->setValueNotifyingHost( 1.0f );


    for (int ch = 0; ch < buffer->getNumChannels(); ++ch)
        originalBuffer.copyFrom (ch, 0, *buffer, ch, 0, buffer->getNumSamples());
    testPluginProcessor->processBlock(*buffer, midiBuffer);

    //Finally, check that with max wet the signal is again affected
    CHECK_THAT(*buffer,
               !AudioBuffersMatch(originalBuffer));

    delete buffer;
    delete testPluginProcessor;
}


TEST_CASE("Big Buffer Wet Parameter", "[parameters]")
{
    testPluginProcessor = new AudioPluginAudioProcessor();
    juce::AudioBuffer<float> *buffer = Helpers::generateAudioSampleBuffer(2, 4096 * 256);
    juce::AudioBuffer<float> originalBuffer(*buffer);
    ImageProcessing::drawAudioBufferImage(buffer, "RandomWet");

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

    buffer = Helpers::generateAudioSampleBuffer(2, 4096 * 256);
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

    buffer = Helpers::generateAudioSampleBuffer(2, 4096 * 256);
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
TEST_CASE("Filter", "[functionality]")
{
    int samplesPerBlock = 4096;
    int sampleRate = 44100;
    testPluginProcessor = new AudioPluginAudioProcessor();
    juce::MemoryMappedAudioFormatReader *reader = Helpers::readSineSweep();
    juce::AudioBuffer<float> *buffer = new juce::AudioBuffer<float>(reader->numChannels, reader->lengthInSamples);
    reader->read(buffer->getArrayOfWritePointers(), 1, 0, reader->lengthInSamples);
    juce::AudioBuffer<float> originalBuffer(*buffer);

    //Dismiss the partial chunk for now
    int chunkAmount = buffer->getNumSamples() / samplesPerBlock;
    ImageProcessing::drawAudioBufferImage(&originalBuffer, "Filter");

    juce::MidiBuffer midiBuffer;

    testPluginProcessor->prepareToPlay(sampleRate, samplesPerBlock);

    //Process the sine sweep, one chunk at a time
    for (int i = 0; i < chunkAmount; i++) {
        juce::AudioBuffer<float> processBuffer(buffer->getNumChannels(), samplesPerBlock);
	for (int ch = 0; ch < buffer->getNumChannels(); ++ch) {
            processBuffer.copyFrom(0, 0, *buffer, ch, i * samplesPerBlock, samplesPerBlock);
        }

        testPluginProcessor->processBlock(processBuffer, midiBuffer);
	for (int ch = 0; ch < buffer->getNumChannels(); ++ch) {
            buffer->copyFrom(0, i * samplesPerBlock, processBuffer, ch, 0, samplesPerBlock);
	}
    }
    ImageProcessing::drawAudioBufferImage(&originalBuffer, "Postfilter");

    //Check that originalBuffer has higher total energy
    CHECK_THAT(originalBuffer,
               !AudioBufferHigherEnergy(*buffer));

    juce::Array<float> maxEnergies;
    for (int i = 0; i < fft_size / 2; i++) {
        //Set the threshold for the lowest 32 frequency bands
        if (i < 32) {
            maxEnergies.set(i, 100);
	}
	//Skip the rest
	else {
            maxEnergies.set(i, -1);
	
	}
    }
    //Check that lower end frequencies are within limits
    CHECK_THAT(*buffer,
               AudioBufferCheckMaxEnergy(maxEnergies));

    //I guess programming C++ like this in the year 2022 isn't a good idea to do publicly
    delete buffer;
    delete reader;
    delete testPluginProcessor;
}

TEST_CASE("Filter Parameter", "[parameters]")
{
    testPluginProcessor = new AudioPluginAudioProcessor();
    juce::MemoryMappedAudioFormatReader *reader = Helpers::readSineSweep();
    juce::AudioBuffer<float> *buffer = new juce::AudioBuffer<float>(reader->numChannels, reader->lengthInSamples);
    reader->read(buffer->getArrayOfWritePointers(), 1, 0, reader->lengthInSamples);
    juce::AudioBuffer<float> originalBuffer(*buffer);
    int chunkAmount = buffer->getNumSamples() / 4096;

    ImageProcessing::drawAudioBufferImage(buffer, "RandomFilter");

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
               !AudioBufferHigherEnergy(*buffer));

    juce::Array<float> maxEnergies;
    for (int i = 0; i < fft_size / 2; i++) {
        if (i < 32) {
            maxEnergies.set(i, 100);
	}
	else {
            maxEnergies.set(i, -1);
	
	}
    }
    CHECK_THAT(*buffer,
               AudioBufferCheckMaxEnergy(maxEnergies));

    juce::Array<float> minEnergies;
    for (int i = 0; i < fft_size / 2; i++) {
        if (i > 64 && i < fft_size / 2 - 64) {
            minEnergies.set(i, 100);
	}
	else {
            minEnergies.set(i, -1);
	}
    }
    CHECK_THAT(*buffer,
               AudioBufferCheckMinEnergy(minEnergies));

    ImageProcessing::drawAudioBufferImage(buffer, "RandomFilter1");

    delete testPluginProcessor;
}


TEST_CASE("Processblock Benchmark", "[benchmarking]")
{
    testPluginProcessor = new AudioPluginAudioProcessor();
    juce::AudioBuffer<float> *buffer = Helpers::generateAudioSampleBuffer();

    juce::MidiBuffer midiBuffer;

    testPluginProcessor->prepareToPlay(44100, 4096);

    //Example of a simple benchmark
    BENCHMARK("Plugin Processor Processblock") {
        return testPluginProcessor->processBlock(*buffer, midiBuffer);
    };

    //Example of an advanced benchmark with varying random input
    BENCHMARK_ADVANCED("Plugin Processor Processblock ADVANCED")(Catch::Benchmark::Chronometer meter) {
        juce::Array<juce::AudioBuffer<float>> v;
        for (int j = 0; j < meter.runs(); j++) {
            v.add(*Helpers::generateAudioSampleBuffer());
        }
        meter.measure([&v, midiBuffer] (int i) mutable { return testPluginProcessor->processBlock(v.getReference(i), midiBuffer); });
    };

    delete buffer;
    delete testPluginProcessor;
}

