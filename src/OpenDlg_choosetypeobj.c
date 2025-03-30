/*Nom de File:OpenDlg_choosetypeobj.c
Role:File qui gere la creation de la boite de dialogue qui
permet a user de faire un choix sur la nature de l'objet qu'il
souhaite creer
BY CHRISTIAN EDOUARDO LE 21/09/2024 a 13h58*/

#include <windows.h>
#include "resource.h"
#include "OpenDialogBox.h"

BOOL OpenDlg_choosetypeobj(int iType,HWND hwndOwner,HINSTANCE hinst)
{
    size_t cBytes,length;
    HGLOBAL hgbl;
    LPDLGTEMPLATE lpdt;
    LPDLGITEMTEMPLATE lpdit;
    LPWORD lpw;
    LPWSTR lpwsz;
    int i,iPosItemX,iPosItemY;
    RECT rc,rcDlg,rcScreen;
    int nbChild=0;
    wchar_t *wchaine;
    char *name;
    wchar_t *wnameChild[]={L"Button",L"ListBox",
        L"EditBox",L"DialogBox",L"ComboBox",L"ImageStatic",L"Static"};
    int nbName=sizeof(wnameChild)/sizeof(wnameChild[0]);

    //informations a passer a la procedure de fenetre via lparma du inidialog
    MY_DLGDATA infoDlg;
    int id=DLG_CHOOSETYPEOBJET;
    infoDlg.idDlg=id;
    infoDlg.iTypeDlg=iType;
    infoDlg.hWndOwner=hwndOwner;
    infoDlg.hinst=hinst;


    //espace necessaire a allouer pour une boite de  child 9 controls
    cBytes=sizeof(DLGTEMPLATE)+(3*MAXDLGCHAR)+
                9*(sizeof(DLGITEMTEMPLATE)+(2*MAXDLGCHAR));
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
    rcScreen.left=0;
    rcScreen.top=0;
    rcScreen.right=GetSystemMetrics(SM_CXSCREEN)/2;
    rcScreen.bottom=GetSystemMetrics(SM_CYSCREEN)/2;

    //After :CONVERTIR rcScreen en unite de boite de dialogue/2

    rcDlg.left=0;
    rcDlg.top=0;
    rcDlg.bottom=75*(rcScreen.bottom)/100;
    rcDlg.right=70*(rcScreen.right)/100;
    rc.right=rcScreen.right/2;
    rc.bottom=rcScreen.bottom/2;
    rc.left=rcDlg.right/2;
    rc.top=rcDlg.bottom/2;
    //faire rc.right pointer sur la largeur pareil pour rc.bottom sur la hauteur
    OffsetRect(&rc,-rc.left,-rc.top);
    //centrer rcDlg
    OffsetRect(&rcDlg,rc.right,0);

    lpdt=(LPDLGTEMPLATE)GlobalLock(hgbl);
    lpdt=AlignLpdt(lpdt);
    lpdt->x = rcDlg.left; lpdt->y = rcDlg.top;
    lpdt->cx = rcDlg.right-rcDlg.left;
    lpdt->cy = rcDlg.bottom-rcDlg.top;
    //conversion WORD Boundaries
    lpw = (LPWORD)(lpdt + 1);
    *lpw++ = 0; // No menu
    *lpw++ = 0; // Predefined dialog box class (by default)

    wchaine=L"Choose Type's object";
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
    // Define an static text'sGroup item.
    //-----------------------
    nbChild++;
    name="Choose object to create";
    wchaine=L"Choose object to create";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = rcDlg.right-2*cxChar; lpdit->cy = 12*cyChar;
    lpdit->x = cxChar; lpdit->y = 5;
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

    //Create check boxes
    for( i=0;i<nbName;i++)
    {
        nbChild++;
        lpdit = AlignLpdit(lpw);//aligne sur DWORD
        lpdit->cx = rcDlg.right-5*cxChar; lpdit->cy = cyChar;
        lpdit->x = 2*cxChar; lpdit->y =(10*i)+(20)+i*lpdit->cy;

        lpdit->id = ID_BUTTON+i; // button identifier
        lpdit->style = WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON|
        WS_TABSTOP;
        if(i==0) lpdit->style|=WS_GROUP;
        lpw = (LPWORD)(lpdit + 1);

        *lpw++ = 0xFFFF;
        *lpw++ = 0x0080; // Button class
        length=wcslen(wnameChild[i])+1;//+1 pour le nul;
        lpwsz=(LPWSTR)lpw;
        memcpy(lpw,wnameChild[i],sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
        lpw+=length;
        *lpw++ = 0; // No creation data
    }

    //-----------------------
    // Define an suiv button.
    //-----------------------
    nbChild++;
    name="Next";
    wchaine=L"Next";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2; lpdit->cy = cyChar;
    lpdit->x = rcDlg.right-lpdit->cx-cxChar; lpdit->y = rcDlg.bottom-lpdit->cy-cyChar;
    iPosItemX=lpdit->x-cxChar;
    iPosItemY=lpdit->y;
    lpdit->id = ID_SUIV; // Next button identifier
    lpdit->style = WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON|
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
    lpdit->id = IDCANCEL; // CANCEL'S button identifier
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


    //Set nombre de childs control a creer
    lpdt->cdit =nbChild;// Number of controls
    GlobalUnlock(hgbl);
    if(iType==MODAL)
    {
        lpdt->style=WS_BORDER|WS_SYSMENU|
            WS_CAPTION|WS_POPUP|DS_MODALFRAME;
        DialogBoxIndirectParam(hinst,(LPDLGTEMPLATE)hgbl,
                        hwndOwner,(DLGPROC)DialogProc,(LPARAM)&infoDlg);
    }
    else if(iType==MODELESS)
    {
        lpdt->style=WS_BORDER|WS_SYSMENU|
            WS_CAPTION|WS_POPUP|WS_VISIBLE;

        if(!hDlgModeless)
        {
            //Si on a pas encore cree de tableau qui stocke la liste des handles
            //de boites modeles crees
            //On fait une allocation, et on initialise les champs de la structure
            hDlgModeless=malloc(sizeof(HWNDMODELESS)*1);
            if(!hDlgModeless)
            {
                MessageBox(hwndOwner,TEXT("Error in allocating memory for hModeless"),
                            NULL,MB_ICONERROR);
                return FALSE;
            }
            //Initialisation
            hDlgModeless[0].nbHwndOpen=0;
            hDlgModeless[0].newHwnd=CreateDialogIndirectParam(
            hinst,(LPDLGTEMPLATE)hgbl,hwndOwner,
            (DLGPROC)DialogProc,(LPARAM)&infoDlg);
            if(hDlgModeless[0].newHwnd)
            {
                hDlgModeless[0].nbHwndOpen+=1;
            }
            else
            {
                MessageBox(hwndOwner,TEXT("Error in creating modeless dialogBox "),
                NULL,MB_ICONERROR);
                return FALSE;
            }
        }
        else if(hDlgModeless)
        {
            //Si le tableau des handles de boites de dialogues modeless
            //est deja cree, alors on va juste tenter d'ajouter un nouveau
            //en faisant une reallocation
            //hDlgModeless[0].nb contient le nombre de handle deja ouvert
            i=hDlgModeless[0].nbHwndOpen;
            i++;
            hDlgModeless=realloc(hDlgModeless,sizeof(HWNDMODELESS)*(i));
            if(!hDlgModeless)
            {
                MessageBox(hwndOwner,TEXT("Error in raallocating memory for hModeless"),
                            NULL,MB_ICONERROR);
                return FALSE;
            }
            i=hDlgModeless[0].nbHwndOpen;
            hDlgModeless[i].newHwnd=CreateDialogIndirectParam(
                hinst,(LPDLGTEMPLATE)hgbl,hwndOwner,
                (DLGPROC)DialogProc,(LPARAM)&infoDlg);
            if(hDlgModeless[i].newHwnd)
            {
                hDlgModeless[0].nbHwndOpen+=1;
            }
            else
            {
                MessageBox(hwndOwner,TEXT("Error in creating modeless dialogBox "),
                            NULL,MB_ICONERROR);
                return FALSE;
            }
        }
    }
    GlobalFree(hgbl);

    return TRUE;
}
