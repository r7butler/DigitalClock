#include <windows.h>
#include <time.h>

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
void UpdateTime(HWND hwnd);
void ResizeFont(HWND hwnd, HFONT* hFont, int height);

HFONT hFont; // Global font object

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
                              WS_OVERLAPPEDWINDOW, // Allow standard window features
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
    static BOOL titleVisible = TRUE;  // Initially show the title bar

    switch (msg) {
    case WM_KEYDOWN:
        if (wp == 'T') {  // Toggle title bar visibility on 'T' key press
            DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);
            if (titleVisible) {
                SetWindowLong(hwnd, GWL_STYLE, dwStyle & ~WS_CAPTION);
                titleVisible = FALSE;
            } else {
                SetWindowLong(hwnd, GWL_STYLE, dwStyle | WS_CAPTION);
                titleVisible = TRUE;
            }
            SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
        }
        break;

    case WM_CREATE:
        SetTimer(hwnd, 1, 1000, NULL);  // Set a timer to update every second
        break;

    case WM_TIMER:
        if (wp == 1) {  // Ensure we're handling the correct timer
            InvalidateRect(hwnd, NULL, TRUE);  // Force the window to redraw
        }
        break;

    case WM_SIZE:  // Handle window resizing
        InvalidateRect(hwnd, NULL, TRUE);  // Invalidate the window to force a redraw
        break;

    case WM_PAINT:
        UpdateTime(hwnd);
        break;

    case WM_DESTROY:
        KillTimer(hwnd, 1);  // Remove the timer
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProcW(hwnd, msg, wp, lp);
    }
    return 0;
}




void UpdateTime(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    RECT rect;
    GetClientRect(hwnd, &rect); // Get the size of the area inside the window

    SetBkMode(hdc, OPAQUE);
    SetBkColor(hdc, RGB(0, 0, 0)); // Background color black
    FillRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));

    SetTextColor(hdc, RGB(255, 255, 255)); // Text color white
    SelectObject(hdc, hFont);
    time_t now = time(NULL);
    struct tm *timeinfo = localtime(&now);
    wchar_t timeStr[64];
    wcsftime(timeStr, sizeof(timeStr), L"%I:%M:%S %p", timeinfo);
    DrawTextW(hdc, timeStr, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    EndPaint(hwnd, &ps);
}



void ResizeFont(HWND hwnd, HFONT* hFont, int height) {
    if (*hFont) DeleteObject(*hFont); // Delete old font

    int fontSize = max(20, height / 15); // Calculate new font size
    *hFont = CreateFontW(fontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET,
                         OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                         DEFAULT_PITCH | FF_DONTCARE, L"Arial");
}
