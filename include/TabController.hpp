#ifndef TAB_CONTROLLER_HPP
#define TAB_CONTROLLER_HPP

#include <string>
#include <vector>
#include <windows.h>
#include <oleacc.h>

struct BrowserTab {
    std::wstring title;
    long x, y, width, height; // Screen coordinates of the tab
};

class TabController {
public:
    TabController();
    ~TabController();

    bool Initialize();

    // Gets all tab titles from a given browser window handle using MSAA
    std::vector<BrowserTab> GetOpenTabs(HWND browserHwnd);

    // Closes a specific tab using a simulated middle mouse click based on its coordinates
    void CloseTab(const BrowserTab& tab);

private:
    void TraverseAccessibleTree(IAccessible* pAcc, std::vector<BrowserTab>& tabs);
};

#endif // TAB_CONTROLLER_HPP
