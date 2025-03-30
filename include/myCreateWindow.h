#ifndef MYCREATEWINDOW_H_INCLUDED
#define MYCREATEWINDOW_H_INCLUDED

#include <windows.h>
#include <stdio.h>

typedef struct INFWND INFWND;
struct INFWND
{
    LPSTR _1_szClassName;
    LPSTR _2_szWndName;
    DWORD _3_dwWndStyle;
    int _4_x;
    int _5_y;
    int _6_nWidth;
    int _7_nHeight;
    HWND _8_hWndParent;
    HMENU _9_hMenu;
    HINSTANCE _10_hInstance;
    LPVOID  _11_lpParam;
};
HWND myCreateWindow(INFWND infWnd);



#endif // MYCREATEWINDOW_H_INCLUDED
