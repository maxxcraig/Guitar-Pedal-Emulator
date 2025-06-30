#pragma once
#include <JuceHeader.h>

class AboutScreen : public juce::Component {
public:
    AboutScreen(std::function<void()> onBack)
        : goBack(std::move(onBack)) {
        
        // Load the background image
        backgroundImage = juce::ImageCache::getFromFile(
            juce::File::getCurrentWorkingDirectory().getChildFile("resources/About.png"));

        // Back button setup
        backButton.setButtonText("Back");
        backButton.onClick = [this] { goBack(); };
        addAndMakeVisible(backButton);

        // Grey background component
        addAndMakeVisible(textBackground);
        textBackground.setInterceptsMouseClicks(false, false);

        // About text
        aboutText.setText(
        "This app was developed in C++ using the JUCE framework.\n"
        "I use a variety of mathematical and geometric techniques\n"
        "to shape sound waves and achieve the desired audio effects.\n"
        "Some methods are standard in DSP, while others were crafted\n"
        "through trial and error to create a more personal sound.\n"
        "My goal was to build a quick, lightweight, and easy-to-use\n"
        "pedal board that anyone can enjoy for free. It's taken lots\n"
        "of work and iteration, but here we are!\n"
        "\n"
        "If you have any suggestions—whether code, UI, or audio—feel\n"
        "free to reach out at max8alton@gmail.com. Depending on your\n"
        "OS, you may need to manually grant the app permission to use\n"
        "audio inputs and outputs via system settings or terminal.\n"
        "\n"
        "Also feel free to reach out with any questions about setup.\n"
        "Thanks for downloading, and enjoy!",

            juce::dontSendNotification);
        aboutText.setJustificationType(juce::Justification::centred);
        aboutText.setColour(juce::Label::textColourId, juce::Colours::white);
        aboutText.setFont(juce::Font(18.0f));
        addAndMakeVisible(aboutText);
    }

    void paint(juce::Graphics& g) override {
        if (backgroundImage.isValid())
            g.drawImage(backgroundImage, getLocalBounds().toFloat());
        else
            g.fillAll(juce::Colours::darkblue);
    }

    void resized() override {
        backButton.setBounds(20, 20, 100, 30);

        int width = getWidth();
        int height = getHeight();

        // Shared bounds
        juce::Rectangle<int> textArea(width / 4 + 30, height / 3 - 40, width / 2, 300);
        textBackground.setBounds(textArea.expanded(20, 20));
        aboutText.setBounds(textArea);
    }

private:
    juce::TextButton backButton;
    juce::Label aboutText;
    juce::Image backgroundImage;
    std::function<void()> goBack;

    // Background for the text
    class TextBackgroundComponent : public juce::Component {
        void paint(juce::Graphics& g) override {
            g.setColour(juce::Colours::darkgrey.withAlpha(0.8f));
            g.fillRoundedRectangle(getLocalBounds().toFloat(), 20.0f);
        }
    } textBackground;
};
