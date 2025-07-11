#pragma once
#include "BaseEffects.h"
#include <cmath>
#include <algorithm>

class Phaser : public BaseEffect {
public:
    float processSample(float sample) override {
        // Update LFO with more pronounced sweep
        float lfoValue = std::sin(phase) * depth;
        phase += rate * 2.0f * juce::MathConstants<float>::pi / sampleRate;
        if (phase > juce::MathConstants<float>::twoPi)
            phase -= juce::MathConstants<float>::twoPi;

        // Calculate all-pass filter coefficients with wider sweep range
        float centerFreq = 400.0f + lfoValue * 2000.0f;
        centerFreq = std::clamp(centerFreq, 100.0f, 3000.0f);
        float omega = 2.0f * juce::MathConstants<float>::pi * centerFreq / sampleRate;
        float alpha = (1.0f - std::tan(omega * 0.5f)) / (1.0f + std::tan(omega * 0.5f));
        
        // Apply 6-stage all-pass filtering for more pronounced effect
        float output = sample;
        for (int i = 0; i < 6; ++i) {
            float delayed = delayLine[i] + output * alpha;
            output = delayed - alpha * output;
            delayLine[i] = delayed;
        }
        
        // Apply feedback with proper limiting
        output = output + feedback * lastOutput;
        
        // Prevent runaway feedback with soft limiting
        if (std::abs(output) > 0.8f) {
            output = std::tanh(output) * 0.8f;
        }
        lastOutput = output * 0.9f;
        
        // Mix with dry signal with safe levels
        return (1.0f - mix) * sample + mix * output * 0.6f;
    }

    void setParameter(float value) override {
        setDepth(value * 0.1f + 0.5f); // Scale to good range
    }

    void setDepth(float value) { 
        depth = std::clamp(value, 0.3f, 1.0f); 
    }
    
    void setRate(float value) { 
        rate = std::clamp(value, 0.2f, 3.0f); 
    }
    
    void setMix(float value) { 
        mix = std::clamp(value, 0.0f, 1.0f); 
    }

    void setSampleRate(double rateIn) override {
        sampleRate = rateIn;
    }

private:
    float depth = 0.7f;
    float rate = 0.4f;
    float mix = 0.5f;
    float feedback = 0.25f;  // Reduced to prevent clipping
    float phase = 0.0f;
    float delayLine[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    float lastOutput = 0.0f;
    double sampleRate = 44100.0;
};
