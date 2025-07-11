#pragma once
#include "BaseEffects.h"
#include <vector>
#include <algorithm>

// Simple delay line class based on the example
class DelayLine {
public:
    void clear() {
        std::fill(rawData.begin(), rawData.end(), 0.0f);
    }
    
    void resize(size_t newSize) {
        rawData.resize(newSize);
        leastRecentIndex = 0;
        clear();
    }
    
    float get(size_t delayInSamples) const {
        if (delayInSamples >= rawData.size()) return 0.0f;
        return rawData[(leastRecentIndex + 1 + delayInSamples) % rawData.size()];
    }
    
    void push(float valueToAdd) {
        rawData[leastRecentIndex] = valueToAdd;
        leastRecentIndex = leastRecentIndex == 0 ? rawData.size() - 1 : leastRecentIndex - 1;
    }
    
    size_t size() const { return rawData.size(); }

private:
    std::vector<float> rawData;
    size_t leastRecentIndex = 0;
};

class Delay : public BaseEffect {
public:
    Delay() {
        setSampleRate(44100.0);
    }

    void setSampleRate(double rate) override {
        sampleRate = rate;
        int bufferSize = static_cast<int>(rate * maxDelaySeconds);
        delayLine.resize(bufferSize);
    }

    float processSample(float input) override {
        if (delayLine.size() == 0) return input;

        // Attenuate input heavily to prevent overloading
        float attenuatedInput = input * 0.4f;

        // Calculate delay in samples
        int delaySamples = static_cast<int>(delayTime * sampleRate);
        delaySamples = std::clamp(delaySamples, 1, static_cast<int>(delayLine.size()) - 1);
        
        // Get delayed sample
        float delayedSample = delayLine.get(delaySamples);
        
        // Very conservative feedback to prevent buildup
        float feedbackSignal = delayedSample * feedback * 0.6f;
        
        // Clean write to delay line - no saturation
        float writeValue = attenuatedInput + feedbackSignal;
        
        // Hard limit to prevent any clipping
        if (writeValue > 0.8f) writeValue = 0.8f;
        if (writeValue < -0.8f) writeValue = -0.8f;
        
        delayLine.push(writeValue);
        
        // Boost wet signal to compensate for input attenuation
        float dryGain = 1.0f - mix;
        float wetGain = mix * 2.0f; // Boost wet to maintain audibility
        
        return input * dryGain + delayedSample * wetGain;
    }

    void setParameter(float value) override {
        // Map 0-10 to 100ms-400ms delay time
        delayTime = 0.1f + (value / 10.0f) * 0.3f;
    }

    void setDelayTime(float time) { delayTime = std::clamp(time, 0.01f, maxDelaySeconds); }
    void setFeedback(float fb) { feedback = std::clamp(fb, 0.0f, 0.5f); }
    void setMix(float m) { mix = std::clamp(m, 0.0f, 0.8f); }

private:
    DelayLine delayLine;
    double sampleRate = 44100.0;
    
    float delayTime = 0.25f;   // 250ms default - more obvious delay
    float feedback = 0.25f;    // Reduced to 25% feedback 
    float mix = 0.3f;          // Reduced to 30% wet signal
    float maxDelaySeconds = 1.0f;
};
