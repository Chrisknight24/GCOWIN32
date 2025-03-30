/*Nom du File:MyManageColor.c
Role: File qui gere la creation de la boite de dialogue qui
saffiche lorsque
user clique sur le bouton "colors" present dans la boite de parametrage
de certains objets, cette nouvelle boite lui permettra de configurer la couleur du background
et du texte present dans l'objet qu'il souhaite creer
BY CHRISTIAN EDOUARDO le 21/09/2024 a 15h26*/

#include <windows.h>
#include "resource.h"
#include "OpenDialogBox.h"

LRESULT CALLBACK ColorProc(HWND hDlg,UINT msg, WPARAM wParam, LPARAM lParam);

int iDefColValue[50];
RECT rcCombo;

static BOOL isFirstTimecolor=TRUE;
MYCOLOROBJ mycolorobj[2];

static CHOOSECOLOR cc ;
static COLORREF crCustColors[16] ;

BOOL PopColor(HWND hwndOwner,COLORREF defClolor)
{

    cc.lStructSize = sizeof (CHOOSECOLOR) ;
    cc.hwndOwner = hwndOwner ;
    cc.hInstance = NULL ;

    //couleur par defaut au debut et choix selectionne par user au retour
    cc.rgbResult = defClolor;
    //add 16 colors personalisees
    cc.lpCustColors = crCustColors ;
    //RGBINIT permet dafficher la RgbResult selectionnee plus au comme couleur par defaut
    cc.Flags =CC_RGBINIT | CC_FULLOPEN ;
    cc.lCustData = 0 ;
    cc.lpfnHook = NULL ;
    cc.lpTemplateName = NULL ;
    //retourne 1 si une couleur a ete choisie et 0 sinon
    return ChooseColor (&cc) ;
}
BOOL MyManageColor(int idDlg,HWND hwndOwner)
{
    //Creation de la boite de dialogue "colors"
    //La procedure de cette boite se situe aussi dans ce File

    //Faire une copie de l'etat actuels des reglages de colors
    //car les donnees du cache changeront a chaque modif de User
    //il fdaut etre capable de restaurer, si user, choisit d'Cancel plus tard

    mycolorobj[BK]= mycache[idDlg].mycolorobj[BK];
    mycolorobj[TXT]=mycache[idDlg].mycolorobj[TXT];

     size_t cBytes,length;
    HGLOBAL hgbl;
    LPDLGTEMPLATE lpdt;
    LPDLGITEMTEMPLATE lpdit;
    LPWORD lpw;
    LPWSTR lpwsz;
    int iPosItemX,iPosItemY,cx,cy,x,y;
    RECT rcParent,rcDlg;
    int nbChild=0;
    wchar_t *wchaine,wszEdit[1024];
    char *name;
    HINSTANCE hinst;

    MY_DLGDATA infoDlg;
    int id=idDlg;//id du parent


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
    rcDlg.left=rcParent.left +(rcParent.right-rcParent.left)/2;
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

    wchaine=L"Colors Settings";
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
    // Define an Apply button.
    //-----------------------
    nbChild++;
    name="Apply";
    wchaine=L"Apply";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2; lpdit->cy = cyChar;
    lpdit->x = rcDlg.right-lpdit->cx-cxChar; lpdit->y = rcDlg.bottom-lpdit->cy-cyChar;
    iPosItemX=lpdit->x-cxChar;
    iPosItemY=lpdit->y;
    lpdit->id = ID_APPLY; // apply button identifier
    lpdit->style = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON|
    WS_TABSTOP|WS_GROUP|WS_DISABLED;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080; // Button class

    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    *lpw++ = 0; // No creation data


    //-----------------------
    // Define an Cancel button.
    //-----------------------
    nbChild++;
    name="Cancel";
    wchaine=L"Cancel";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2; lpdit->cy = cyChar;
    lpdit->x = iPosItemX-lpdit->cx; lpdit->y = iPosItemY;
    iPosItemX=lpdit->x-cxChar;
    iPosItemY=lpdit->y;
    lpdit->id = IDCANCEL;
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
    // Define an Retablir button.
    //-----------------------
    nbChild++;
    name="Set default";
    wchaine=L"Set default";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2; lpdit->cy = cyChar;
    lpdit->x = iPosItemX-lpdit->cx; lpdit->y = iPosItemY;
    iPosItemX=lpdit->x-cxChar;
    iPosItemY=lpdit->y;
    lpdit->id = ID_SET_DEFAULT;
    lpdit->style = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON|
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
    // Group qui encadre la zone de parametrage de couleur
    //-----------------------
    nbChild++;
    name="";
    wchaine=L"colors";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = rcDlg.right-2*cxChar; lpdit->cy = 12*cyChar;
    lpdit->x = cxChar; lpdit->y = cxChar;
    lpdit->id = IDC_STATIC_GROUP;
    lpdit->style =WS_CHILD | WS_VISIBLE|BS_GROUPBOX;
    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080; // Button class

    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    *lpw++ = 0; // No creation data



    //-----------------------
    // Group pour "BACKGOUND 'S COLOR"
    //-----------------------
    nbChild++;
    name="";
    wchaine=L"Background";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = rcDlg.right-4*cxChar; lpdit->cy = 5*cyChar;
    lpdit->x = 2*cxChar; lpdit->y = 2*cxChar;
    lpdit->id = IDC_STATIC_GROUP;
    lpdit->style =WS_CHILD | WS_VISIBLE|BS_GROUPBOX;
    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080; // Button class

    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    *lpw++ = 0; // No creation data


    //------------------
    //radio button default color in background group
    //--------------------
    nbChild++;
    name="Default colors: ";
    wchaine=L"Default colors:";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2; lpdit->cy = cyChar;
    lpdit->x = 3*cxChar; lpdit->y = 4*cxChar;
    lpdit->id = ID_DEFCOLOR_BK;
    lpdit->style = WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON|
    WS_TABSTOP|WS_GROUP;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080; // Button class

    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    *lpw++ = 0; // No creation data



    //------------------
    //radio button RGB color in background group
    //--------------------
    nbChild++;
    name="RGB colors: ";
    wchaine=L"RGB colors:";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2; lpdit->cy = cyChar;
    lpdit->x = 3*cxChar; lpdit->y = 8*cxChar;
    lpdit->id = ID_RGB_BK;
    lpdit->style = WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON|
    WS_TABSTOP;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080; // Button class

    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    *lpw++ = 0; // No creation data



    //------------------
    //Combobox ou user choisi default color
    //--------------------
    nbChild++;

    name="COLOR_3DDKSHADOWWWW";
    wchaine=L"";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2; lpdit->cy = 2*cyChar;
    lpdit->x = 3*cxChar+18*cxChar/2; lpdit->y = 4*cxChar;
    lpdit->id = ID_COMBO_BK;
    lpdit->style =WS_VISIBLE|WS_CHILD|WS_BORDER|WS_VSCROLL|
    CBS_SORT|CBS_HASSTRINGS|WS_DISABLED|CBS_DROPDOWNLIST;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0085; // combobox class

    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    *lpw++ = 0; // No creation data



    //------------------
    //static "R" du RGB value in background group
    //--------------------
    nbChild++;
    name="R:";
    wchaine=L"R:";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name))*cxChar/2; lpdit->cy = cyChar;
    lpdit->x = 12*cxChar; lpdit->y = 17*cxChar/2;
    lpdit->id = IDC_STATIC;
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
    //edit "R" associe a RGB de background
    //--------------------
    nbChild++;
    name="AB";
    wsprintfW(wszEdit,L"%d",mycache[idDlg].mycolorobj[BK].qr);
    wchaine=wszEdit;
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2+2; lpdit->cy = cyChar/2;
    lpdit->x = x+cx; lpdit->y = y;
    lpdit->id = ID_R_BK;
    lpdit->style = WS_CHILD | WS_VISIBLE|WS_DISABLED|
    ES_NUMBER;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0081; // static class

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
    //static "G" du RGB value in background group
    //--------------------
    nbChild++;
    name="G:";
    wchaine=L"G:";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name))*cxChar/2; lpdit->cy = cyChar;
    lpdit->x =x+cx; lpdit->y =y;
    lpdit->id = IDC_STATIC;
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
    //edit "G" associe a RGB de background
    //--------------------
    nbChild++;
    name="AB";
    wsprintfW(wszEdit,L"%d",mycache[idDlg].mycolorobj[BK].qg);
    wchaine=wszEdit;

    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2+2; lpdit->cy = cyChar/2;
    lpdit->x = x+cx; lpdit->y = y;
    lpdit->id = ID_G_BK;
    lpdit->style = WS_CHILD | WS_VISIBLE|WS_DISABLED|
    ES_NUMBER;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0081; // static class

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
    //static "B" du RGB value in background group
    //--------------------
    nbChild++;
    name="B:";
    wchaine=L"B:";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name))*cxChar/2; lpdit->cy = cyChar;
    lpdit->x =x+cx; lpdit->y =y;
    lpdit->id = IDC_STATIC;
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
    //edit "B" associe a RGB de background
    //--------------------
    nbChild++;
    name="AB";
    wsprintfW(wszEdit,L"%d",mycache[idDlg].mycolorobj[BK].qb);
    wchaine=wszEdit;

    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2+2; lpdit->cy = cyChar/2;
    lpdit->x = x+cx; lpdit->y = y;
    lpdit->id = ID_B_BK;
    lpdit->style = WS_CHILD | WS_VISIBLE|WS_DISABLED|
    ES_NUMBER;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0081; // EDIT class

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
    //bOUTTON ... POUR choisir dans une palette
    //--------------------
    nbChild++;
    name="...";
    wchaine=L"...";

    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2+2; lpdit->cy = cyChar;
    lpdit->x = x+cx+2; lpdit->y = y-cy/2;
    lpdit->id = ID_CHOOSE_CLR_BK;
    lpdit->style = WS_CHILD | WS_VISIBLE|BS_PUSHBUTTON|WS_DISABLED;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080; // BUTTON class

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
    // Group pour "TEXT 'S COLOR"
    //-----------------------
    nbChild++;
    name="";
    wchaine=L"Text";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = rcDlg.right-4*cxChar; lpdit->cy = 5*cyChar;
    lpdit->x = 2*cxChar; lpdit->y = 13*cxChar;
    lpdit->id = IDC_STATIC_GROUP;
    lpdit->style =WS_CHILD | WS_VISIBLE|BS_GROUPBOX;
    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080; // Button class

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
    //radio button default color in text group
    //--------------------
    nbChild++;
    name="Default colors: ";
    wchaine=L"Default colors:";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2; lpdit->cy = cyChar;
    lpdit->x = x+cxChar; lpdit->y = y+2*cxChar;
    lpdit->id = ID_DEFCOLOR_TXT;
    lpdit->style = WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON|
    WS_TABSTOP|WS_GROUP;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080; // Button class

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
    //radio button RGB color in TEXT group
    //--------------------
    nbChild++;
    name="RGB colors: ";
    wchaine=L"RGB colors:";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2; lpdit->cy = cyChar;
    lpdit->x = x; lpdit->y = y+4*cxChar;
    lpdit->id = ID_RGB_TXT;
    lpdit->style = WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON|
    WS_TABSTOP;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080; // Button class

    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    *lpw++ = 0; // No creation data


    //------------------
    //Combobox ou user choisi default color
    //--------------------
    nbChild++;

    name="COLOR_3DDKSHADOWWWW";
    wchaine=L"";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2; lpdit->cy = 2*cyChar;
    lpdit->x = x+18*cxChar/2; lpdit->y = y;
    lpdit->id = ID_COMBO_TXT;
    lpdit->style =WS_VISIBLE|WS_CHILD|WS_BORDER|WS_VSCROLL|
    CBS_SORT|CBS_HASSTRINGS|WS_DISABLED|CBS_DROPDOWNLIST;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0085; // combobox class

    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    *lpw++ = 0; // No creation data



    //------------------
    //static "R" du RGB value in TEXT group
    //--------------------
    nbChild++;
    name="R:";
    wchaine=L"R:";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name))*cxChar/2; lpdit->cy = cyChar;
    lpdit->x = 12*cxChar; lpdit->y = 39*cxChar/2;
    lpdit->id = IDC_STATIC;
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
    //edit "R" associe a RGB de text
    //--------------------
    nbChild++;
    name="AB";
    wsprintfW(wszEdit,L"%d",mycache[idDlg].mycolorobj[TXT].qr);
    wchaine=wszEdit;

    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2+2; lpdit->cy = cyChar/2;
    lpdit->x = x+cx; lpdit->y = y;
    lpdit->id = ID_R_TXT;
    lpdit->style = WS_CHILD | WS_VISIBLE|WS_DISABLED|
    ES_NUMBER;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0081; // static class

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
    //static "G" du RGB value in background group
    //--------------------
    nbChild++;
    name="G:";
    wchaine=L"G:";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name))*cxChar/2; lpdit->cy = cyChar;
    lpdit->x =x+cx; lpdit->y =y;
    lpdit->id = IDC_STATIC;
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
    //edit "G" associe a RGB de txt
    //--------------------
    nbChild++;
    name="AB";
    wsprintfW(wszEdit,L"%d",mycache[idDlg].mycolorobj[TXT].qg);
    wchaine=wszEdit;

    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2+2; lpdit->cy = cyChar/2;
    lpdit->x = x+cx; lpdit->y = y;
    lpdit->id = ID_G_TXT;
    lpdit->style = WS_CHILD | WS_VISIBLE|WS_DISABLED|
    ES_NUMBER;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0081; // static class

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
    //static "B" du RGB value in text group
    //--------------------
    nbChild++;
    name="B:";
    wchaine=L"B:";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name))*cxChar/2; lpdit->cy = cyChar;
    lpdit->x =x+cx; lpdit->y =y;
    lpdit->id = IDC_STATIC;
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
    //edit "B" associe a RGB de TEXT group
    //--------------------
    nbChild++;
    name="AB";
    wsprintfW(wszEdit,L"%d",mycache[idDlg].mycolorobj[TXT].qb);
    wchaine=wszEdit;

    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2+2; lpdit->cy = cyChar/2;
    lpdit->x = x+cx; lpdit->y = y;
    lpdit->id = ID_B_TXT;
    lpdit->style = WS_CHILD | WS_VISIBLE|WS_DISABLED|
    ES_NUMBER;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0081; // static class

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
    //bOUTTON ... POUR choisir dans une palette
    //--------------------
    nbChild++;
    name="...";
    wchaine=L"...";

    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2+2; lpdit->cy = cyChar;
    lpdit->x = x+cx+2; lpdit->y = y-cy/2;
    lpdit->id = ID_CHOOSE_CLR_TXT;
    lpdit->style = WS_CHILD | WS_VISIBLE|BS_PUSHBUTTON|WS_DISABLED;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080; // BUTTON class

    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    *lpw++ = 0; // No creation data
    cx=lpdit->cx;
    cy=lpdit->cy;
    x=lpdit->x;
    y=lpdit->y;


    //Set nombre de childs control a creer
    lpdt->cdit =nbChild;// Number of controls
    GlobalUnlock(hgbl);


    lpdt->style=WS_BORDER|WS_SYSMENU|
        WS_CAPTION|WS_POPUP|DS_MODALFRAME;
    DialogBoxIndirectParam(hinst,(LPDLGTEMPLATE)hgbl,
    hwndOwner,(DLGPROC)ColorProc,(LPARAM)&infoDlg);


    GlobalFree(hgbl);

    return TRUE;
}

