#include <catch2/catch_all.hpp>
#include "ImageProcessing.h"

void ImageProcessing::drawAudioFormatReaderImage(juce::MemoryMappedAudioFormatReader *formatReader, juce::String imageName)
{
    if(!draw_images) {
        return;
    }

    juce::Image spectrogramImage (juce::Image::RGB, 1000, 500, true);
    juce::dsp::FFT forwardFFT (fft_order);
    std::array<float, fft_size> fifo;
    std::array<float, fft_size * 2> *fftData = new std::array<float, fft_size * 2>();
    int fifoIndex = 0;

    juce::int64 audioBlocks = formatReader->lengthInSamples / fft_size;
    for (int j = 0; j < audioBlocks; j++) {
        juce::AudioBuffer<float> *bufferToDraw = new juce::AudioBuffer<float>(formatReader->numChannels, fft_size);
        formatReader->read(bufferToDraw, 0, fft_size, fft_size * j, true, true);
        auto *bufferReadPointer = bufferToDraw->getReadPointer(0, 0);
        for (auto i = 0; i < bufferToDraw->getNumSamples(); ++i) {
            if (fifoIndex == fft_size) {
                std::fill (fftData->begin(), fftData->end(), 0.0f);
                std::copy (fifo.begin(), fifo.end(), fftData->begin());
                fifoIndex = 0;
                auto rightHandEdge = spectrogramImage.getWidth() - 1;
                auto imageHeight   = spectrogramImage.getHeight();
                spectrogramImage.moveImageSection (0, 0, 1, 0, rightHandEdge, imageHeight);
                forwardFFT.performFrequencyOnlyForwardTransform (fftData->data());
                auto maxLevel = juce::FloatVectorOperations::findMinAndMax (fftData->data(), fft_size / 2);

                for (auto y = 1; y < imageHeight; ++y) {
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

float ImageProcessing::calculateFFTMaxValue(juce::AudioBuffer<float> *bufferToCalculate)
{
    juce::dsp::FFT forwardFFT (fft_order);
    juce::dsp::WindowingFunction<float> window (fft_size, juce::dsp::WindowingFunction<float>::hann);
    std::array<float, fft_size> fifo;
    std::array<float, fft_size * 2> *fftData = new std::array<float, fft_size * 2>();
    int fifoIndex = 0;
    float maxValue = -1;
    auto *bufferReadPointer = bufferToCalculate->getReadPointer(0, 0);
    for (auto i = 0; i < bufferToCalculate->getNumSamples(); ++i) {
        if (fifoIndex == fft_size) {
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

void ImageProcessing::drawScale(juce::Image spectrogramImage)
{
    auto rightHandEdge = spectrogramImage.getWidth() - 1;
    auto imageHeight   = spectrogramImage.getHeight();

    //Draw scale to right hand side
    float tenKlimit = 10000.0f / (44100.0f / 2.0f);
    float klimit = 1000.0f / (44100.0f / 2.0f);
    float hundredlimit = 100.0f / (44100.0f / 2.0f);
    for (auto x = 0; x < 1; x++) {
        juce::Graphics spectrogramGraphics(spectrogramImage);
        spectrogramGraphics.setColour(juce::Colours::white);
        spectrogramGraphics.setFont (20.0f);
        int klimitCrossed = 0;
        int hundredlimitCrossed = 0;
        int tenKlimitCrossed = 0;
        spectrogramImage.moveImageSection (0, 0, 1, 0, rightHandEdge, imageHeight);
        for (auto y = 1; y < imageHeight; ++y) {
            auto skewedProportionY = 1.0f - std::exp (std::log ((float) y / (float) imageHeight) * 0.3f);
            if (skewedProportionY < tenKlimit && !tenKlimitCrossed) {
                spectrogramGraphics.drawText("10k", spectrogramImage.getWidth() - 70, y -25, 50, 50, juce::Justification::right, false);
                tenKlimitCrossed = 1;
            } else if (skewedProportionY < klimit && !klimitCrossed) {
                spectrogramGraphics.drawText("1k", spectrogramImage.getWidth() - 70, y -25, 50, 50, juce::Justification::right, false);
                klimitCrossed = 1;
            } else if (skewedProportionY < hundredlimit && !hundredlimitCrossed) {
                spectrogramGraphics.drawText("100", spectrogramImage.getWidth() - 70, y - 25, 50, 50, juce::Justification::right, false);
                hundredlimitCrossed = 1;
            }
        }
    }


    for (int i = 0; i < 10; i++) {
        int klimitCrossed = 0;
        int hundredlimitCrossed = 0;
        int tenKlimitCrossed = 0;
        spectrogramImage.moveImageSection (0, 0, 1, 0, rightHandEdge, imageHeight);
        for (auto y = 1; y < imageHeight; ++y) {
            auto skewedProportionY = 1.0f - std::exp (std::log ((float) y / (float) imageHeight) * 0.3f);
            if (skewedProportionY < tenKlimit && !tenKlimitCrossed) {
                spectrogramImage.setPixelAt (rightHandEdge, y, juce::Colour::fromHSV (1.0f, 0.0f, 1.0f, 1.0f));
                tenKlimitCrossed = 1;
            } else if (skewedProportionY < klimit && !klimitCrossed) {
                spectrogramImage.setPixelAt (rightHandEdge, y, juce::Colour::fromHSV (1.0f, 0.0f, 1.0f, 1.0f));
                klimitCrossed = 1;
            } else if (skewedProportionY < hundredlimit && !hundredlimitCrossed) {
                spectrogramImage.setPixelAt (rightHandEdge, y, juce::Colour::fromHSV (1.0f, 0.0f, 1.0f, 1.0f));
                hundredlimitCrossed = 1;
            }
        }
    }

}


void ImageProcessing::drawAudioBufferImage(juce::AudioBuffer<float> *bufferToDraw, juce::String imageName)
{
    if(!draw_images) {
        return;
    }
    int imageWidth = bufferToDraw->getNumSamples() / fft_size + 200 + 5;
    juce::Image spectrogramImage (juce::Image::RGB, imageWidth, 1000, true);
    auto rightHandEdge = spectrogramImage.getWidth() - 1;
    auto imageHeight   = spectrogramImage.getHeight();

    juce::dsp::FFT forwardFFT (fft_order);
    juce::dsp::WindowingFunction<float> window (fft_size, juce::dsp::WindowingFunction<float>::hann);
    std::array<float, fft_size> fifo;
    std::array<float, fft_size * 2> *fftData = new std::array<float, fft_size * 2>();
    int fifoIndex = 0;
    float maxValue = calculateFFTMaxValue(bufferToDraw);
    auto *bufferReadPointer = bufferToDraw->getReadPointer(0, 0);

    drawScale(spectrogramImage);

    for (auto i = 0; i < bufferToDraw->getNumSamples(); ++i) {
        if (fifoIndex == fft_size) {
            std::fill (fftData->begin(), fftData->end(), 0.0f);
            std::copy (fifo.begin(), fifo.end(), fftData->begin());
            fifoIndex = 0;
            spectrogramImage.moveImageSection (0, 0, 1, 0, rightHandEdge, imageHeight);
            window.multiplyWithWindowingTable (fftData->data(), fft_size);
            forwardFFT.performFrequencyOnlyForwardTransform (fftData->data());
            auto maxLevel = juce::FloatVectorOperations::findMinAndMax (fftData->data(), fft_size / 2);
            for (auto y = 1; y < imageHeight; ++y) {
                auto skewedProportionY = 1.0f - std::exp (std::log ((float) y / (float) imageHeight) * 0.3f);
                auto fftDataIndex = (size_t) juce::jlimit (0, fft_size / 2, (int) (skewedProportionY * fft_size / 2));
                auto level = juce::jmap (fftData->data()[fftDataIndex], 0.0f, maxValue, 0.0f, 1.0f);
                spectrogramImage.setPixelAt (rightHandEdge, y, juce::Colour::fromHSV (level, 1.0f, level, 1.0f));
            }
        }
        fifo[(size_t) fifoIndex++] = bufferReadPointer[i];
    }

    juce::FileOutputStream stream (juce::File (imageName + ".png"));
    juce::PNGImageFormat pngWriter;
    pngWriter.writeImageToStream(spectrogramImage, stream);
}

