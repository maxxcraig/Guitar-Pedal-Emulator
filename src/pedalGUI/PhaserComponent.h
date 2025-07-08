#pragma once

#include "PedalComponent.h"
#include "../pedalSoundEffects/Phaser.h"
#include "CustomToggleButton.h"
#include "CustomLookAndFeel.h"

class PhaserComponent : public PedalComponent
{
public:
    PhaserComponent()
        : PedalComponent("Phaser", std::make_unique<Phaser>(), "Phaser.png")
    {
        phaserPtr = dynamic_cast<Phaser*>(effectProcessor.get());

        // === RATE KNOB ===
        rateKnob.setLookAndFeel(&customKnobLook);
        rateKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        rateKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        rateKnob.setRange(0.0, 10.0, 0.1);
        rateKnob.setValue(5.0);
        rateKnob.setRotaryParameters(juce::MathConstants<float>::pi * 0.7f,
                                     juce::MathConstants<float>::pi * 2.3f,
                                     true);
        rateKnob.onValueChange = [this]() {
            if (phaserPtr)
                phaserPtr->setRate(rateKnob.getValue());
        };
        addAndMakeVisible(rateKnob);

        // === DEPTH KNOB ===
        depthKnob.setLookAndFeel(&customKnobLook);
        depthKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        depthKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        depthKnob.setRange(0.0, 10.0, 0.1);
        depthKnob.setValue(5.0);
        depthKnob.setRotaryParameters(juce::MathConstants<float>::pi * 0.7f,
                                       juce::MathConstants<float>::pi * 2.3f,
                                       true);
        depthKnob.onValueChange = [this]() {
            if (phaserPtr)
                phaserPtr->setDepth(depthKnob.getValue());
        };
        addAndMakeVisible(depthKnob);

        // === RESONANCE (RES) KNOB ===
        resKnob.setLookAndFeel(&customKnobLook);
        resKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        resKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        resKnob.setRange(0.0, 1.0, 0.01);
        resKnob.setValue(0.5);
        resKnob.setRotaryParameters(juce::MathConstants<float>::pi * 0.7f,
                                     juce::MathConstants<float>::pi * 2.3f,
                                     true);
        resKnob.onValueChange = [this]() {
            if (phaserPtr)
                phaserPtr->setMix(resKnob.getValue()); // using mix for resonance
        };
        addAndMakeVisible(resKnob);

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
        const int knobSize = 38;
        const int resSize = 32;

        const int rateCenterX = 73;
        const int rateCenterY = 20;

        const int depthCenterX = rateCenterX + 54;
        const int depthCenterY = rateCenterY;

        const int resCenterX = rateCenterX + 27;
        const int resCenterY = rateCenterY + 17;

        rateKnob.setBounds(rateCenterX - knobSize / 2, rateCenterY - knobSize / 2, knobSize, knobSize);
        depthKnob.setBounds(depthCenterX - knobSize / 2, depthCenterY - knobSize / 2, knobSize, knobSize);
        resKnob.setBounds(resCenterX - resSize / 2, resCenterY - resSize / 2, resSize, resSize);

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
        obj->setProperty("rate", rateKnob.getValue());
        obj->setProperty("depth", depthKnob.getValue());
        obj->setProperty("res", resKnob.getValue());
        return juce::var(obj);
    }
    
    void setKnobStates(const juce::var& states) override
    {
        if (auto* obj = states.getDynamicObject())
        {
            if (obj->hasProperty("rate"))
                rateKnob.setValue(obj->getProperty("rate"), juce::dontSendNotification);
            if (obj->hasProperty("depth"))
                depthKnob.setValue(obj->getProperty("depth"), juce::dontSendNotification);
            if (obj->hasProperty("res"))
                resKnob.setValue(obj->getProperty("res"), juce::dontSendNotification);
            
            // Update the effect processor
            if (phaserPtr)
            {
                phaserPtr->setRate(rateKnob.getValue());
                phaserPtr->setDepth(depthKnob.getValue());
                phaserPtr->setMix(resKnob.getValue());
            }
        }
    }
    
    void updateToggleButton() override
    {
        pedalButton.setToggleState(isEnabled(), juce::dontSendNotification);
        pedalButton.repaint();
    }

private:
    juce::Slider rateKnob, depthKnob, resKnob;
    CustomToggleButton pedalButton;
    float level = 1.0f;
    Phaser* phaserPtr = nullptr;
    inline static CustomLookAndFeel customKnobLook;
};