LRESULT CALLBACK ColorProc(HWND hDlg,UINT msg, WPARAM wParam, LPARAM lParam)
{
    static MY_DLGDATA *infoDlg;
    static int idDlg=-1;
    //Recuperer les caracteristiques de la boite de dialogue,
    //NB: le champs id, contient l'id de la boite de dialogue hote
    if(msg==WM_INITDIALOG)
    {
        infoDlg=(MY_DLGDATA*)lParam;
        idDlg=infoDlg->idDlg;

    }
    if(idDlg==-1) return FALSE;
    static HWND hDlgParent;
    static BOOL isApplyOn=FALSE;
    switch(msg)
    {
    case WM_INITDIALOG:

        //Charge les donnes de colors du cache sur la fenetre de couleur
        //nouvellement cree
        hDlgParent=GetParent(hDlg);

        MyLoadColorObj(hDlg,infoDlg);
        return TRUE;//force le systeme a atrribuer le focus

    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case ID_CHOOSE_CLR_BK:
            {
                if(PopColor(hDlg,mycache[idDlg].mycolorobj[BK].rgbColor))
                {
                    char szQ[4];
                    HWND hwndtarget;

                    //Set qr
                    hwndtarget=GetDlgItem(hDlg,ID_R_BK);
                    wsprintf(szQ,"%d",GetRValue(cc.rgbResult));
                    SetWindowText(hwndtarget,szQ);
                    //Set qg
                    hwndtarget=GetDlgItem(hDlg,ID_G_BK);
                    wsprintf(szQ,"%d",GetGValue(cc.rgbResult));
                    SetWindowText(hwndtarget,szQ);
                    //Set qB
                    hwndtarget=GetDlgItem(hDlg,ID_B_BK);
                    wsprintf(szQ,"%d",GetBValue(cc.rgbResult));
                    SetWindowText(hwndtarget,szQ);

                    SendMessage(hDlg,WM_COMMAND,
                        MAKELONG(ID_B_BK,EN_CHANGE),
                            (LPARAM)hwndtarget);
                }
            }
            return TRUE;
        case ID_RGB_BK:
            mycache[idDlg].mycolorobj[BK].isDefChoosen=FALSE;
            EnableWindow(GetDlgItem(hDlg,ID_CHOOSE_CLR_BK),TRUE);
            EnableWindow(GetDlgItem(hDlg,ID_R_BK),TRUE);
            EnableWindow(GetDlgItem(hDlg,ID_G_BK),TRUE);
            EnableWindow(GetDlgItem(hDlg,ID_B_BK),TRUE);
            EnableWindow(GetDlgItem(hDlg,ID_COMBO_BK),FALSE);
            MyLoadColorObj(hDlg,infoDlg);
            //Activer le bouton Apply en cas de changement
            if(mycolorobj[BK].rgbColor!=
                    mycache[idDlg].mycolorobj[BK].rgbColor||
               mycolorobj[BK].isDefChoosen!=
                    mycache[idDlg].mycolorobj[BK].isDefChoosen)
            {
                if(isApplyOn==FALSE )
                {
                    EnableWindow(GetDlgItem(hDlg,ID_APPLY),TRUE);
                    isApplyOn=TRUE;
                }
            }
            else
            {
                EnableWindow(GetDlgItem(hDlg,ID_APPLY),FALSE);
                isApplyOn=FALSE;
            }

            return TRUE;


        case ID_CHOOSE_CLR_TXT:
            {
                if(PopColor(hDlg,mycache[idDlg].mycolorobj[TXT].rgbColor))
                {
                    char szQ[4];
                    HWND hwndtarget;

                    //Set qr
                    hwndtarget=GetDlgItem(hDlg,ID_R_TXT);
                    wsprintf(szQ,"%d",GetRValue(cc.rgbResult));
                    SetWindowText(hwndtarget,szQ);
                    //Set qg
                    hwndtarget=GetDlgItem(hDlg,ID_G_TXT);
                    wsprintf(szQ,"%d",GetGValue(cc.rgbResult));
                    SetWindowText(hwndtarget,szQ);
                    //Set qB
                    hwndtarget=GetDlgItem(hDlg,ID_B_TXT);
                    wsprintf(szQ,"%d",GetBValue(cc.rgbResult));
                    SetWindowText(hwndtarget,szQ);

                    SendMessage(hDlg,WM_COMMAND,
                        MAKELONG(ID_B_TXT,EN_CHANGE),
                            (LPARAM)hwndtarget);
                }
            }
            return TRUE;

         case ID_RGB_TXT:
            mycache[idDlg].mycolorobj[TXT].isDefChoosen=FALSE;
            EnableWindow(GetDlgItem(hDlg,ID_CHOOSE_CLR_TXT),TRUE);
            EnableWindow(GetDlgItem(hDlg,ID_R_TXT),TRUE);
            EnableWindow(GetDlgItem(hDlg,ID_G_TXT),TRUE);
            EnableWindow(GetDlgItem(hDlg,ID_B_TXT),TRUE);
            EnableWindow(GetDlgItem(hDlg,ID_COMBO_TXT),FALSE);
            MyLoadColorObj(hDlg,infoDlg);

            //Activer le bouton Apply en cas de changement
            if(mycolorobj[TXT].rgbColor!=
                    mycache[idDlg].mycolorobj[TXT].rgbColor||
               mycolorobj[TXT].isDefChoosen!=
                    mycache[idDlg].mycolorobj[TXT].isDefChoosen)
            {
                if(isApplyOn==FALSE )
                {
                    EnableWindow(GetDlgItem(hDlg,ID_APPLY),TRUE);
                    isApplyOn=TRUE;
                }
            }
            else
            {
                EnableWindow(GetDlgItem(hDlg,ID_APPLY),FALSE);
                isApplyOn=FALSE;
            }

            return TRUE;

        case ID_DEFCOLOR_BK:
            mycache[idDlg].mycolorobj[BK].isDefChoosen=TRUE;
            EnableWindow(GetDlgItem(hDlg,ID_COMBO_BK),TRUE);
            //mise a jour imediate
            //InvalidateRect(GetDlgItem(hwndAspect,1),NULL,TRUE);
            MyRefreshAspect(hDlgParent,idDlg);

            EnableWindow(GetDlgItem(hDlg,ID_CHOOSE_CLR_BK),FALSE);
            EnableWindow(GetDlgItem(hDlg,ID_R_BK),FALSE);
            EnableWindow(GetDlgItem(hDlg,ID_G_BK),FALSE);
            EnableWindow(GetDlgItem(hDlg,ID_B_BK),FALSE);

            MyLoadColorObj(hDlg,infoDlg);

            //mise a jour imediate
            //InvalidateRect(GetDlgItem(hwndAspect,1),NULL,TRUE);
            MyRefreshAspect(hDlgParent,idDlg);

            //Activer le bouton Apply en cas de changement
            if(mycolorobj[BK].iIndexDefColor!=
                    mycache[idDlg].mycolorobj[BK].iIndexDefColor||
               mycolorobj[BK].isDefChoosen!=
                    mycache[idDlg].mycolorobj[BK].isDefChoosen)
            {
                if(isApplyOn==FALSE )
                {
                    EnableWindow(GetDlgItem(hDlg,ID_APPLY),TRUE);
                    isApplyOn=TRUE;
                }
            }
            else
            {
                EnableWindow(GetDlgItem(hDlg,ID_APPLY),FALSE);
                isApplyOn=FALSE;
            }


            return TRUE;

        case ID_DEFCOLOR_TXT:
            mycache[idDlg].mycolorobj[TXT].isDefChoosen=TRUE;
            EnableWindow(GetDlgItem(hDlg,ID_COMBO_TXT),TRUE);
            //mise a jour imediate
            //InvalidateRect(GetDlgItem(hwndAspect,1),NULL,TRUE);
            MyRefreshAspect(hDlgParent,idDlg);

            EnableWindow(GetDlgItem(hDlg,ID_CHOOSE_CLR_TXT),FALSE);
            EnableWindow(GetDlgItem(hDlg,ID_R_TXT),FALSE);
            EnableWindow(GetDlgItem(hDlg,ID_G_TXT),FALSE);
            EnableWindow(GetDlgItem(hDlg,ID_B_TXT),FALSE);
            MyLoadColorObj(hDlg,infoDlg);

            //Activer le bouton Apply en cas de changement
            if(mycolorobj[TXT].iIndexDefColor!=
                    mycache[idDlg].mycolorobj[TXT].iIndexDefColor||
               mycolorobj[TXT].isDefChoosen!=
                    mycache[idDlg].mycolorobj[TXT].isDefChoosen)
            {
                    if(isApplyOn==FALSE )
                {
                    EnableWindow(GetDlgItem(hDlg,ID_APPLY),TRUE);
                    isApplyOn=TRUE;
                }
            }
            else
            {
                EnableWindow(GetDlgItem(hDlg,ID_APPLY),FALSE);
                isApplyOn=FALSE;
            }

            return TRUE;

        case ID_COMBO_TXT:
        case ID_COMBO_BK:
        {
            int iTypeCombo=(LOWORD(wParam)==ID_COMBO_TXT?
                        TXT:BK);

            int iIndex;//index dans la comboBox
            //Retire l'index ou user a clique
            iIndex = SendMessage ((HWND)lParam, CB_GETCURSEL, 0, 0) ;
            //
            mycache[idDlg].mycolorobj[iTypeCombo].defColor=iDefColValue[iIndex];
            mycache[idDlg].mycolorobj[iTypeCombo].iIndexDefColor=iIndex;
            //mise a jour imediate
            //InvalidateRect(GetDlgItem(hwndAspect,1),NULL,TRUE);
            MyRefreshAspect(hDlgParent,idDlg);

            //Activer le bouton Apply en cas de changement
            if(mycolorobj[iTypeCombo].iIndexDefColor!=
                    mycache[idDlg].mycolorobj[iTypeCombo].iIndexDefColor||
               mycolorobj[iTypeCombo].isDefChoosen!=
                    mycache[idDlg].mycolorobj[iTypeCombo].isDefChoosen)
            {
                    if(isApplyOn==FALSE )
                {
                    EnableWindow(GetDlgItem(hDlg,ID_APPLY),TRUE);
                    isApplyOn=TRUE;
                }
            }
            else
            {
                EnableWindow(GetDlgItem(hDlg,ID_APPLY),FALSE);
                isApplyOn=FALSE;
            }
        }
            return TRUE;

        case ID_B_BK:
        case ID_G_BK:
        case ID_R_BK:
            {

                TCHAR buf[1024];
                int *qt=NULL,qr,qg,qb,idBK;

                idBK=LOWORD(wParam);
                if(HIWORD(wParam)==EN_CHANGE)
                {
                    if(idBK==ID_R_BK)
                        qt=&mycache[idDlg].mycolorobj[BK].qr;
                    else if(idBK==ID_G_BK)
                        qt=&mycache[idDlg].mycolorobj[BK].qg;
                    else if(idBK==ID_B_BK)
                        qt=&mycache[idDlg].mycolorobj[BK].qb;

                    if(!qt)
                    {
                        MyPerror("mvklss");
                        MyMsgError();
                        exit(EXIT_FAILURE);
                    }
                    GetWindowText((HWND)lParam,buf,1024);
                    sscanf(buf,"%d",qt);
                    qr=mycache[idDlg].mycolorobj[BK].qr;
                    qg=mycache[idDlg].mycolorobj[BK].qg;
                    qb=mycache[idDlg].mycolorobj[BK].qb;
                    mycache[idDlg].mycolorobj[BK].rgbColor=RGB(qr,qg,qb);

                    //mise a jour imediate
                    //InvalidateRect(GetDlgItem(hwndAspect,1),NULL,TRUE);
                    MyRefreshAspect(hDlgParent,idDlg);
                    MyRefreshAspect(hDlgParent,idDlg);

                    //Activer le bouton Apply en cas de changement
                    if(mycolorobj[BK].rgbColor!=
                        mycache[idDlg].mycolorobj[BK].rgbColor||
                        mycolorobj[BK].isDefChoosen!=
                        mycache[idDlg].mycolorobj[BK].isDefChoosen)
                    {
                        if(isApplyOn==FALSE )
                        {
                            EnableWindow(GetDlgItem(hDlg,ID_APPLY),TRUE);
                            isApplyOn=TRUE;
                        }
                    }
                    else
                    {
                        EnableWindow(GetDlgItem(hDlg,ID_APPLY),FALSE);
                        isApplyOn=FALSE;
                    }
                }
            }
            return TRUE;

        case ID_B_TXT:
        case ID_G_TXT:
        case ID_R_TXT:
            {
                TCHAR buf[1024];
                int *qt=NULL,qr,qg,qb,idTXT;


                idTXT=LOWORD(wParam);
                if(HIWORD(wParam)==EN_CHANGE)
                {
                    if(idTXT==ID_R_TXT)
                        qt=&mycache[idDlg].mycolorobj[TXT].qr;
                    else if(idTXT==ID_G_TXT)
                        qt=&mycache[idDlg].mycolorobj[TXT].qg;
                    else if(idTXT==ID_B_TXT)
                        qt=&mycache[idDlg].mycolorobj[TXT].qb;

                    if(!qt)
                    {
                        MyPerror("vkldsklf");
                        MyMsgError();
                        exit(EXIT_FAILURE);
                    }
                    GetWindowText((HWND)lParam,buf,1024);
                    sscanf(buf,"%d",qt);
                    qr=mycache[idDlg].mycolorobj[TXT].qr;
                    qg=mycache[idDlg].mycolorobj[TXT].qg;
                    qb=mycache[idDlg].mycolorobj[TXT].qb;
                    mycache[idDlg].mycolorobj[TXT].rgbColor=RGB(qr,qg,qb);
                    //mise a jour imediate
                    //InvalidateRect(GetDlgItem(hwndAspect,1),NULL,TRUE);
                    MyRefreshAspect(hDlgParent,idDlg);

                    //Activer le bouton Apply en cas de changement
                    if(mycolorobj[TXT].rgbColor!=
                        mycache[idDlg].mycolorobj[TXT].rgbColor||
                        mycolorobj[TXT].isDefChoosen!=
                        mycache[idDlg].mycolorobj[TXT].isDefChoosen)
                    {
                        if(isApplyOn==FALSE )
                        {
                            EnableWindow(GetDlgItem(hDlg,ID_APPLY),TRUE);
                            isApplyOn=TRUE;
                        }
                    }
                    else
                    {
                        EnableWindow(GetDlgItem(hDlg,ID_APPLY),FALSE);
                        isApplyOn=FALSE;
                    }
                }
            }
            return TRUE;


        case IDCANCEL:
            //Restaurer les anciennes valeurs des colors
            //Si User ne souhaite rien Apply
            mycache[idDlg].mycolorobj[BK]=mycolorobj[BK];
            mycache[idDlg].mycolorobj[TXT]=mycolorobj[TXT];
            //mise a jour imediate
            //InvalidateRect(GetDlgItem(hwndAspect,1),NULL,TRUE);
            MyRefreshAspect(hDlgParent,idDlg);
            MyRefreshAspect(hDlgParent,idDlg);

            //faLL THROUGH
        case ID_APPLY:
            idDlg=-1;
            isFirstTimecolor=TRUE;
            EndDialog(hDlg,0);
            return TRUE;

        case ID_SET_DEFAULT:
            //User souhaite retablir les valeurs par defaut
            mycache[idDlg].mycolorobj[BK].iIndexDefColor=1;//BK_PARENT
            mycache[idDlg].mycolorobj[TXT].iIndexDefColor=0;//TXT_PARENT
            mycache[idDlg].mycolorobj[BK].isDefChoosen=TRUE;
            mycache[idDlg].mycolorobj[TXT].isDefChoosen=TRUE;
            mycache[idDlg].mycolorobj[BK].defColor=COLOR_BK_PARENT;
            mycache[idDlg].mycolorobj[TXT].defColor=COLOR_TXT_PARENT;
            isFirstTimecolor=TRUE;
            MyLoadColorObj(hDlg,infoDlg);
            //InvalidateRect(hDlg,NULL,TRUE);
            MyRefreshAspect(hDlgParent,idDlg);

            //Activer le bouton Apply
            EnableWindow(GetDlgItem(hDlg,ID_APPLY),TRUE);
            isApplyOn=TRUE;
            return TRUE;

        }
        break;
    }
    return FALSE;
}

