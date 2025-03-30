/*Nom de File:MyChildProcDlgButton.c
Role:File qui gere l les fenetres childs personnalisees de la boite
de dialogue DLG_BUTTON
BY CHIRSTIAN EDOUARDO LE 05/09/2024 a 12h30*/

#include <windows.h>
#include "resource.h"
#include "OpenDialogBox.h"

LRESULT CALLBACK MyChildProcDlgGroupBox(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{

/*
    id=GetDlgCtrlID(hwnd);

    if(id==ID_STYLE_LIST)
    {

    }//fin if id==ID_STYLE_LIST
    else if(id==)
  */
    switch(msg)
    {
    case WM_DESTROY:

        return 0;
    }
    return DefWindowProc(hwnd,msg,wParam,lParam);
}
