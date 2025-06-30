#include "CustomReverb.h"

CustomReverb::CustomReverb() {
    params.roomSize = 0.5f;
    params.damping = 0.5f;
    params.wetLevel = 0.3f;
    params.dryLevel = 0.7f;
    params.width = 1.0f;
    reverb.setParameters(params);
}

float CustomReverb::processSample(float input) {
    float in[] = { input };
    reverb.processMono(in, 1);
    return in[0];
}

void CustomReverb::setParameter(float value) {
    setMix(value); // default mapping
}

void CustomReverb::setMix(float value) {
    params.wetLevel = value;
    reverb.setParameters(params);
}

void CustomReverb::setDecay(float value) {
    params.roomSize = value;
    reverb.setParameters(params);
}

void CustomReverb::setTone(float value) {
    params.damping = value;
    reverb.setParameters(params);
}

void CustomReverb::setSampleRate(double rate) {
    reverb.setSampleRate(rate);
}
