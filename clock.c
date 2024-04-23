#include <windows.h>
#include <time.h>

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
void ShowTime(HWND hwnd, HFONT hFont);

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) {
    WNDCLASSW wc = {0};

    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hInstance = hInst;
    wc.lpszClassName = L"DigitalClock";
    wc.lpfnWndProc = WindowProcedure;

    if (!RegisterClassW(&wc))
        return -1;

    HWND hwnd = CreateWindowW(L"DigitalClock", L"Digital Clock",
                              WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME | WS_CAPTION),
                              CW_USEDEFAULT, CW_USEDEFAULT, 480, 120, 
                              NULL, NULL, hInst, NULL);
    

    if (hwnd == NULL) {
        MessageBoxW(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, ncmdshow);
    UpdateWindow(hwnd);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    static HFONT hFont = NULL;
    static BOOL mouseInside = FALSE;  // Track mouse position: inside or outside

    switch (msg) {
        case WM_CREATE:
            // Create the font once when the window is created
            hFont = CreateFontW(72, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET,
                                OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                                DEFAULT_PITCH | FF_DONTCARE, L"Arial");
            SetTimer(hwnd, 1, 1000, NULL);
            break;

        case WM_MOUSEMOVE:
            if (!mouseInside) {
                // Mouse has entered the window, add title bar
                mouseInside = TRUE;
                SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) | WS_CAPTION);
                SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
                TRACKMOUSEEVENT tme;
                tme.cbSize = sizeof(tme);
                tme.dwFlags = TME_LEAVE;
                tme.hwndTrack = hwnd;
                TrackMouseEvent(&tme);
            }
            break;

        case WM_MOUSELEAVE:
            // Mouse has left the window, remove title bar
            mouseInside = FALSE;
            SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_CAPTION);
            SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
            break;

        case WM_PAINT:
            ShowTime(hwnd, hFont);
            break;

        case WM_TIMER:
            InvalidateRect(hwnd, NULL, TRUE); // Redraw the window
            break;

        case WM_DESTROY:
            DeleteObject(hFont);  // Clean up the font object
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProcW(hwnd, msg, wp, lp);
    }
    return 0;
}

void ShowTime(HWND hwnd, HFONT hFont) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    RECT rect;
    GetClientRect(hwnd, &rect);
    SetBkColor(hdc, RGB(0, 0, 0));  // Background color black
    SetTextColor(hdc, RGB(255, 255, 255));  // Text color white
    SelectObject(hdc, hFont);  // Use the selected font
    time_t now = time(NULL);
    struct tm *localTime = localtime(&now);
    wchar_t timeStr[64];
    wcsftime(timeStr, sizeof(timeStr), L"%I:%M:%S %p", localTime); // 12-hour format with AM/PM
    DrawTextW(hdc, timeStr, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    EndPaint(hwnd, &ps);
}

