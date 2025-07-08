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
        "This comprehensive guitar pedal board application showcases cutting-edge audio technology and modern software architecture. Built from the ground up in C++ using the professional JUCE framework, it delivers real-time audio processing with studio-quality effects.\n\n"
        
        "The app features eight meticulously crafted guitar pedals including Overdrive, Distortion, Reverb, Chorus, Delay, Tremolo, Phaser, and Blues Driver. Each effect utilizes advanced DSP algorithms, combining industry-standard techniques with custom mathematical approaches to create unique, responsive tones.\n\n"
        
        "Key technical achievements include seamless real-time audio streaming, drag-and-drop pedal arrangement, comprehensive save/load functionality with JSON persistence, and cloud-based user authentication through Supabase integration. The intuitive interface allows musicians to create, experiment with, and save custom pedal board configurations.\n\n"
    
        
        "Feel free to reach out to me (max8alton@gmail.com) with any questions, suggestions, or comments. This entire project is on my public GitHub repo https://github.com/maxxcraig/Guitar-Pedal-App.",

            juce::dontSendNotification);
        aboutText.setJustificationType(juce::Justification::topLeft);
        aboutText.setColour(juce::Label::textColourId, juce::Colours::white);
        aboutText.setFont(juce::Font(15.0f));
        addAndMakeVisible(aboutText);

        // Load the max.png image
        maxImage = juce::ImageCache::getFromFile(
            juce::File::getCurrentWorkingDirectory().getChildFile("resources/max.png"));

        // "Me!" label
        meLabel.setText("Me!", juce::dontSendNotification);
        meLabel.setJustificationType(juce::Justification::centred);
        meLabel.setColour(juce::Label::textColourId, juce::Colours::white);
        meLabel.setFont(juce::Font(20.0f, juce::Font::bold));
        addAndMakeVisible(meLabel);
    }

    void paint(juce::Graphics& g) override {
        if (backgroundImage.isValid())
            g.drawImage(backgroundImage, getLocalBounds().toFloat());
        else
            g.fillAll(juce::Colours::darkblue);

        // Draw the max.png image on the right side with rounded corners
        if (maxImage.isValid()) {
            int imageSize = 500; // Size for the image (2.5x larger)
            int imageX = getWidth() - 550; // Position from right
            int imageY = getHeight() / 2 - imageSize / 2; // Center vertically
            
            juce::Rectangle<float> imageArea(imageX, imageY, imageSize, imageSize);
            
            // Save the current graphics state
            juce::Graphics::ScopedSaveState saveState(g);
            
            // Create a rounded rectangle path for clipping
            juce::Path roundedRect;
            roundedRect.addRoundedRectangle(imageArea, 20.0f);
            g.reduceClipRegion(roundedRect);
            
            // Draw the image with rounded corners
            g.drawImage(maxImage, imageArea, juce::RectanglePlacement::centred);
        }
    }

    void resized() override {
        backButton.setBounds(20, 20, 100, 30);

        int width = getWidth();
        int height = getHeight();

        // Left side text area (centered vertically)
        int marginLeft = 80;
        int textWidth = width - 700; // Leave more space for larger image
        int textHeight = 450;
        int marginTop = (height - textHeight) / 2; // Center vertically
        
        juce::Rectangle<int> textArea(marginLeft, marginTop, textWidth, textHeight);
        textBackground.setBounds(textArea.expanded(20, 20));
        aboutText.setBounds(textArea);

        // "Me!" label closer to the bottom of the larger image
        int imageSize = 500;
        int imageX = getWidth() - 550;
        int imageY = getHeight() / 2 - imageSize / 2;
        meLabel.setBounds(imageX, imageY + imageSize - 35, imageSize, 40); // Moved closer to image
    }

private:
    juce::TextButton backButton;
    juce::Label aboutText;
    juce::Label meLabel;
    juce::Image backgroundImage;
    juce::Image maxImage;
    std::function<void()> goBack;

    // Background for the text
    class TextBackgroundComponent : public juce::Component {
        void paint(juce::Graphics& g) override {
            g.setColour(juce::Colours::darkgrey.withAlpha(0.8f));
            g.fillRoundedRectangle(getLocalBounds().toFloat(), 20.0f);
        }
    } textBackground;
};
