#include <catch2/catch_all.hpp>
#include "ImageProcessing.h"

void ImageProcessing::drawAudioFormatReaderImage(juce::MemoryMappedAudioFormatReader *formatReader, juce::String imageName) {
    juce::Image spectrogramImage (juce::Image::RGB, 1000, 500, true);
    juce::dsp::FFT forwardFFT (fft_order);                          // [3]
    std::array<float, fft_size> fifo;                    // [4]
    std::array<float, fft_size * 2> *fftData = new std::array<float, fft_size * 2>();   // [5]
    int fifoIndex = 0;                                  // [6]
    bool nextFFTBlockReady = false;                     // [7]
 
    WARN(INT_MAX);
    WARN(formatReader->lengthInSamples);
    int audioBlocks = formatReader->lengthInSamples / fft_size;
    int blocksRead = 0;
    int chunksRead = 1;
for (int j = 0; j < audioBlocks; j++) {
    juce::AudioBuffer<float> *bufferToDraw = new juce::AudioBuffer<float>(formatReader->numChannels, fft_size);
    formatReader->read(bufferToDraw, 0, fft_size, fft_size * j, true, true);
    auto *bufferReadPointer = bufferToDraw->getReadPointer(0, 0);
    for (auto i = 0; i < bufferToDraw->getNumSamples(); ++i) {
        if (fifoIndex == fft_size)       // [8]
        {
            if (! nextFFTBlockReady)    // [9]
            {
                std::fill (fftData->begin(), fftData->end(), 0.0f);
                std::copy (fifo.begin(), fifo.end(), fftData->begin());
                nextFFTBlockReady = true;
            }
            fifoIndex = 0;
            if (nextFFTBlockReady) {
                auto rightHandEdge = spectrogramImage.getWidth() - 1;
                auto imageHeight   = spectrogramImage.getHeight();
                spectrogramImage.moveImageSection (0, 0, 1, 0, rightHandEdge, imageHeight);         // [1]
                forwardFFT.performFrequencyOnlyForwardTransform (fftData->data());                   // [2]
                auto maxLevel = juce::FloatVectorOperations::findMinAndMax (fftData->data(), fft_size / 2); // [3]
 
                for (auto y = 1; y < imageHeight; ++y)                                              // [4]
                {
		     /*int fftDataIndex = juce::jlimit(0, fft_size / 2, (int)(((fft_size) / 2.0f) / imageHeight * y));
                     auto level = juce::jmap ((*fftData)[fftDataIndex], 0.0f, juce::jmax (maxLevel.getEnd(), 1e-5f), 0.0f, 1.0f);
                     spectrogramImage.setPixelAt (rightHandEdge, y, juce::Colour::fromHSV (level, 1.0f, level, 1.0f)); // [5]*/
            auto skewedProportionY = 1.0f - std::exp (std::log ((float) y / (float) imageHeight) * 0.2f);
            auto fftDataIndex = (size_t) juce::jlimit (0, fft_size / 2, (int) (skewedProportionY * fft_size / 2));
            auto level = juce::jmap (fftData->data()[fftDataIndex], 0.0f, juce::jmax (maxLevel.getEnd(), 1e-5f), 0.0f, 1.0f);
 
            spectrogramImage.setPixelAt (rightHandEdge, y, juce::Colour::fromHSV (level, 1.0f, level, 1.0f)); // [5]

                }



                nextFFTBlockReady = false;
            }
        }
	blocksRead++;
        fifo[(size_t) fifoIndex++] = bufferReadPointer[i]; // [9]
    }
    /*if (blocksRead >= formatReader->sampleRate) {
        formatReader->mapSectionOfFile(juce::Range<juce::int64> (chunksRead * formatReader->sampleRate, formatReader->sampleRate * (chunksRead + 1)));
	blocksRead = 0;
	chunksRead = chunksRead + 1;
    }*/
    //std::fill(fifo.begin(), fifo.end(), 0.0f);
    /*if (j % 16 == 0) {
        WARN("Return value");
        std::fill(fftData->begin(), fftData->end(), 0.0f);
     }*/
}
    juce::FileOutputStream stream (juce::File (imageName + ".png"));
    juce::PNGImageFormat pngWriter;
    pngWriter.writeImageToStream(spectrogramImage, stream);
}

