#pragma once

#include "BaseEffects.h"
#include <vector>
#include <algorithm>

class CustomReverb : public BaseEffect {
public:
    CustomReverb();
    float processSample(float input) override;
    void setParameter(float value) override;
    void setSampleRate(double rate) override;

    void setMix(float value);
    void setDecay(float value);
    void setTone(float value);

private:
    // Simple multi-tap delay reverb
    struct DelayLine {
        std::vector<float> buffer;
        int writeIndex = 0;
        float delayTime = 0.0f;
        float gain = 0.0f;
        
        void setDelay(float timeInSamples, float g) {
            int size = static_cast<int>(timeInSamples);
            if (size > 0 && size != buffer.size()) {
                buffer.resize(size, 0.0f);
                writeIndex = 0;
            }
            gain = g;
        }
        
        float process(float input) {
            if (buffer.empty()) return 0.0f;
            
            buffer[writeIndex] = input;
            float output = buffer[writeIndex] * gain;
            
            writeIndex = (writeIndex + 1) % buffer.size();
            return output;
        }
    };
    
    static const int numDelayLines = 6;
    DelayLine delayLines[numDelayLines];
    
    double sampleRate = 44100.0;
    float mix = 0.6f;
    float decay = 0.5f;
    float tone = 0.5f;
    float lastOutput = 0.0f;
};
