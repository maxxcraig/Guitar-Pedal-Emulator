#pragma once
#include "BaseEffects.h"
#include <cmath>

class Tremolo : public BaseEffect {
public:
    float processSample(float sample) override {
        float mod = (std::sin(phase) + 1.0f) / 2.0f;
        phase += rate * 2.0f * juce::MathConstants<float>::pi / sampleRate;
        if (phase >= juce::MathConstants<float>::twoPi)
            phase -= juce::MathConstants<float>::twoPi;

        float tremSample = sample * ((1.0f - depth) + mod * depth);
        return (1.0f - mix) * sample + mix * tremSample * volume;
    }

    void setDepth(float d) { depth = d; }
    void setMix(float m) { mix = m; }
    void setVolume(float v) { volume = v; }
    void setRate(float r) { rate = r; }

    void setParameter(float value) override {
        depth = value / 10.0f;
    }

    void setSampleRate(double rateIn) override {
        sampleRate = rateIn;
    }

private:
    float rate = 5.0f;      // Tremolo speed in Hz (optional to expose in UI)
    float depth = 0.5f;     // How deep the tremolo modulates amplitude
    float mix = 0.5f;       // Wet/dry mix
    float volume = 1.0f;    // Output volume
    float phase = 0.0f;
    double sampleRate = 44100.0;
};
