/*Nom du File:MyManageMenu.c
Role:File qui gere la creation de la boite de dialogue
qui permet la gestion des menus*/

#include <windows.h>
#include "resource.h"
#include "OpenDialogBox.h"

LRESULT CALLBACK DlgMenuProc(HWND hDlg,UINT msg, WPARAM wParam, LPARAM lParam);
MYMENULIST *menuTemp=NULL;
MYMENULIST *copieMenuTemp=NULL;
int idGroupChoosen=-1;
int g_idMenuActu=0;
UINT g_idItemActu=WM_USER+1;
BOOL fShouldInotify=TRUE;
MYCACHE_ITEM g_mycacheItem;

BOOL MyManageMenu(HWND hwndOwner)
{
    //Creation de la boite de dialogue "Menu"
    //La procedure de cette boite se situe aussi dans ce File

     size_t cBytes,length;
    HGLOBAL hgbl;
    LPDLGTEMPLATE lpdt;
    LPDLGITEMTEMPLATE lpdit;
    LPWORD lpw;
    LPWSTR lpwsz;
    int iPosItemX,iPosItemY,cx,x,y,cy;
    RECT rcParent,rcDlg;
    int nbChild=0;
    wchar_t *wchaine;
    char *name;
    HINSTANCE hinst;

    MY_DLGDATA infoDlg;
    int id=DLG_DIALOGBOX;//id du parent


    hinst=(HINSTANCE)GetWindowLongPtr(hwndOwner,
                            GWLP_HINSTANCE);

    //informations a passer a la procedure de fenetre via lparma du inidialog

    infoDlg.idDlg=id;
    infoDlg.iTypeDlg=MODAL;
    infoDlg.hWndOwner=hwndOwner;
    infoDlg.hinst=hinst;
    //espace necessaire a allouer pour une boite de  child 100 controls
     cBytes=sizeof(DLGTEMPLATE)+(3*MAXDLGCHAR)+
                    100*(sizeof(DLGITEMTEMPLATE)+(2*MAXDLGCHAR));
    //Obtenir la puissance de deux directement superieure a cBytes
    cBytes=GetPow2(cBytes);
    if(cBytes==-1) return FALSE;

    hgbl=GlobalAlloc(GMEM_ZEROINIT,cBytes);
    if(!hgbl)
    {
        MessageBox(hwndOwner,TEXT("Allocation of hgbl  in openDialogBox fun  failed!"),
                    TEXT("ERROR"),MB_ICONERROR);
        return FALSE;
    }

    //Dimension de dlgColr
    GetClientRect(hwndOwner,&rcParent);//Screen coordinates
    rcDlg.left=rcParent.left;
    rcDlg.top=rcParent.top;
    rcDlg.right=rcParent.right;
    rcDlg.bottom=rcParent.bottom-(GetSystemMetrics(SM_CYCAPTION));

    rcDlg.left /=2;
    rcDlg.top /=2;
    rcDlg.right /=2;
    rcDlg.bottom /=2;

    lpdt=(LPDLGTEMPLATE)GlobalLock(hgbl);
    lpdt=AlignLpdt(lpdt);
    lpdt->x = rcDlg.left; lpdt->y = rcDlg.top;
    lpdt->cx = rcDlg.right-rcDlg.left;
    lpdt->cy = rcDlg.bottom-rcDlg.top;
    //conversion WORD Boundaries
    lpw = (LPWORD)(lpdt + 1);
    *lpw++ = 0; // No menu
    *lpw++ = 0; // Predefined dialog box class (by default)

    wchaine=L"Menu Settings";
    length=wcslen(wchaine)+1;//+1 pour le nul
    memcpy(lpw,wchaine,sizeof(wchar_t)*length);
    lpw+=length;

    //Controls childs
    cxChar=LOWORD(GetDialogBaseUnits());
    cyChar=HIWORD(GetDialogBaseUnits());
    OffsetRect(&rcDlg,-rcDlg.left,-(rcDlg.top));
    iPosItemX=rcDlg.right-cxChar/2;
    iPosItemY=rcDlg.bottom-5-cyChar/2;


    //-----------------------
    // Define an Cancel button.
    //-----------------------
    nbChild++;
    name="Cancel";
    wchaine=L"&Cancel";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2; lpdit->cy = cyChar;
    lpdit->x = rcDlg.right/2- lpdit->cx-cxChar/2; lpdit->y = rcDlg.bottom-lpdit->cy-cyChar;
    iPosItemX=lpdit->x+lpdit->cx;
    iPosItemY=lpdit->y;
    lpdit->id = IDCANCEL; // apply button identifier
    lpdit->style = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON|
    WS_TABSTOP|WS_GROUP;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080; // Button class

    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    *lpw++ = 0; // No creation data


    //-----------------------
    // Define an Apply button.
    //-----------------------
    nbChild++;
    name="Apply";
    wchaine=L"A&pply";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2; lpdit->cy = cyChar;
    lpdit->x = iPosItemX+cxChar; lpdit->y = iPosItemY;
    iPosItemX=lpdit->x-cxChar;
    iPosItemY=lpdit->y;
    lpdit->id = ID_APPLY;
    lpdit->style = WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON|
    WS_TABSTOP;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080; // Button class

    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    *lpw++ = 0; // No creation data

    //-----------------------
    // Define an Group Box.
    //-----------------------
    nbChild++;
    name="1. Choose";
    wchaine=L"1.Choose";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = 15*cxChar; lpdit->cy = 3*cyChar;
    lpdit->x = rcDlg.right/2 -lpdit->cx/2; lpdit->y = 5;
    lpdit->id = IDC_STATIC_GROUP;
    lpdit->style = WS_CHILD | WS_VISIBLE|BS_GROUPBOX;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080; // BUTTON claSS
    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    *lpw++ = 0; // No creation data

    cx=lpdit->cx;
    cy=lpdit->cy;
    x=lpdit->x;
    y=lpdit->y;


    //-----------------------
    // Radio Button.
    //-----------------------
    nbChild++;
    name="Modify Existing Menu     ";
    wchaine=L"Modify Existing Menu     ";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2; lpdit->cy = cyChar;
    lpdit->x = x+cxChar; lpdit->y = cyChar;
    lpdit->id =IDR_MOD_EX__MENU;
    lpdit->style = WS_GROUP|WS_CHILD | WS_VISIBLE|BS_AUTORADIOBUTTON;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080; // BUTTON claSS
    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    *lpw++ = 0; // No creation data

    cx=lpdit->cx;
    cy=lpdit->cy;
    x=lpdit->x;
    y=lpdit->y;



    //-----------------------
    // Radio Button.
    //-----------------------
    nbChild++;
    name="Create New Menu     ";
    wchaine=L"Create New Menu     ";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2; lpdit->cy = cyChar;
    lpdit->x = x; lpdit->y = y+cyChar;
    lpdit->id =IDR_CREATE_MENU;
    lpdit->style = WS_CHILD | WS_VISIBLE|BS_AUTORADIOBUTTON;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080; // BUTTON claSS
    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    *lpw++ = 0; // No creation data

    cx=lpdit->cx;
    cy=lpdit->cy;
    x=lpdit->x;
    y=lpdit->y;

    //-----------------------
    // Define an Group Box.
    //-----------------------
    nbChild++;
    name="Manage";
    wchaine=L"Manage";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = rcDlg.right/2-2*cxChar; lpdit->cy = 9*cyChar;
    lpdit->x = 2*cxChar; lpdit->y = 4*cyChar;
    lpdit->id = IDB_MANAGE_GROUP;
    lpdit->style = WS_CHILD | WS_VISIBLE|BS_GROUPBOX;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080; // BUTTON claSS
    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;
    *lpw++ = 0; // No creation data


    //-----------------------
    // Define an Group Box.
    //-----------------------
    nbChild++;
    name="Create";
    wchaine=L"Create";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = rcDlg.right/2-2*cxChar; lpdit->cy = 9*cyChar;
    lpdit->x = rcDlg.right/2; lpdit->y = 4*cyChar;
    lpdit->id = IDB_CREATE_GROUP;
    lpdit->style = WS_CHILD | WS_VISIBLE|BS_GROUPBOX;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080; // BUTTON claSS
    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;
    *lpw++ = 0; // No creation data

    cx=lpdit->cx;
    cy=lpdit->cy;
    x=lpdit->x;
    y=lpdit->y;


    //------------------
    //STATIC
    //--------------------
    nbChild++;
    name="Select menu's name:";
    wchaine=L"Select menu's name:";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name))*cxChar/2; lpdit->cy = cyChar;
    lpdit->x = 3*cxChar; lpdit->y = 9*cxChar;
    lpdit->id = IDS_MOD_MENU;
    lpdit->style = WS_CHILD | WS_VISIBLE;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0082; // static class

    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    *lpw++ = 0; // No creation data

    cx=lpdit->cx;
    cy=lpdit->cy;
    x=lpdit->x;
    y=lpdit->y;


    //------------------
    //STATIC
    //--------------------
    nbChild++;
    name="Select menu's name:";
    wchaine=L"Select menu's name:";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name))*cxChar/2; lpdit->cy = cyChar;
    lpdit->x = rcDlg.right/2+cxChar; lpdit->y = 9*cxChar;
    lpdit->id = IDS_CREATE_MENU;
    lpdit->style = WS_CHILD | WS_VISIBLE;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0082; // static class

    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    *lpw++ = 0; // No creation data

    cx=lpdit->cx;
    cy=lpdit->cy;
    x=lpdit->x;
    y=lpdit->y;

   //---------------------------------------
   // Edit Create
   //------------------------------------
    nbChild++;

    name="COLOR_3DDKSHADOWWWW";
    wchaine=L"";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2; lpdit->cy =5*cxChar/4;
    lpdit->x = x+cx; lpdit->y = 9*cxChar;
    lpdit->id = IDE_CREATE_MENU;
    lpdit->style =WS_VISIBLE|WS_CHILD|WS_BORDER;
    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0081; // edit class

    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    *lpw++ = 0; // No creation data
    cx=lpdit->cx;
    cy=lpdit->cy;
    x=lpdit->x;
    y=lpdit->y;


    //---------------------------------------
   //combobox Mod
   //------------------------------------
    nbChild++;

    name="COLOR_3DDKSHADOWWWW";
    wchaine=L"";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2; lpdit->cy = 2*cyChar;
    lpdit->x = 13*cxChar; lpdit->y = y;
    lpdit->id = IDC_MOD_MENU;
    lpdit->style =WS_VISIBLE|WS_CHILD|WS_BORDER|WS_VSCROLL|
    CBS_HASSTRINGS|CBS_DROPDOWNLIST;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0085; // combobox class

    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    *lpw++ = 0; // No creation data
    cx=lpdit->cx;
    cy=lpdit->cy;
    x=lpdit->x;
    y=lpdit->y;


    //-----------------------
    // Define an Group Box.
    //-----------------------
    nbChild++;
    name="Option";
    wchaine=L"Option";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = rcDlg.right/2-4*cxChar; lpdit->cy = 29*cyChar/5;
    lpdit->x = 3*cxChar; lpdit->y = 6*cyChar;
    lpdit->id = IDB_OPTION_GROUP;
    lpdit->style = WS_CHILD | WS_VISIBLE|BS_GROUPBOX;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080; // BUTTON claSS
    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;
    *lpw++ = 0; // No creation data

    cx=lpdit->cx;
    cy=lpdit->cy;
    x=lpdit->x;
    y=lpdit->y;


    //-----------------------
    // Define an RadioButton.
    //-----------------------
    nbChild++;
    name="Delete Menu     ";
    wchaine=L"Delete Menu     ";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar; lpdit->cy = cyChar;
    lpdit->x = x+cxChar; lpdit->y = y+4*cyChar/7;
    lpdit->id =IDR_MOD_DEL_MENU;
    lpdit->style = WS_CHILD | WS_VISIBLE|BS_AUTORADIOBUTTON|
    WS_GROUP;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080; // BUTTON claSS
    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    *lpw++ = 0; // No creation data
    cx=lpdit->cx;
    cy=lpdit->cy;
    x=lpdit->x;
    y=lpdit->y;



    //-----------------------
    // Define an RadioButton.
    //-----------------------
    nbChild++;
    name="Modify Menu     ";
    wchaine=L"Modify Menu     ";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar; lpdit->cy = cyChar;
    lpdit->x = x; lpdit->y = y+cyChar;
    lpdit->id =IDR_MOD_MOD_MENU;
    lpdit->style = WS_CHILD | WS_VISIBLE|BS_AUTORADIOBUTTON;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080; // BUTTON claSS
    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    *lpw++ = 0; // No creation data
    cx=lpdit->cx;
    cy=lpdit->cy;
    x=lpdit->x;
    y=lpdit->y;



    //-----------------------
    // Define an RadioButton.
    //-----------------------
    nbChild++;
    name="Add Item     ";
    wchaine=L"Add Item     ";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar; lpdit->cy = cyChar;
    lpdit->x = x; lpdit->y = y+cyChar;
    lpdit->id =IDR_MOD_ADD_ITEM;
    lpdit->style = WS_CHILD | WS_VISIBLE|BS_AUTORADIOBUTTON;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080; // BUTTON claSS
    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    *lpw++ = 0; // No creation data
    cx=lpdit->cx;
    cy=lpdit->cy;
    x=lpdit->x;
    y=lpdit->y;


    //-----------------------
    // Define an RadioButton.
    //-----------------------
    nbChild++;
    name="Modify Item     ";
    wchaine=L"Modify Item     ";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar; lpdit->cy = cyChar;
    lpdit->x = x; lpdit->y = y+cyChar;
    lpdit->id =IDR_MOD_MOD_ITEM;
    lpdit->style = WS_CHILD | WS_VISIBLE|BS_AUTORADIOBUTTON;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080; // BUTTON claSS
    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    *lpw++ = 0; // No creation data
    cx=lpdit->cx;
    cy=lpdit->cy;
    x=lpdit->x;
    y=lpdit->y;


    //-----------------------
    // Define an RadioButton.
    //-----------------------
    nbChild++;
    name="Del Item     ";
    wchaine=L"Del Item     ";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar; lpdit->cy = cyChar;
    lpdit->x = x; lpdit->y = y+cyChar;
    lpdit->id =IDR_MOD_DEL_ITEM;
    lpdit->style = WS_CHILD | WS_VISIBLE|BS_AUTORADIOBUTTON;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080; // BUTTON claSS
    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    *lpw++ = 0; // No creation data
    cx=lpdit->cx;
    cy=lpdit->cy;
    x=lpdit->x;
    y=lpdit->y;


    //-----------------------
    // Define an Go Button.
    //-----------------------
    nbChild++;
    name="Go";
    wchaine=L"Go";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar; lpdit->cy = cyChar;
    lpdit->x = rcDlg.right/2-lpdit->cx-2*cxChar; lpdit->y = y+4*cyChar/3;
    lpdit->id =IDB_MOD_GO_MENU;
    lpdit->style = WS_CHILD | WS_VISIBLE;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080; // BUTTON claSS
    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    *lpw++ = 0; // No creation data
    cx=lpdit->cx;
    cy=lpdit->cy;
    x=lpdit->x;
    y=lpdit->y;



    //-----------------------
    // Define an Go Button.
    //-----------------------
    nbChild++;
    name="Go";
    wchaine=L"Go";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar; lpdit->cy = cyChar;
    lpdit->x = rcDlg.right-cx-4*cxChar; lpdit->y = y;
    lpdit->id =IDB_CREATE_GO_MENU;
    lpdit->style = WS_CHILD | WS_VISIBLE;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080; // BUTTON claSS
    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    *lpw++ = 0; // No creation data
    cx=lpdit->cx;
    cy=lpdit->cy;
    x=lpdit->x;
    y=lpdit->y;


    //-----------------------
    // Define an GroupBox
    //-----------------------
    nbChild++;
    name="Define menu as";
    wchaine=L"Define menu as";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = 16*cxChar; lpdit->cy = 3*cyChar;
    lpdit->x = (rcDlg.right)/2 +cxChar; lpdit->y = 6*cyChar;
    lpdit->id =IDB_CREATE_DEF_GROUP;
    lpdit->style = WS_CHILD | WS_VISIBLE|BS_GROUPBOX;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080; // BUTTON claSS
    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    *lpw++ = 0; // No creation data
    cx=lpdit->cx;
    cy=lpdit->cy;
    x=lpdit->x;
    y=lpdit->y;


    //-----------------------
    // Define an RadioButton.
    //-----------------------
    nbChild++;
    name="Main Menu     ";
    wchaine=L"Main Menu     ";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2; lpdit->cy = cyChar;
    lpdit->x = x+cxChar; lpdit->y = y+cxChar;
    lpdit->id =IDR_CREATE_MAIN;
    lpdit->style = WS_CHILD | WS_VISIBLE|BS_AUTORADIOBUTTON;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080; // BUTTON claSS
    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    *lpw++ = 0; // No creation data
    cx=lpdit->cx;
    cy=lpdit->cy;
    x=lpdit->x;
    y=lpdit->y;



    //-----------------------
    // Define an RadioButton.
    //-----------------------
    nbChild++;
    name="Sub Menu     ";
    wchaine=L"Sub Menu     ";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2; lpdit->cy = cyChar;
    lpdit->x = x; lpdit->y = y+cyChar;
    lpdit->id =IDR_CREATE_SUBMAIN;
    lpdit->style = WS_CHILD | WS_VISIBLE|BS_AUTORADIOBUTTON;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080; // BUTTON claSS
    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    *lpw++ = 0; // No creation data
    cx=lpdit->cx;
    cy=lpdit->cy;
    x=lpdit->x;
    y=lpdit->y;


    //------------------
    //STATIC
    //--------------------
    nbChild++;
    name="Select menu's parent:";
    wchaine=L"Select menu's parent:";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2; lpdit->cy = cyChar;
    lpdit->x = x-cxChar; lpdit->y = 9*cyChar;
    lpdit->id = IDS_CREATE_PARENT;
    lpdit->style = WS_CHILD | WS_VISIBLE;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0082; // static class

    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    *lpw++ = 0; // No creation data

    cx=lpdit->cx;
    cy=lpdit->cy;
    x=lpdit->x;
    y=lpdit->y;


    //---------------------------------------
   //combobox Create
   //------------------------------------
    nbChild++;

    name="COLOR_3DDKSHADOWWWW";
    wchaine=L"";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2; lpdit->cy = 2*cyChar;
    lpdit->x = x+cx; lpdit->y = y;
    lpdit->id = IDC_CREATE_MENUPARENT;
    lpdit->style =WS_VISIBLE|WS_CHILD|WS_BORDER|WS_VSCROLL|
    CBS_HASSTRINGS|CBS_DROPDOWNLIST;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0085; // combobox class

    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    *lpw++ = 0; // No creation data
    cx=lpdit->cx;
    cy=lpdit->cy;
    x=lpdit->x;
    y=lpdit->y;


    //Set nombre de childs control a cree=
    lpdt->cdit =nbChild;// Number of controls
    GlobalUnlock(hgbl);

    lpdt->style=WS_BORDER|WS_SYSMENU|
        WS_CAPTION|WS_POPUP|DS_MODALFRAME;
    DialogBoxIndirectParam(hinst,(LPDLGTEMPLATE)hgbl,
    hwndOwner,(DLGPROC)DlgMenuProc,(LPARAM)&infoDlg);


    GlobalFree(hgbl);

    return TRUE;
}

