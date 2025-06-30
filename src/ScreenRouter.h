#pragma once
#include <JuceHeader.h>
#include "screens/SavedBoardsScreen.h"

class ScreenRouter : public juce::Component
{
public:
    ScreenRouter();

    void showLoginScreen();
    void showHomeScreen();
    void showPedalBoardScreen();
    void showAboutScreen();
    void clearScreen();
    void showCreateAccountScreen();
    void showSavedBoardsScreen();
    void showPedalBoardScreenFromSave(const juce::String& boardName);


    void resized() override;

private:
    std::unique_ptr<juce::Component> currentScreen;
};
