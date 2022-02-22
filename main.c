#include <stdio.h>
#include <Windows.h>
#include <Tlhelp32.h>

#define EDIT_TEXT_ID     1
#define BTN_KILL_PROC    2
#define BTN_FIND_PROC    3
#define STATIC_TEXT_PROC 4


HWND editText;
HWND btnKillProc;
HWND btnFindProc;
HWND staticTextProc;

char g_info[1024];


void processesKill(const char* filename)
{
    if (strcmp(filename, "threads.exe") == 0)
    {
        printf("process can't be killed\n");
        MessageBox(NULL, "I can't kill myself", "Oops", MB_OK);
        return;
    }
       
    _Bool b_isDel = FALSE;
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof(pEntry);
    _Bool b_hRes = Process32First(hSnapShot, &pEntry);

    while (b_hRes)
    {
        if (strcmp(pEntry.szExeFile, filename) == 0)
        {
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0, pEntry.th32ProcessID);

            if (hProcess != NULL)
            {
                b_isDel = TRUE;
                TerminateProcess(hProcess, 1);
                CloseHandle(hProcess);
            }
        }

        b_hRes = Process32Next(hSnapShot, &pEntry);
    }

    if (b_isDel)
        printf("process deleted\n");
    else
        printf("process was not found\n");

    CloseHandle(hSnapShot);
}

void processesReadInfo(const char* filename)
{
    _Bool b_isFnd = FALSE;
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof(pEntry);
    _Bool b_hRes = Process32First(hSnapShot, &pEntry);

    while (b_hRes)
    {
        if (strcmp(pEntry.szExeFile, filename) == 0)
        {
            b_isFnd = TRUE;
            memset(g_info, 0, 1024);
            sprintf(g_info, "%s\nProcess identifier: %lu\nNumber of execution threads: %lu\n\
Parent process identifier: %lu",
            pEntry.szExeFile, pEntry.th32ProcessID, pEntry.cntThreads, pEntry.th32ParentProcessID);
        }

        b_hRes = Process32Next(hSnapShot, &pEntry);
    }
    
    if (b_isFnd)
        printf("process found\n");
    else
        printf("process was not found\n");

    CloseHandle(hSnapShot);
}


LRESULT CALLBACK wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static char str[128];

    switch (uMsg)
    {
        case WM_CREATE:
        {
            

            // creating a field text for the process name search
            editText = CreateWindowEx(0, "edit", "Enter a process name", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_RIGHT,
                50, 50, 200, 20, hwnd, EDIT_TEXT_ID, NULL, NULL);

            // button for kill processes
            btnKillProc = CreateWindowEx(0, "button", "Kill", WS_VISIBLE | WS_CHILD,
                80, 80, 50, 20, hwnd, BTN_KILL_PROC, NULL, NULL);

            // button for search processes
            btnFindProc = CreateWindowEx(0, "button", "Find", WS_VISIBLE | WS_CHILD,
                160, 80, 50, 20, hwnd, BTN_FIND_PROC, NULL, NULL);

            // text field
            staticTextProc = CreateWindowEx(0, "static", "", WS_VISIBLE | WS_CHILD,
                300, 50, 300, 300, hwnd, STATIC_TEXT_PROC, NULL, NULL);

            break;
        }
        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
                case EDIT_TEXT_ID:
                {
                    GetWindowText(lParam, str, 128);
                    break;
                }
                case BTN_KILL_PROC:
                {
                    processesKill(str);
                    break;
                }
                case BTN_FIND_PROC:
                {
                    processesReadInfo(str);
                    SetWindowText(staticTextProc, g_info);
                    break;
                }
            }

            break;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            break;
        }
        default:
        {
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }
}


int main()
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HGLRC hrc;
    HDC hdc;
    MSG msg;

    _Bool b_quit = FALSE;

    HBRUSH menuColor = CreateSolidBrush(RGB(220, 220, 220));

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = wndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = menuColor;
    wcex.lpszClassName = "wndClass";
    wcex.lpszMenuName = NULL;
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
        return 0;

    hwnd = CreateWindowEx(0, "wndClass", "Process killer", WS_OVERLAPPEDWINDOW, 100, 100, 640, 480,
        NULL, NULL, NULL, NULL);

    ShowWindow(hwnd, SW_SHOWNORMAL);

    hdc = GetDC(hwnd);

    while (!b_quit)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                b_quit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
         
        }
    }

    return 0;
}
