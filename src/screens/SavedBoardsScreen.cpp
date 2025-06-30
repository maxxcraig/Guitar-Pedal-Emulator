#include "SavedBoardsScreen.h"

SavedBoardsScreen::SavedBoardsScreen(std::function<void()> goHome,
                                     std::function<void(const juce::String&)> loadBoard)
    : onBackToHome(goHome), onLoadBoard(loadBoard)

{
    backgroundImage = juce::ImageCache::getFromFile(
        juce::File::getCurrentWorkingDirectory().getChildFile("resources/home-background.png"));

    backButton.setButtonText("Home");
    backButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    backButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    backButton.onClick = [this]() { onBackToHome(); };
    addAndMakeVisible(backButton);

    // Load saved boards from disk
    loadBoards();

    // If no saved boards
    if (savedBoards.isEmpty()) {
        noBoardsLabel.setText("No saved boards yet...", juce::dontSendNotification);
        noBoardsLabel.setColour(juce::Label::textColourId, juce::Colours::white);
        noBoardsLabel.setFont(juce::Font(18.0f));
        noBoardsLabel.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(noBoardsLabel);
    } else {
        for (int i = 0; i < savedBoards.size(); ++i) {
            auto name = savedBoards[i];
            auto* btn = new juce::TextButton(name);
            btn->onClick = [this, name]() {
                if (onLoadBoard)
                    onLoadBoard(name);
            };
            boardButtons.add(btn);
            addAndMakeVisible(btn);
        }

    }
}

void SavedBoardsScreen::paint(juce::Graphics& g)
{
    if (backgroundImage.isValid())
        g.drawImage(backgroundImage, getLocalBounds().toFloat());
    else
        g.fillAll(juce::Colours::darkcyan);
}

void SavedBoardsScreen::resized()
{
    backButton.setBounds(10, 10, 100, 35); 
    if (!savedBoards.isEmpty()) {
        auto area = getLocalBounds().reduced(60);
        area.removeFromTop(60); // leave space for back
        int buttonHeight = 40;
        int spacing = 20;
        for (int i = 0; i < boardButtons.size(); ++i)
        {
            boardButtons[i]->setBounds(area.removeFromTop(buttonHeight));
            area.removeFromTop(spacing);
        }
    } else {
        // Leave space for the back button at the top
        noBoardsLabel.setBounds(getLocalBounds().withTrimmedTop(60));
    }
}


void SavedBoardsScreen::loadBoards()
{
    juce::File dir = juce::File::getCurrentWorkingDirectory()
                    .getChildFile("Boards");
    if (dir.exists()) {
        auto files = dir.findChildFiles(juce::File::findFiles, false, "*.json");
        for (auto& file : files)
            savedBoards.add(file.getFileNameWithoutExtension());
    }
}