LRESULT CALLBACK DlgMenuProc(HWND hDlg,UINT msg, WPARAM wParam, LPARAM lParam)
{
    static MY_DLGDATA *infoDlg;

    static int idOptSelected=-1,idDefMenuSelected=-1;
    static int iIndexMenuParentSelected=-1;
    //Recuperer les caracteristiques de la boite de dialogue,
    //NB: le champs id, contient l'id de la boite de dialogue hote
    if(msg==WM_INITDIALOG)
    {
        infoDlg=(MY_DLGDATA*)lParam;
        //idDlg=infoDlg->idDlg;
        MySetAppIconToWnd(hDlg);

    }
    switch(msg)
    {
    case WM_INITDIALOG:
        fShouldInotify=TRUE;
        if(!MyCopieMenuTab(mycache[DLG_DIALOGBOX].menu,
                           &menuTemp))
        {
            MyPerror("V,EPRL");

        }

        MyApplyMenuTempOnCombos(hDlg,IDC_MOD_MENU);
        MyApplyMenuTempOnCombos(hDlg,IDC_CREATE_MENUPARENT);

        EnableWindow(GetDlgItem(hDlg,IDB_MOD_GO_MENU),FALSE);
        EnableWindow(GetDlgItem(hDlg,IDB_CREATE_GO_MENU),FALSE);
        EnableWindow(GetDlgItem(hDlg,ID_APPLY),FALSE);
        MyEnableMenuObj(hDlg,FALSE,ALL);
        return FALSE;//force le systeme a atrribuer le focus

    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case IDR_MOD_EX__MENU:
            if(isEmptyCombo(GetDlgItem(hDlg,IDC_MOD_MENU)))
            {
                if(fShouldInotify)
                MessageBox(NULL,TEXT("There is No menu. Create It first!"),TEXT("INFORMATION!"),
                           MB_ICONINFORMATION|MB_TASKMODAL);

                CheckRadioButton(hDlg,IDR_MOD_EX__MENU,IDR_CREATE_MENU,
                                 idGroupChoosen==-1?0:IDR_CREATE_MENU);
                return TRUE;
            }

            if(idOptSelected!=-1)
            {
                CheckRadioButton(hDlg,IDR_MOD_DEL_MENU,IDR_MOD_DEL_ITEM,
                                 0);
                idOptSelected=-1;
            }
            MyEnableMenuObj(hDlg,TRUE,MANAGE);
            MyEnableMenuObj(hDlg,FALSE,CREATE);
            idGroupChoosen=LOWORD(wParam);
            return TRUE;

        case IDR_CREATE_MENU:
            if(idDefMenuSelected!=-1)
            {
                CheckRadioButton(hDlg,IDR_CREATE_MENU,IDR_CREATE_SUBMAIN,
                                 0);
                idDefMenuSelected=-1;
            }

            MyEnableMenuObj(hDlg,FALSE,MANAGE);
            MyEnableMenuObj(hDlg,TRUE,CREATE);
            idGroupChoosen=LOWORD(wParam);
            return TRUE;

        case IDE_CREATE_MENU:
            if(HIWORD(wParam)==EN_CHANGE)
            {
                TCHAR szBuf[1024];
                GetWindowText((HWND)lParam,szBuf,1023);
                if(*szBuf!='\0' )
                {
                    EnableWindow(GetDlgItem(hDlg,IDB_CREATE_DEF_GROUP),TRUE);
                    EnableWindow(GetDlgItem(hDlg,IDR_CREATE_MAIN),TRUE);
                    EnableWindow(GetDlgItem(hDlg,IDR_CREATE_SUBMAIN),TRUE);
                    if(idDefMenuSelected==IDR_CREATE_SUBMAIN)
                    {
                        EnableWindow(GetDlgItem(hDlg,IDS_CREATE_PARENT),TRUE);
                        EnableWindow(GetDlgItem(hDlg,IDC_CREATE_MENUPARENT),TRUE);
                        if(iIndexMenuParentSelected!=-1)
                        {
                            EnableWindow(GetDlgItem(hDlg,IDB_CREATE_GO_MENU),TRUE);
                        }
                        else
                        {
                            EnableWindow(GetDlgItem(hDlg,IDB_CREATE_GO_MENU),FALSE);
                        }

                    }
                    else if(idDefMenuSelected==IDR_CREATE_MAIN)
                        EnableWindow(GetDlgItem(hDlg,IDB_CREATE_GO_MENU),TRUE);
                    else
                        EnableWindow(GetDlgItem(hDlg,IDB_CREATE_GO_MENU),FALSE);
                }
                else
                {
                    EnableWindow(GetDlgItem(hDlg,IDB_CREATE_DEF_GROUP),FALSE);
                    EnableWindow(GetDlgItem(hDlg,IDR_CREATE_MAIN),FALSE);
                    EnableWindow(GetDlgItem(hDlg,IDR_CREATE_SUBMAIN),FALSE);
                    EnableWindow(GetDlgItem(hDlg,IDS_CREATE_PARENT),FALSE);
                    EnableWindow(GetDlgItem(hDlg,IDC_CREATE_MENUPARENT),FALSE);

                    EnableWindow(GetDlgItem(hDlg,IDB_CREATE_GO_MENU),FALSE);
                }
            }
            return TRUE;

        case IDR_CREATE_SUBMAIN:
            if(isEmptyCombo(GetDlgItem(hDlg,IDC_CREATE_MENUPARENT)))
            {
                if(fShouldInotify)
                MessageBox(NULL,TEXT("There is no main menu!"),TEXT("INFORMATION!"),
                           MB_ICONINFORMATION|MB_TASKMODAL);
                if(idDefMenuSelected==-1)
                    EnableWindow(GetDlgItem(hDlg,IDB_CREATE_GO_MENU),FALSE);
                CheckRadioButton(hDlg,IDR_CREATE_MAIN,IDR_CREATE_SUBMAIN,
                               idDefMenuSelected==-1?0:IDR_CREATE_MAIN  );
                return TRUE;
            }
            idDefMenuSelected=LOWORD(wParam);
            EnableWindow(GetDlgItem(hDlg,IDS_CREATE_PARENT),TRUE);
            EnableWindow(GetDlgItem(hDlg,IDC_CREATE_MENUPARENT),TRUE);
            if(iIndexMenuParentSelected!=-1)
                EnableWindow(GetDlgItem(hDlg,IDB_CREATE_GO_MENU),TRUE);
            else
                EnableWindow(GetDlgItem(hDlg,IDB_CREATE_GO_MENU),FALSE);
            return TRUE;

        case IDR_CREATE_MAIN:
            idDefMenuSelected=LOWORD(wParam);
            EnableWindow(GetDlgItem(hDlg,IDS_CREATE_PARENT),FALSE);
            EnableWindow(GetDlgItem(hDlg,IDC_CREATE_MENUPARENT),FALSE);
            EnableWindow(GetDlgItem(hDlg,IDB_CREATE_GO_MENU),TRUE);
            return TRUE;

        case IDC_CREATE_MENUPARENT:
            if(HIWORD(wParam)==CBN_SELCHANGE)
            {
                int iIndex;
                iIndex = SendMessage ((HWND)lParam, CB_GETCURSEL, 0, 0) ;
                if(iIndex==CB_ERR) return TRUE;

                EnableWindow(GetDlgItem(hDlg,IDB_CREATE_GO_MENU),TRUE);
                iIndexMenuParentSelected=iIndex;
            }
            return TRUE;

        case IDB_CREATE_GO_MENU:
            //Open Dlg For validate creation of menu.
            {
                int iResult;
                TCHAR szNameMenu[1024],szRep[2048];

                GetWindowText(GetDlgItem(hDlg,IDE_CREATE_MENU),szNameMenu,1023);
                wsprintf(szRep,"Do you want to add <%s> as menu?",szNameMenu);

                iResult=MessageBox(NULL,szRep,TEXT("QUESTION?"),
                MB_ICONQUESTION|MB_YESNO|MB_TASKMODAL);

                if(iResult==IDYES)
                {
                    MyAddMenu(hDlg,szNameMenu,idDefMenuSelected);
                    SetWindowText(GetDlgItem(hDlg,IDE_CREATE_MENU),"");
                    EnableWindow(GetDlgItem(hDlg,ID_APPLY),TRUE);
                }
                else if (iResult==IDNO)
                {
                    return TRUE;
                }
            }
            return TRUE;

        case IDB_MOD_GO_MENU:
            //Open dialog depending on idOptSelected
            if(idOptSelected==-1)
            {
                MyPerror("NVEORU");
                return TRUE;
            }
           OpenMenuDialogBox(idOptSelected,hDlg);
            EnableWindow(GetDlgItem(hDlg,ID_APPLY),TRUE);
            return TRUE;

        case IDC_MOD_MENU:
            if(HIWORD(wParam)==CBN_SELCHANGE)
            {
                int iIndex;
                iIndex = SendMessage ((HWND)lParam, CB_GETCURSEL, 0, 0) ;
                if(iIndex==CB_ERR ||iIndex<0)
                {
                    EnableWindow(GetDlgItem(hDlg,IDB_MOD_GO_MENU),FALSE);
                    return TRUE;
                }
                //Get nAME of menu selected
                TCHAR szNameMenu[1024];

                SendMessage((HWND)lParam,CB_GETLBTEXT,iIndex,(LPARAM)szNameMenu);
                //Check if Rdio opTion is selected
                if(idOptSelected!=-1)
                {
                    //enable GoMod
                    EnableWindow(GetDlgItem(hDlg,IDB_MOD_GO_MENU),TRUE);
                }
                else
                {
                    EnableWindow(GetDlgItem(hDlg,IDB_MOD_GO_MENU),FALSE);
                }
            }
            return TRUE;

        case IDR_MOD_ADD_ITEM:
        case IDR_MOD_DEL_MENU:
        case IDR_MOD_MOD_ITEM:
        case IDR_MOD_MOD_MENU:
        case IDR_MOD_DEL_ITEM:
            idOptSelected=LOWORD(wParam);
            {
                int iIndex;
                HWND hwndComboMenuName=GetDlgItem(hDlg,IDC_MOD_MENU);
                iIndex = SendMessage (hwndComboMenuName,
                    CB_GETCURSEL, 0, 0) ;
                if(iIndex==CB_ERR) return TRUE;

                EnableWindow(GetDlgItem(hDlg,IDB_MOD_GO_MENU),TRUE);
            }
            return TRUE;

        case ID_APPLY:
        //Transferer les modifs dans mycache
        if(!MyCopieMenuTab(menuTemp,
                           &mycache[DLG_DIALOGBOX].menu))
        {
            MyPerror("M KARTOI");

        }


        case IDCANCEL:
            if(!MyFreeMenuTab(&menuTemp) ||
               !MyFreeMenuTab(&copieMenuTemp))
            {
                MyPerror(" sritie");
            }


            idOptSelected=-1;idDefMenuSelected=-1;
            iIndexMenuParentSelected=-1;
            idGroupChoosen=-1;
            EndDialog(hDlg,0);
            return TRUE;
        }
        break;

    }
    return FALSE;
}
void MyEnableMenuObj(HWND hDlg,BOOL fEnable,int manner)
{
    if(manner==ALL || manner==MANAGE)
    {
        EnableWindow(GetDlgItem(hDlg,IDB_MANAGE_GROUP),fEnable);
        EnableWindow(GetDlgItem(hDlg,IDB_OPTION_GROUP),fEnable);
        EnableWindow(GetDlgItem(hDlg,IDR_MOD_ADD_ITEM),fEnable);
        EnableWindow(GetDlgItem(hDlg,IDR_MOD_DEL_MENU),fEnable);
        EnableWindow(GetDlgItem(hDlg,IDR_MOD_MOD_ITEM),fEnable);
        EnableWindow(GetDlgItem(hDlg,IDR_MOD_MOD_MENU),fEnable);
        EnableWindow(GetDlgItem(hDlg,IDR_MOD_DEL_ITEM),fEnable);
        EnableWindow(GetDlgItem(hDlg,IDC_MOD_MENU),fEnable);
        EnableWindow(GetDlgItem(hDlg,IDS_MOD_MENU),fEnable);
        EnableWindow(GetDlgItem(hDlg,IDB_MOD_GO_MENU),fEnable==TRUE?FALSE:FALSE);
    }
    if(manner==ALL || manner==CREATE)
    {
        EnableWindow(GetDlgItem(hDlg,IDB_CREATE_GROUP),fEnable);
        EnableWindow(GetDlgItem(hDlg,IDE_CREATE_MENU),fEnable);
        EnableWindow(GetDlgItem(hDlg,IDS_CREATE_MENU),fEnable);
        EnableWindow(GetDlgItem(hDlg,IDB_CREATE_GO_MENU),fEnable==TRUE?FALSE:FALSE);
        EnableWindow(GetDlgItem(hDlg,IDS_CREATE_PARENT),fEnable==TRUE?FALSE:FALSE);
        EnableWindow(GetDlgItem(hDlg,IDB_CREATE_DEF_GROUP),fEnable==TRUE?FALSE:FALSE);
        EnableWindow(GetDlgItem(hDlg,IDR_CREATE_MAIN),fEnable==TRUE?FALSE:FALSE);
        EnableWindow(GetDlgItem(hDlg,IDR_CREATE_SUBMAIN),fEnable==TRUE?FALSE:FALSE);
        EnableWindow(GetDlgItem(hDlg,IDC_CREATE_MENUPARENT),fEnable==TRUE?FALSE:FALSE);
    }
}

