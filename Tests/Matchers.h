#include <juce_audio_processors/juce_audio_processors.h>

#define FLOAT_ACCURACY 0.000001

class AudioBufferMatcher : public Catch::Matchers::MatcherBase<juce::AudioBuffer<float>> {
    juce::AudioBuffer<float> otherBuffer;
public:
    AudioBufferMatcher(juce::AudioBuffer<float> other) : otherBuffer(other) {}

    bool match(juce::AudioBuffer<float> const& in) const override {
        //return in >= m_begin && in <= m_end;
        auto *inRead = in.getArrayOfReadPointers();
        auto *othRead = otherBuffer.getArrayOfReadPointers();

        if (in.getNumChannels() != otherBuffer.getNumChannels() || in.getNumSamples() != otherBuffer.getNumSamples()) {
            return false;
        }

        for (int ch = 0; ch < in.getNumChannels(); ++ch) {
            for (int sam = 0; sam < in.getNumSamples(); ++sam) {
                if (std::fabs(inRead[ch][sam] - othRead[ch][sam]) > std::numeric_limits<float>::epsilon()) {
                    return false;
                }
            }
        }

        return true;
    }

    std::string describe() const override {
        std::ostringstream ss;
        ss << "AudioBuffers are the same ";
        return ss.str();
    }
};

AudioBufferMatcher AudioBuffersMatch(juce::AudioBuffer<float> other)
{
    return { other };
}

class AudioBufferEnergyMatcher : public Catch::Matchers::MatcherBase<juce::AudioBuffer<float>> {
    juce::AudioBuffer<float> otherBuffer;
public:
    AudioBufferEnergyMatcher(juce::AudioBuffer<float> other) : otherBuffer(other), forwardFFT (fftOrder), window { fftSize, juce::dsp::WindowingFunction<float>::WindowingMethod::hann } {}

    bool match(juce::AudioBuffer<float> const& in) const override {
        auto *inRead = in.getArrayOfReadPointers();
        auto *othRead = otherBuffer.getArrayOfReadPointers();

        std::array<float, fftSize> inFifo;
        std::array<float, fftSize * 2> inFftData;
        std::array<float, fftSize> othFifo;
        std::array<float, fftSize * 2> othFftData;
        int fifoIndex = 0;
        bool nextFFTBlockReady = false;
        int amountOfTransforms = 0;

        float inPower = 0;
        float othPower = 0;

        if (in.getNumChannels() != otherBuffer.getNumChannels() || in.getNumSamples() != otherBuffer.getNumSamples()) {
            return false;
        }


        for (int ch = 0; ch < in.getNumChannels(); ++ch) {
            for (int sam = 0; sam < in.getNumSamples(); ++sam) {
                float inSample = inRead[ch][sam];
                float othSample = othRead[ch][sam];
                if (fifoIndex == fftSize) {     // [8]
                    if (! nextFFTBlockReady) {  // [9]
                        std::fill (inFftData.begin(), inFftData.end(), 0.0f);
                        std::copy (inFifo.begin(), inFifo.end(), inFftData.begin());
                        std::fill (othFftData.begin(), othFftData.end(), 0.0f);
                        std::copy (othFifo.begin(), othFifo.end(), othFftData.begin());
                        nextFFTBlockReady = true;
                    }

                    fifoIndex = 0;
                }
                //WARN(fifoIndex);
                inFifo[(size_t) fifoIndex] = inSample;
                othFifo[(size_t) fifoIndex] = othSample;
                fifoIndex = fifoIndex + 1;
                if (nextFFTBlockReady) {
                    //window.multiplyWithWindowingTable (inFftData, fftSize);
                    forwardFFT.performFrequencyOnlyForwardTransform (inFftData.data(), true);
                    //forwardFFT.performRealOnlyForwardTransform(inFftData.data(), true);
                    forwardFFT.performFrequencyOnlyForwardTransform (othFftData.data(), true);
                    amountOfTransforms++;
                    for(long unsigned int fftIndex = 0; fftIndex < inFftData.size() / 2; fftIndex = fftIndex + 1) {
                        //WARN("IN  Value: " << inFftData[fftIndex] << "  " << fftIndex);
                        inPower += inFftData[fftIndex];
                    }
                    for(long unsigned int fftIndex = 0; fftIndex < othFftData.size() / 2; fftIndex = fftIndex + 1) {
                        //    WARN("OTH Value: " << othFftData[fftIndex]);
                        othPower += othFftData[fftIndex];
                    }
                    nextFFTBlockReady = false;
                    //WARN(amountOfTransforms);

                }
            }
        }
        if (inPower > othPower) {
            inPower = 0;
            othPower = 0;
            return true;
        } else {
            return false;
        }
        //return false;
    }

    std::string describe() const override {
        std::ostringstream ss;
        ss << "Other buffer has higher total energy";
        return ss.str();
    }

    static constexpr auto fftOrder = 10;
    static constexpr auto fftSize  = 1 << fftOrder;
private:

    juce::dsp::WindowingFunction<float> window;
    juce::dsp::FFT forwardFFT;

};

AudioBufferEnergyMatcher AudioBufferHigherEnergy(juce::AudioBuffer<float> other)
{
    return { other };
}

