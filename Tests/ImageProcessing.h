#include <juce_audio_processors/juce_audio_processors.h>
#include "juce_dsp/juce_dsp.h"
#include "juce_graphics/juce_graphics.h"
//#include "juce_core/juce_core.h"
#include "Settings.h"

class ImageProcessing {
    public:
        static void drawAudioFormatReaderImage(juce::MemoryMappedAudioFormatReader* reader, juce::String imageName);
        static float calculateFFTMaxValue(juce::AudioBuffer<float>* bufferToCalculate);
        static void drawAudioBufferImage(juce::AudioBuffer<float>* bufferToDraw, juce::String imageName);
    private:
        static void drawScale(juce::Image spectrogramImage);
};
