#pragma once
#include "BaseEffects.h"
#include <vector>
#include <algorithm>

class Delay : public BaseEffect {
public:
    void setSampleRate(double rate) override {
        sampleRate = rate;
        delayBuffer.resize(static_cast<int>(rate * maxDelaySeconds), 0.0f);
    }

    float processSample(float inputSample) override {
        if (delayBuffer.empty()) return inputSample;
        
        int delaySamples = static_cast<int>(sampleRate * delayTime);
        delaySamples = std::clamp(delaySamples, 1, static_cast<int>(delayBuffer.size()) - 1);
        
        int readIndex = writeIndex - delaySamples;
        if (readIndex < 0) readIndex += delayBuffer.size();
        
        float delayedSample = delayBuffer[readIndex];
        
        // Clean up the delayed sample with soft limiting
        if (std::abs(delayedSample) > 0.8f) {
            delayedSample = std::tanh(delayedSample) * 0.8f;
        }
        
        delayBuffer[writeIndex] = inputSample + delayedSample * feedback;
        writeIndex = (writeIndex + 1) % delayBuffer.size();
        
        return (1.0f - mix) * inputSample + mix * delayedSample;
    }

    void setParameter(float value) override {
        delayTime = value / 40.0f + 0.05f; // maps 0–10 to 0.05–0.3 sec
    }

    void setFeedback(float fb) { feedback = std::clamp(fb, 0.0f, 0.5f); }
    void setMix(float m)       { mix = std::clamp(m, 0.0f, 0.7f); }

private:
    std::vector<float> delayBuffer;
    int writeIndex = 0;
    double sampleRate = 44100.0;
    float feedback = 0.25f;
    float delayTime = 0.1f;   // Shorter default delay
    float mix = 0.4f;
    float maxDelaySeconds = 0.4f;
};
