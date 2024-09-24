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
            switch(wParam) {
                case WTS_SESSION_LOCK:
                    printf("System is locked.\n");
                    break;
                case WTS_SESSION_UNLOCK:
                    printf("System is unlocked.\n");
                    break;
                case WTS_SESSION_LOGON:
                    printf("User logged on to the session.\n");
                    break;
                case WTS_SESSION_LOGOFF:
                    printf("User logged off the session.\n");
                    break;
                default:
                    // Using default handler for other cases
                    return DefWindowProc(hwnd, uMsg, wParam, lParam);
            }

            // Indicating the message is handled
            return 0;
            
        case WM_POWERBROADCAST:
            switch (wParam) {
                case PBT_APMSUSPEND:
                    printf("System is suspending.\n");
                    break;
                case PBT_APMRESUMEAUTOMATIC:
                    printf("System resumed from suspend.\n");
                    break;
                default:
                    // Using default handler for other cases
                    return DefWindowProc(hwnd, uMsg, wParam, lParam);
            }
            // Indicating the message is handled
            return TRUE;

        case WM_DESTROY:
            printf("Power monitor has been terminated.\n");
            PostQuitMessage(0);
            // Indicating the message is posted
            return 0;        
    }

    // Using default handler for other cases
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int main() {
    const char CLASS_NAME[] = "Power Monitor Class";

    WNDCLASS wc = { 0 };
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
    BOOL success = WTSRegisterSessionNotification(hwnd, NOTIFY_FOR_THIS_SESSION);
    if (!success) {
        printf("Failed to register for session notifications.\n");
        return 1;
    }

    // Register for suspend/resume notifications
    HPOWERNOTIFY hSuspendResumeNotify = RegisterSuspendResumeNotification(hwnd, DEVICE_NOTIFY_WINDOW_HANDLE);
    if (hSuspendResumeNotify == NULL) {
        printf("Failed to register for suspend/resume notifications.\n");
        WTSUnRegisterSessionNotification(hwnd);
        return 1;
    }

    printf("Listening for locked/unlocked and suspend/resume notifications...\n");

    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Unregister notifications before exiting
    UnregisterSuspendResumeNotification(hSuspendResumeNotify);
    WTSUnRegisterSessionNotification(hwnd);

    return 0;
}
