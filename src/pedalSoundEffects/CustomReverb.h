#pragma once

#include "BaseEffects.h"
#include <JuceHeader.h>

class CustomReverb : public BaseEffect {
public:
    CustomReverb();
    float processSample(float input) override;
    void setParameter(float value) override;
    void setSampleRate(double rate) override;

    void setMix(float value);
    void setDecay(float value);  // maps to roomSize
    void setTone(float value);   // maps to damping

private:
    juce::Reverb reverb;
    juce::Reverb::Parameters params;
};
