#pragma once

#include "PedalComponent.h"
#include "../pedalSoundEffects/Delay.h"
#include "CustomToggleButton.h"
#include "CustomLookAndFeel.h"

class DelayComponent : public PedalComponent
{
public:
    DelayComponent()
        : PedalComponent("Delay", std::make_unique<Delay>(), "Delay.png")
    {
        delayPtr = dynamic_cast<Delay*>(effectProcessor.get());

        // === TIME KNOB ===
        timeKnob.setLookAndFeel(&customKnobLook);
        timeKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        timeKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        timeKnob.setRange(0.0, 10.0, 0.1);
        timeKnob.setValue(5.0);
        timeKnob.setRotaryParameters(juce::MathConstants<float>::pi * 0.7f,
                                     juce::MathConstants<float>::pi * 2.3f,
                                     true);
        timeKnob.onValueChange = [this]() {
            if (delayPtr)
                delayPtr->setParameter(timeKnob.getValue());
        };
        addAndMakeVisible(timeKnob);

        // === FEEDBACK KNOB ===
        feedbackKnob.setLookAndFeel(&customKnobLook);
        feedbackKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        feedbackKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        feedbackKnob.setRange(0.0, 1.0, 0.01);
        feedbackKnob.setValue(0.5);
        feedbackKnob.setRotaryParameters(juce::MathConstants<float>::pi * 0.7f,
                                         juce::MathConstants<float>::pi * 2.3f,
                                         true);
        feedbackKnob.onValueChange = [this]() {
            if (delayPtr)
                delayPtr->setFeedback(feedbackKnob.getValue());
        };
        addAndMakeVisible(feedbackKnob);

        // === MIX KNOB ===
        mixKnob.setLookAndFeel(&customKnobLook);
        mixKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        mixKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        mixKnob.setRange(0.0, 1.0, 0.01);
        mixKnob.setValue(0.5);
        mixKnob.setRotaryParameters(juce::MathConstants<float>::pi * 0.7f,
                                     juce::MathConstants<float>::pi * 2.3f,
                                     true);
        mixKnob.onValueChange = [this]() {
            if (delayPtr)
                delayPtr->setMix(mixKnob.getValue());
        };
        addAndMakeVisible(mixKnob);

        // === BYPASS BUTTON ===
        pedalButton.setClickingTogglesState(true);
        pedalButton.onClick = [this]() {
            setEnabled(pedalButton.getToggleState());
            pedalButton.repaint();
        };
        addAndMakeVisible(pedalButton);
    }

    void resized() override
{
    const int knobSize = 38;    // size for time & mix
    const int feedbackSize = 32; // smaller knob for feedback

    const int timeCenterX = 73;
    const int timeCenterY = 20;

    const int mixCenterX = timeCenterX + 54;
    const int mixCenterY = timeCenterY;

    const int feedbackCenterX = timeCenterX + 27;
    const int feedbackCenterY = timeCenterY + 14;

    timeKnob.setBounds(timeCenterX - knobSize / 2, timeCenterY - knobSize / 2, knobSize, knobSize);
    mixKnob.setBounds(mixCenterX - knobSize / 2, mixCenterY - knobSize / 2, knobSize, knobSize);  // same size as timeKnob
    feedbackKnob.setBounds(feedbackCenterX - feedbackSize / 2, feedbackCenterY - feedbackSize / 2, feedbackSize, feedbackSize);  // smaller knob

    pedalButton.setBounds(51, getHeight() - 61, 98, 52);
}


    float processSample(float sample)
    {
        if (effectProcessor && isEnabled())
            sample = effectProcessor->processSample(sample);
        return sample * level;
    }
    
    // Save/load knob states
    juce::var getKnobStates() const override
    {
        juce::DynamicObject::Ptr obj = new juce::DynamicObject();
        obj->setProperty("time", timeKnob.getValue());
        obj->setProperty("feedback", feedbackKnob.getValue());
        obj->setProperty("mix", mixKnob.getValue());
        return juce::var(obj);
    }
    
    void setKnobStates(const juce::var& states) override
    {
        if (auto* obj = states.getDynamicObject())
        {
            if (obj->hasProperty("time"))
                timeKnob.setValue(obj->getProperty("time"), juce::dontSendNotification);
            if (obj->hasProperty("feedback"))
                feedbackKnob.setValue(obj->getProperty("feedback"), juce::dontSendNotification);
            if (obj->hasProperty("mix"))
                mixKnob.setValue(obj->getProperty("mix"), juce::dontSendNotification);
            
            // Update the effect processor
            if (delayPtr)
            {
                delayPtr->setParameter(timeKnob.getValue());
                delayPtr->setFeedback(feedbackKnob.getValue());
                delayPtr->setMix(mixKnob.getValue());
            }
        }
    }
    
    void updateToggleButton() override
    {
        pedalButton.setToggleState(isEnabled(), juce::dontSendNotification);
        pedalButton.repaint();
    }

private:
    juce::Slider timeKnob, feedbackKnob, mixKnob;
    CustomToggleButton pedalButton;
    float level = 1.0f;
    Delay* delayPtr = nullptr;
    inline static CustomLookAndFeel customKnobLook;
};