BOOL isEmptyCombo(HWND hwndCombo)
{
    if(!hwndCombo) return FALSE;
    int iCount;

    iCount=SendMessage(hwndCombo,CB_GETCOUNT,0,0);
    if(iCount<=0) return TRUE;
    //cette fonction verifie si le combo contient des item ou pas
    return FALSE;
}

BOOL MyAddMenu(HWND hDlg,char *szNameMenu,int idDefMenuSelected)
{
    //Cette fonction ajoute szNameMenu entant que nouveau menu
    //soit un main menu ou un sous menu, tout depend de la
    //nature de idDefMenuSelected

    if(!menuTemp)
    {
        MyPerror("m odiodig");
        return FALSE;
    }

    //Ajout proprement dit!!!
    //Un menu s'ajoute toujours a ilLastinexElt=0;
    int *nbMenus=&menuTemp[0].iLastIndexMenu;
    int *nbElt=&menuTemp[*nbMenus].iLastIinexElt;

    menuTemp[*nbMenus].iLastIinexElt=0;//reinitialisation importante
    menuTemp[*nbMenus].tabOrder[0].nbOrder=0;//renitialisation importante
    if(*nbMenus>=MIN_NBMENU || *nbElt>=MIN_NBELT_PER_MENU)
    {
        MessageBox(NULL,TEXT("Unable to add more menu,not enough space"),
                   TEXT("ERROR!!"),MB_TASKMODAL|MB_ICONERROR);
        return FALSE;
    }


    //Add szName au comboMod
    HWND hwndComboMod;
    hwndComboMod=GetDlgItem(hDlg,IDC_MOD_MENU);
    if(!hwndComboMod)
    {
        MessageBox(NULL,"Unable to find combo menu",TEXT("ERROR!!!"),MB_ICONERROR);
        return TRUE;
    }
    SendMessage(hwndComboMod,CB_INSERTSTRING,-1,
        (LPARAM)szNameMenu);
    MessageBox(hDlg,TEXT(" Success! New menu added!"),
            TEXT("INFORMATION!"),MB_ICONINFORMATION|MB_TASKMODAL);
    EnableWindow(GetDlgItem(hDlg,ID_APPLY),TRUE);


    //ADD au combo  Create
    HWND hwndComboCreate;
    hwndComboCreate=GetDlgItem(hDlg,IDC_CREATE_MENUPARENT);
    if(!hwndComboCreate)
    {
        MessageBox(NULL,"Unable to find combo menu",TEXT("ERROR!!!"),MB_ICONERROR);
        return TRUE;
    }
    SendMessage(hwndComboCreate,CB_INSERTSTRING,-1,
    (LPARAM)szNameMenu);


    if(idDefMenuSelected==IDR_CREATE_MAIN)
    {
        menuTemp[*nbMenus].idMenu=g_idMenuActu++;
        menuTemp[*nbMenus].iIndexComboCreate=*nbMenus;
        menuTemp[*nbMenus].iIndexParent=-1;
        menuTemp[*nbMenus].isSubMenu=FALSE;
        menuTemp[*nbMenus].tabElt[0].isPopPupMenu=TRUE;

        MYDESCPOPUPMENU *menuDesc=&menuTemp[*nbMenus].tabElt[0].dataPop;

        menuDesc->hMenu=NULL;//Pas encore rempli
        strcpy(menuDesc->szFlagName,"MF_POPUP");
        strcpy(menuDesc->szNameMenu,szNameMenu);
        menuDesc->uFlag=MF_POPUP;//pas encore rempli

        *nbElt=1;
        *nbMenus+=1;
    }
    else if(idDefMenuSelected==IDR_CREATE_SUBMAIN)
    {
        menuTemp[*nbMenus].idMenu=g_idMenuActu++;
        menuTemp[*nbMenus].iIndexComboCreate=*nbMenus;
        menuTemp[*nbMenus].iIndexParent=MyGetIndexCombo(hDlg,IDC_CREATE_MENUPARENT);
        menuTemp[*nbMenus].isSubMenu=TRUE;
        menuTemp[*nbMenus].tabElt[0].isPopPupMenu=TRUE;

        //Ajout dans tabOrder
        int iIndexParent=menuTemp[*nbMenus].iIndexParent;
        if(!MyAddChildToOrder(menuTemp,iIndexParent,*nbMenus,TRUE))
        {
            MyPerror(" ofodifoid");
            return FALSE;
        }

        MYDESCPOPUPMENU *menuDesc=&menuTemp[*nbMenus].tabElt[0].dataPop;

        menuDesc->hMenu=NULL;//Pas encore rempli
        strcpy(menuDesc->szFlagName,"MF_POPUP");
        strcpy(menuDesc->szNameMenu,szNameMenu);
        menuDesc->uFlag=MF_POPUP;//pas encore rempli

        *nbElt=1;
        *nbMenus+=1;
    }
    return TRUE;
}

int MyGetIndexCombo(HWND hDlg,int id)
{
    //Cette fonction tente de reuperer le iIndex
    if(!hDlg) return -1;
    HWND hwndComboCreate=GetDlgItem(hDlg,id);
    if(!hwndComboCreate)
    {
        MyPerror("sdkjalk55");
        return -1;
    }
    int iIndexCombo;

    iIndexCombo=SendMessage(hwndComboCreate,CB_GETCURSEL,0,0);
    return iIndexCombo;
}


BOOL MyDelMenu(MYMENULIST *g_menuTab,int iIndexMenu,int idMenu,HWND hDlgMenu)
{
    //C'est pas tres coherent de nommer g_menuTab comme variable locale
    //Mias le hic c'et qu'il existe une variable globale menuTemp, c'est difficile
    //de faire les modif :)
    //cette fonction supprime g_menuTab[iIndexMenu]
    //et remplace tous les elts de g_menuTab qui reconnaissait iIndexMenu par -1
    if(!hDlgMenu)
    {
        MyPerror("DKSJVDJI");
        return FALSE;
    }
    if(iIndexMenu<0)
    {
        MyPerror("dsvkVDJI");
        return FALSE;
    }
    if(!g_menuTab)
    {
        MyPerror("m vjrijer");
        return FALSE;
    }

    if(iIndexMenu>=g_menuTab[0].iLastIndexMenu)
    {
        MyPerror("anosinj");
        return FALSE;
    }
    int nbMenus;
    int i,newIindex;

    //Supprimer iIndexMenu de tabOrder
    if(g_menuTab[iIndexMenu].isSubMenu)
    {
        int iIndexParent=g_menuTab[iIndexMenu].iIndexParent;
        if(!MyDelChildToOrder(g_menuTab,iIndexMenu,iIndexParent,TRUE,TRUE))
        {
            MyPerror(" voiotrf");
            MyMsgError();
            return FALSE;
        }
    }

    //Supprimer d'abord les childs
    nbMenus=g_menuTab[0].iLastIndexMenu;

    for(i=0;i<nbMenus;i++)
    {
        if(g_menuTab[i].isSubMenu && g_menuTab[i].iIndexParent==iIndexMenu)
        {
            //Supprimer le child menu qui a iIndexMenu comme menuParent
            if(!MyDelMenu(g_menuTab,i,g_menuTab[i].idMenu,hDlgMenu))
            {
                MyPerror(" nrtu00s");
                return FALSE;
            }
            newIindex=MyGetIindexMenuWithIdMenu(g_menuTab,idMenu);
            if(newIindex==-1)
            {
                MyPerror(",fkoskfk");
                return FALSE;
            }
            iIndexMenu=newIindex;
            i=-1;nbMenus=g_menuTab[0].iLastIndexMenu;
        }

    }

    newIindex=MyGetIindexMenuWithIdMenu(g_menuTab,idMenu);
    if(newIindex==-1)
    {
        MyPerror(",fkoskfk");
        exit(EXIT_FAILURE);
    }
    iIndexMenu=newIindex;

    nbMenus=g_menuTab[0].iLastIndexMenu;
    for(i=iIndexMenu;i<nbMenus-1;i++)
    {
        if(i==0 )
        {
            g_menuTab[i+1].iLastIndexMenu=g_menuTab[0].iLastIndexMenu;
            //g_menuTab[i+1].iLastIinexElt=g_menuTab[0].iLastIinexElt;
        }

        //Avertir a tous les popup que iIndex=i+1 est devenu i
        //Seul les sous menu et les parents de sousMenus sont concernes
        for(int j=0;j<g_menuTab[0].iLastIndexMenu;j++)
        {
            if(g_menuTab[j].isSubMenu && g_menuTab[j].iIndexParent==i+1)
            {
                g_menuTab[j].iIndexParent=i;

            }
            //FAIRE PAREIL AVEC LES TAB ORDERS
            for(int k=0;k<g_menuTab[j].tabOrder[0].nbOrder;k++)
            {
                if(g_menuTab[j].tabOrder[k].isMenuChild &&
                   g_menuTab[j].tabOrder[k].iIndexChild==i+1)
                    g_menuTab[j].tabOrder[k].iIndexChild=i;
            }
        }

        g_menuTab[i].idMenu=g_menuTab[i+1].idMenu;
        g_menuTab[i].iIndexComboCreate=g_menuTab[i+1].iIndexComboCreate;
        g_menuTab[i].iIndexParent=g_menuTab[i+1].iIndexParent;
        g_menuTab[i].iLastIinexElt=g_menuTab[i+1].iLastIinexElt;
        g_menuTab[i].iLastIndexMenu=g_menuTab[i+1].iLastIndexMenu;
        g_menuTab[i].isSubMenu=g_menuTab[i+1].isSubMenu;
        for(int j=0;j<MIN_NBELT_PER_MENU;j++)
        {
            g_menuTab[i].tabElt[j]=g_menuTab[i+1].tabElt[j];
        }
        for(int j=0;j<(MIN_NBELT_PER_MENU+MIN_NBMENU);j++)
        {
            g_menuTab[i].tabOrder[j]=g_menuTab[i+1].tabOrder[j];
        }
    }
    g_menuTab[0].iLastIndexMenu-=1;

    //Supprimer iIndexMenus des combo mod et create

    HWND hwndCombo;

    //Mod
    hwndCombo=GetDlgItem(hDlgMenu,IDC_MOD_MENU);
    if(!hwndCombo)
    {
        MyPerror("vnoiroi");
        exit(EXIT_FAILURE);
    }
    SendMessage(hwndCombo,CB_DELETESTRING,iIndexMenu,0);
    //mise ajour
    SendMessage(hDlgMenu,WM_COMMAND,
                MAKELONG(IDC_MOD_MENU,CBN_SELCHANGE),(LPARAM)hwndCombo);
    InvalidateRect(hwndCombo,NULL,TRUE);
    idGroupChoosen=-1;
    EnableWindow(GetDlgItem(hDlgMenu,IDB_MOD_GO_MENU),FALSE);
    EnableWindow(GetDlgItem(hDlgMenu,IDB_CREATE_GO_MENU),FALSE);
    MyEnableMenuObj(hDlgMenu,FALSE,ALL);

    SendMessage(hDlgMenu,WM_COMMAND,
                MAKELONG(IDR_MOD_EX__MENU,0),(LPARAM)GetDlgItem(hDlgMenu,IDR_MOD_EX__MENU));


    //CREATE
    hwndCombo=GetDlgItem(hDlgMenu,IDC_CREATE_MENUPARENT);
    if(!hwndCombo)
    {
        MyPerror("vnoiroi");
        exit(EXIT_FAILURE);
    }
    SendMessage(hwndCombo,CB_DELETESTRING,iIndexMenu,0);
    InvalidateRect(hwndCombo,NULL,TRUE);
    SetWindowText(GetDlgItem(hDlgMenu,IDE_CREATE_MENU),"");

    return TRUE;
}

