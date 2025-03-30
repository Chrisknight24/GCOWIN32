/*Nom de File:OpenDlg_listbox.c
Role:File qui gere la creation de la boite de dialogue qui cree les objets
listbox
BY CHRISTIAN EDOUARDO LE 21/09/2024 a 13h31*/

#include <windows.h>
#include "resource.h"
#include "OpenDialogBox.h"

BOOL OpenDlg_listbox(int iType,HWND hwndOwner,HINSTANCE hinst)
{
    size_t cBytes,length;
    HGLOBAL hgbl;
    LPDLGTEMPLATE lpdt;
    LPDLGITEMTEMPLATE lpdit;
    LPWORD lpw;
    LPWSTR lpwsz;
    int i,iPosItemX,iPosItemY,cx,cy,x,y;
    RECT rc,rcDlg,rcScreen;
    int nbChild=0;
    wchar_t *wchaine;
    char *name;
    wchar_t *wnameChild[]={L"Button",L"ListBox",
        L"EditBox",L"DialogBox",L"ComboBox",L"GroupBox",L"Static"};
    int nbName=sizeof(wnameChild)/sizeof(wnameChild[0]);

    //informations a passer a la procedure de fenetre via lparma du inidialog
    MY_DLGDATA infoDlg;
    int id=DLG_LISTBOX;
    infoDlg.idDlg=id;
    infoDlg.iTypeDlg=iType;
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
    rcScreen.left=0;
    rcScreen.top=0;
    rcScreen.right=GetSystemMetrics(SM_CXSCREEN)/2;
    rcScreen.bottom=GetSystemMetrics(SM_CYSCREEN)/2;

    //After :CONVERTIR rcScreen en unite de boite de dialogue/2

    rcDlg.left=0;
    rcDlg.top=0;
    rcDlg.bottom=  80*(rcScreen.bottom)/100;
    rcDlg.right=80*(rcScreen.right)/100;
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

    wchaine=L"ListBox Settings";
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
    // Define an create button.
    //-----------------------
    nbChild++;
    lpdit = AlignLpdit(lpw);
    if(!isTypeModDlg)
    {
        name="Create";
        wchaine=L"Create";
        lpdit->id = ID_CREATE; // CREATE button identifier
    }
    else if(isTypeModDlg)
    {
        name="Apply";
        wchaine=L"Apply";
        lpdit->id = ID_APPLY; // APPLY button identifier

    }
    lpdit->cx = (strlen(name)+1)*cxChar/2; lpdit->cy = cyChar;
    lpdit->x = rcDlg.right-lpdit->cx-cxChar; lpdit->y = rcDlg.bottom-lpdit->cy-cyChar;
    iPosItemX=lpdit->x-cxChar;
    iPosItemY=lpdit->y;
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
    // Define an PRECEDENT button.
    //-----------------------
    nbChild++;
    lpdit = AlignLpdit(lpw);
    if(!isTypeModDlg)
    {
        name="previous";
        wchaine=L"previous";
        lpdit->id = ID_PREV; // PREV's button identifier

    }
    else if(isTypeModDlg)
    {
        name="Cancel";
        wchaine=L"Cancel";
        lpdit->id = IDCANCEL; // CANCEL'S button identifier

    }

    lpdit->cx = (strlen(name)+1)*cxChar/2; lpdit->cy = cyChar;
    lpdit->x = iPosItemX-lpdit->cx; lpdit->y = iPosItemY;
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
    // Group qui contient la liste des styles et leurs description
    //-----------------------
    nbChild++;
    name="Precedent";
    wchaine=L"";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = rcDlg.right-2*cxChar; lpdit->cy = 13*cyChar;
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
    // Definition du texte static:Styles.
    //-----------------------
    nbChild++;
    name="Styles";
    wchaine=L"Styles";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2; lpdit->cy = cyChar/2;
    lpdit->x = 2*cxChar; lpdit->y = 13*cxChar;
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


    //-----------------------
    // Definition du texte static:Description.
    //-----------------------
    nbChild++;
    name="Description:";
    wchaine=L"Description:";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2; lpdit->cy = cyChar/2;
    lpdit->x = (rcDlg.right-2*cxChar)/2+cxChar; lpdit->y = 13*cxChar;
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

    //-----------------------
    // Definition du texte static Qui affichera le nom du style a decrire.
    //-----------------------
    nbChild++;
    name="                             ";
    wchaine=L"                             ";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2; lpdit->cy = cyChar/2;
    lpdit->x = (rcDlg.right-2*cxChar)/2+cxChar+5*cxChar; lpdit->y = 13*cxChar;
    lpdit->id = IDS_STYLE_TO_DESC;
    lpdit->style = WS_CHILD | WS_VISIBLE;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0082; // static class

    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    *lpw++ = 0; // No creation data


    //-----------------------
    // Definition du texte static:Aspect.
    //-----------------------
    nbChild++;
    name="Aspect";
    wchaine=L"Aspect";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2; lpdit->cy = cyChar;
    lpdit->x = 2*cxChar; lpdit->y = 2*cxChar;
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


    //-----------------------
    // Definition des textes static:Title,x,y,w,h.
    //-----------------------
    {
        wchar_t *szTitle[]={L"Title:",L"    X:",L"    Y:",L"    W:",L"    H:"};
        CHAR szLongestName[]="Title";//Le plus long nom de szTitle
        nbName=sizeof(szTitle)/sizeof(szTitle[0]);
        int  id;
        for(i=0;i<nbName;i++)
        {
            id=ID_X_DEC+2*(i-1);
            //Cas du static
            nbChild++;
            //name="Aspect";
            wchaine=szTitle[i];
            lpdit = AlignLpdit(lpw);
            lpdit->cx = (strlen(szLongestName)+1)*cxChar/2; lpdit->cy = cyChar;
            lpdit->x = (rcDlg.right-2*cxChar)/2+cxChar; lpdit->y = 2*cxChar*(i+1);
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

            if(i==0)
            {//Apres le static Title, on cree un edit pour entrer la valeur du Title
                nbChild++;
                name="";
                wchaine=L"";
                lpdit = AlignLpdit(lpw);
                lpdit->cx = (10)*cxChar/2; lpdit->cy = 5*cyChar/8;
                lpdit->x =cx+x; lpdit->y =y;
                lpdit->id = ID_EDIT_TITLE;
                lpdit->style = WS_CHILD | WS_VISIBLE | WS_BORDER
                | ES_LEFT | ES_AUTOHSCROLL;

                lpw = (LPWORD)(lpdit + 1);
                *lpw++ = 0xFFFF;
                *lpw++ = 0x0081; // EDIT class

                length=wcslen(wchaine)+1;//+1 pour le nul;
                lpwsz=(LPWSTR)lpw;
                memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
                lpw+=length;

                *lpw++ = 0; // No creation data

            }
            else if(i!=0)
            {
                //pour les autres static text,
                //on definit des buton < static >! < et >
                // sont des boutons incrementant et decrementant

                //Cas du bouton <
                nbChild++;
                name="<";
                wchaine=L"<";
                lpdit = AlignLpdit(lpw);
                lpdit->cx = (strlen(name)+1)*cxChar/2; lpdit->cy = cyChar;
                lpdit->x =cx+x; lpdit->y =y-5;
                lpdit->id = id;
                lpdit->style = WS_CHILD | WS_VISIBLE |BS_PUSHBUTTON|
                WS_TABSTOP|WS_GROUP;

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

                //Cas du bouton >
                nbChild++;
                name=">";
                wchaine=L">";
                lpdit = AlignLpdit(lpw);
                lpdit->cx = (strlen(name)+1)*cxChar/2; lpdit->cy = cyChar;
                lpdit->x =cx+x+2*cxChar; lpdit->y =y;
                lpdit->id = id+1;
                lpdit->style = WS_CHILD | WS_VISIBLE |BS_PUSHBUTTON|
                WS_TABSTOP;

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

                //Cas du static text qui stocke la valeur < static >
                nbChild++;
                name="ABC";
                wchaine=L"1234";
                lpdit = AlignLpdit(lpw);
                lpdit->cx = (strlen(name)+1)*cxChar/2; lpdit->cy = cyChar;
                lpdit->x =x-2*cxChar; lpdit->y =y;
                lpdit->id = ID_STATIC_X+(i-1);
                lpdit->style = WS_CHILD | WS_VISIBLE|SS_CENTER|SS_CENTERIMAGE;

                lpw = (LPWORD)(lpdit + 1);
                *lpw++ = 0xFFFF;
                *lpw++ = 0x0082; //STATIC  class

                length=wcslen(wchaine)+1;//+1 pour le nul;
                lpwsz=(LPWSTR)lpw;
                memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
                lpw+=length;

                *lpw++ = 0; // No creation data

            }

        }
    }


    //-----------------------
    // Definition du childs qui affiche l'aspect du bouton:sous static Aspect.
    //-----------------------
    nbChild++;
    name="";

    wchaine=szMyUtfClassChildName[id];

    lpdit = AlignLpdit(lpw);
    lpdit->cx = (rcDlg.right-2*cxChar)/2-2*cxChar; lpdit->cy = 4*cyChar;
    lpdit->x = 2*cxChar; lpdit->y = 3*cxChar;
    lpdit->id = ID_ASPECT;
    lpdit->style = WS_CHILD | WS_VISIBLE|WS_POPUP|WS_VSCROLL|
    WS_HSCROLL|WS_GROUP|WS_TABSTOP|WS_CLIPCHILDREN;

    //Classe
    lpw = (LPWORD)(lpdit + 1);
    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    //Title
    wchaine=L"";
    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    *lpw++ = 0; // No creation data


    //-----------------------
    // Definition du childs qui contient la liste des styles:sous
    //static styles.
    //-----------------------
    nbChild++;
    name="";

    wchaine=szMyUtfClassChildName[id];
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (rcDlg.right-2*cxChar)/2-2*cxChar; lpdit->cy = 4*cyChar;
    lpdit->x = 2*cxChar; lpdit->y = 14*cxChar;
    lpdit->id = ID_STYLE_LIST;
    lpdit->style = WS_CHILD | WS_VISIBLE|WS_POPUP|WS_VSCROLL|
    WS_HSCROLL|WS_GROUP|WS_TABSTOP|
    WS_CLIPCHILDREN;

    //Classe
    lpw = (LPWORD)(lpdit + 1);
    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    //Title
    wchaine=L"";
    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    *lpw++ = 0; // No creation data

    x=lpdit->x;
    y=lpdit->y;
    cx=lpdit->cx;
    cy=lpdit->cy;

    //-------------------------------
        //Definition du childs qui specifie l'ensemble des styles
        //choisis par User
    //-------------------------------

    nbChild++;
    name="";

    wchaine=szMyUtfClassChildName[id];
    lpdit = AlignLpdit(lpw);
    lpdit->cx = rcDlg.right-4*cxChar; lpdit->cy = 2*cyChar;
    lpdit->x =x; lpdit->y = y+cy;
    lpdit->id = ID_EDIT_STYLES_CHOOSEN;
    lpdit->style = WS_CHILD | WS_VISIBLE|ES_MULTILINE|
    ES_AUTOVSCROLL|ES_LEFT|WS_BORDER|ES_READONLY;

    //Classe

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0081; //STATIC  class

    //Title
    wchaine=L"";
    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    *lpw++ = 0; // No creation data


    //-----------------------
    // Definition du childs qui contient la  description des des
    // styles:sous static Description.
    //-----------------------
    nbChild++;
    name="";

    wchaine=szMyUtfClassChildName[id];
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (rcDlg.right-2*cxChar)/2-2*cxChar; lpdit->cy = 4*cyChar;
    lpdit->x =(rcDlg.right-2*cxChar)/2+cxChar; lpdit->y = 14*cxChar;
    lpdit->id = ID_DESCRIPTION;
    lpdit->style = WS_CHILD | WS_VISIBLE|WS_POPUP|
    WS_CLIPCHILDREN|WS_GROUP;

    //Classe

    lpw = (LPWORD)(lpdit + 1);
    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    //Title
    wchaine=L"";
    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    *lpw++ = 0; // No creation data


    //STATIC NAMEID:
    nbChild++;
    name="NAMEID:";
    wchaine=L"NAMEID:";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2; lpdit->cy = cyChar;
    lpdit->x =rcDlg.right/2+rcDlg.right/4 -4*cxChar ; lpdit->y =cyChar;
    lpdit->id = ID_STATIC;
    lpdit->style = WS_CHILD | WS_VISIBLE;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0082; //STATIC  class

    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    *lpw++ = 0; // No creation data
    x=lpdit->x;
    y=lpdit->y;
    cx=lpdit->cx;
    cy=lpdit->cy;

    //EDIT:NAMEID
    nbChild++;
    name="";
    wchaine=L"";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (17)*cxChar/2; lpdit->cy = 5*cyChar/8;
    lpdit->x =cx+x+cxChar/2; lpdit->y =y;
    lpdit->id = IDE_NAMEID;
    lpdit->style = WS_CHILD | WS_VISIBLE | WS_BORDER
                | ES_LEFT | ES_AUTOHSCROLL;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0081; // EDIT class

    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    *lpw++ = 0; // No creation data

    //------------------------------------
    //Bouton "colors"
    //-------------------------------------
    nbChild++;
    name="colors";
    wchaine=L"colors";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2; lpdit->cy = cyChar;
    lpdit->x =rcDlg.right/2+rcDlg.right/4 -4*cxChar ; lpdit->y =7*cxChar/2;
    lpdit->id = ID_COLOR; // COLOR button identifier
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



    //------------------------------------
    //Bouton "font"
    //-------------------------------------
    nbChild++;
    name="font";
    wchaine=L"font";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2; lpdit->cy = cyChar;
    lpdit->x =rcDlg.right/2+rcDlg.right/4 -4*cxChar ; lpdit->y =6*cxChar;
    lpdit->id = ID_FONT; // FONT button identifier
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
