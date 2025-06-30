#pragma once
#include "BaseEffects.h"
#include <cmath>

class Phaser : public BaseEffect {
public:
    float processSample(float sample) override {
        float mod = std::sin(phase) * depth * mix;
        phase += rate * 2.0f * juce::MathConstants<float>::pi / sampleRate;
        if (phase > juce::MathConstants<float>::twoPi)
            phase -= juce::MathConstants<float>::twoPi;

        return sample * (1.0f + mod);
    }

    void setParameter(float value) override {
        setDepth(value);
    }

    void setDepth(float value) { depth = value / 10.0f; }
    void setRate(float value) { rate = value; }
    void setMix(float value) { mix = value; }

    void setSampleRate(double rateIn) override {
        sampleRate = rateIn;
    }

private:
    float depth = 0.5f;
    float rate = 0.5f;
    float mix = 0.5f;
    float phase = 0.0f;
    double sampleRate = 44100.0;
};