BOOL MyModifyItem(int iIndexMenu,HWND hDlgMenu)
{
    if( iIndexMenu<0 || iIndexMenu>=menuTemp[0].iLastIndexMenu)
    {
        MyPerror("c9itew09ti");
        return FALSE;
    }

    if(menuTemp[iIndexMenu].iLastIinexElt==1)
    {//Un juste pour le nom du menu
        char szRep[2048];
        sprintf(szRep,"%s don't have item! ",
                 menuTemp[iIndexMenu].tabElt[0].dataPop.szNameMenu);
        MessageBox(hDlgMenu,szRep,"Error",MB_TASKMODAL|
                   MB_ICONERROR);
        return FALSE;
    }
    //cette fonction permet de modifier un item existant deja dans iIndexMenu

    DLGPARAMINFO dlgparaminfo;

    dlgparaminfo.hwndParentDlg=hDlgMenu;
    dlgparaminfo.idDlg=IDD_MODITEMMENU;
    dlgparaminfo.cbSizeMoreData=sizeof(int);
    dlgparaminfo.moreData=(VOID*)&iIndexMenu;


    DialogBoxParamW((HINSTANCE)GetWindowLongPtr(hDlgMenu,GWLP_HINSTANCE)
                      , MAKEINTRESOURCEW(IDD_MODITEMMENU), hDlgMenu,(DLGPROC)SimpleMenuOptProc,
                                         (LPARAM)&dlgparaminfo);


    return TRUE;
}

BOOL MyAddItemToMenu(int iIndexMenu,HWND hDlgMenu)
{
    if( !menuTemp || iIndexMenu<0 || iIndexMenu>=menuTemp[0].iLastIndexMenu)
    {
        MyPerror("vw8eur8bw9u");
        return FALSE;
    }

    //cette fonction permet d'ajouter un nouveau index a iIndexMenu

    DLGPARAMINFO dlgparaminfo;

    dlgparaminfo.hwndParentDlg=hDlgMenu;
    dlgparaminfo.idDlg=IDD_ADDITEMMENU;
    dlgparaminfo.cbSizeMoreData=sizeof(int);
    dlgparaminfo.moreData=(VOID*)&iIndexMenu;


    DialogBoxParamW((HINSTANCE)GetWindowLongPtr(hDlgMenu,GWLP_HINSTANCE)
                      , MAKEINTRESOURCEW(IDD_ADDITEMMENU), hDlgMenu,(DLGPROC)SimpleMenuOptProc,
                                         (LPARAM)&dlgparaminfo);


    return TRUE;
}


BOOL MyModMenu(int iIndexMenu,HWND hDlgMenu)
{
    if( !menuTemp ||  iIndexMenu<0 || iIndexMenu>=menuTemp[0].iLastIndexMenu)
    {
        MyPerror("vw8ngydt");
        return FALSE;
    }
    //cette fonction permet de modifier les parametre du menu(nom,isSubMenu)

    DLGPARAMINFO dlgparaminfo;

    dlgparaminfo.hwndParentDlg=hDlgMenu;
    dlgparaminfo.idDlg=IDD_MODMENU;
    dlgparaminfo.cbSizeMoreData=sizeof(int);
    dlgparaminfo.moreData=(VOID*)&iIndexMenu;

    //copie de menuTemp
    if(!MyCopieMenuTab(menuTemp,&copieMenuTemp))
    {
        MyPerror(" askdoakf");
        return FALSE;
    }

    fShouldInotify=FALSE;

    if(!MyDelMenu(menuTemp,iIndexMenu,menuTemp[iIndexMenu].idMenu,hDlgMenu))
    {
        MyPerror(" im odit");
        return FALSE;
    }

    DialogBoxParamW((HINSTANCE)GetWindowLongPtr(hDlgMenu,GWLP_HINSTANCE)
                      , MAKEINTRESOURCEW(IDD_MODMENU), hDlgMenu,(DLGPROC)SimpleMenuOptProc,
                                         (LPARAM)&dlgparaminfo);

    //Restaurer la generation de iIndexMenu
    //Apply les modifs de nom effectuees sur les combos
    //copie de menuTemp

    if(!MyCopieMenuTab(copieMenuTemp,&menuTemp))
    {
        MyPerror("fsojd");
        return FALSE;
    }

    MyApplyMenuTempOnCombos(hDlgMenu,IDC_CREATE_MENUPARENT);
    MyApplyMenuTempOnCombos(hDlgMenu,IDC_MOD_MENU);
    SendMessage(hDlgMenu,WM_COMMAND,
                MAKELONG(IDR_MOD_EX__MENU,BN_PUSHED),(LPARAM)GetDlgItem(hDlgMenu,IDR_MOD_EX__MENU));

    return TRUE;
}


BOOL MyDelItemToMenu(int iIndexMenu,HWND hDlgMenu)
{
    if( !menuTemp ||  iIndexMenu<0 || iIndexMenu>=menuTemp[0].iLastIndexMenu)
    {
        MyPerror("vw8ngydt");
        return FALSE;
    }
    //cette fonction permet de supprimer un item du menu

    if(menuTemp[iIndexMenu].iLastIinexElt<=1)
    {//Un juste pour le nom du menu
        char szRep[2048];
        sprintf(szRep,"%s don't have item!",
                 menuTemp[iIndexMenu].tabElt[0].dataPop.szNameMenu);
        MessageBox(hDlgMenu,szRep,"Error",MB_TASKMODAL|
                   MB_ICONERROR);
        return FALSE;
    }


    DLGPARAMINFO dlgparaminfo;

    dlgparaminfo.hwndParentDlg=hDlgMenu;
    dlgparaminfo.idDlg=IDD_DELITEMMENU;
    dlgparaminfo.cbSizeMoreData=sizeof(int);
    dlgparaminfo.moreData=(VOID*)&iIndexMenu;


    DialogBoxParamW((HINSTANCE)GetWindowLongPtr(hDlgMenu,GWLP_HINSTANCE)
                      , MAKEINTRESOURCEW(IDD_DELITEMMENU), hDlgMenu,(DLGPROC)SimpleMenuOptProc,
                                         (LPARAM)&dlgparaminfo);

    return TRUE;
}

