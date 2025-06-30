#pragma once
#include "BaseEffects.h"

class Overdrive : public BaseEffect {
public:
    Overdrive();

    void setGain(float newGain);
    float processSample(float inputSample) override;
    void setParameter(float value) override {
        setGain(value);
    }

private:
    float gain = 1.0f;
};