BOOL MyLoadColorObj(HWND hDlg,MY_DLGDATA *infoDlg)
{
    //cette fonction rempli la boite couleur avec des infromations
    //precedemment enregistres par User, ou par le programme, s'il s'a
    //git du tout premier chargement.Ces infos sont situes dans la
    //structure mycache.mycolorobj
    if(!hDlg)
        return FALSE;
    HWND hwndCombo,hwndEdit;
    int idDlg,iIndexColor;
    int qr,qg,qb;
    BOOL isDefChoosen;
    TCHAR buf[1024];
    static BOOL isFirstTimeBK=TRUE;//y'en a deux pour BK et TXT
    static BOOL isFirstTimeTXT=TRUE;//y'en a deux pour BK et TXT

    //hDlg color est le fils de la boite de dialogue qui
    //l'engendre
    idDlg=infoDlg->idDlg;



    //--------------------------
    //Chargement de la section BK
    //----------------------------
    if(isFirstTimecolor)
    {
        isFirstTimecolor=FALSE;
        isFirstTimeBK=TRUE;
        isFirstTimeTXT=TRUE;
    }
    hwndCombo=GetDlgItem(hDlg,ID_COMBO_BK);

    isDefChoosen=mycache[idDlg].mycolorobj[BK].isDefChoosen;
    if(isDefChoosen==TRUE)
    {
        //Cocher le radio button "def color"
        //definir l'actuelle couleur sur iIndexColor
        //charger la combobox en contenu
        //iIndexDefColor n'est disponible que lorsque User choisi Default color

        //si user choisi l'option Default
    /*    //ajoute la liste deroulante du combo de "Default colors"
        GetWindowRect(hwndCombo,&rcCombo);
        MyScreenRcToClientRc(hDlg,&rcCombo);
        MoveWindow(hwndCombo,rcCombo.left,rcCombo.top,
                   rcCombo.right-rcCombo.left,
              2*cyChar,TRUE);
*/
        if(isFirstTimeBK==TRUE)
        {
            isFirstTimeBK=FALSE;

            CheckRadioButton(hDlg,ID_DEFCOLOR_BK,ID_RGB_BK,ID_DEFCOLOR_BK);
            SendMessage(hDlg,WM_COMMAND,
                        MAKELONG(ID_DEFCOLOR_BK,BN_CLICKED),
                        (LPARAM)GetDlgItem(hDlg,ID_DEFCOLOR_BK));
        }
        iIndexColor=mycache[idDlg].mycolorobj[BK].iIndexDefColor;
        MyFillComboColor(hwndCombo,iIndexColor);
    }
    else if(isDefChoosen==FALSE)
    {
        //si user choisi l'option RGB
        /*//Supprime la liste deroulante du combo de "Default colors"
        GetWindowRect(hwndCombo,&rcCombo);
        MyScreenRcToClientRc(hDlg,&rcCombo);
        MoveWindow(hwndCombo,rcCombo.left,rcCombo.top,
                   rcCombo.right-rcCombo.left,
                   cyChar,TRUE);
*/
        if(isFirstTimeBK==TRUE)
        {
            isFirstTimeBK=FALSE;
            CheckRadioButton(hDlg,ID_DEFCOLOR_BK,ID_RGB_BK,ID_RGB_BK);

            SendMessage(hDlg,WM_COMMAND,
                        MAKELONG(ID_RGB_BK,BN_CLICKED),
                        (LPARAM)GetDlgItem(hDlg,ID_RGB_BK));
        }
        //qr,qv,qb, ne sont utilisables que lorsque user choisi l'option RGB
        qr=mycache[idDlg].mycolorobj[BK].qr;
        qb=mycache[idDlg].mycolorobj[BK].qb;
        qg=mycache[idDlg].mycolorobj[BK].qg;

        //modifier le Title de chaque edit
            //R
        hwndEdit=GetDlgItem(hDlg,ID_R_BK);
        wsprintf(buf,"%d",qr);
        SetWindowText(hwndEdit,buf);
           //G
        hwndEdit=GetDlgItem(hDlg,ID_G_BK);
        wsprintf(buf,"%d",qg);
        SetWindowText(hwndEdit,buf);
           //B
        hwndEdit=GetDlgItem(hDlg,ID_B_BK);
        wsprintf(buf,"%d",qb);
        SetWindowText(hwndEdit,buf);
    }


    //--------------------------
    //Chargement de la section TXT
    //-----------------------------
    hwndCombo=GetDlgItem(hDlg,ID_COMBO_TXT);

    isDefChoosen=mycache[idDlg].mycolorobj[TXT].isDefChoosen;
    if(isDefChoosen==TRUE)
    {
        //Cocher le radio button "def color"
        //definir l'actuelle couleur sur iIndexColor
        //charger la combobox en contenu
        //iIndexDefColor n'est disponible que lorsque User choisi Default color
        //si user choisi l'option Default
        //ajoute la liste deroulante du combo de "Default colors"

        if(isFirstTimeTXT==TRUE)
        {
            isFirstTimeTXT=FALSE;
            CheckRadioButton(hDlg,ID_DEFCOLOR_TXT,ID_RGB_TXT,ID_DEFCOLOR_TXT);
            SendMessage(hDlg,WM_COMMAND,
                        MAKELONG(ID_DEFCOLOR_TXT,BN_CLICKED),
                        (LPARAM)GetDlgItem(hDlg,ID_DEFCOLOR_TXT));
        }
        iIndexColor=mycache[idDlg].mycolorobj[TXT].iIndexDefColor;
        MyFillComboColor(hwndCombo,iIndexColor);
    }
    else if(isDefChoosen==FALSE)
    {
        //si user choisi l'option RGB
        if(isFirstTimeTXT==TRUE)
        {
            isFirstTimeTXT=FALSE;
            CheckRadioButton(hDlg,ID_DEFCOLOR_TXT,ID_RGB_TXT,ID_RGB_TXT);

            SendMessage(hDlg,WM_COMMAND,
                        MAKELONG(ID_RGB_TXT,BN_CLICKED),
                        (LPARAM)GetDlgItem(hDlg,ID_RGB_TXT));
        }
        //qr,qv,qb, ne sont utilisables que lorsque user choisi l'option RGB
        qr=mycache[idDlg].mycolorobj[TXT].qr;
        qb=mycache[idDlg].mycolorobj[TXT].qb;
        qg=mycache[idDlg].mycolorobj[TXT].qg;
        //modifier le Title de chaque edit
            //R
        hwndEdit=GetDlgItem(hDlg,ID_R_TXT);
        wsprintf(buf,"%d",qr);
        SetWindowText(hwndEdit,buf);
           //G
        hwndEdit=GetDlgItem(hDlg,ID_G_TXT);
        wsprintf(buf,"%d",qg);
        SetWindowText(hwndEdit,buf);
           //B
        hwndEdit=GetDlgItem(hDlg,ID_B_TXT);
        wsprintf(buf,"%d",qb);
        SetWindowText(hwndEdit,buf);
    }
    return TRUE;
}

