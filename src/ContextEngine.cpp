#include "ContextEngine.hpp"
#include <algorithm>
#include <cctype>

ContextEngine::ContextEngine() {
    // Hardcode some simple associations for demonstration
    // In a real project, this might load from a JSON config file.
    
    // Developer Context (VS Code, Visual Studio, IDEs)
    appContextMap[L"Code.exe"] = {L"github", L"stackoverflow", L"cppreference", L"c++", L"tutorial"};
    appContextMap[L"devenv.exe"] = {L"github", L"stackoverflow", L"microsoft docs", L"c++", L"cmake"};

    // Writing/Research Context (Word, Notion)
    appContextMap[L"WINWORD.EXE"] = {L"docs", L"research", L"wiki", L"grammar"};
    appContextMap[L"Notion.exe"] = {L"wiki", L"docs", L"project", L"notes"};
    
    // Entertainment Context (Games, Spotify)
    appContextMap[L"Spotify.exe"] = {L"music", L"lyrics", L"youtube"};
    appContextMap[L"Steam.exe"] = {L"wiki", L"guide", L"walkthrough", L"steam"};
}

ContextEngine::~ContextEngine() {}

std::wstring ContextEngine::ToLower(const std::wstring& str) {
    std::wstring lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
                   [](wchar_t c) { return std::tolower(c); });
    return lowerStr;
}

bool ContextEngine::ContainsKeyword(const std::wstring& text, const std::wstring& keyword) {
    std::wstring lowerText = ToLower(text);
    std::wstring lowerKeyword = ToLower(keyword);
    return lowerText.find(lowerKeyword) != std::wstring::npos;
}

int ContextEngine::CalculateSurvivalScore(const std::wstring& tabTitle, const WindowInfo& activeWindow) {
    int score = 0;
    
    // 1. Base Score (all tabs start with 10)
    score += 10;
    
    // 2. Keyword Matching based on Active Process
    std::wstring activeProc = activeWindow.processName;
    if (appContextMap.find(activeProc) != appContextMap.end()) {
        const auto& keywords = appContextMap[activeProc];
        for (const auto& kw : keywords) {
            if (ContainsKeyword(tabTitle, kw)) {
                score += 50; // High bonus for matching context
                break;       // Add bonus once per tab
            }
        }
    }
    
    // 3. Penalty for random tabs we don't care about (e.g., social media while coding)
    if (activeProc == L"Code.exe" || activeProc == L"devenv.exe") {
        if (ContainsKeyword(tabTitle, L"facebook") || 
            ContainsKeyword(tabTitle, L"twitter") ||
            ContainsKeyword(tabTitle, L"instagram")) {
            score -= 20; // Penalty
        }
    }

    // 4. Important always-alive keywords (like email / calendar)
    if (ContainsKeyword(tabTitle, L"mail") || ContainsKeyword(tabTitle, L"calendar")) {
        score += 30;
    }

    return score;
}