// Fonction de la boîte de dialogue
LRESULT CALLBACK SimpleMenuOptProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    //DlfNotif gere plusieurs boites de dialogue differentes
    //A differencier

    static int iTypeDlg;
    static int iIndexMenu;
    if(message==WM_INITDIALOG)
    {
        iTypeDlg=((DLGPARAMINFO*)lParam)->idDlg;
        VOID *p=((DLGPARAMINFO*)lParam)->moreData;
        int *i=(int*)p;
        iIndexMenu=*i;
        MySetAppIconToWnd(hDlg);

    }


    switch(iTypeDlg)
    {
    case IDD_MODMENU:
    {
        static int idDefMenuSelected=-1;
        static int iIndexMenuParentSelected=-1;
        static int iIndexBeforeOrg=-1,iIndexAfterOrg=-1;
        static int iNewBef=-1,iNewAft=-1;
        static int iUserChoiceOnSetPos=-1;//Variable qui  stocke le choix de User dans Set Item pos(before,after,no manage)
        static int tabPosCombo[MIN_NBELT_PER_MENU+MIN_NBMENU-1];//ce tableau effectue la correspondance entre les iindex situe dans les comboBefore/after et ceux de menuTemp.tabOrder,iInexChilds

        switch (message)
        {
        case WM_INITDIALOG:
            idDefMenuSelected=-1;
            iIndexMenuParentSelected=-1;
            iIndexBeforeOrg=-1;iIndexAfterOrg=-1;
            iNewBef=-1,iNewAft=-1;
            iUserChoiceOnSetPos=-1;//Variable qui  stocke le choix de User dans Set Item pos(before,after,no manage)

            MyApplyMenuTempOnCombos(hDlg,IDC_CREATE_MENUPARENT);
            {
                /*char szNameMenu[1024];
                SendMessage(GetDlgItem(GetParent(hDlg),IDC_MOD_MENU),
                            CB_GETLBTEXT,iIndexMenu,(lparam)szNameMenu);
                */
                SetWindowText(GetDlgItem(hDlg,IDE_CREATE_MENU),
                              copieMenuTemp[iIndexMenu].tabElt[0].dataPop.szNameMenu);
            }
            SendMessage(hDlg,WM_COMMAND,
                    MAKELONG(IDE_CREATE_MENU,EN_CHANGE),
               (LPARAM) GetDlgItem(hDlg,IDE_CREATE_MENU));

            {
                //centrer sur hDlgMenu
                RECT rcParent, rcDlg;
                 HWND hDlgMenu=((DLGPARAMINFO*)lParam)->hwndParentDlg;
                 ShowWindow(hDlg,SW_HIDE);
                GetWindowRect(hDlgMenu, &rcParent);
                GetWindowRect(hDlg, &rcDlg);

                int dlgWidth = rcDlg.right - rcDlg.left;
                int dlgHeight = rcDlg.bottom - rcDlg.top;

                int x = rcParent.left + (rcParent.right - rcParent.left) / 2 - dlgWidth / 2;
                int y = rcParent.top + (rcParent.bottom - rcParent.top) / 2 - dlgHeight / 2;
                 SetWindowPos(hDlg, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
                ShowWindow(hDlg,SW_NORMAL);
            }

            //Desactive le GroupBox des set AFTER et BEFORE
            EnableWindow(GetDlgItem(hDlg,IDB_GB_SETPOS_ITEM),FALSE);
            EnableWindow(GetDlgItem(hDlg,IDR_BEFORE),FALSE);
            EnableWindow(GetDlgItem(hDlg,IDR_AFTER),FALSE);
            EnableWindow(GetDlgItem(hDlg,IDC_SET_AFTER),FALSE);
            EnableWindow(GetDlgItem(hDlg,IDC_SET_BEFORE),FALSE);
            EnableWindow(GetDlgItem(hDlg,IDR_NOCHANGE),FALSE);
            return TRUE;

        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
            case IDE_CREATE_MENU:

                if(HIWORD(wParam)==EN_CHANGE)
                {
                    TCHAR szBuf[1024];
                    GetWindowText((HWND)lParam,szBuf,1023);
                    if(*szBuf!='\0' )
                    {
                        EnableWindow(GetDlgItem(hDlg,IDB_CREATE_DEF_GROUP),TRUE);
                        EnableWindow(GetDlgItem(hDlg,IDR_CREATE_MAIN),TRUE);
                        EnableWindow(GetDlgItem(hDlg,IDR_CREATE_SUBMAIN),TRUE);
                        if(idDefMenuSelected==IDR_CREATE_SUBMAIN)
                        {
                            EnableWindow(GetDlgItem(hDlg,IDS_CREATE_PARENT),TRUE);
                            EnableWindow(GetDlgItem(hDlg,IDC_CREATE_MENUPARENT),TRUE);
                            if(iIndexMenuParentSelected!=-1)
                            {
                                EnableWindow(GetDlgItem(hDlg,ID_APPLY),TRUE);
                            }
                            else
                            {
                                EnableWindow(GetDlgItem(hDlg,IDS_CREATE_PARENT),FALSE);
                                EnableWindow(GetDlgItem(hDlg,IDC_CREATE_MENUPARENT),FALSE);

                                EnableWindow(GetDlgItem(hDlg,ID_APPLY),FALSE);
                            }

                        }
                        else if(idDefMenuSelected==IDR_CREATE_MAIN)
                            EnableWindow(GetDlgItem(hDlg,ID_APPLY),TRUE);
                        else
                        {
                            EnableWindow(GetDlgItem(hDlg,IDS_CREATE_PARENT),FALSE);
                            EnableWindow(GetDlgItem(hDlg,IDC_CREATE_MENUPARENT),FALSE);

                            EnableWindow(GetDlgItem(hDlg,ID_APPLY),FALSE);
                        }

                    }
                    else
                    {
                        EnableWindow(GetDlgItem(hDlg,IDB_CREATE_DEF_GROUP),FALSE);
                        EnableWindow(GetDlgItem(hDlg,IDR_CREATE_MAIN),FALSE);
                        EnableWindow(GetDlgItem(hDlg,IDR_CREATE_SUBMAIN),FALSE);
                        EnableWindow(GetDlgItem(hDlg,IDS_CREATE_PARENT),FALSE);
                        EnableWindow(GetDlgItem(hDlg,IDC_CREATE_MENUPARENT),FALSE);

                        EnableWindow(GetDlgItem(hDlg,ID_APPLY),FALSE);
                    }
                }
                return TRUE;

            case IDR_CREATE_SUBMAIN:
                if(isEmptyCombo(GetDlgItem(hDlg,IDC_CREATE_MENUPARENT)))
                {
                    if(fShouldInotify)
                    MessageBox(NULL,TEXT("There is no main Menu!"),TEXT("INFORMATION!"),
                           MB_ICONINFORMATION);

                        EnableWindow(GetDlgItem(hDlg,ID_APPLY),FALSE);
                    CheckRadioButton(hDlg,IDR_CREATE_MAIN,IDR_CREATE_SUBMAIN,
                               idDefMenuSelected==-1?0:IDR_CREATE_MAIN  );
                    return TRUE;
                }
                idDefMenuSelected=LOWORD(wParam);
                EnableWindow(GetDlgItem(hDlg,IDS_CREATE_PARENT),TRUE);
                EnableWindow(GetDlgItem(hDlg,IDC_CREATE_MENUPARENT),TRUE);
                if(iIndexMenuParentSelected!=-1)
                    EnableWindow(GetDlgItem(hDlg,ID_APPLY),TRUE);
                else
                    EnableWindow(GetDlgItem(hDlg,ID_APPLY),FALSE);

                return TRUE;

            case IDR_CREATE_MAIN:
                idDefMenuSelected=LOWORD(wParam);
                EnableWindow(GetDlgItem(hDlg,IDS_CREATE_PARENT),FALSE);
                EnableWindow(GetDlgItem(hDlg,IDC_CREATE_MENUPARENT),FALSE);
                EnableWindow(GetDlgItem(hDlg,ID_APPLY),TRUE);

                //
                //Desactive le GroupBox des set AFTER et BEFORE
                EnableWindow(GetDlgItem(hDlg,IDB_GB_SETPOS_ITEM),FALSE);
                EnableWindow(GetDlgItem(hDlg,IDR_BEFORE),FALSE);
                EnableWindow(GetDlgItem(hDlg,IDR_AFTER),FALSE);
                EnableWindow(GetDlgItem(hDlg,IDC_SET_AFTER),FALSE);
                EnableWindow(GetDlgItem(hDlg,IDC_SET_BEFORE),FALSE);
                EnableWindow(GetDlgItem(hDlg,IDR_NOCHANGE),FALSE);
                return TRUE;

            case IDC_CREATE_MENUPARENT:
                if(HIWORD(wParam)==CBN_SELCHANGE)
                {
                    int iIndex;
                    iIndex = SendMessage ((HWND)lParam, CB_GETCURSEL, 0, 0) ;
                    if(iIndex==CB_ERR) return TRUE;

                    EnableWindow(GetDlgItem(hDlg,ID_APPLY),TRUE);
                    iIndexMenuParentSelected=iIndex;

                    //Zone SetItem Menu position
                    //recuperer l'iIndexParent selectionner dans la combo MenuParent
                    //et init les comboAfter et before
                    int iIndexParent=MyGetIndexCombo(hDlg,IDC_CREATE_MENUPARENT);
                    //Ce iIndexParent est relatif a menuTemp,il faut trouver
                    //celui relatif a copieMenuTemp, puisque ici on work
                    //avec le menuTemp
                    for(int i=0;i<copieMenuTemp[0].iLastIndexMenu;i++)
                    {
                        if(menuTemp[iIndexParent].idMenu==copieMenuTemp[i].idMenu)
                        {
                            iIndexParent=i;
                            break;
                        }
                    }
                    //renitialisation
                    iIndexAfterOrg=-1;
                    iIndexBeforeOrg=-1;
                    iUserChoiceOnSetPos=-1;
                    //Rempli les combo ,bref ffaire les initialisation necessaires dans GB_AFTER/BEFORE
                    if(!MyInitMenuChildOrder(hDlg,copieMenuTemp,iIndexMenu,iIndexParent,TRUE,tabPosCombo))
                    {
                        MyPerror("teoijve");
                        MyMsgError();
                        return TRUE;
                    }
                    //Prend les valeurs de iIndexAfter et before
                    iIndexBeforeOrg =SendMessage(GetDlgItem(hDlg,IDC_SET_BEFORE),
                                CB_GETCURSEL,0,0);
                    iIndexAfterOrg =SendMessage(GetDlgItem(hDlg,IDC_SET_AFTER),
                                CB_GETCURSEL,0,0);
                          //Selectionner par defaut Nochange
                    SendMessage(hDlg,WM_COMMAND,
                        MAKELONG(IDR_NOCHANGE,BN_CLICKED),
                        (LPARAM)GetDlgItem(hDlg,IDR_NOCHANGE));
                    CheckRadioButton(hDlg,IDR_BEFORE,IDR_NOCHANGE,IDR_NOCHANGE);

                }
                return TRUE;

            case IDR_BEFORE:
                if(HIWORD(wParam)!=BN_CLICKED) return FALSE;

                iUserChoiceOnSetPos=IDR_BEFORE;
                //Enable le comboBox correspondant ET DISABLE L'AUTRE
                EnableWindow(GetDlgItem(hDlg,IDC_SET_BEFORE),TRUE);
                EnableWindow(GetDlgItem(hDlg,IDC_SET_AFTER),FALSE);
                //Simule pour mettre a jour iNewBef
                SendMessage(hDlg,WM_COMMAND,
                            MAKELONG(IDC_SET_BEFORE,CBN_SELCHANGE),
                    (LPARAM)GetDlgItem(hDlg,IDC_SET_BEFORE));
                return TRUE;

            case IDC_SET_BEFORE:
                if(HIWORD(wParam)==CBN_SELCHANGE)
                {
                    //Obtiens le New IiNDEX
                    iNewBef=SendMessage((HWND)lParam,CB_GETCURSEL,0,0);
                    //Renitialise iNewAft
                    iNewAft=-1;//important

                }
                return TRUE;
            case IDR_AFTER:
                if(HIWORD(wParam)!=BN_CLICKED) return FALSE;
                iUserChoiceOnSetPos=IDR_AFTER;
                //Enable le comboBox correspondant ET DISABLE l'autre
                EnableWindow(GetDlgItem(hDlg,IDC_SET_AFTER),TRUE);
                EnableWindow(GetDlgItem(hDlg,IDC_SET_BEFORE),FALSE);
                //Simule pour mettre a jour iNewAfter
                SendMessage(hDlg,WM_COMMAND,
                            MAKELONG(IDC_SET_AFTER,CBN_SELCHANGE),
                    (LPARAM)GetDlgItem(hDlg,IDC_SET_AFTER));
                return TRUE;

            case IDC_SET_AFTER:
                if(HIWORD(wParam)==CBN_SELCHANGE)
                {
                    //Obtiens le New IiNDEXAfter
                    iNewAft=SendMessage((HWND)lParam,CB_GETCURSEL,0,0);
                    //Renitialise iNewBef
                    iNewBef=-1;//important
                }
                return TRUE;

            case IDR_NOCHANGE:
                if(HIWORD(wParam!=BN_CLICKED))
                    return FALSE;
                iUserChoiceOnSetPos=IDR_NOCHANGE;
                //reinitialisation importante
                iNewAft=max(0,iIndexAfterOrg);
                iNewBef=max(0,iIndexBeforeOrg);
                //Set les deux IindexAfter et before a leur origine
                SendMessage(GetDlgItem(hDlg,IDC_SET_BEFORE),CB_SETCURSEL,
                            iNewBef,0);
                SendMessage(GetDlgItem(hDlg,IDC_SET_AFTER),CB_SETCURSEL,
                            iNewAft,0);

                //Desactiver les deux combo
                EnableWindow(GetDlgItem(hDlg,IDC_SET_AFTER),FALSE);
                EnableWindow(GetDlgItem(hDlg,IDC_SET_BEFORE),FALSE);
                return 0;

            case ID_APPLY:
                if(idDefMenuSelected==-1)
                {
                    MyPerror("\n ASCMEIRUU");
                    return TRUE;
                }
                MENUELT *melt=&copieMenuTemp[iIndexMenu].tabElt[0];
                char szMenuName[1024];
                GetWindowText(GetDlgItem(hDlg,IDE_CREATE_MENU),szMenuName,1023);
                if(!(*szMenuName))
                {
                    MyPerror("NU9IEU");
                    return TRUE;
                }
                if(idDefMenuSelected==IDR_CREATE_MAIN)
                {
                    int iLastIindexParent=copieMenuTemp[iIndexMenu].iIndexParent;
                    copieMenuTemp[iIndexMenu].isSubMenu=FALSE;
                    copieMenuTemp[iIndexMenu].iIndexParent=-1;
                    melt->isPopPupMenu=TRUE;
                    melt->dataPop.hMenu=NULL;
                    melt->dataPop.uFlag=MF_POPUP;
                    strcpy(melt->dataPop.szFlagName,"MF_POPUP");
                    strcpy(melt->dataPop.szNameMenu,szMenuName);

                    if(iLastIindexParent!=-1)
                    {
                        //Si le menu devenu main etait d'abord un sous menu
                        //Supprimer ce menu main de lordre de lancien parent
                        if(!MyDelChildToOrder(copieMenuTemp,iIndexMenu,iLastIindexParent,TRUE,FALSE))
                        {
                            MyPerror(" sodifisdf");
                            MyMsgError();
                            return TRUE;
                        }

                    }
                }
                else if(idDefMenuSelected==IDR_CREATE_SUBMAIN)
                {
                    //Determinons iIndexParent selectionne dans la combo et qui sera le prochain parent
                    int iIndexParent=MyGetIndexCombo(hDlg,IDC_CREATE_MENUPARENT);
                    for(int i=0;i<copieMenuTemp[0].iLastIndexMenu;i++)
                    {
                        if(menuTemp[iIndexParent].idMenu==copieMenuTemp[i].idMenu)
                        {
                            iIndexParent=i;
                            break;
                        }
                    }
                    int iLastIindexParent=copieMenuTemp[iIndexMenu].iIndexParent;
                    copieMenuTemp[iIndexMenu].iIndexParent=iIndexParent;
                    copieMenuTemp[iIndexMenu].isSubMenu=TRUE;
                    melt->isPopPupMenu=TRUE;
                    melt->dataPop.hMenu=NULL;
                    melt->dataPop.uFlag=MF_POPUP;
                    strcpy(melt->dataPop.szFlagName,"MF_POPUP");
                    strcpy(melt->dataPop.szNameMenu,szMenuName);

                    if(iLastIindexParent!=-1 && iLastIindexParent!=iIndexParent)
                    {
                        //Si iIndexMenu etait un sous menu, et que son parent a change
                        //1:Retirer iIndexMenu de lordre de l'ancien parent, ou du meme parent en cas d'egalite
                        //2:Ajouter iIndexMenu au nouveau parent
                        if(!MyDelChildToOrder(copieMenuTemp,iIndexMenu,iLastIindexParent,TRUE,FALSE))
                        {
                            MyPerror(" moiwoeif");
                            MyMsgError();
                            return TRUE;
                        }

                         //2:Ajouter iIndexMenu a l'ordre du nouveau parent
                        if(!MyAddChildToOrder(copieMenuTemp,iIndexParent,iIndexMenu,TRUE))
                        {
                            MyPerror(" akfoigoig");
                            MyMsgError();
                            return TRUE;
                        }
                    }
                    else if(iLastIindexParent==-1 )
                    {
                        //Si le menu qui souahite devenir sous menu etait un main Menu
                        //1:Ajouter ce menu a lordre du nouveau parent
                        if(!MyAddChildToOrder(copieMenuTemp,iIndexParent,iIndexMenu,TRUE))
                        {
                            MyPerror(" akfoigoig");
                            MyMsgError();
                            return TRUE;
                        }
                        //Leger changement au cas ou user aurait choisi un NONE
                        //dans les setbef et setAfter, carces index etaient bases sur un nbOrderChild
                        //Et on vient d'augmenter cet order child, donc il faut donc mettre a jour
                        //iNewBef ou NewAfter(suivant le choix) si user a choisi NONE
                        if(iUserChoiceOnSetPos==IDR_BEFORE)
                        {
                            int l=iNewBef;
                            l=tabPosCombo[l];

                            if(l+1==copieMenuTemp[iIndexParent].tabOrder[0].nbOrder)
                            {
                                //Si usser a choisi NONE
                                tabPosCombo[l]+=1;
                            }

                        }
                        else if(iUserChoiceOnSetPos==IDR_AFTER)
                        {
                            int l=iNewAft;
                            l=tabPosCombo[l];
                            if(l+1==copieMenuTemp[iIndexParent].tabOrder[0].nbOrder)
                            {
                                //Si user a choisi NONE
                                tabPosCombo[l]+=1;
                            }
                        }
                    }
                    //Zone pos menu item
                    //Verifie le champs set Pos before after
                    //Si iUserChoice==IDR_NOMANAGE,alors tout est deja regle
                    //Sinoon il faut reorganiser tabOrder Next les choix de User
                    //Avant tout, verifions la coherence dans le cas iUserChoice!=IDR_NOMANAGE
                    if(iUserChoiceOnSetPos==-1)
                    {
                        MyPerror("gerooeiw");
                        MyMsgError();
                        return TRUE;
                    }
                    if(iUserChoiceOnSetPos==IDR_AFTER|| iUserChoiceOnSetPos==IDR_BEFORE)
                    {
                        //,reorganisons tabOrder

                        if(!MySetPosChildMenu(copieMenuTemp,iIndexParent,iIndexMenu,iNewAft,iNewBef,TRUE,tabPosCombo))
                        {
                            MyPerror("werjgk");
                            MyMsgError();
                            return TRUE;
                        }
                    }

                }
            case IDCANCEL:
                idDefMenuSelected=-1;
                iIndexMenuParentSelected=-1;
                EndDialog(hDlg,LOWORD(wParam));
                return TRUE;
            }
            break;

        }
        break;
    }
        break;//fin case IDD_MOD_DIALOG

    case IDD_ADDITEMMENU:
    {
        char szNameIdItem[1024];
        char szNameItem[1024]="";

        switch(message)
        {
        case WM_INITDIALOG:
           {
                //centrer sur hDlgMenu
                RECT rcParent, rcDlg;
                 HWND hDlgMenu=((DLGPARAMINFO*)lParam)->hwndParentDlg;
                 ShowWindow(hDlg,SW_HIDE);
                GetWindowRect(hDlgMenu, &rcParent);
                GetWindowRect(hDlg, &rcDlg);

                int dlgWidth = rcDlg.right - rcDlg.left;
                int dlgHeight = rcDlg.bottom - rcDlg.top;

                int x = rcParent.left + (rcParent.right - rcParent.left) / 2 - dlgWidth / 2;
                int y = rcParent.top + (rcParent.bottom - rcParent.top) / 2 - dlgHeight / 2;
                 SetWindowPos(hDlg, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
                ShowWindow(hDlg,SW_NORMAL);
            }

            //renitialiser g_cacheItem
            g_mycacheItem.szLastStylesChosen[0]='\0';
            g_mycacheItem.uLastStylesChosen=0;

            SetWindowText(GetDlgItem(hDlg,IDE_MOD_PARENTMENU),
                    menuTemp[iIndexMenu].tabElt[0].dataPop.szNameMenu);
            //Definir le nameId de l'item qui s'ajoute
            MySetNameId(szNameIdItem,-1);//-1 concerne les id des menus
            SetWindowText(GetDlgItem(hDlg,IDE_MOD_IDNAMEITEM),
                    szNameIdItem);

            return TRUE;

        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
            case ID_ADD:
                //check si tous les elts sont remplies.
                {
                    //Check si flag est selectionne
                    if(g_mycacheItem.szLastStylesChosen[0]=='\0')
                    {
                        MessageBox(hDlg,TEXT("You need to fill <Flag> 's option"),
                            TEXT("ERROR:"),MB_TASKMODAL|MB_ICONERROR);
                        return TRUE;
                    }
                    //check si Item's name est filled
                    GetWindowText(GetDlgItem(hDlg,IDE_MOD_NAMEITEM),
                                  szNameItem,1023);
                    if(!(*szNameItem))
                    {
                        MessageBox(hDlg,TEXT("You need to fill <Item Name>'s option"),
                            TEXT("ERROR:"),MB_TASKMODAL|MB_ICONERROR);
                        return TRUE;
                    }

                    //Check si la chaine remplie dans IDE_IDNAMEITEM est valide
                    GetWindowText(GetDlgItem(hDlg,IDE_MOD_IDNAMEITEM),
                                  szNameIdItem,1023);
                    if(!(*szNameIdItem))
                    {
                        MessageBox(hDlg,TEXT("You need to fill <NAMEID>'s option! We will use it for define contants for this item in GCODEF.h"),
                            TEXT("ERROR:"),MB_TASKMODAL|MB_ICONERROR);
                        return TRUE;
                    }

                    if(!isValidNameId(szNameIdItem,-1))
                        return TRUE;

                }

                //Si tous les elts sont remplis,alors remplir
                {
                    int i=menuTemp[iIndexMenu].iLastIinexElt;
                    if(i>=MIN_NBELT_PER_MENU)
                    {
                        MessageBox(NULL,TEXT("Unable to Add another item:Not enough Space"),
                                   TEXT("ERROR!!!"),MB_TASKMODAL|MB_ICONERROR);
                        return TRUE;
                    }
                    MENUELT *melt=&menuTemp[iIndexMenu].tabElt[i];
                    melt->isPopPupMenu=FALSE;
                    melt->dataItem.idItem=g_idItemActu++;
                    strcpy(melt->dataItem.szNameItem,szNameItem);
                    melt->dataItem.mycacheItem=g_mycacheItem;
                    melt->dataItem.uFlag=g_mycacheItem.uLastStylesChosen;
                    strcpy(melt->dataItem.szFlagName,g_mycacheItem.szLastStylesChosen);

                    //(opt)szNameidItem
                    GetWindowText(GetDlgItem(hDlg,IDE_MOD_IDNAMEITEM),
                                  szNameIdItem,1023);
                    if(*szNameIdItem)
                        strcpy(melt->dataItem.szNameIdItem,szNameIdItem);
                    else
                        melt->dataItem.szNameIdItem[0]='\0';

                    //Ajout dans tabOrder
                    if(!MyAddChildToOrder(menuTemp,iIndexMenu,
                            menuTemp[iIndexMenu].iLastIinexElt,FALSE))
                    {
                        MyPerror(" asdofoif");
                        MyMsgError();
                        return FALSE;
                    }

                    //Important
                    menuTemp[iIndexMenu].iLastIinexElt+=1;

                    MessageBox(hDlg,TEXT("Success! New item added"),
                               TEXT("INFORMATION!!!"),MB_TASKMODAL|MB_ICONINFORMATION);
                }
            case IDCANCEL:
                EndDialog(hDlg,LOWORD(wParam));
                return TRUE;

            }
            break;//fin case WM_COMMAND

        }//fin switch

    }
        break;//FIN case IDD_MOD_DIALOG

    case IDD_MODITEMMENU:
    {
        char szNameIdItem[1024];
        char szNameItem[1024]="";
        static int iIndexItem=-1;
        static int iIndexBeforeOrg=-1,iIndexAfterOrg=-1;
        static int iNewBef=-1,iNewAft=-1;
        static int iUserChoiceOnSetPos=-1;//Variable qui  stocke le choix de User dans Set Item pos(before,after,no manage)
        static int *tabiIndexCombo=NULL;//ce tableau effectue la correspondance entre les iindex situe dans comboNameItem et ceux de menuTemp
        static int tabPosCombo[MIN_NBELT_PER_MENU+MIN_NBMENU-1];//ce tableau effectue la correspondance entre les iindex situe dans les comboBefore/after et ceux de menuTemp.tabOrder,iInexChilds
        switch(message)
        {
        case WM_INITDIALOG:
        {
                //centrer sur hDlgMenu
                RECT rcParent, rcDlg;
                 HWND hDlgMenu=((DLGPARAMINFO*)lParam)->hwndParentDlg;
                 ShowWindow(hDlg,SW_HIDE);
                GetWindowRect(hDlgMenu, &rcParent);
                GetWindowRect(hDlg, &rcDlg);

                int dlgWidth = rcDlg.right - rcDlg.left;
                int dlgHeight = rcDlg.bottom - rcDlg.top;

                int x = rcParent.left + (rcParent.right - rcParent.left) / 2 - dlgWidth / 2;
                int y = rcParent.top + (rcParent.bottom - rcParent.top) / 2 - dlgHeight / 2;
                 SetWindowPos(hDlg, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
                ShowWindow(hDlg,SW_NORMAL);
            }
            iIndexItem=-1;
            SetWindowText(GetDlgItem(hDlg,IDE_MOD_PARENTMENU),
                    menuTemp[iIndexMenu].tabElt[0].dataPop.szNameMenu);
            //Charger IDC_MOD_NAMEITEM  avec les items de iIndexMenu
           {
               free(tabiIndexCombo);
               int nbElt=menuTemp[iIndexMenu].iLastIinexElt;
               if(nbElt<=1)
               {
                   MyPerror("v mriueu");
                   return TRUE;
               }
               tabiIndexCombo=malloc(sizeof(int)*nbElt);
               if(!tabiIndexCombo)
               {
                   MyPerror(" nerjietj");
                   exit(EXIT_FAILURE);
               }

               HWND hwndCombo=GetDlgItem(hDlg,IDC_MOD_NAMEITEM);
               if(!hwndCombo)
               {
                   MyPerror("nv itur9yi");
                   exit(EXIT_FAILURE);
               }
               int i,j=0;
               char *p;
               for(i=1;i<nbElt;i++)//1 car a 0 on a szNameMenu
               {
                   if(menuTemp[iIndexMenu].tabElt[i].isPopPupMenu) continue;
                   p=menuTemp[iIndexMenu].tabElt[i].dataItem.szNameItem;
                   SendMessage(hwndCombo,CB_INSERTSTRING,j,(LPARAM)p);
                   tabiIndexCombo[j]=i;
                   j++;
               }
           }
           //Desactive le GroupBox des set AFTER et BEFORE
           EnableWindow(GetDlgItem(hDlg,IDB_GB_SETPOS_ITEM),FALSE);
           EnableWindow(GetDlgItem(hDlg,IDR_BEFORE),FALSE);
           EnableWindow(GetDlgItem(hDlg,IDR_AFTER),FALSE);
           EnableWindow(GetDlgItem(hDlg,IDC_SET_AFTER),FALSE);
            EnableWindow(GetDlgItem(hDlg,IDC_SET_BEFORE),FALSE);
            EnableWindow(GetDlgItem(hDlg,IDR_NOCHANGE),FALSE);

            return TRUE;

        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
            case  IDC_MOD_NAMEITEM:
                if(HIWORD(wParam)==CBN_SELCHANGE)
                {
                    //Recupere le nom de l'item


                    HWND hwndEditNameItem=GetDlgItem(hDlg,IDE_MOD_NAMEITEM);
                    HWND hwndEditNameIdItem=GetDlgItem(hDlg,IDE_MOD_IDNAMEITEM);

                    if(!hwndEditNameItem || !hwndEditNameIdItem)
                    {
                        MyPerror(" mijidfsop");
                        exit(EXIT_FAILURE);
                    }

                    char *c;
                    int i=SendMessage((HWND)lParam,CB_GETCURSEL,0,0);
                    i=tabiIndexCombo[i];
                    iIndexItem=i;

                    g_mycacheItem=menuTemp[iIndexMenu].tabElt[iIndexItem].dataItem.mycacheItem;

                    if(!MyLoadCache(-1,NULL,hDlg))
                    {
                        MyPerror("e,wrikt");
                        MyMsgError();
                        exit(EXIT_FAILURE);
                    }
                    //FILL szNameItem
                    c=menuTemp[iIndexMenu].tabElt[i].dataItem.szNameItem;
                    SetWindowText(hwndEditNameItem,c);
                    //Fill (opt)szNameIdItem
                    c=menuTemp[iIndexMenu].tabElt[i].dataItem.szNameIdItem;
                    if(*c)
                        SetWindowText(hwndEditNameIdItem,c);
                    else
                        SetWindowText(hwndEditNameIdItem,"");
                    //Fill flag's option
                    //renitialisation
                    iIndexAfterOrg=-1;
                    iIndexBeforeOrg=-1;
                    iUserChoiceOnSetPos=-1;
                    //Rempli les combo ,bref ffaire les initialisation necessaires dans GB_AFTER/BEFORE
                    if(!MyInitMenuChildOrder(hDlg,menuTemp,iIndexItem,iIndexMenu,FALSE,tabPosCombo))
                    {
                        MyPerror(",vosidf");
                        MyMsgError();
                        return TRUE;
                    }
                    //Prend les valeurs de iIndexAfter et before
                   iIndexBeforeOrg =SendMessage(GetDlgItem(hDlg,IDC_SET_BEFORE),
                                            CB_GETCURSEL,0,0);
                   iIndexAfterOrg =SendMessage(GetDlgItem(hDlg,IDC_SET_AFTER),
                                            CB_GETCURSEL,0,0);

                        //Selectionner par defaut Nochange
                    SendMessage(hDlg,WM_COMMAND,
                        MAKELONG(IDR_NOCHANGE,BN_CLICKED),
                        (LPARAM)GetDlgItem(hDlg,IDR_NOCHANGE));
                    CheckRadioButton(hDlg,IDR_BEFORE,IDR_NOCHANGE,IDR_NOCHANGE);

                }
                return TRUE;


            case IDR_BEFORE:
                if(HIWORD(wParam)!=BN_CLICKED) return FALSE;

                iUserChoiceOnSetPos=IDR_BEFORE;
                //Enable le comboBox correspondant ET DISABLE L'AUTRE
                EnableWindow(GetDlgItem(hDlg,IDC_SET_BEFORE),TRUE);
                EnableWindow(GetDlgItem(hDlg,IDC_SET_AFTER),FALSE);
                //Update iNewBefore et iNewAft
               SendMessage(hDlg,WM_COMMAND,
                        MAKELONG(IDC_SET_BEFORE,CBN_SELCHANGE),
                        (LPARAM )GetDlgItem(hDlg,IDC_SET_BEFORE));
               return TRUE;

            case IDC_SET_BEFORE:
                if(HIWORD(wParam)==CBN_SELCHANGE)
                {
                    //Obtiens le New IiNDEX
                    iNewBef=SendMessage((HWND)lParam,CB_GETCURSEL,0,0);
                    iNewAft=-1;
                }
                return TRUE;
            case IDR_AFTER:
                if(HIWORD(wParam)!=BN_CLICKED) return FALSE;
                iUserChoiceOnSetPos=IDR_AFTER;
                //Enable le comboBox correspondant ET DISABLE l'autre
                EnableWindow(GetDlgItem(hDlg,IDC_SET_AFTER),TRUE);
                EnableWindow(GetDlgItem(hDlg,IDC_SET_BEFORE),FALSE);
                 //Update iNewBefore et iNewAft
               SendMessage(hDlg,WM_COMMAND,
                        MAKELONG(IDC_SET_AFTER,CBN_SELCHANGE),
                        (LPARAM )GetDlgItem(hDlg,IDC_SET_AFTER));

                return TRUE;

            case IDC_SET_AFTER:
                if(HIWORD(wParam)==CBN_SELCHANGE)
                {
                    //Obtiens le New IiNDEXAfter
                    iNewAft=SendMessage((HWND)lParam,CB_GETCURSEL,0,0);
                    iNewBef=-1;
                }
                return TRUE;

            case IDR_NOCHANGE:
                if(HIWORD(wParam!=BN_CLICKED))
                    return FALSE;
                iUserChoiceOnSetPos=IDR_NOCHANGE;
                //reinitialisation importante
                iNewAft=max(0,iIndexAfterOrg);
                iNewBef=max(0,iIndexBeforeOrg);
                //Set les deux IindexAfter et before a leur origine
                SendMessage(GetDlgItem(hDlg,IDC_SET_BEFORE),CB_SETCURSEL,
                            iIndexBeforeOrg,0);
                SendMessage(GetDlgItem(hDlg,IDC_SET_AFTER),CB_SETCURSEL,
                            iIndexAfterOrg,0);

                //Desactiver les deux combo
                EnableWindow(GetDlgItem(hDlg,IDC_SET_AFTER),FALSE);
                EnableWindow(GetDlgItem(hDlg,IDC_SET_BEFORE),FALSE);
                return 0;
            case ID_APPLY:
                //check si tous les elts sont remplies.
                {
                    //Check si la chaine remplie dans IDE_IDNAMEITEM est valide
                    GetWindowText(GetDlgItem(hDlg,IDE_MOD_IDNAMEITEM),
                                  szNameIdItem,1023);
                    if(!(*szNameIdItem))
                    {
                        MessageBox(hDlg,TEXT("You need to fill <NAMEID>'s option! We will use it for define contants for this item in GCODEF.h"),
                            TEXT("ERROR:"),MB_TASKMODAL|MB_ICONERROR);
                        return TRUE;
                    }

                    {
                        //On modifie subtilement idObjToDelete
                        int copieIdObjToDel=idObjToDelete;
                        idObjToDelete=iIndexItem;//Utile dans isValidNameId
                        if(!isValidNameId(szNameIdItem,-1))
                            return TRUE;
                        idObjToDelete=copieIdObjToDel;//restauration
                    }
                    //check si iIndexItem est selectionne
                    if(iIndexItem==-1)
                    {
                        MessageBox(hDlg,TEXT("You need to select iIndex to modify on <1.item to modify>'s option"),
                                   "ERROR!!!",MB_TASKMODAL|MB_ICONERROR);
                        return TRUE;
                    }
                    //Check si flag est selectionne
                    if(g_mycacheItem.szLastStylesChosen[0]=='\0')
                    {
                        MessageBox(hDlg,TEXT("You need to fill <Flag> 's option"),
                            TEXT("ERROR:"),MB_TASKMODAL|MB_ICONERROR);
                        return TRUE;
                    }
                    //check si Item's name est filled

                    GetWindowText(GetDlgItem(hDlg,IDE_MOD_NAMEITEM),
                                  szNameItem,1023);
                    if(!(*szNameItem))
                    {
                        MessageBox(hDlg,TEXT("You need to fill <Item Name>'s option"),
                            TEXT("ERROR:"),MB_TASKMODAL|MB_ICONERROR);
                        return TRUE;
                    }
                    GetWindowText(GetDlgItem(hDlg,IDE_MOD_IDNAMEITEM),
                                  szNameIdItem,1023);
                }
                //Verifie le champs set Pos before after
                //Si iUserChoice==IDR_NOMANAGE,alors tout st deja regle
                //Sinoon il faut reorganiser tabOrder Next les choix de User
                //Avant tout, verifions la coherence dans le cas iUserChoice!=IDR_NOMANAGE
                if(iUserChoiceOnSetPos==-1)
                {
                    MyPerror(", aosdifosd");
                    MyMsgError();
                    return TRUE;
                }
                if(iUserChoiceOnSetPos==IDR_AFTER|| iUserChoiceOnSetPos==IDR_BEFORE)
                {
                    //Sinon ,reorganisons tabOrder
                    if(!MySetPosChildMenu(menuTemp,iIndexMenu,iIndexItem,iNewAft,iNewBef,FALSE,tabPosCombo))
                    {
                        MyPerror("m oifodisdf");
                        MyMsgError();
                        return TRUE;
                    }
                }
                {
                    int i=iIndexItem;
                    MENUELT *melt=&menuTemp[iIndexMenu].tabElt[i];
                    melt->isPopPupMenu=FALSE;
                    /*melt->dataItem.idItem=g_idItemActu++;*///ne change pas
                    strcpy(melt->dataItem.szNameItem,szNameItem);
                    melt->dataItem.mycacheItem=g_mycacheItem;
                    melt->dataItem.uFlag=g_mycacheItem.uLastStylesChosen;
                    strcpy(melt->dataItem.szFlagName,g_mycacheItem.szLastStylesChosen);

                    //szNameidItem
                    GetWindowText(GetDlgItem(hDlg,IDE_MOD_IDNAMEITEM),
                                  szNameIdItem,1023);
                    if(*szNameIdItem)
                        strcpy(melt->dataItem.szNameIdItem,szNameIdItem);
                    else
                        melt->dataItem.szNameIdItem[0]='\0';


                    MessageBox(hDlg,TEXT("Success! Modification of item succeed!"),
                               TEXT("INFORMATION!!!"),MB_TASKMODAL|MB_ICONINFORMATION);

                }
            case IDCANCEL:
                 EndDialog(hDlg,LOWORD(wParam));
                return TRUE;

            }
            break;//fin case WM_COMMAND

        }//fin switch

    }
        break;//fin case IDD_MODITEMMENU


    case IDD_DELITEMMENU:
    {

        static int iIndexItem=-1;
        static int *tabiIndexCombo=NULL;//ce tableau effectue la correspondance entre les iindex situe dans comboNameItem et ceux de menuTemp
        switch(message)
        {
        case WM_INITDIALOG:
        {
                //centrer sur hDlgMenu
                RECT rcParent, rcDlg;
                 HWND hDlgMenu=((DLGPARAMINFO*)lParam)->hwndParentDlg;
                 ShowWindow(hDlg,SW_HIDE);
                GetWindowRect(hDlgMenu, &rcParent);
                GetWindowRect(hDlg, &rcDlg);

                int dlgWidth = rcDlg.right - rcDlg.left;
                int dlgHeight = rcDlg.bottom - rcDlg.top;

                int x = rcParent.left + (rcParent.right - rcParent.left) / 2 - dlgWidth / 2;
                int y = rcParent.top + (rcParent.bottom - rcParent.top) / 2 - dlgHeight / 2;
                 SetWindowPos(hDlg, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
                ShowWindow(hDlg,SW_NORMAL);
            }

            SetWindowText(GetDlgItem(hDlg,IDE_MOD_PARENTMENU),
                    menuTemp[iIndexMenu].tabElt[0].dataPop.szNameMenu);

            iIndexItem=-1;
            //Charger IDC_MOD_NAMEITEM  avec les items de iIndexMenu
           {
               free(tabiIndexCombo);
               int nbElt=menuTemp[iIndexMenu].iLastIinexElt;
               if(nbElt<=1)
               {
                   MyPerror("v mriueu");
                   return TRUE;
               }
               tabiIndexCombo=malloc(sizeof(int)*nbElt);
               if(!tabiIndexCombo)
               {
                   MyPerror(" nerjietj");
                    MyMsgError();
                    return TRUE;
                   exit(EXIT_FAILURE);
               }

               HWND hwndCombo=GetDlgItem(hDlg,IDC_MOD_NAMEITEM);
               if(!hwndCombo)
               {
                   MyPerror("nv itur9yi");
                   exit(EXIT_FAILURE);
               }
               int i,j=0;
               char *p;
               for(i=1;i<nbElt;i++)//1 car a 0 on a szNameMenu
               {
                   if(menuTemp[iIndexMenu].tabElt[i].isPopPupMenu) continue;
                   p=menuTemp[iIndexMenu].tabElt[i].dataItem.szNameItem;
                   SendMessage(hwndCombo,CB_INSERTSTRING,j,(LPARAM)p);
                   tabiIndexCombo[j]=i;
                   j++;
               }
           }
            return TRUE;

        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
            case  IDC_MOD_NAMEITEM:
                if(HIWORD(wParam)==CBN_SELCHANGE)
                {


                    int i=SendMessage((HWND)lParam,CB_GETCURSEL,0,0);
                    if(i<0)
                    {
                        MyPerror(" osdioig");
                        MyMsgError();
                        return TRUE;
                    }
                    i=tabiIndexCombo[i];
                    iIndexItem=i;

                }
                return TRUE;

            case ID_APPLY:
                //check si tous les elts sont remplies.
                {
                    //check si iIndexItem est selectionne
                    if(iIndexItem==-1)
                    {
                        MessageBox(hDlg,TEXT("You need to select iIndex to delete on <1.item to del>'s option"),
                                   "ERROR!!!",MB_TASKMODAL|MB_ICONERROR);
                        return TRUE;
                    }
                    //Suppression de idItem
                    {
                        if(iIndexItem==0)
                        {
                            MyPerror("M VAEK");
                            MyMsgError();
                            return TRUE;
                        }
                        int nbElt=menuTemp[iIndexMenu].iLastIinexElt;
                        for(int i=iIndexItem;i<nbElt-1;i++)
                        {
                            menuTemp[iIndexMenu].tabElt[i]=menuTemp[iIndexMenu].tabElt[i+1];
                        }
                        menuTemp[iIndexMenu].iLastIinexElt-=1;//important;

                        //Supprimer de tabOrder
                        if(!MyDelChildToOrder(menuTemp,iIndexItem,iIndexMenu,FALSE,TRUE))
                        {
                            MyPerror(" iouidug");
                            MyMsgError();
                            return FALSE;
                        }
                    }

                }
            case IDCANCEL:
                 EndDialog(hDlg,LOWORD(wParam));
                return TRUE;

            }
            break;//fin case WM_COMMAND

        }//fin switch

    }
        break;//fin case IDD_DELITEMMENU

    }

    return FALSE;
}