BOOL MyFillComboColor(HWND hwndCombo,int iIndex)
{
    //cette fonction rempli hwndCombo, avec les colors
    //iIndex, cest l'index du texte a definir comme Title du combobox
    FILE *File;
    char buf[1024];
    char *p,*c;
    int i;

    HRSRC hResInfo=FindResource((HINSTANCE)GetWindowLongPtr(hwndZC,GWLP_HINSTANCE),
        "GCO_DEFCOL","TEXT");
    TCHAR szNameFile[1024]="";
    MyCreateTempTxtFile(hResInfo,szNameFile);
    File=fopen(szNameFile,"r");

    if(!File)
    {
        MyPerror("GDSLKGK");
        MyMsgError();
        exit(EXIT_FAILURE);
    }

    //Vider le contenu de la combo
    SendMessage(hwndCombo,CB_RESETCONTENT,0,0);

    i=0;
    while(fgets(buf,1024,File))
    {
        p=strtok(buf," ");
        //Recuperer la valeur int de chaque index i
        c=strtok(NULL," ");
        sscanf(c,"%d",iDefColValue+i);
        SendMessage(hwndCombo,CB_INSERTSTRING,i,(LPARAM)p);

        i++;
    }
    fclose(File);
    //REMOVE
    remove(szNameFile);

    //Set le Title par defaut
    SendMessage(hwndCombo,CB_SETCURSEL,iIndex,0);
    return TRUE;
}

