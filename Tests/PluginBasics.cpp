#include <PluginProcessor.h>
#include <catch2/catch_all.hpp>
#include "Helpers.h"
#include "Matchers.h"

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

/*TEST_CASE("AudioBuffer is not higher than itself", "[dummy]")
{
  juce::AudioBuffer<float> *buffer = Helpers::generateAudioSampleBuffer();
  CHECK_THAT(*buffer,
             !AudioBufferHigherEnergy(*buffer));

}*/


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
  juce::AudioBuffer<float> originalBuffer(2, 4096);

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


  delete testPluginProcessor;
}

TEST_CASE("Filter Parameter", "[parameters]")
{
  testPluginProcessor = new AudioPluginAudioProcessor();
  juce::AudioBuffer<float> *buffer = Helpers::generateMaxAudioSampleBuffer();
  juce::AudioBuffer<float> originalBuffer(2, 4096);
//  CHECK_THAT(*buffer,
//             AudioBufferHigherEnergy(*buffer));

  for (int ch = 0; ch < buffer->getNumChannels(); ++ch)
          originalBuffer.copyFrom (ch, 0, *buffer, ch, 0, buffer->getNumSamples());

  juce::MidiBuffer midiBuffer;

  testPluginProcessor->prepareToPlay(44100, 4096);
  testPluginProcessor->processBlock(*buffer, midiBuffer);

//  CHECK_THAT(*buffer,
//             AudioBufferHigherEnergy(originalBuffer));
  CHECK_THAT(*buffer,
             AudioBufferHigherEnergy(*buffer));

  delete testPluginProcessor;
}

