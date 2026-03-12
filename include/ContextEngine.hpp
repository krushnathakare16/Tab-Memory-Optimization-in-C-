#ifndef CONTEXT_ENGINE_HPP
#define CONTEXT_ENGINE_HPP

#include <string>
#include <vector>
#include <map>
#include "SystemMonitor.hpp"

struct TabInfo {
    std::wstring title;
    // URL would be available if we use the Native Messaging approach
    std::wstring url; 
    int survivalScore;
};

class ContextEngine {
public:
    ContextEngine();
    ~ContextEngine();

    // Calculates a survival score for a tab based on the current active window
    int CalculateSurvivalScore(const std::wstring& tabTitle, const WindowInfo& activeWindow);

private:
    // Maps a process name (e.g., "Code.exe") to relevant keywords
    std::map<std::wstring, std::vector<std::wstring>> appContextMap;
    
    // Helper to check if string contains substring (case insensitive)
    bool ContainsKeyword(const std::wstring& text, const std::wstring& keyword);
    std::wstring ToLower(const std::wstring& str);
};

#endif // CONTEXT_ENGINE_HPP