BOOL MyScreenRcToClientRc(HWND hwnd,LPRECT rc)
{
    //cette fonction traduire les coordonnees de rc qui sont des
    //coordonnees decran en coordonnees, clientes du hwnd specifie
    if(!hwnd) return FALSE;
    POINT ptBeg;
    POINT ptEnd;

    ptBeg.x=rc->left;
    ptBeg.y=rc->top;
    ptEnd.x=rc->right;
    ptEnd.y=rc->bottom;

    ScreenToClient(hwnd,&ptBeg);
    ScreenToClient(hwnd,&ptEnd);

    rc->left=ptBeg.x;
    rc->top=ptBeg.y;
    rc->right=ptEnd.x;
    rc->bottom=ptEnd.y;

    return TRUE;
}
COLORREF MyGetSysColor(MYCOLOROBJ colorobj, HWND hwndParentObj)
{
    //Cette fonction prend en entree la strucutre MYCOLOROBJ
    //et retourne la couleur choisie .
    COLORREF couleur;
    HWND hwndParentTemp;
    if(colorobj.isDefChoosen)
    {
        if(colorobj.defColor==COLOR_BK_PARENT)
        {
            int iIndex=MyGetIindexObjWithHwnd(hwndParentObj);
            if(iIndex==-1)
            {
                MyPerror("gfddfsise");
                exit(EXIT_FAILURE);
            }

            hwndParentTemp=uObjet[iIndex].hwndParent;
            iIndex=MyGetIindexObjWithHwnd(hwndParentTemp);
            if(iIndex==-1)
            {
                //testChildGroupBox
                if(isChildGB(hwndParentTemp))
                {
                    hwndParentTemp=GetParent(hwndParentTemp);
                    if(!hwndParentTemp)
                    {
                        MyPerror("\n error unKnown ManageColr ;!hwndParentTemp");
                        return 0;
                    }
                    iIndex=MyGetIindexObjWithHwnd(hwndParentTemp);
                    if(iIndex==-1)
                    {
                        MyPerror("\n error unKnown ManageColr ;!hwndParentTemp");
                        return 0;
                    }
                    //iIndex pointe sur le GB
                    return MyGetSysColor(uObjet[iIndex].mycolorobj[BK],uObjet[iIndex].hwnd);
                }
            }

            if(iIndex==-1)
            {

                 PAINTSTRUCT ps;
                HDC hdc=BeginPaint(GetParent(hwndZC),&ps);
                couleur=GetBkColor(hdc);
                EndPaint(GetParent(hwndZC),&ps);
            }
            else
            {
                couleur=MyGetSysColor(uObjet[iIndex].mycolorobj[BK],uObjet[iIndex].hwnd);
            }

        }
        else if(colorobj.defColor==COLOR_TXT_PARENT)
        {
            int iIndex=MyGetIindexObjWithHwnd(hwndParentObj);
            if(iIndex==-1)
            {
                MyPerror("fdgodogp");
                exit(EXIT_FAILURE);
            }

            hwndParentTemp=uObjet[iIndex].hwndParent;
            iIndex=MyGetIindexObjWithHwnd(hwndParentTemp);
            if(iIndex==-1)
            {
                //testChildGroupBox
                if(isChildGB(hwndParentTemp))
                {
                    hwndParentTemp=GetParent(hwndParentTemp);
                    if(!hwndParentTemp)
                    {
                        MyPerror("\n error unKnown ManageColr ;!hwndParentTemp");
                        return 0;
                    }
                    iIndex=MyGetIindexObjWithHwnd(hwndParentTemp);
                    if(iIndex==-1)
                    {
                        MyPerror("\n error unKnown ManageColr ;!hwndParentTemp");
                        return 0;
                    }
                    //iIndex pointe sur le GB
                    return MyGetSysColor(uObjet[iIndex].mycolorobj[TXT],uObjet[iIndex].hwnd);
                }
            }

            if(iIndex==-1)
            {
                 PAINTSTRUCT ps;
                HDC hdc=BeginPaint(GetParent(hwndZC),&ps);
                couleur=GetTextColor(hdc);
                EndPaint(GetParent(hwndZC),&ps);
            }
            else
            {
                couleur=MyGetSysColor(uObjet[iIndex].mycolorobj[TXT],uObjet[iIndex].hwnd);
            }
        }
        else
            couleur=GetSysColor(colorobj.defColor);
    }
    else
    {
        couleur=colorobj.rgbColor;
    }
    return couleur;
}
BOOL MySetColor(MYCOLOROBJ* colorobj,COLORREF couleur)
{
    colorobj->isDefChoosen=FALSE;
    colorobj->rgbColor=couleur;
    return TRUE;
}
