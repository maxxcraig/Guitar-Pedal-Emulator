#include "SupabaseClient.h"

SupabaseClient::SupabaseClient() {
    loadCredentials();
}

void SupabaseClient::loadCredentials() {
    // Load credentials from .env file for security
    // This keeps sensitive data out of source control
    
    auto envFile = juce::File::getCurrentWorkingDirectory().getChildFile(".env");
    if (envFile.existsAsFile()) {
        auto envContent = envFile.loadFileAsString();
        auto lines = juce::StringArray::fromLines(envContent);
        
        for (auto& line : lines) {
            if (line.startsWith("SUPABASE_URL=")) {
                supabaseUrl = line.substring(13); // Remove "SUPABASE_URL="
            } else if (line.startsWith("SUPABASE_ANON_KEY=")) {
                supabaseKey = line.substring(18); // Remove "SUPABASE_ANON_KEY="
            }
        }
    }
    
    // Fallback for missing .env file (for other developers)
    if (supabaseUrl.isEmpty() || supabaseKey.isEmpty()) {
        std::cout << "[SupabaseClient] ERROR: Missing .env file with SUPABASE_URL and SUPABASE_ANON_KEY" << std::endl;
        std::cout << "[SupabaseClient] Please create a .env file with your Supabase credentials" << std::endl;
        supabaseUrl = "MISSING_ENV_FILE";
        supabaseKey = "MISSING_ENV_FILE";
        return;
    }
    
    std::cout << "[SupabaseClient] Initialized with URL: " << supabaseUrl << std::endl;
}

AuthResponse SupabaseClient::signUp(const juce::String& email, const juce::String& password) {
    std::cout << "[SupabaseClient] Attempting sign up for: " << email << std::endl;
    
    AuthResponse response;
    
    // Validate input
    if (!isValidEmail(email)) {
        response.errorMessage = "Invalid email format";
        return response;
    }
    
    if (!isValidPassword(password)) {
        response.errorMessage = "Password must be at least 6 characters";
        return response;
    }
    
    // Create JSON body for Supabase auth API
    juce::DynamicObject::Ptr jsonData = new juce::DynamicObject();
    jsonData->setProperty("email", email);
    jsonData->setProperty("password", password);
    
    juce::var jsonVar(jsonData);
    juce::String jsonBody = juce::JSON::toString(jsonVar);
    
    // Make request to Supabase
    juce::String responseJson = makeRequest("POST", "/auth/v1/signup", jsonBody);
    
    if (responseJson.isEmpty()) {
        response.errorMessage = "Network error - could not connect to authentication server";
        return response;
    }
    
    return parseAuthResponse(responseJson);
}

AuthResponse SupabaseClient::signIn(const juce::String& email, const juce::String& password) {
    std::cout << "[SupabaseClient] Attempting sign in for: " << email << std::endl;
    
    AuthResponse response;
    
    // Validate input
    if (!isValidEmail(email)) {
        response.errorMessage = "Invalid email format";
        return response;
    }
    
    if (password.isEmpty()) {
        response.errorMessage = "Password cannot be empty";
        return response;
    }
    
    // Create JSON body for Supabase auth API
    juce::DynamicObject::Ptr jsonData = new juce::DynamicObject();
    jsonData->setProperty("email", email);
    jsonData->setProperty("password", password);
    
    juce::var jsonVar(jsonData);
    juce::String jsonBody = juce::JSON::toString(jsonVar);
    
    // Make request to Supabase
    juce::String responseJson = makeRequest("POST", "/auth/v1/token?grant_type=password", jsonBody);
    
    if (responseJson.isEmpty()) {
        response.errorMessage = "Network error - could not connect to authentication server";
        return response;
    }
    
    return parseAuthResponse(responseJson);
}

void SupabaseClient::signOut() {
    std::cout << "[SupabaseClient] User signed out" << std::endl;
}

