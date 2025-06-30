#pragma once
#include "BaseEffects.h"
#include <cmath>

class Distortion : public BaseEffect {
public:
    float processSample(float sample) override {
        float shaped = std::tanh(sample * gain);

        // Apply simple tone control: blend of bright (original) and dark (filtered)
        float dark = shaped * (1.0f - tone);         // darkens the signal
        float bright = shaped * tone;                // brightens the signal
        float toned = dark + bright;

        return toned * volume;
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
        setGain(value);  // Default knob
    }

    void setSampleRate(double rate) override {}

private:
    float gain = 5.0f;
    float tone = 0.5f;
    float volume = 0.5f;
};
