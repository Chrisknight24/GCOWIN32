/*Nom de File:OpenDialogBox.c
Role:File qui gere la creation des fonction qui permettent la creation
des template en memoire des boites de dialogues,ainsi que leur ouverture

BY CHIRSTIAN EDOUARDO LE 15/08/2024 a 13h16*/

#include <windows.h>
#include "resource.h"
# include "OpenDialogBox.h"

//Variable qui stocke l'id, le hwndOwner ect d'une dialogue box a creer
int cxChar,cyChar;

BOOL OpenDialogBox(int iType,int id,HWND hwndOwner,HINSTANCE hinst,
HWND hwndToClose,int iTypeDlgToClose)
{
    cxChar=LOWORD(GetDialogBaseUnits());
    cyChar=HIWORD(GetDialogBaseUnits());

    SetCurrentDirectory(szCurrentPath);

    //Fermer l'ancienne boite si necessaire
    if(hwndToClose)
    {
        if(iTypeDlgToClose==MODAL)
           EndDialog(hwndToClose,TRUE);
        else
            DeleteWindow(hwndToClose,hDlgModeless);

    }

    g_hwndParentObjToSet=hwndOwner;
    switch(id)
    {
    case DLG_CHOOSETYPEOBJET:
        OpenDlg_choosetypeobj(iType,hwndOwner,hinst);
        break;//fin case DLG_CHOOSETYPEOBJET

    case DLG_BUTTON:
        OpenDlg_button(iType,hwndOwner,hinst);
        break;//fin case DLG_BUTTON

    case DLG_LISTBOX:
        OpenDlg_listbox(iType,hwndOwner,hinst);
        break;//fin case DLG_LISTBOX

    case DLG_EDITBOX:
        OpenDlg_editbox(iType,hwndOwner,hinst);
        break;//fin case DLG_EDITBOX

    case DLG_COMBOBOX:
    OpenDlg_combobox(iType,hwndOwner,hinst);
        break;//fin case DLG_COMBOBOX

    case DLG_STATIC:
        OpenDlg_static(iType,hwndOwner,hinst);
        break;

    case DLG_DIALOGBOX:
        OpenDlg_dialogbox(iType,hwndOwner,hinst);
        break;
    default:
        g_hwndParentObjToSet=NULL;
        return FALSE;
    }
     g_hwndParentObjToSet=NULL;
    return TRUE;
}

LPWORD lpwAlign(LPWORD lpIn)
{
    //Ma fonction d'alignement :)
    ULONG ul;
    ul = (ULONG)lpIn;
    ul ++;
    ul >>=1;
    ul <<=1;
    return (LPWORD)ul;
}

// Alignement sur DWORD (4 octets)
LPDLGTEMPLATE AlignLpdt(LPVOID ptr) {
    // Calculer l'alignement nécessaire
    DWORD offset = (DWORD)ptr % sizeof(DWORD);
    if (offset != 0) {
        ptr += sizeof(DWORD) - offset;
    }
    return (LPDLGTEMPLATE)ptr;
}

// Alignement sur DWORD (4 octets)
LPDLGITEMTEMPLATE AlignLpdit(LPVOID ptr) {
    // Calculer l'alignement nécessaire
    DWORD offset = (DWORD)ptr % sizeof(DWORD);
    if (offset != 0) {
        ptr += sizeof(DWORD) - offset;
    }
    return (LPDLGITEMTEMPLATE)ptr;
}

size_t GetPow2(size_t maxdlgtemp)
{//Obtient la puissance de  2 directement superieure
    // a maxdlgtemp
    size_t  k=2;
    if(maxdlgtemp<0)
    {
        return -1;
    }
    while((int)(maxdlgtemp/k))
    {
        k=k<<1;
    }
    return k;
}