BOOL MyApplyMenuTempOnCombos(HWND hDlg, int idCombo)
{
    //cette fonction se charge particulierement de rempli les combos
    //avec les noms contenus dans la variable globale menuTemp
    if(!hDlg)
    {
        MyPerror("mnv erute8");
        return FALSE;
    }

    HWND hwndCombo=GetDlgItem(hDlg,idCombo);
    if(!hwndCombo)
    {
        MyPerror("vmiurte");
        return FALSE;
    }
    int iIndexActu=SendMessage(hwndCombo,CB_GETCURSEL,0,0);
    //remove all content
    SendMessage(hwndCombo,CB_RESETCONTENT,0,0);
    //Add new string based on menuTemp
    int i;
    char *szNameMenu;
    for(i=0;i<menuTemp[0].iLastIndexMenu;i++)
    {
        szNameMenu=menuTemp[i].tabElt[0].dataPop.szNameMenu;
        SendMessage(hwndCombo,CB_INSERTSTRING,i,(LPARAM)szNameMenu);
    }

    SendMessage(hwndCombo,CB_SETCURSEL,iIndexActu,0);
    return TRUE;
}

BOOL MyAllocMenuTab(MYMENULIST **pg_menuTab)
{
    //Cette fonction verifie si *pg_menuTAB EST null, SI OUI, il realise
    //une allocation puis initialise les champs
    //Sion ,si c'est non null,il initialise juste les champs

    int i;
    MYMENULIST *mnl=NULL;

    if(!(*pg_menuTab))
    {
        *pg_menuTab=malloc(sizeof(MYMENULIST)*MIN_NBMENU);
        if(!(*pg_menuTab))
        {
            MyPerror(" jfjldf");
            return FALSE;
        }

        mnl=*pg_menuTab;
        for(i=0;i<MIN_NBMENU;i++)
        {
            mnl[i].tabElt=malloc(sizeof(MENUELT)*MIN_NBELT_PER_MENU);
            if(!mnl[i].tabElt)
            {
                MyPerror("m ojijgds");
                return FALSE;
            }

            mnl[i].tabOrder=malloc(sizeof(MYMENUORDER)*(MIN_NBELT_PER_MENU+MIN_NBMENU));
            if(!mnl[i].tabOrder)
            {
                MyPerror("v oifdoif");
                MyMsgError();
                return FALSE;
            }
            memset(mnl[i].tabOrder,0,
                   sizeof(MYMENUORDER)*(MIN_NBELT_PER_MENU+MIN_NBMENU));
        }

    }

    mnl=*pg_menuTab;
    for(i=0;i<MIN_NBMENU;i++)
    {
        mnl[i].iLastIndexMenu=0;
        mnl[i].iLastIinexElt=0;
        mnl[i].tabOrder[0].nbOrder=0;
    }

    return TRUE;
}
BOOL MyFreeMenuTab(MYMENULIST **pg_menuTab)
{
    //Libere le tableau, et rend *pg_menuTab a NULL
    if(!(*pg_menuTab))
        return TRUE;
    int i;
    MYMENULIST *mnl=*pg_menuTab;
    //free dabord les tabElt
    for(i=0;i<MIN_NBMENU;i++)
    {
        free(mnl[i].tabElt);
        free(mnl[i].tabOrder);
    }
    free(*pg_menuTab);
    *pg_menuTab=NULL;

    return TRUE;
}

