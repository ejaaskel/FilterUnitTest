#include "juce_dsp/juce_dsp.h"

template <typename Type>
class Filter
{
public:
    //==============================================================================
    Filter() {
        auto& preFilter = processorChain.template get<preFilterIndex>();
        preFilter.setMode(juce::dsp::LadderFilterMode::HPF12);
        preFilter.setCutoffFrequencyHz (10);
        preFilter.setResonance         (0.5f);
        preFilter.setDrive (juce::Decibels::decibelsToGain (0));
    }

    //==============================================================================
    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        processorChain.prepare(spec);
	sampleRate = spec.sampleRate;
    }

    //==============================================================================
    template <typename ProcessContext>
    void process (const ProcessContext& context) noexcept
    {
        processorChain.process (context);
    }

    //==============================================================================
    void reset() noexcept {
        processorChain.reset();
    }

private:
    //==============================================================================
    enum
    {
        preFilterIndex,
    };

    float sampleRate;

    juce::dsp::ProcessorChain<juce::dsp::LadderFilter<Type>
                              > processorChain;
};