void ImageProcessing::drawAudioBufferImage(juce::AudioBuffer<float>* bufferToDraw, juce::String imageName) {

    juce::dsp::FFT forwardFFT (fft_order);                          // [3]
    juce::Image spectrogramImage (juce::Image::RGB, 1000, 500, true);
 
    std::array<float, fft_size> fifo;                    // [4]
    std::array<float, fft_size * 2> *fftData = new std::array<float, fft_size * 2>();   // [5]
    int fifoIndex = 0;                                  // [6]
    bool nextFFTBlockReady = false;                     // [7]
    WARN(INT_MAX);
    auto *bufferReadPointer = bufferToDraw->getReadPointer(0, 0);
    //int multiplier = 0;
    //auto* channelData = bufferToDraw->buffer->getReadPointer (0, bufferToDraw->startSample); 
    for (auto i = 0; i < bufferToDraw->getNumSamples(); ++i) {
    //for (auto i = 0; i < 1024 * 64; ++i) {
        if (fifoIndex == fft_size)       // [8]
        {
            if (! nextFFTBlockReady)    // [9]
            {
                std::fill (fftData->begin(), fftData->end(), 0.0f);
                std::copy (fifo.begin(), fifo.end(), fftData->begin());
                nextFFTBlockReady = true;
            }
            fifoIndex = 0;
            if (nextFFTBlockReady) {

                //TODO: Draw the line to the image
                //multiplier = (++multiplier) % 16;
                auto rightHandEdge = spectrogramImage.getWidth() - 1;
                auto imageHeight   = spectrogramImage.getHeight();
 
                // first, shuffle our image leftwards by 1 pixel..
                spectrogramImage.moveImageSection (0, 0, 1, 0, rightHandEdge, imageHeight);         // [1]
 
                // then render our FFT data..
                forwardFFT.performFrequencyOnlyForwardTransform (fftData->data());                   // [2]
 
                // find the range of values produced, so we can scale our rendering to
                // show up the detail clearly
                auto maxLevel = juce::FloatVectorOperations::findMinAndMax (fftData->data(), fft_size / 2); // [3]
 
                for (auto y = 1; y < imageHeight; ++y)                                              // [4]
                {
	             //WARN("DRAWING\n");
                     /*auto skewedProportionY = 1.0f - std::exp (std::log ((float) y / (float) imageHeight) * 0.2f);
                     auto fftDataIndex = (size_t) juce::jlimit (0, fft_size / 2, (int) (skewedProportionY * ((int)fft_size) / 2));*/
		     int fftDataIndex = juce::jlimit(0, fft_size / 2, (int)(((fft_size) / 2.0f) / imageHeight * y));
		     //WARN(fftDataIndex);
                     auto level = juce::jmap ((*fftData)[fftDataIndex], 0.0f, juce::jmax (maxLevel.getEnd(), 1e-5f), 0.0f, 1.0f);
                     //WARN(level << "\n"); 
                     spectrogramImage.setPixelAt (rightHandEdge, y, juce::Colour::fromHSV (level, 1.0f, level, 1.0f)); // [5]
                }



                nextFFTBlockReady = false;
            }
        }
        //fifo[(size_t) fifoIndex++] = bufferToDraw->getSample(0, fifoIndex + (multiplier * 1024)); // [9]
        //fifo[(size_t) fifoIndex++] = bufferToDraw->getSample(0, i); // [9]
	WARN(i);
	WARN(bufferReadPointer[i]);
        fifo[(size_t) fifoIndex++] = bufferReadPointer[i]; // [9]
    }
    juce::FileOutputStream stream (juce::File (imageName + ".png"));
    juce::PNGImageFormat pngWriter;
    pngWriter.writeImageToStream(spectrogramImage, stream);
}
