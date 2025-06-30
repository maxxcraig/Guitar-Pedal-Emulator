#include "Overdrive.h"
#include <cmath>

Overdrive::Overdrive() {}

void Overdrive::setGain(float newGain) {
    gain = newGain;
}

float Overdrive::processSample(float inputSample) {
    float processed = std::tanh(gain * inputSample);
    return processed;
}
 