#pragma once
#include "BaseEffects.h"
#include <cmath>

class BluesDriver : public BaseEffect {
public:
    float processSample(float sample) override {
        // Simple soft clipping with gain
        float driven = sample * gain;
        driven = (1.0f + tone * driven) / (1.0f + std::abs(tone * driven));  // tone-based shaping

        return driven * volume;
    }

    void setGain(float value) {
        gain = value;
    }

    void setTone(float value) {
        tone = value;
    }

    void setMix(float value) {
        volume = value;
    }

    void setParameter(float value) override {
        setGain(value);  // Default knob affects gain
    }

    void setSampleRate(double rate) override {}

private:
    float gain = 5.0f;
    float tone = 0.5f;
    float volume = 0.5f;
};
