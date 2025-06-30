#pragma once
#include "BaseEffects.h"
#include <vector>

class Delay : public BaseEffect {
public:
    void setSampleRate(double rate) override {
        sampleRate = rate;
        delayBuffer.resize(static_cast<int>(rate * maxDelaySeconds), 0.0f);
    }

    float processSample(float inputSample) override {
        int delaySamples = static_cast<int>(sampleRate * delayTime);
        float delayedSample = delayBuffer[writeIndex];
        delayBuffer[writeIndex] = inputSample + delayedSample * feedback;

        writeIndex = (writeIndex + 1) % delayBuffer.size();
        return (1.0f - mix) * inputSample + mix * delayedSample;
    }

    void setParameter(float value) override {
        delayTime = value / 10.0f; // maps 0–10 to 0–1 sec
    }

    void setFeedback(float fb) { feedback = fb; }
    void setMix(float m)       { mix = m; }

private:
    std::vector<float> delayBuffer;
    int writeIndex = 0;
    double sampleRate = 44100.0;
    float feedback = 0.4f;
    float delayTime = 0.5f;
    float mix = 0.5f;
    float maxDelaySeconds = 2.0f;
};
