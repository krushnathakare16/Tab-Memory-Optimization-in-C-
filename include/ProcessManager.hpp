#ifndef PROCESS_MANAGER_HPP
#define PROCESS_MANAGER_HPP

#include <string>
#include <vector>
#include <windows.h>
#include <psapi.h>

struct ProcessInfo {
    DWORD processId;
    std::wstring processName;
    SIZE_T memoryUsageBytes;
};

class ProcessManager {
public:
    ProcessManager();
    ~ProcessManager();

    // Fetches all matching processes by name (e.g., "chrome.exe", "msedge.exe")
    std::vector<ProcessInfo> GetProcessesByName(const std::wstring& targetName);

    // Kills a process by ID
    bool KillProcess(DWORD processId);

    // Suspends a process by ID (useful for inactive tabs)
    bool SuspendProcess(DWORD processId);

    // Resumes a suspended process
    bool ResumeProcess(DWORD processId);

private:
    std::wstring GetProcessName(DWORD processId);
    SIZE_T GetProcessMemoryUsage(DWORD processId);
};

#endif // PROCESS_MANAGER_HPP
