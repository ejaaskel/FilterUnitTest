#include <juce_audio_processors/juce_audio_processors.h>
#include "settings.h"


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
    AudioBufferEnergyMatcher(juce::AudioBuffer<float> other) : otherBuffer(other), forwardFFT (fft_order), window { fft_size, juce::dsp::WindowingFunction<float>::WindowingMethod::hann } {}

    bool match(juce::AudioBuffer<float> const& in) const override {
        auto *inRead = in.getArrayOfReadPointers();
        auto *othRead = otherBuffer.getArrayOfReadPointers();

        std::array<float, fft_size> inFifo;
        std::array<float, fft_size * 2> inFftData;
        std::array<float, fft_size> othFifo;
        std::array<float, fft_size * 2> othFftData;
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
                if (fifoIndex == fft_size) {     // [8]
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
                    //window.multiplyWithWindowingTable (inFftData, fft_size);
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

//    static constexpr auto fft_order = 10;
//    static constexpr auto fft_size  = 1 << fft_order;
private:
    juce::dsp::FFT forwardFFT;
    juce::dsp::WindowingFunction<float> window;

};

AudioBufferEnergyMatcher AudioBufferHigherEnergy(juce::AudioBuffer<float> other)
{
    return { other };
}

juce::Array<float> calculateBinEnergies() {

}

class AudioBufferMaxEnergyMatcher : public Catch::Matchers::MatcherBase<juce::AudioBuffer<float>> {
    juce::Array<float> maxEnergies;
public:
    AudioBufferMaxEnergyMatcher(juce::Array<float> other) : maxEnergies(other), forwardFFT (fft_order), window { fft_size, juce::dsp::WindowingFunction<float>::WindowingMethod::hann } {}

    bool match(juce::AudioBuffer<float> const& in) const override {
        auto *inRead = in.getArrayOfReadPointers();

        std::array<float, fft_size> inFifo;
        std::array<float, fft_size * 2> inFftData;
	juce::Array<float> binEnergies;
        int fifoIndex = 0;
        bool nextFFTBlockReady = false;
        int amountOfTransforms = 0;


        for (int ch = 0; ch < in.getNumChannels(); ++ch) {
            for (int sam = 0; sam < in.getNumSamples(); ++sam) {
                float inSample = inRead[ch][sam];
                if (fifoIndex == fft_size) {     // [8]
                    std::fill (inFftData.begin(), inFftData.end(), 0.0f);
                    std::copy (inFifo.begin(), inFifo.end(), inFftData.begin());
                    nextFFTBlockReady = true;
                    forwardFFT.performFrequencyOnlyForwardTransform (inFftData.data(), true);
                    amountOfTransforms++;
                    for(long unsigned int fftIndex = 0; fftIndex < inFftData.size() / 2; fftIndex = fftIndex + 1) {
                        binEnergies.set(fftIndex, binEnergies[fftIndex] + inFftData[fftIndex]);
			
                    }

                    fifoIndex = 0;
                }
                inFifo[(size_t) fifoIndex] = inSample;
                fifoIndex = fifoIndex + 1;
            }
        }
	for (int i = 0; i < maxEnergies.size(); i++) {
            if (maxEnergies[i] == -1) {
                continue; 
	    }
	    if (binEnergies[i] > maxEnergies[i]) {
	        return false;
	    }
	}
	for (int i =0; i < fft_size / 2; i++) {

            WARN(" " << i << " " << binEnergies[i]);	
	}
	WARN(fft_size);
	WARN(maxEnergies.size());
        return true;
    }

    std::string describe() const override {
        std::ostringstream ss;
        ss << "Energies too high";
        return ss.str();
    }
private:
    juce::dsp::FFT forwardFFT;
    juce::dsp::WindowingFunction<float> window;

};

AudioBufferMaxEnergyMatcher AudioBufferMaxerEnergy(juce::Array<float> maxEnergiesThing)
{
    return { maxEnergiesThing };
}


