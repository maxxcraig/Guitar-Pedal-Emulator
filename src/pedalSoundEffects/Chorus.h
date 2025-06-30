#pragma once
#include "BaseEffects.h"
#include <vector>
#include <cmath>

class Chorus : public BaseEffect {
public:
    void setSampleRate(double rate) override {
        sampleRate = rate;
        int maxDelaySamples = static_cast<int>(sampleRate * maxDelayTime);
        delayBuffer.resize(maxDelaySamples, 0.0f);
    }

    float processSample(float inputSample) override {
        // Write current input into the buffer
        delayBuffer[writeIndex] = inputSample;

        // Calculate modulated delay time in samples
        float lfo = std::sin(phase);
        float modulatedDelay = baseDelay + depth * lfo; // delay in seconds
        int delaySamples = static_cast<int>(modulatedDelay * sampleRate);

        int readIndex = writeIndex - delaySamples;
        if (readIndex < 0)
            readIndex += delayBuffer.size();

        float delayedSample = delayBuffer[readIndex];

        // Advance write head and LFO
        writeIndex = (writeIndex + 1) % delayBuffer.size();
        phase += rate * 2.0f * juce::MathConstants<float>::pi / sampleRate;
        if (phase > juce::MathConstants<float>::twoPi)
            phase -= juce::MathConstants<float>::twoPi;

        // Mix dry and wet signal
        return (1.0f - mix) * inputSample + mix * delayedSample;
    }

    void setParameter(float value) override {
        setDepth(value / 10.0f);
    }

    void setRate(float newRate)   { rate = newRate; }
    void setDepth(float newDepth) { depth = newDepth; }
    void setMix(float newMix)     { mix = newMix; }

private:
    std::vector<float> delayBuffer;
    int writeIndex = 0;

    double sampleRate = 44100.0;
    float rate = 0.25f;            // Hz
    float depth = 0.002f;          // seconds (max ± offset to baseDelay)
    float baseDelay = 0.005f;      // seconds
    float mix = 0.5f;

    float phase = 0.0f;
    const float maxDelayTime = 0.03f; // max 30ms buffer
};
