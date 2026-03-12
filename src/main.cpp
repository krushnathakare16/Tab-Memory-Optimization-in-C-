#include <iostream>
#include <thread>
#include <chrono>
#include <windows.h>
#include "ProcessManager.hpp"
#include "SystemMonitor.hpp"
#include "ContextEngine.hpp"
#include "TabController.hpp"

void SetColor(int colorCode) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorCode);
}

int main() {
    SetColor(11); // Light Cyan
    std::wcout << L"Starting Browser Tab Memory Optimizer (Pure C++)..." << std::endl;
    SetColor(7);  // Default White

    ProcessManager procManager;
    SystemMonitor sysMonitor;
    ContextEngine contextEngine;
    TabController tabController;

    if (!tabController.Initialize()) {
        SetColor(12); // Red
        std::wcout << L"Failed to initialize UI Automation/MSAA." << std::endl;
        SetColor(7);
        return 1;
    }

    while (true) {
        WindowInfo activeWindow = sysMonitor.GetActiveWindow();
        
        SetColor(14); // Yellow
        std::wcout << L"\n=========================================\n";
        SetColor(10); // Light Green
        std::wcout << L"[Context Focus] App: " << activeWindow.processName 
                   << L" | Title: " << activeWindow.title << std::endl;
        SetColor(7);

        std::wstring targetBrowser = L"chrome.exe";
        std::vector<ProcessInfo> browsers = procManager.GetProcessesByName(targetBrowser);
        
        SIZE_T totalMemory = 0;
        for (const auto& proc : browsers) {
            totalMemory += proc.memoryUsageBytes;
        }
        
        SetColor(13); // Magenta
        std::wcout << L"[Hardware] Chrome Memory Usage: " << (totalMemory / 1024 / 1024) << L" MB" << std::endl;
        SetColor(7);

        if (!browsers.empty()) {
            HWND browserHwnd = nullptr;
            for (const auto& proc : browsers) {
                browserHwnd = sysMonitor.GetWindowForProcess(proc.processId);
                if (browserHwnd) break; 
            }

            if (browserHwnd) {
                std::vector<BrowserTab> tabs = tabController.GetOpenTabs(browserHwnd);
                
                SetColor(13); // Magenta
                std::wcout << L"[Hardware] Total Chrome Memory: " << (totalMemory / 1024 / 1024) << L" MB";
                if (tabs.size() > 0) {
                    std::wcout << L" | Avg per Tab: " << (totalMemory / 1024 / 1024) / tabs.size() << L" MB";
                }
                std::wcout << std::endl;
                SetColor(7);

                std::wcout << L"Found " << tabs.size() << L" open tabs in foreground window." << std::endl;

                for (const auto& tab : tabs) {
                    int score = contextEngine.CalculateSurvivalScore(tab.title, activeWindow);
                    std::wcout << L"  -> Tab: [" << tab.title << L"] | Score: " << score;
                    if (score < 20) {
                        SetColor(12); // Red for low priority
                        std::wcout << L" (Low Priority - CLOSING TAB!)";
                        SetColor(7);
                        
                        // Close the tab by simulating a middle mouse click on its coordinates
                        tabController.CloseTab(tab);
                        Sleep(500); // Give the UI a moment to respond
                    } else if (score >= 60) {
                        SetColor(10); // Green for high priority
                        std::wcout << L" (High Priority Context Match!)";
                        SetColor(7);
                    }
                    std::wcout << std::endl;
                }
            } else {
                SetColor(13);
                std::wcout << L"[Hardware] Total Chrome Memory: " << (totalMemory / 1024 / 1024) << L" MB" << std::endl;
                SetColor(8); // Dark Gray
                std::wcout << L"Browser is running in background, no main window visible." << std::endl;
                SetColor(7);
            }
        } else {
            SetColor(8);
            std::wcout << L"Chrome is not currently running." << std::endl;
            SetColor(7);
        }

        Sleep(5000);
    }

    return 0;
}
