#include "TabController.hpp"
#include <iostream>

TabController::TabController() {}
TabController::~TabController() {
    CoUninitialize();
}

bool TabController::Initialize() {
    HRESULT hr = CoInitialize(NULL);
    return SUCCEEDED(hr);
}

std::vector<BrowserTab> TabController::GetOpenTabs(HWND browserHwnd) {
    std::vector<BrowserTab> tabs;
    IAccessible* pAccMain = nullptr;

    HRESULT hr = AccessibleObjectFromWindow(browserHwnd, OBJID_WINDOW, 
                                            IID_IAccessible, (void**)&pAccMain);
    if (SUCCEEDED(hr) && pAccMain) {
        TraverseAccessibleTree(pAccMain, tabs);
        pAccMain->Release();
    }
    return tabs;
}

void TabController::TraverseAccessibleTree(IAccessible* pAcc, std::vector<BrowserTab>& tabs) {
    if (!pAcc) return;

    long childCount = 0;
    if (FAILED(pAcc->get_accChildCount(&childCount)) || childCount == 0) {
        return;
    }

    VARIANT* pVars = new VARIANT[childCount];
    long obtained = 0;
    HRESULT hr = AccessibleChildren(pAcc, 0, childCount, pVars, &obtained);

    if (SUCCEEDED(hr)) {
        for (long i = 0; i < obtained; i++) {
            if (pVars[i].vt == VT_DISPATCH) {
                IAccessible* pChild = nullptr;
                if (SUCCEEDED(pVars[i].pdispVal->QueryInterface(IID_IAccessible, (void**)&pChild))) {
                    
                    // Check if role is ROLE_SYSTEM_PAGETAB
                    VARIANT varSelf;
                    varSelf.vt = VT_I4;
                    varSelf.lVal = CHILDID_SELF;
                    
                    VARIANT varRole;
                    VariantInit(&varRole);
                    if (SUCCEEDED(pChild->get_accRole(varSelf, &varRole))) {
                        if (varRole.vt == VT_I4 && varRole.lVal == ROLE_SYSTEM_PAGETAB) {
                            BSTR bstrName = nullptr;
                            if (SUCCEEDED(pChild->get_accName(varSelf, &bstrName)) && bstrName) {
                                BrowserTab tab;
                                tab.title = std::wstring(bstrName, SysStringLen(bstrName));
                                
                                // Get physical location
                                long x, y, w, h;
                                if (SUCCEEDED(pChild->accLocation(&x, &y, &w, &h, varSelf))) {
                                    tab.x = x;
                                    tab.y = y;
                                    tab.width = w;
                                    tab.height = h;
                                }

                                // Only push valid titles
                                if (!tab.title.empty() && tab.title != L"New Tab") {
                                    tabs.push_back(tab);
                                }
                                SysFreeString(bstrName);
                            }
                        }
                    }
                    VariantClear(&varRole);

                    // Recurse down
                    TraverseAccessibleTree(pChild, tabs);
                    pChild->Release();
                }
            }
            VariantClear(&pVars[i]);
        }
    }
    delete[] pVars;
}

void TabController::CloseTab(const BrowserTab& tab) {
    if (tab.width == 0 || tab.height == 0) return;

    // Save current mouse position
    POINT ptStr;
    GetCursorPos(&ptStr);

    // Calculate center of the tab
    int targetX = tab.x + (tab.width / 2);
    int targetY = tab.y + (tab.height / 2);

    // Convert to absolute mouse coordinates (0 to 65535)
    int screenX = GetSystemMetrics(SM_CXSCREEN);
    int screenY = GetSystemMetrics(SM_CYSCREEN);
    
    // Set up inputs for click: move, middle button down, middle button up
    INPUT inputs[3] = {};
    
    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dx = (targetX * 65535) / screenX;
    inputs[0].mi.dy = (targetY * 65535) / screenY;
    inputs[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

    inputs[1].type = INPUT_MOUSE;
    inputs[1].mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;

    inputs[2].type = INPUT_MOUSE;
    inputs[2].mi.dwFlags = MOUSEEVENTF_MIDDLEUP;

    // Send the inputs to simulate a middle-click to close the tab
    SendInput(3, inputs, sizeof(INPUT));

    // Optional: Restore mouse position
    SetCursorPos(ptStr.x, ptStr.y);
}
