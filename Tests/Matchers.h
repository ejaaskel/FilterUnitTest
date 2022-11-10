#include <juce_audio_processors/juce_audio_processors.h>
#include "settings.h"
juce::Array<float> calculateBinEnergies(const juce::dsp::FFT *forwardFFT, juce::AudioBuffer<float> in) {
        auto *inRead = in.getArrayOfReadPointers();

        std::array<float, fft_size> inFifo;
        std::array<float, fft_size * 2> inFftData;
        juce::Array<float> binEnergies;
        int fifoIndex = 0;
        int amountOfTransforms = 0;
        for (int ch = 0; ch < in.getNumChannels(); ++ch) {
            for (int sam = 0; sam < in.getNumSamples(); ++sam) {
                float inSample = inRead[ch][sam];
                if (fifoIndex == fft_size) {     // [8]
                    std::fill (inFftData.begin(), inFftData.end(), 0.0f);
                    std::copy (inFifo.begin(), inFifo.end(), inFftData.begin());
                    forwardFFT->performFrequencyOnlyForwardTransform (inFftData.data(), true);
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
	return binEnergies;

}


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

        juce::Array<float> inEnergies = calculateBinEnergies(&forwardFFT, in);
        juce::Array<float> othEnergies = calculateBinEnergies(&forwardFFT, otherBuffer);

	for (int i = 0; i < inEnergies.size(); i++) {
	    inPower += inEnergies[i];
	    othPower += othEnergies[i];
	}

        if (inPower < othPower) {
            return true;
        }
        else {
            return false;
        }
    }

    std::string describe() const override {
        std::ostringstream ss;
        ss << "Other buffer has higher total energy";
        return ss.str();
    }
private:
    juce::dsp::FFT forwardFFT;
    juce::dsp::WindowingFunction<float> window;

};

AudioBufferEnergyMatcher AudioBufferHigherEnergy(juce::AudioBuffer<float> other)
{
    return { other };
}


class AudioBufferMaxEnergyMatcher : public Catch::Matchers::MatcherBase<juce::AudioBuffer<float>> {
    juce::Array<float> maxEnergies;
public:
    AudioBufferMaxEnergyMatcher(juce::Array<float> other) : maxEnergies(other), forwardFFT (fft_order), window { fft_size, juce::dsp::WindowingFunction<float>::WindowingMethod::hann } {}

    bool match(juce::AudioBuffer<float> const& in) const override {
	juce::Array<float> binEnergies = calculateBinEnergies(&forwardFFT, in);


	for (int i = 0; i < maxEnergies.size(); i++) {
            if (maxEnergies[i] == -1) {
                continue; 
	    }
	    if (binEnergies[i] > maxEnergies[i]) {
	        return false;
	    }
	}
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

AudioBufferMaxEnergyMatcher AudioBufferCheckMaxEnergy(juce::Array<float> maxEnergiesArray)
{
    return { maxEnergiesArray };
}

class AudioBufferMinEnergyMatcher : public Catch::Matchers::MatcherBase<juce::AudioBuffer<float>> {
    juce::Array<float> minEnergies;
public:
    AudioBufferMinEnergyMatcher(juce::Array<float> other) : minEnergies(other), forwardFFT (fft_order), window { fft_size, juce::dsp::WindowingFunction<float>::WindowingMethod::hann } {}

    bool match(juce::AudioBuffer<float> const& in) const override {
	juce::Array<float> binEnergies = calculateBinEnergies(&forwardFFT, in);


	for (int i = 0; i < minEnergies.size(); i++) {
            if (minEnergies[i] == -1) {
                continue; 
	    }
	    if (binEnergies[i] < minEnergies[i]) {
	        return false;
	    }
	}
        return true;
    }

    std::string describe() const override {
        std::ostringstream ss;
        ss << "Energies too low";
        return ss.str();
    }
private:
    juce::dsp::FFT forwardFFT;
    juce::dsp::WindowingFunction<float> window;

};

AudioBufferMinEnergyMatcher AudioBufferCheckMinEnergy(juce::Array<float> minEnergiesArray)
{
    return { minEnergiesArray };
}

