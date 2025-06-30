#include <JuceHeader.h>
#include "ScreenRouter.h"
#include "screens/LoginScreen.h"
#include "screens/HomeScreen.h"
#include "screens/AboutScreen.h"
#include "screens/PedalBoardScreen.h"
#include "screens/CreateAccountScreen.h"
#include "screens/SavedBoardsScreen.h"

ScreenRouter::ScreenRouter()
{
    setSize(1000, 600);
    showLoginScreen();
}

void ScreenRouter::showLoginScreen()
{
    clearScreen();
    currentScreen.reset(new LoginScreen(
    [this]() { showHomeScreen(); },
    [this]() { showCreateAccountScreen(); }
    ));

    addAndMakeVisible(*currentScreen);
    resized();
}

void ScreenRouter::showHomeScreen()
{
    clearScreen();
    currentScreen.reset(new HomeScreen(
    [this]() { showPedalBoardScreen(); },
    [this]() { showAboutScreen(); },
    [this]() { showLoginScreen(); }, // logout
    [this]() { showSavedBoardsScreen(); } // view saved
    ));
    addAndMakeVisible(*currentScreen);
    resized();
}

void ScreenRouter::showPedalBoardScreen()
{
    clearScreen();
    currentScreen.reset(new PedalBoardScreen([this]() {
        showHomeScreen();
    }));
    addAndMakeVisible(*currentScreen);
    resized();
}
void ScreenRouter::showCreateAccountScreen()
{
    clearScreen();

    auto* createAccountScreen = new CreateAccountScreen([this]() {
        showLoginScreen();
    });

    addAndMakeVisible(createAccountScreen);
    currentScreen.reset(createAccountScreen);
    resized(); 
}


void ScreenRouter::showSavedBoardsScreen()
{
    clearScreen();
    currentScreen = std::make_unique<SavedBoardsScreen>(
        [this]() { showHomeScreen(); },
        [this](const juce::String& boardName) {
            showPedalBoardScreenFromSave(boardName);
        });

    addAndMakeVisible(*currentScreen);
    resized();
}






void ScreenRouter::showAboutScreen()
{
    clearScreen();
    currentScreen.reset(new AboutScreen([this]() {
        showHomeScreen();
    }));
    addAndMakeVisible(*currentScreen);
    resized();
}

void ScreenRouter::showPedalBoardScreenFromSave(const juce::String& boardName)
{
    std::cout << "[ScreenRouter] showPedalBoardScreenFromSave called" << std::endl;
    
    // Load the file contents first
    juce::File dir = juce::File::getCurrentWorkingDirectory()
                        .getChildFile("Boards");
    juce::File file = dir.getChildFile(boardName + ".json");

    juce::var boardVar;
    if (file.existsAsFile()) {
        boardVar = juce::JSON::parse(file);
        std::cout << "boardVar is object: " << boardVar.isObject() << std::endl;
    }

    // Check if we already have a PedalBoardScreen
    if (auto* existingScreen = dynamic_cast<PedalBoardScreen*>(currentScreen.get())) {
        std::cout << "[ScreenRouter] reusing existing PedalBoardScreen" << std::endl;
        if (!boardVar.isVoid()) {
            std::cout << "[ScreenRouter] calling loadBoardData..." << std::endl;
            existingScreen->loadBoardData(boardVar);
            std::cout << "[ScreenRouter] finished loadBoardData call" << std::endl;
        }
        return;
    }
    
    // Otherwise, create a new screen
    clearScreen();
    juce::Thread::sleep(50); // Give time for audio cleanup
    
    auto* screen = new PedalBoardScreen([this]() { showHomeScreen(); });
    currentScreen.reset(screen);
    addAndMakeVisible(*currentScreen);
    resized();

    // Now load the data into the pedal board
    if (!boardVar.isVoid()) {
        if (auto* pedalBoardScreen = dynamic_cast<PedalBoardScreen*>(currentScreen.get())) {
            std::cout << "[ScreenRouter] calling loadBoardData..." << std::endl;
            pedalBoardScreen->loadBoardData(boardVar);
            std::cout << "[ScreenRouter] finished loadBoardData call" << std::endl;
        }
    }
}



void ScreenRouter::clearScreen()
{
    if (currentScreen)
        removeChildComponent(currentScreen.get());
    currentScreen.reset();
}

void ScreenRouter::resized()
{
    if (currentScreen)
        currentScreen->setBounds(getLocalBounds());
}
