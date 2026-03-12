🚀 Browser Tab Memory Optimizer - C++ Project Walkthrough
The project has been fully built from scratch using 100% C++. It monitors your system's active context (the application you are currently looking at) and scans your open Chrome/Edge tabs, scoring them based on relevance.

🛠 Architecture & Implementation
The application is built completely modularly using modern C++ abstractions and standard Windows native APIs.

Core Modules
ProcessManager
:
Uses psapi.h and EnumProcesses to find browser executables running in the background.
Calculates the Working Set Memory (RAM consumed) to show you how much memory Chrome is currently eating.
Now calculates the average memory usage per active tab so you can easily identify total vs average impact.
SystemMonitor
:
Uses GetForegroundWindow and GetWindowThreadProcessId to constantly track which application the user is currently working on. It differentiates between your IDE (Code.exe), Word, and Games.
TabController
:
Since MinGW was lacking some modern UI Automation headers, I implemented a robust fallback using Microsoft Active Accessibility (MSAA - oleacc.h).
It forcefully walks the internal UI tree of the Chrome browser window, looking for UI elements with the role ROLE_SYSTEM_PAGETAB to extract the tab titles without requiring a browser extension!
Tab Closing: It extracts specific physical screen coordinates (X & Y) for each tab and simulates a MOUSEEVENTF_MIDDLEDOWN and MOUSEEVENTF_MIDDLEUP (Middle-Click) via the Windows INPUT API to seamlessly and dynamically close any tab identified as low priority.
ContextEngine
:
The "brain" of the project. It maps applications to keywords.
Example logic: If User is active in VS Code (Code.exe), give a +50 survival score to a tab titled "StackOverflow", but give a -20 penalty to a tab titled "Facebook".
C++ Console UI
Since this is a C++ command-line application, I used SetConsoleTextAttribute to format the terminal into a readable, colored dashboard that updates every 5 seconds.

🧪 How to Verify
Since the .exe is already compiled, you can run and test it right now!

Open your terminal in the Cpp_Project folder.
Run the executable:
powershell
.\optimizer.exe
Open Google Chrome. Open a few tabs with titles like "C++ Tutorial", "StackOverflow", "Facebook", and "YouTube".
Click on your Code Editor (e.g. VS Code). Watch the C++ console assign high scores to the programming tabs, and flag the social media tabs as "Low Priority - CLOSING TAB!" in Red.
The application will compute exact screen coordinates and auto-close those unnecessary background tabs while you work!
NOTE

Simulated inputs (Middle Mouses) require the targeted window (Chrome) to be visible on your screen for the clicks to land correctly, which is the standard limitation of native C++ GUI automation without deep-hooking extensions. If Chrome is in the background, it will still accurately compute memory savings and score tabs without closing them erroneously.