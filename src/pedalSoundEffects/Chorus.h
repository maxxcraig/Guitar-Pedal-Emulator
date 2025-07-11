#pragma once
#include "BaseEffects.h"
#include <vector>
#include <cmath>
#include <algorithm>

class Chorus : public BaseEffect {
public:
    void setSampleRate(double rate) override {
        sampleRate = rate;
        int maxDelaySamples = static_cast<int>(sampleRate * maxDelayTime);
        delayBuffer.resize(maxDelaySamples, 0.0f);
    }

    float processSample(float inputSample) override {
        // Ensure delay buffer is initialized
        if (delayBuffer.empty()) return inputSample;
        
        // Write current input into the buffer
        delayBuffer[writeIndex] = inputSample;

        // Calculate modulated delay time in samples with safety bounds
        float lfo = std::sin(phase);
        float modulatedDelay = baseDelay + depth * lfo; // delay in seconds
        modulatedDelay = std::clamp(modulatedDelay, 0.001f, maxDelayTime - 0.001f);
        int delaySamples = static_cast<int>(modulatedDelay * sampleRate);
        delaySamples = std::clamp(delaySamples, 1, static_cast<int>(delayBuffer.size()) - 1);

        int readIndex = writeIndex - delaySamples;
        if (readIndex < 0)
            readIndex += delayBuffer.size();

        float delayedSample = delayBuffer[readIndex];
        
        // Clean up the delayed sample
        if (std::abs(delayedSample) > 0.9f) {
            delayedSample = std::tanh(delayedSample) * 0.9f;
        }

        // Advance write head and LFO
        writeIndex = (writeIndex + 1) % delayBuffer.size();
        phase += rate * 2.0f * juce::MathConstants<float>::pi / sampleRate;
        if (phase > juce::MathConstants<float>::twoPi)
            phase -= juce::MathConstants<float>::twoPi;

        // Mix dry and wet signal with cleaner level control
        return (1.0f - mix) * inputSample + mix * delayedSample * 0.8f;
    }

    void setParameter(float value) override {
        setDepth(value / 30.0f + 0.001f); // maps 0-10 to 0.001-0.334, more audible
    }

    void setRate(float newRate)   { rate = std::clamp(newRate, 0.2f, 1.5f); }
    void setDepth(float newDepth) { depth = std::clamp(newDepth, 0.001f, 0.008f); }
    void setMix(float newMix)     { mix = std::clamp(newMix, 0.0f, 0.8f); }

private:
    std::vector<float> delayBuffer;
    int writeIndex = 0;

    double sampleRate = 44100.0;
    float rate = 0.6f;            // Hz
    float depth = 0.003f;         // seconds (max ± offset to baseDelay)
    float baseDelay = 0.006f;     // seconds
    float mix = 0.5f;

    float phase = 0.0f;
    const float maxDelayTime = 0.025f; // max 25ms buffer
};
