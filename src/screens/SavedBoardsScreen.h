#pragma once

#include <JuceHeader.h>

class SavedBoardsScreen : public juce::Component {
public:
    SavedBoardsScreen(std::function<void()> goHome,
                      std::function<void(const juce::String&)> loadBoard);

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void loadBoards();

    std::function<void()> onBackToHome;
    std::function<void(const juce::String&)> onLoadBoard;

    juce::TextButton backButton;
    juce::Label noBoardsLabel;

    juce::OwnedArray<juce::TextButton> boardButtons;
    juce::StringArray savedBoards;

    juce::Image backgroundImage;
};
