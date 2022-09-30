#include <catch2/catch_all.hpp>
#include "ImageProcessing.h"

void ImageProcessing::drawAudioFormatReaderImage(juce::MemoryMappedAudioFormatReader *formatReader, juce::String imageName) {
    juce::Image spectrogramImage (juce::Image::RGB, 1000, 500, true);
    juce::dsp::FFT forwardFFT (fft_order);
    std::array<float, fft_size> fifo;
    std::array<float, fft_size * 2> *fftData = new std::array<float, fft_size * 2>();
    //float scopeData [scopeSize];
    int fifoIndex = 0;
 
    juce::int64 audioBlocks = formatReader->lengthInSamples / fft_size;
    for (int j = 0; j < audioBlocks; j++) {
        juce::AudioBuffer<float> *bufferToDraw = new juce::AudioBuffer<float>(formatReader->numChannels, fft_size);
        formatReader->read(bufferToDraw, 0, fft_size, fft_size * j, true, true);
        auto *bufferReadPointer = bufferToDraw->getReadPointer(0, 0);
        for (auto i = 0; i < bufferToDraw->getNumSamples(); ++i) {
            if (fifoIndex == fft_size)
            {
                std::fill (fftData->begin(), fftData->end(), 0.0f);
                std::copy (fifo.begin(), fifo.end(), fftData->begin());
                fifoIndex = 0;
                auto rightHandEdge = spectrogramImage.getWidth() - 1;
                auto imageHeight   = spectrogramImage.getHeight();
                spectrogramImage.moveImageSection (0, 0, 1, 0, rightHandEdge, imageHeight);
                forwardFFT.performFrequencyOnlyForwardTransform (fftData->data());
                auto maxLevel = juce::FloatVectorOperations::findMinAndMax (fftData->data(), fft_size / 2);

                for (auto y = 1; y < imageHeight; ++y)
                {
                    auto skewedProportionY = 1.0f - std::exp (std::log ((float) y / (float) imageHeight) * 0.2f);
                    auto fftDataIndex = (size_t) juce::jlimit (0, fft_size / 2, (int) (skewedProportionY * fft_size / 2));
                    auto level = juce::jmap (fftData->data()[fftDataIndex], 0.0f, juce::jmax (maxLevel.getEnd(), 1e-5f), 0.0f, 1.0f);
                    spectrogramImage.setPixelAt (rightHandEdge, y, juce::Colour::fromHSV (level, 1.0f, level, 1.0f));
                }
            }
            fifo[(size_t) fifoIndex++] = bufferReadPointer[i];
        }
    }
    juce::FileOutputStream stream (juce::File (imageName + ".png"));
    juce::PNGImageFormat pngWriter;
    pngWriter.writeImageToStream(spectrogramImage, stream);
}

float ImageProcessing::calculateFFTMaxValue(juce::AudioBuffer<float> *bufferToCalculate){
    juce::dsp::FFT forwardFFT (fft_order);
    juce::dsp::WindowingFunction<float> window (fft_size, juce::dsp::WindowingFunction<float>::hann);
    std::array<float, fft_size> fifo;
    std::array<float, fft_size * 2> *fftData = new std::array<float, fft_size * 2>();
    int fifoIndex = 0;
    float maxValue = -1;
    auto *bufferReadPointer = bufferToCalculate->getReadPointer(0, 0);
    for (auto i = 0; i < bufferToCalculate->getNumSamples(); ++i) {
        if (fifoIndex == fft_size)
        {
            std::fill (fftData->begin(), fftData->end(), 0.0f);
            std::copy (fifo.begin(), fifo.end(), fftData->begin());
            fifoIndex = 0;
            window.multiplyWithWindowingTable (fftData->data(), fft_size);
            forwardFFT.performFrequencyOnlyForwardTransform (fftData->data());
            auto maxLevel = juce::FloatVectorOperations::findMinAndMax (fftData->data(), fft_size / 2);
	    if (maxLevel.getEnd() > maxValue) {
	        maxValue = maxLevel.getEnd();
	    }
        }
        fifo[(size_t) fifoIndex++] = bufferReadPointer[i];
    }
    return maxValue;
}

void ImageProcessing::drawAudioBufferImage(juce::AudioBuffer<float> *bufferToDraw, juce::String imageName) {
    juce::Image spectrogramImage (juce::Image::RGB, 1000, 500, true);
    juce::dsp::FFT forwardFFT (fft_order);
    juce::dsp::WindowingFunction<float> window (fft_size, juce::dsp::WindowingFunction<float>::hann);
    std::array<float, fft_size> fifo;
    std::array<float, fft_size * 2> *fftData = new std::array<float, fft_size * 2>();
    int fifoIndex = 0;
    float maxValue = calculateFFTMaxValue(bufferToDraw); 
        auto *bufferReadPointer = bufferToDraw->getReadPointer(0, 0);
        for (auto i = 0; i < bufferToDraw->getNumSamples(); ++i) {
            if (fifoIndex == fft_size)
            {
                std::fill (fftData->begin(), fftData->end(), 0.0f);
                std::copy (fifo.begin(), fifo.end(), fftData->begin());
                fifoIndex = 0;
                auto rightHandEdge = spectrogramImage.getWidth() - 1;
                auto imageHeight   = spectrogramImage.getHeight();
                spectrogramImage.moveImageSection (0, 0, 1, 0, rightHandEdge, imageHeight);
		window.multiplyWithWindowingTable (fftData->data(), fft_size);
                forwardFFT.performFrequencyOnlyForwardTransform (fftData->data());
                auto maxLevel = juce::FloatVectorOperations::findMinAndMax (fftData->data(), fft_size / 2);
                WARN(maxLevel.getEnd());
                for (auto y = 1; y < imageHeight; ++y)
                {
                    auto skewedProportionY = 1.0f - std::exp (std::log ((float) y / (float) imageHeight) * 0.2f);
                    auto fftDataIndex = (size_t) juce::jlimit (0, fft_size / 2, (int) (skewedProportionY * fft_size / 2));
                    auto level = juce::jmap (fftData->data()[fftDataIndex], 0.0f, maxValue, 0.0f, 1.0f);
                    //auto level = juce::jmap (fftData->data()[fftDataIndex], 0.0f, juce::jmax (maxLevel.getEnd(), 1e-5f), 0.0f, 1.0f);
                    spectrogramImage.setPixelAt (rightHandEdge, y, juce::Colour::fromHSV (level, 1.0f, level, 1.0f));
                }
            }
            fifo[(size_t) fifoIndex++] = bufferReadPointer[i];
        }

    juce::FileOutputStream stream (juce::File (imageName + ".png"));
    juce::PNGImageFormat pngWriter;
    pngWriter.writeImageToStream(spectrogramImage, stream);
}

