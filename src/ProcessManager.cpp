#include "ProcessManager.hpp"
#include <iostream>

ProcessManager::ProcessManager() {}

ProcessManager::~ProcessManager() {}

std::vector<ProcessInfo> ProcessManager::GetProcessesByName(const std::wstring& targetName) {
    std::vector<ProcessInfo> matchedProcesses;
    DWORD processIds[1024];
    DWORD bytesReturned;
    
    if (!EnumProcesses(processIds, sizeof(processIds), &bytesReturned)) {
        return matchedProcesses;
    }

    DWORD numProcesses = bytesReturned / sizeof(DWORD);
    for (DWORD i = 0; i < numProcesses; i++) {
        if (processIds[i] != 0) {
            std::wstring procName = GetProcessName(processIds[i]);
            if (procName == targetName) {
                ProcessInfo info;
                info.processId = processIds[i];
                info.processName = procName;
                info.memoryUsageBytes = GetProcessMemoryUsage(processIds[i]);
                matchedProcesses.push_back(info);
            }
        }
    }

    return matchedProcesses;
}

std::wstring ProcessManager::GetProcessName(DWORD processId) {
    std::wstring processName = L"<unknown>";
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
    return processName;
}

SIZE_T ProcessManager::GetProcessMemoryUsage(DWORD processId) {
    SIZE_T memUsage = 0;
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    
    if (hProcess) {
        PROCESS_MEMORY_COUNTERS pmc;
        if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
            memUsage = pmc.WorkingSetSize;
        }
        CloseHandle(hProcess);
    }
    return memUsage;
}

bool ProcessManager::KillProcess(DWORD processId) {
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processId);
    if (hProcess == NULL) return false;
    
    bool result = TerminateProcess(hProcess, 1);
    CloseHandle(hProcess);
    return result;
}

bool ProcessManager::SuspendProcess(DWORD processId) {
    // NT API for Suspending process
    typedef LONG(NTAPI* NtSuspendProcess)(IN HANDLE ProcessHandle);
    NtSuspendProcess pfnNtSuspendProcess = (NtSuspendProcess)GetProcAddress(GetModuleHandleA("ntdll"), "NtSuspendProcess");
    
    if (!pfnNtSuspendProcess) return false;

    HANDLE hProcess = OpenProcess(PROCESS_SUSPEND_RESUME, FALSE, processId);
    if (!hProcess) return false;

    LONG status = pfnNtSuspendProcess(hProcess);
    CloseHandle(hProcess);
    
    return (status >= 0);
}

bool ProcessManager::ResumeProcess(DWORD processId) {
    typedef LONG(NTAPI* NtResumeProcess)(IN HANDLE ProcessHandle);
    NtResumeProcess pfnNtResumeProcess = (NtResumeProcess)GetProcAddress(GetModuleHandleA("ntdll"), "NtResumeProcess");
    
    if (!pfnNtResumeProcess) return false;

    HANDLE hProcess = OpenProcess(PROCESS_SUSPEND_RESUME, FALSE, processId);
    if (!hProcess) return false;

    LONG status = pfnNtResumeProcess(hProcess);
    CloseHandle(hProcess);
    
    return (status >= 0);
}
