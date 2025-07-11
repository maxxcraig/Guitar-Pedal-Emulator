#include "CustomReverb.h"
#include <algorithm>
#include <cmath>

CustomReverb::CustomReverb() {
    // Initialize with default sample rate
    setSampleRate(44100.0);
}

float CustomReverb::processSample(float input) {
    // Simple multi-tap delay reverb
    float reverbSum = 0.0f;
    
    // Process through all delay lines
    for (int i = 0; i < numDelayLines; ++i) {
        reverbSum += delayLines[i].process(input);
    }
    
    // Apply simple tone control (low-pass filter)
    float filtered = tone * reverbSum + (1.0f - tone) * lastOutput;
    lastOutput = filtered;
    
    // Mix dry and wet signal
    return (1.0f - mix) * input + mix * filtered;
}

void CustomReverb::setParameter(float value) {
    // Scale parameter to much more audible wet level range
    setMix(value * 0.15f + 0.2f); // Maps 0-10 to 0.2-1.7 (will be clamped)
}

void CustomReverb::setMix(float value) {
    mix = std::clamp(value, 0.0f, 1.2f); // Allow much higher mix levels
}

void CustomReverb::setDecay(float value) {
    decay = std::clamp(value, 0.1f, 0.8f);
    setSampleRate(sampleRate); // Recalculate delay times
}

void CustomReverb::setTone(float value) {
    tone = std::clamp(value, 0.3f, 0.9f);
}

void CustomReverb::setSampleRate(double rate) {
    sampleRate = rate;
    
    // Set up delay lines based on the algorithm from the forum post
    // Using prime number delays to avoid resonances
    float baseDelay = 30.0f; // Base delay in ms
    float delayTimes[] = {23.0f, 37.0f, 41.0f, 47.0f, 53.0f, 61.0f}; // Prime numbers
    
    for (int i = 0; i < numDelayLines; ++i) {
        float delayMs = delayTimes[i];
        float delaySamples = (delayMs / 1000.0f) * sampleRate;
        float gain = std::pow(10.0f, -((3.0f * delayMs) / (baseDelay * 3.0f))) * decay;
        
        delayLines[i].setDelay(delaySamples, gain * 1.5f); // Much higher gain for audible reverb
    }
}
