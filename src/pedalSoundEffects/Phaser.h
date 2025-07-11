#pragma once
#include "BaseEffects.h"
#include <cmath>
#include <algorithm>

class Phaser : public BaseEffect {
public:
    Phaser() {
        setSampleRate(44100.0);
    }

    float processSample(float input) override {
        // Simple LFO
        float lfo = std::sin(lfoPhase);
        lfoPhase += rate * 2.0f * M_PI / sampleRate;
        if (lfoPhase > 2.0f * M_PI) lfoPhase -= 2.0f * M_PI;

        // Modulated center frequency - reduced sweep range
        float baseFreq = 800.0f;
        float sweep = 300.0f * depth; // Reduced from 600 to 300
        float modFreq = baseFreq + lfo * sweep;
        
        // Calculate w = 2πf / sampleRate
        float w = 2.0f * M_PI * modFreq / sampleRate;
        float alpha = (1.0f - std::tan(w * 0.5f)) / (1.0f + std::tan(w * 0.5f));
        alpha = std::clamp(alpha, -0.95f, 0.95f);

        // Apply feedback to input
        float inputWithFeedback = input + feedbackSample * feedback;
        
        // Process through 4 allpass stages
        float output = inputWithFeedback;
        for (int i = 0; i < 4; ++i) {
            float y = -alpha * output + prevInput[i] + alpha * prevOutput[i];
            prevInput[i] = output;
            prevOutput[i] = y;
            output = y;
        }
        
        // Store feedback sample
        feedbackSample = output;
        
        // Mix dry and wet
        return input * (1.0f - mix) + output * mix;
    }

    void setParameter(float value) override {
        // Map 0-10 to depth range - more subtle
        depth = (value / 10.0f) * 0.5f; // 0 to 0.5 for subtler effect
    }

    void setDepth(float value) { depth = std::clamp(value, 0.0f, 1.0f); }
    void setRate(float value)  { rate = std::clamp(value, 0.1f, 3.0f); }
    void setMix(float value)   { mix = std::clamp(value, 0.0f, 1.0f); }
    void setFeedback(float value) { feedback = std::clamp(value, 0.0f, 0.7f); }

    void setSampleRate(double rateIn) override {
        sampleRate = rateIn;
        lfoPhase = 0.0f;
        feedbackSample = 0.0f;
        for (int i = 0; i < 4; ++i) {
            prevInput[i] = 0.0f;
            prevOutput[i] = 0.0f;
        }
    }

private:
    double sampleRate = 44100.0;
    float lfoPhase = 0.0f;
    float prevInput[4] = {0.0f};
    float prevOutput[4] = {0.0f};
    float feedbackSample = 0.0f;
    
    float rate = 0.5f;      // LFO rate in Hz
    float depth = 0.4f;     // Reduced sweep depth
    float mix = 0.3f;       // Reduced wet/dry mix
    float feedback = 0.15f; // Reduced feedback amount
};
