#include "SystemMonitor.hpp"
#include <psapi.h>

SystemMonitor::SystemMonitor() {}
SystemMonitor::~SystemMonitor() {}

std::wstring SystemMonitor::GetWindowTextStr(HWND hwnd) {
    const int bufferSize = 1024;
    wchar_t windowTitle[bufferSize];
    if (GetWindowTextW(hwnd, windowTitle, bufferSize) > 0) {
        return std::wstring(windowTitle);
    }
    return L"";
}

std::wstring SystemMonitor::GetProcessNameFromWindow(HWND hwnd) {
    DWORD processId;
    std::wstring processName = L"<unknown>";
    
    GetWindowThreadProcessId(hwnd, &processId);
    if (processId != 0) {
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
        if (hProcess) {
            HMODULE hMod;
            DWORD cbNeeded;
            if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
                wchar_t szProcessName[MAX_PATH];
                if (GetModuleBaseNameW(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(wchar_t))) {
                    processName = szProcessName;
                }
            }
            CloseHandle(hProcess);
        }
    }
    return processName;
}

WindowInfo SystemMonitor::GetActiveWindow() {
    WindowInfo info;
    info.handle = GetForegroundWindow();
    
    if (info.handle) {
        info.title = GetWindowTextStr(info.handle);
        info.processName = GetProcessNameFromWindow(info.handle);
    }

    return info;
}

struct FindWindowData {
    DWORD processId;
    HWND hWnd;
};

BOOL CALLBACK EnumWindowsProcMatchId(HWND hWnd, LPARAM lParam) {
    FindWindowData* data = (FindWindowData*)lParam;
    DWORD procId = 0;
    GetWindowThreadProcessId(hWnd, &procId);
    
    // We want a visible, top-level window
    if (procId == data->processId && GetWindow(hWnd, GW_OWNER) == (HWND)0 && IsWindowVisible(hWnd)) {
        data->hWnd = hWnd;
        return FALSE; // Found it, stop enumerating
    }
    return TRUE;
}

HWND SystemMonitor::GetWindowForProcess(DWORD processId) {
    FindWindowData data = { processId, nullptr };
    EnumWindows(EnumWindowsProcMatchId, (LPARAM)&data);
    return data.hWnd;
}
