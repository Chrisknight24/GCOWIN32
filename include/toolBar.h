#ifndef TOOLBAR_H_INCLUDED
#define TOOLBAR_H_INCLUDED
#define ID_STOP_TOOL 7000


//Les extern
extern int g_iIndexStopTB;
extern int g_iIndexRunTB;
extern BOOL g_fisTestRunAble;
//Les prototypes
HWND CreateToolBar(HWND hwndParent);
LRESULT RelabelButton(HWND hWndToolbar,PSTR pszNew,int idButton);
BOOL MyEnableButtonToolBar(HWND hwndToolBar,int idButton,BOOL fEnable,BOOL fShouldGray);

#endif // TOOLBAR_H_INCLUDED
