#pragma once

#include <JuceHeader.h>

struct AuthResponse {
    bool success = false;
    juce::String errorMessage;
    juce::String userId;
    juce::String accessToken;
    juce::String email;
};

class SupabaseClient {
public:
    SupabaseClient();
    
    // Authentication methods
    AuthResponse signUp(const juce::String& email, const juce::String& password);
    AuthResponse signIn(const juce::String& email, const juce::String& password);
    void signOut();
    
    // Utility methods
    bool isValidEmail(const juce::String& email);
    bool isValidPassword(const juce::String& password);
    
private:
    juce::String supabaseUrl;
    juce::String supabaseKey;
    
    // HTTP request helper
    juce::String makeRequest(const juce::String& method, 
                           const juce::String& endpoint, 
                           const juce::String& jsonBody = "");
    
    // Parse auth response from Supabase
    AuthResponse parseAuthResponse(const juce::String& responseJson);
    
    // Load credentials from environment/config
    void loadCredentials();
};