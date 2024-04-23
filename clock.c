#include <windows.h>
#include <time.h>

// Function prototypes
void UpdateTime(HWND hwnd, HFONT hFont);
HFONT CreateCustomFont(int height, int width);
void ResizeFont(HWND hwnd, HFONT *hFont, int height);
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam); // Add this line

// Global font object
HFONT hFont;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow)
{
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

    if (hwnd == NULL)
    {
        MessageBoxW(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, ncmdshow);
    UpdateWindow(hwnd);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    static int clientWidth, clientHeight; // Client area dimensions
    static BOOL titleVisible = TRUE;  // Initially show the title bar

    switch (msg)
    {
    case WM_KEYDOWN:
        if (wp == 'F')
        { // Toggle title bar visibility on 'T' key press
            DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);
            if (titleVisible)
            {
                SetWindowLong(hwnd, GWL_STYLE, dwStyle & ~WS_CAPTION);
                titleVisible = FALSE;
            }
            else
            {
                SetWindowLong(hwnd, GWL_STYLE, dwStyle | WS_CAPTION);
                titleVisible = TRUE;
            }
            SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
        }
        break;
    case WM_CREATE:
        clientHeight = 120; // Default starting height
        clientWidth = 480; // Default starting height
        hFont = CreateCustomFont(clientHeight, clientWidth);
        SetTimer(hwnd, 1, 1000, NULL);
        break;
    case WM_SIZE:
        clientWidth = LOWORD(lp);
        clientHeight = HIWORD(lp);
        if (hFont)
            DeleteObject(hFont);                // Delete the old font
        hFont = CreateCustomFont(clientHeight, clientWidth); // Create a new font with the appropriate size
        InvalidateRect(hwnd, NULL, TRUE);       // Redraw window
        break;
    case WM_PAINT:
        UpdateTime(hwnd, hFont);
        break;
    case WM_TIMER:
        InvalidateRect(hwnd, NULL, FALSE);
        break;
    case WM_DESTROY:
        if (hFont)
            DeleteObject(hFont);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wp, lp);
    }
    return 0;
}

HFONT CreateCustomFont(int height, int width)
{
    int fontSize = max( 50, max(width / 7 , height / 2) );
    return CreateFontW(fontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET,
                       OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                       DEFAULT_PITCH | FF_DONTCARE, L"Arial");
}

void UpdateTime(HWND hwnd, HFONT hFont)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    RECT rect;
    GetClientRect(hwnd, &rect);

    SetBkMode(hdc, OPAQUE);
    SetBkColor(hdc, RGB(0, 0, 0));
    FillRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));

    SetTextColor(hdc, RGB(255, 255, 255));
    SelectObject(hdc, hFont);
    time_t now = time(NULL);
    struct tm *timeinfo = localtime(&now);
    wchar_t timeStr[64];
    wcsftime(timeStr, sizeof(timeStr), L"%I:%M:%S %p", timeinfo);
    DrawTextW(hdc, timeStr, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    EndPaint(hwnd, &ps);
}
