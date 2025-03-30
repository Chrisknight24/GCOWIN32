#ifndef MANAGEFONT_H_INCLUDED
#define MANAGEFONT_H_INCLUDED
#include <windows.h>
#include <commdlg.h>


#define ID_USE_PARENT_FONT 3000
#define ID_USE_ANOTHER_FONT 3001
#define IDB_GROUPBOX2 3002

//prototypes
BOOL PopFontChooseFont (HWND hwnd,LOGFONT *logfont);
void PopFontInitialize (HWND hwndEdit,HFONT *hFont,LOGFONT *logfont);
void PopFontSetFont (HWND hwndEdit,HFONT *hFont,LOGFONT *logfont);
void PopFontDeinitialize (HFONT *hFont);
BOOL MyManageFont(int idDlg,HWND hDlg);
void MyUpDateDimChar(HWND hwndParent);
void MySetFontParent(HWND hwndParent,MYFONT *myf);

#endif // MANAGEFONT_H_INCLUDED