BOOL MyCopieMenuTab(const MYMENULIST *source,MYMENULIST **pdest)
{

    if(!source)
    {
        MyPerror("mejrj");
        return FALSE;
    }

    if(!pdest)
    {
        MyPerror("m d kfgdlgf");
        return FALSE;
    }
    if(!(*pdest))
    {
        if(!MyAllocMenuTab(pdest))
        {
            MyPerror(", afkdf");
            return FALSE;
        }
        if(!(*pdest))
        {
            MyPerror("m aojkjd");
            return FALSE;
        }
    }

    MYMENULIST *mnl=*pdest;
    //debut copie
    for(int i=0;i<MIN_NBMENU;i++)
    {
        mnl[i].idMenu=source[i].idMenu;
        mnl[i].iIndexComboCreate=source[i].iIndexComboCreate;
        mnl[i].iIndexParent=source[i].iIndexParent;
        mnl[i].iLastIinexElt=source[i].iLastIinexElt;
        mnl[i].iLastIndexMenu=source[i].iLastIndexMenu;
        mnl[i].isSubMenu=source[i].isSubMenu;
        for(int j=0;j<MIN_NBELT_PER_MENU;j++)
        {
            mnl[i].tabElt[j]=source[i].tabElt[j];
        }
        for(int j=0;j<(MIN_NBELT_PER_MENU+MIN_NBMENU);j++)
        {
            mnl[i].tabOrder[j]=source[i].tabOrder[j];
        }
    }

    //Apres la copie on verifie que cest bien identique
    if(!isIdenticalTabMenu(source,*pdest))
    {
        MyPerror("m jfjsdf");
        exit(EXIT_FAILURE);
    }
    return TRUE;
}

BOOL isIdenticalTabMenu(const MYMENULIST *menuTab1,const MYMENULIST *menuTab2)
{
    //vERIFIE SI les deux tab ont le meme contenues
    //NB: LA verification ne se fait pas encore avec le champs tabOrder
    if(menuTab1==menuTab2) return TRUE;
    if(menuTab1[0].iLastIndexMenu!=menuTab2[0].iLastIndexMenu)
        return FALSE;
    int i,j;

    for(i=0;i<menuTab1[0].iLastIndexMenu;i++)
    {
        if(menuTab1[i].idMenu!=menuTab2[i].idMenu)
            return FALSE;
        if(menuTab1[i].iIndexComboCreate!=menuTab2[i].iIndexComboCreate)
            return FALSE;
        if(menuTab1[i].iIndexParent!=menuTab2[i].iIndexParent)
            return FALSE;
        if(menuTab1[i].iLastIinexElt!=menuTab2[i].iLastIinexElt)
            return FALSE;
        if(menuTab1[i].iLastIndexMenu!=menuTab2[i].iLastIndexMenu)
            return FALSE;
        if(menuTab1[i].isSubMenu!=menuTab2[i].isSubMenu)
            return FALSE;
        for(j=0;j<menuTab1[i].iLastIinexElt;j++)
            if(!isIdenticalTabElt(&menuTab1[i].tabElt[j],&menuTab2[i].tabElt[j],menuTab1[i].iLastIinexElt))
            {
                return FALSE;
            }

    }
    return TRUE;
}

