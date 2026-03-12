#ifndef SYSTEM_MONITOR_HPP
#define SYSTEM_MONITOR_HPP

#include <string>
#include <windows.h>

struct WindowInfo {
    HWND handle;
    std::wstring title;
    std::wstring processName;
};

class SystemMonitor {
public:
    SystemMonitor();
    ~SystemMonitor();

    // Returns the current foreground window the user is interacting with
    WindowInfo GetActiveWindow();

    // Returns a top-level window handle for a given process ID
    HWND GetWindowForProcess(DWORD processId);

private:
    std::wstring GetWindowTextStr(HWND hwnd);
    std::wstring GetProcessNameFromWindow(HWND hwnd);
};

#endif // SYSTEM_MONITOR_HPP