juce::String SupabaseClient::makeRequest(const juce::String& method, 
                                        const juce::String& endpoint, 
                                        const juce::String& jsonBody) {
    
    juce::String fullUrl = supabaseUrl + endpoint;
    std::cout << "[SupabaseClient] Making " << method << " request to: " << fullUrl << std::endl;
    
    try {
        if (method == "POST") {
            std::cout << "[SupabaseClient] POST request with body: " << jsonBody.substring(0, 100) << "..." << std::endl;
            
            // Create URL with parameters
            juce::URL url(fullUrl);
            
            // Prepare headers string manually
            juce::String headerString = "Content-Type: application/json\r\n";
            headerString += "apikey: " + supabaseKey + "\r\n";
            headerString += "Authorization: Bearer " + supabaseKey + "\r\n";
            
            // Use JUCE's URL with POST data
            juce::URL urlWithData = url.withPOSTData(jsonBody);
            
            auto inputStream = urlWithData.createInputStream(
                juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
                .withExtraHeaders(headerString)
                .withConnectionTimeoutMs(10000)
            );
            
            if (inputStream != nullptr) {
                juce::String response = inputStream->readEntireStreamAsString();
                std::cout << "[SupabaseClient] Response received (" << response.length() << " chars): " << response.substring(0, 200) << "..." << std::endl;
                return response;
            } else {
                std::cout << "[SupabaseClient] ERROR: Failed to create input stream" << std::endl;
                return "{\"error\":{\"message\":\"Failed to connect to authentication server\"}}";
            }
        }
        
        std::cout << "[SupabaseClient] ERROR: Unsupported method: " << method << std::endl;
        return "{\"error\":{\"message\":\"Unsupported HTTP method\"}}";
        
    } catch (const std::exception& e) {
        std::cout << "[SupabaseClient] ERROR: Exception during request: " << e.what() << std::endl;
        return "{\"error\":{\"message\":\"Network connection error\"}}";
    }
}

AuthResponse SupabaseClient::parseAuthResponse(const juce::String& responseJson) {
    AuthResponse response;
    
    try {
        juce::var parsedJson;
        auto result = juce::JSON::parse(responseJson, parsedJson);
        
        if (!result.wasOk()) {
            response.errorMessage = "Invalid response from server";
            return response;
        }
        
        auto* jsonObject = parsedJson.getDynamicObject();
        if (!jsonObject) {
            response.errorMessage = "Malformed response from server";
            return response;
        }
        
        // Check for error in response (multiple formats)
        if (jsonObject->hasProperty("error")) {
            auto errorObj = jsonObject->getProperty("error");
            if (auto* errorDynamic = errorObj.getDynamicObject()) {
                response.errorMessage = errorDynamic->getProperty("message").toString();
            } else {
                response.errorMessage = errorObj.toString();
            }
            return response;
        }
        
        // Check for Supabase error format with error_code
        if (jsonObject->hasProperty("error_code")) {
            juce::String errorCode = jsonObject->getProperty("error_code").toString();
            juce::String errorMsg = jsonObject->getProperty("msg").toString();
            
            if (errorCode == "email_not_confirmed") {
                response.errorMessage = "Please check your email and click the confirmation link before signing in.";
            } else {
                response.errorMessage = errorMsg.isNotEmpty() ? errorMsg : "Authentication error: " + errorCode;
            }
            return response;
        }
        
        // Check for success indicators
        // Supabase signup returns user data directly, signin returns access_token + user
        if (jsonObject->hasProperty("access_token") || jsonObject->hasProperty("user") || jsonObject->hasProperty("id")) {
            response.success = true;
            
            // Extract access token (for signin)
            if (jsonObject->hasProperty("access_token")) {
                response.accessToken = jsonObject->getProperty("access_token").toString();
            }
            
            // Extract user information from user object (signin)
            if (jsonObject->hasProperty("user")) {
                auto userObj = jsonObject->getProperty("user");
                if (auto* userDynamic = userObj.getDynamicObject()) {
                    response.userId = userDynamic->getProperty("id").toString();
                    response.email = userDynamic->getProperty("email").toString();
                }
            }
            // Extract user information directly from response (signup)
            else if (jsonObject->hasProperty("id") && jsonObject->hasProperty("email")) {
                response.userId = jsonObject->getProperty("id").toString();
                response.email = jsonObject->getProperty("email").toString();
            }
            
            std::cout << "[SupabaseClient] Authentication successful for user: " << response.email << " (ID: " << response.userId << ")" << std::endl;
            return response;
        }
        
        // If we get here, we have an unexpected response format
        response.errorMessage = "Unexpected response format from server";
        return response;
        
    } catch (const std::exception& e) {
        response.errorMessage = "Error parsing server response: " + juce::String(e.what());
        return response;
    }
}

bool SupabaseClient::isValidEmail(const juce::String& email) {
    // Basic email validation
    return email.contains("@") && 
           email.contains(".") && 
           email.length() > 5 &&
           !email.startsWith("@") &&
           !email.endsWith("@");
}

bool SupabaseClient::isValidPassword(const juce::String& password) {
    // Supabase requires minimum 6 characters
    return password.length() >= 6;
}