BOOL isIdenticalTabElt(const MENUELT *melt1,const MENUELT *melt2,int tailleTab)
{
    //verifie que melt1 et melt2 sont identiques en contenus
    if(melt1==melt2) return TRUE;

    int i;
    for(i=0;i<tailleTab;i++)
    {

        if(melt1[i].isPopPupMenu!=melt2[i].isPopPupMenu)
            return FALSE;
        if(melt1[i].isPopPupMenu)
        {
            //for popup

            if(strcmp(melt1[i].dataPop.szNameMenu,
                melt2[i].dataPop.szNameMenu))
                return FALSE;

            if(strcmp(melt1[i].dataPop.szFlagName,
                melt2[i].dataPop.szFlagName))
                return FALSE;

            if(melt1[i].dataPop.uFlag!=melt2[i].dataPop.uFlag)
                return FALSE;
        }
        else
        {
            //for item
            if(strcmp(melt1[i].dataItem.szNameItem,
                melt2[i].dataItem.szNameItem))
                return FALSE;

            if(strcmp(melt1[i].dataItem.szNameIdItem,
                melt2[i].dataItem.szNameIdItem))
                return FALSE;

           if(strcmp(melt1[i].dataItem.szFlagName,
                melt2[i].dataItem.szFlagName))
                return FALSE;

            if(melt1[i].dataItem.uFlag!=melt2[i].dataItem.uFlag)
                return FALSE;
        }
    }

    return TRUE;
}

BOOL MyGetIindexMenuWithIdMenu(MYMENULIST *menuTab,int idMenu)
{
    //coe son nom l'indique, cette fonction retournw l'index du menu
    //connaissant son id
    if(!menuTab)
        return -1;
    int i;

    for(i=0;i<menuTab[0].iLastIndexMenu;i++)
    {
        if(menuTab[i].idMenu==idMenu)
            return i;
    }
    return -1;
}

BOOL MyDelChildToOrder(MYMENULIST *g_menuTab,int iIndexChildToDel,int iIndexMenu,BOOL isMenuChild,BOOL fShouldDecrease)
{
    //Le role de cette fonction est de supprimer iIndexChildToDel
    // dans le tabOrder
    if(!g_menuTab)
    {
        MyPerror(", dofigdf");
        MyMsgError();
        return FALSE;
    }

    int nbChildOrder=g_menuTab[iIndexMenu].tabOrder[0].nbOrder;
    MYMENUORDER *mno=g_menuTab[iIndexMenu].tabOrder;
    BOOL rep=FALSE;
    int iIndexOrderDelected=-1;
    for(int j=0;j<nbChildOrder;j++)
    {
        if(mno[j].isMenuChild==isMenuChild && mno[j].iIndexChild==iIndexChildToDel)
        {
            iIndexOrderDelected=j;
            for(int k=j;k<nbChildOrder-1;k++)
            {
                mno[k]=mno[k+1];
            }
            mno[0].nbOrder=nbChildOrder-1;//important
            rep= TRUE;
        }

    }

    //Puisque une suppression de iIndexChilds  eu lieu
    //Decroite tous les iIdexChilds de meme isMenuChilds d'un pas
    if(rep==FALSE) return FALSE;
    if(iIndexOrderDelected==-1)
    {
        MyPerror("sdpog");
        MyMsgError();
        return FALSE;
    }

    if(fShouldDecrease)
    {
        for(int i=0;i<mno[0].nbOrder;i++)
        {
            if(isMenuChild==mno[i].isMenuChild &&
            mno[i].iIndexChild>iIndexChildToDel )
            mno[i].iIndexChild-=1;
        }
    }


    return TRUE;
}

BOOL MyInitMenuChildOrder(HWND hDlg,MYMENULIST *g_menuTab,int iIndexChild,int iIndexMenu,BOOL isMenuChild,int *tabPosCombo)
{
    //le role de cette fonction est de charger les combo
    //des IDR SET-AFTER ET BEFORE
    //Et d'initialiser letat sur noChange
    if(!hDlg)
    {
        MyPerror(" sodifds");
        MyMsgError();
        return FALSE;
    }
    if(!g_menuTab)
    {
        MyPerror(" msofodf");
        MyMsgError();
        return FALSE;
    }
    if(iIndexMenu<0 || iIndexMenu>=g_menuTab[0].iLastIndexMenu )
    {
        MyPerror(" soioeirt");
        MyMsgError();
        return FALSE;
    }
    if(isMenuChild && iIndexChild>=g_menuTab[0].iLastIndexMenu)
    {
        MyPerror("mo iujg");
        MyMsgError();
        return FALSE;
    }
    if(!isMenuChild && iIndexChild>=g_menuTab[iIndexMenu].iLastIinexElt)
    {
        MyPerror("m oerigo");
        MyMsgError();
        return FALSE;
    }

    //Activer tous les childs du Gb
    //Active le GroupBox des set AFTER et BEFORE
    HWND hwndComboBefore=GetDlgItem(hDlg,IDC_SET_BEFORE);
    HWND hwndComboAfter=GetDlgItem(hDlg,IDC_SET_AFTER);
    if(!hwndComboAfter || !hwndComboBefore)
    {
        MyPerror(",soffidf");
        MyMsgError();
        return FALSE;
    }
    //Vider le contenu de la combo
    SendMessage(hwndComboAfter,CB_RESETCONTENT,0,0);
    SendMessage(hwndComboBefore,CB_RESETCONTENT,0,0);

    EnableWindow(GetDlgItem(hDlg,IDB_GB_SETPOS_ITEM),TRUE);
    EnableWindow(GetDlgItem(hDlg,IDR_BEFORE),TRUE);
    EnableWindow(GetDlgItem(hDlg,IDR_AFTER),TRUE);
    EnableWindow(hwndComboAfter,FALSE);
    EnableWindow(hwndComboBefore,FALSE);
    EnableWindow(GetDlgItem(hDlg,IDR_NOCHANGE),TRUE);

    //Chargement des combos
    int nbChildOrder=g_menuTab[iIndexMenu].tabOrder[0].nbOrder;
    MYMENUORDER *mno=g_menuTab[iIndexMenu].tabOrder;
    int iIndexBefore=-1,iIndexAfter=-1;
    int i,iIndex=-1;


    for( i=0;i<nbChildOrder;i++)
    {
        if(mno[i].isMenuChild==isMenuChild &&
           mno[i].iIndexChild==iIndexChild)
        {
            if(i==0)
            {
                iIndexBefore=-1;
                iIndexAfter=(i+1<nbChildOrder)?i:-1;
            }
            else
            {
                iIndexBefore=i-1;
                iIndexAfter=(i+1<nbChildOrder)?i:-1;
            }
            continue;
        }
        iIndex++;
        char *c;
        if(mno[i].isMenuChild)
        {
            int iIndexChild2=mno[i].iIndexChild;
             c=g_menuTab[iIndexChild2].tabElt[0].dataPop.szNameMenu;
            //Correspondance iiNdexCombo et i du order
            tabPosCombo[iIndex]=i;
        }
        else
        {
            int iIndexChild2=mno[i].iIndexChild;
            c=g_menuTab[iIndexMenu].tabElt[iIndexChild2].dataItem.szNameItem;
            //Correspondance iiNdexCombo et i du order
            tabPosCombo[iIndex]=i;
        }

        SendMessage(hwndComboAfter,CB_INSERTSTRING,
            (WPARAM)iIndex,(LPARAM)c);
        SendMessage(hwndComboBefore,CB_INSERTSTRING,
            (WPARAM)iIndex,(LPARAM)c);
    }
    //Ajouter "NONE", AUX COMBOBOX
    iIndex+=1;
    SendMessage(hwndComboAfter,CB_INSERTSTRING,
            (WPARAM)iIndex,(LPARAM)"NONE");
    SendMessage(hwndComboBefore,CB_INSERTSTRING,
            (WPARAM)iIndex,(LPARAM)"NONE");
    //Correspondance iiNdexCombo et i du order
    tabPosCombo[iIndex]=i;
    //Selection par defaut
    if(iIndexAfter==-1)
    {
        iIndexAfter=iIndex;//NONE
    }

    if(iIndexBefore==-1)
    {
        iIndexBefore=iIndex;//NONE
    }

    SendMessage(hwndComboAfter,CB_SETCURSEL,
            (WPARAM)iIndexAfter,0);

    SendMessage(hwndComboBefore,CB_SETCURSEL,
            (WPARAM)iIndexBefore,0);

    return TRUE;
}

BOOL MySetPosChildMenu(MYMENULIST *g_menuTab,int iIndexMenu,int iIndexChild,int iComboAfter,int iComboBef,BOOL isMenuChild,int *tabPosCombo)
{
    //Cette fonction tente de positonner iiNDEXChild entre
    //iNew et iAfter dans le tabOrder associe au g_menuTab[iIndexMenu]
    if(!g_menuTab)
    {
        MyPerror(" asodifodf");
        return FALSE;
    }
    if(iIndexMenu<0 || iIndexMenu>=g_menuTab[0].iLastIndexMenu)
    {
        MyPerror("oikflas");
        return FALSE;
    }

    //traduire les iNew en fonction d'indice de tabOrder
    int iNewAfter,iNewBef;//Vrai iIndex dans g_menuTab.tabOrder, les autres iComboBef et aft sont relatifs aux combos

    if(iComboBef!=-1)
    {
        //It means that user wants iComboBef before iIndexChild in tabOrder

        //Get a real value based on g_menuTab, iComboBef is iIndex based on combo
        iNewBef=tabPosCombo[iComboBef];
        if(iNewBef==g_menuTab[iIndexMenu].tabOrder[0].nbOrder)
            iNewBef=-1;

        int iOrderChild=-1;//index position of iIndexChild in tabOrder

        //Find iOrderChild
        int nbOrderChild=g_menuTab[iIndexMenu].tabOrder[0].nbOrder;
        MYMENUORDER *mno=g_menuTab[iIndexMenu].tabOrder;
        for(int i=0;i<g_menuTab[iIndexMenu].tabOrder[0].nbOrder;i++)
        {
            if(mno[i].isMenuChild==isMenuChild &&
               mno[i].iIndexChild==iIndexChild)
            {
                iOrderChild=i;
                break;
            }
        }
        if(iOrderChild==-1)
        {
            MyPerror(" msaokfldk");
            MyMsgError();
            return FALSE;
        }
        MYMENUORDER temp=mno[iOrderChild];


        if(iNewBef>iOrderChild )
        {
            for(int i=iOrderChild;i<iNewBef;i++)
            {
                mno[i]=mno[i+1];
            }
            mno[iNewBef]=temp;
        }
        else if(iNewBef<iOrderChild && iOrderChild-iNewBef>1)
        {
            //ifiOrderChild-InewBef=1, there is no change needed
            //On doit tout faire pour mettre mno[iOderChild] a mno[iNewBef+1]
            //ON FAIT DONC un decalage de iNewBef+1 vers iOrder,de 1,
            //En ecrasant lancien espace occupe par mno[iOrderChild]
            //Puis on stocke mno[iOrderChild]=temp dans lespace inewBef+1 ouvert par le decallage
            for(int i=iOrderChild;i>=iNewBef+2;i--)
            {
                mno[i]=mno[i-1];
            }

            mno[iNewBef+1]=temp;
        }

        mno[0].nbOrder=nbOrderChild;//important

    }
    else if(iComboAfter!=-1)
    {
        iNewAfter=tabPosCombo[iComboAfter];
        /*if(iNewAfter==g_menuTab[iIndexMenu].tabOrder[0].nbOrder)
            iNewAfter=0;
*/
        int iOrderChild=-1;
        //rechercher iIndexChild(son iOrder)
        int nbOrderChild=g_menuTab[iIndexMenu].tabOrder[0].nbOrder;
        MYMENUORDER *mno=g_menuTab[iIndexMenu].tabOrder;
        for(int i=0;i<g_menuTab[iIndexMenu].tabOrder[0].nbOrder;i++)
        {
            if(mno[i].isMenuChild==isMenuChild &&
               mno[i].iIndexChild==iIndexChild)
            {
                iOrderChild=i;
                break;
            }
        }
        if(iOrderChild==-1)
        {
            MyPerror("Q3N21F");
            MyMsgError();
            return FALSE;
        }

        MYMENUORDER temp=mno[iOrderChild];
        if(iNewAfter>iOrderChild)
        {
            for(int i=iOrderChild;i<iNewAfter-1;i++)
            {
                mno[i]=mno[i+1];
            }
            mno[iNewAfter-1]=temp;
        }
        else if(iNewAfter<iOrderChild)
        {
            for(int i=iOrderChild;i>iNewAfter;i--)
            {
                mno[i]=mno[i-1];
            }
            mno[iNewAfter]=temp;
        }
        mno[0].nbOrder=nbOrderChild;//important

    }
        return TRUE;
}

BOOL MyAddChildToOrder(MYMENULIST *g_menuTab,int iIndexMenu,int iIndexChild,BOOL isMenuChild)
{
    //Le role de cette fonction est d'ajouter un nouveau childs
    //a la fin du tabOrder correspondant au iIndexMenu
    if(!g_menuTab)
    {
        MyPerror("ewkoo");
        return FALSE;
    }
    if(iIndexMenu<0 || iIndexMenu>=g_menuTab[0].iLastIndexMenu)
    {
        MyPerror("kewokfow");
        return FALSE;
    }

    //Ajout dans tabOrder
    MYMENUORDER *mno=g_menuTab[iIndexMenu].tabOrder;
    int nbChildOrder=mno[0].nbOrder;
    mno[nbChildOrder].isMenuChild=isMenuChild;
    mno[nbChildOrder].iIndexChild=iIndexChild;
    mno[0].nbOrder++;//iMPORTANT
    return TRUE;
}
