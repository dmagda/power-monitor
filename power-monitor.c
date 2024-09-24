#include <windows.h>
#include <wtsapi32.h>
#include <powrprof.h>
#include <stdio.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "wtsapi32.lib")
#pragma comment(lib, "powrprof.lib")

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_WTSSESSION_CHANGE:
            if (wParam == WTS_SESSION_LOCK) {
                printf("System is locked.\n");
            } else if (wParam == WTS_SESSION_UNLOCK) {
                printf("System is unlocked.\n");
            }
            break;
        case WM_POWERBROADCAST:
            switch (wParam) {
                case PBT_APMSUSPEND:
                    printf("System is suspending.\n");
                    break;
                case PBT_APMRESUMEAUTOMATIC:
                    printf("System resumed from suspend.\n");
                    break;
                default:
                    return DefWindowProc(hwnd, uMsg, wParam, lParam);
            }
            return TRUE;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int main() {
    const char CLASS_NAME[] = "Power Monitor Class";

    WNDCLASS wc = { };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

     // Create the hidden window
    HWND hwnd = CreateWindowEx(
        WS_EX_TOOLWINDOW, // Extended style to create a hidden tool window
        wc.lpszClassName,
        "Hidden Window",
        WS_POPUP, // Window style to make it invisible
        0, 0, 0, 0, // Zero dimensions to make it hidden
        NULL, NULL, wc.hInstance, NULL);

    if (!hwnd) {
        printf("Failed to create hidden window.\n");
        return 1;
    }

    // Register to receive session change notifications
    WTSRegisterSessionNotification(hwnd, NOTIFY_FOR_THIS_SESSION);

    // Register to receive power setting notifications
    GUID GUID_SLEEP_SETTING = GUID_SLEEP_SUBGROUP; // Register for sleep settings
    HPOWERNOTIFY hPowerNotify = RegisterPowerSettingNotification(hwnd, &GUID_SLEEP_SETTING, DEVICE_NOTIFY_WINDOW_HANDLE);
    if (hPowerNotify == NULL) {
        printf("Failed to register for power setting notifications.\n");
    }

    // Register for suspend/resume notifications
    HPOWERNOTIFY hSuspendResumeNotify = RegisterSuspendResumeNotification(hwnd, DEVICE_NOTIFY_WINDOW_HANDLE);
    if (hSuspendResumeNotify == NULL) {
        printf("Failed to register for suspend/resume notifications.\n");
    }

    printf("Listening for locked/unlocked and suspend/resume notifications...\n");

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Unregister notifications before exiting
    UnregisterPowerSettingNotification(hPowerNotify);
    UnregisterSuspendResumeNotification(hSuspendResumeNotify);
    WTSUnRegisterSessionNotification(hwnd);

    return 0;
}
