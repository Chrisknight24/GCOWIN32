#include "OpenDialogBox.h"
#include "manageFont.h"


LRESULT CALLBACK FontProc(HWND hDlg,UINT msg, WPARAM wParam, LPARAM lParam);

MYFONT g_lastFont;
BOOL MyManageFont(int idDlg,HWND hwndOwner)
{
    //Creation de la boite de dialogue "Font"
    //La procedure de cette boite se situe aussi dans ce File

    //Faire une copie de l'etat actuels des reglages de colors
    //car les donnees du cache changeront a chaque modif de User
    //il fdaut etre capable de restaurer, si user, choisit d'Cancel plus tard

    g_lastFont=mycache[idDlg].font;

     size_t cBytes,length;
    HGLOBAL hgbl;
    LPDLGTEMPLATE lpdt;
    LPDLGITEMTEMPLATE lpdit;
    LPWORD lpw;
    LPWSTR lpwsz;
    int iPosItemX,iPosItemY,cx,cy,x,y;
    RECT rcParent,rcDlg;
    int nbChild=0;
    wchar_t *wchaine;
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
    //espace necessaire a allouer pour une boite de  child 15 controls
     cBytes=sizeof(DLGTEMPLATE)+(3*MAXDLGCHAR)+
                    15*(sizeof(DLGITEMTEMPLATE)+(2*MAXDLGCHAR));
    //Obtenir la puissance de deux directement superieure a cBytes
    cBytes=GetPow2(cBytes);
    if(cBytes==-1) return FALSE;

    hgbl=GlobalAlloc(GMEM_ZEROINIT,cBytes);
    if(!hgbl)
    {
        MessageBox(hwndOwner,TEXT("Allocation of hgbl  in MyManageFunc fun  failed!"),
                    TEXT("ERROR"),MB_ICONERROR);
        return FALSE;
    }

    //Dimension de dlgfont
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

    wchaine=L"Fonts Settings";
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
    // Group qui encadre la zone de parametrage du font
    //-----------------------
    nbChild++;
    name="";
    wchaine=L"Font";
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

    x=lpdit->x;
    y=lpdit->y;
    cx=lpdit->cx;
    cy=lpdit->cy;
    *lpw++ = 0; // No creation data


    //-----------------------
    // Group qui encadre la zone de parametrage du font2
    //-----------------------
    nbChild++;
    name="";
    wchaine=L"Select";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = cx-2*cxChar; lpdit->cy = cy/2-2*cyChar;
    lpdit->x = x+cxChar; lpdit->y = y+cyChar;
    lpdit->id = IDC_STATIC_GROUP;
    lpdit->style =WS_CHILD | WS_VISIBLE|BS_GROUPBOX;
    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080; // Button class

    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    x=lpdit->x;
    y=lpdit->y;
    cx=lpdit->cx;
    cy=lpdit->cy;
    *lpw++ = 0; // No creation data

    //------------------
    //radio button use parent font
    //--------------------
    nbChild++;
    name="Use parent's font";
    wchaine=L"Use parent's font";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2; lpdit->cy = cyChar;
    lpdit->x = x+cxChar; lpdit->y = y+2*cxChar;
    lpdit->id = ID_USE_PARENT_FONT;
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
    //radio button use another font
    //--------------------
    nbChild++;
    name="Use another font";
    wchaine=L"Use another font";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2; lpdit->cy = cyChar;
    lpdit->x = x; lpdit->y = y+cyChar;
    lpdit->id = ID_USE_ANOTHER_FONT;
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

    cx=lpdit->cx;
    cy=lpdit->cy;
    x=lpdit->x;
    y=lpdit->y;



     //-----------------------
    // Group :Apply to childs concerned
    //-----------------------
    nbChild++;
    name="";
    wchaine=L"Apply to childs concerned";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = rcDlg.right-4*cxChar; lpdit->cy = 12*cyChar/2-2*cyChar;
    lpdit->x = 2*cxChar; lpdit->y = y+4*cyChar;
    lpdit->id = IDB_GROUPBOX2;
    lpdit->style =WS_CHILD | WS_VISIBLE|BS_GROUPBOX;
    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080; // Button class

    length=wcslen(wchaine)+1;//+1 pour le nul;
    lpwsz=(LPWSTR)lpw;
    memcpy(lpwsz,wchaine,sizeof(wchar_t)*length-1);//-1 car le null de fin de chaine ne tient que sur 1 octet
    lpw+=length;

    x=lpdit->x;
    y=lpdit->y;
    cx=lpdit->cx;
    cy=lpdit->cy;
    *lpw++ = 0; // No creation data

    //------------------
    //radio button use parent font
    //--------------------
    nbChild++;
    name="Yes          ";
    wchaine=L"Yes          ";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2; lpdit->cy = cyChar;
    lpdit->x = x+cxChar; lpdit->y = y+2*cxChar;
    lpdit->id = IDYES;
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
    //radio button use another font
    //--------------------
    nbChild++;
    name="No         ";
    wchaine=L"No         ";
    lpdit = AlignLpdit(lpw);
    lpdit->cx = (strlen(name)+1)*cxChar/2; lpdit->cy = cyChar;
    lpdit->x = x; lpdit->y = y+cyChar;
    lpdit->id = IDNO;
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
    hwndOwner,(DLGPROC)FontProc,(LPARAM)&infoDlg);


    GlobalFree(hgbl);

    return TRUE;
}


LRESULT CALLBACK FontProc(HWND hDlg,UINT msg, WPARAM wParam, LPARAM lParam)
{
    static MY_DLGDATA *infoDlg;
    static int idDlg;
    //Recuperer les caracteristiques de la boite de dialogue,
    //NB: le champs id, contient l'id de la boite de dialogue hote
    if(msg==WM_INITDIALOG)
    {
        infoDlg=(MY_DLGDATA*)lParam;
        idDlg=infoDlg->idDlg;

    }
    static int iUserChoice=-1;
    HWND hwndObjAspect;

    switch(msg)
    {
    case WM_INITDIALOG:
        iUserChoice=-1;
        if(mycache[idDlg].font.isFontParent==TRUE)
        {
            CheckRadioButton(hDlg,ID_USE_PARENT_FONT,
                ID_USE_ANOTHER_FONT,ID_USE_PARENT_FONT);
            SendMessage(hDlg,WM_COMMAND,
                        MAKELONG(ID_USE_PARENT_FONT,BN_CLICKED),
                        (LPARAM)GetDlgItem(hDlg,ID_USE_PARENT_FONT));

        }
        else if(mycache[idDlg].font.isFontParent==FALSE)
        {
            CheckRadioButton(hDlg,ID_USE_ANOTHER_FONT,
                ID_USE_ANOTHER_FONT,ID_USE_ANOTHER_FONT);
            SendMessage(hDlg,WM_COMMAND,
                        MAKELONG(ID_USE_ANOTHER_FONT,BN_CLICKED),
                        (LPARAM)GetDlgItem(hDlg,ID_USE_ANOTHER_FONT));

        }
        else
        {
            SendMessage(hDlg,WM_COMMAND,
            MAKELONG(ID_SET_DEFAULT,BN_CLICKED),
                        (LPARAM)GetDlgItem(hDlg,ID_SET_DEFAULT));

        }
        //Cas du deuxieme groupBox(Apply to childs concerned)
        //Ce groupe n'est activer qu'en cas de modification(isTymodDlg)
        //Et si l'objet qu'on modifie a deja des childs
        //Et si l'obj est soit unDLG_BOX ou un groupBox

        //1: Desactive tout
        EnableWindow(GetDlgItem(hDlg,IDB_GROUPBOX2),FALSE);
        EnableWindow(GetDlgItem(hDlg,IDYES),FALSE);
        EnableWindow(GetDlgItem(hDlg,IDNO),FALSE);

        //2:Active selon les conditions listees ci haut
        if(isTypeModDlg && idObjToDelete )
        {
            int iIndex=GetObjIndex(idObjToDelete);
            if(iIndex!=-1)
            {
                //Verfie si il a des childs
                if(fHasChilds(iIndex))
                {
                    //Tout est bon, on peut activer groupDeux
                    EnableWindow(GetDlgItem(hDlg,IDB_GROUPBOX2),TRUE);
                    EnableWindow(GetDlgItem(hDlg,IDYES),TRUE);
                    EnableWindow(GetDlgItem(hDlg,IDNO),TRUE);
                    //Coche No par defaut
                    if(CheckRadioButton(hDlg,IDYES,IDNO,IDNO))
                    g_fShouldApplyFontOnChilds=FALSE;

                }
            }
        }
        return FALSE;

    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case IDYES:
            if(HIWORD(wParam)==BN_CLICKED)
                g_fShouldApplyFontOnChilds=TRUE;
            return TRUE;
        case IDNO:
            if(HIWORD(wParam)==BN_CLICKED)
                g_fShouldApplyFontOnChilds=FALSE;
            return TRUE;

        case ID_USE_ANOTHER_FONT:
            if(PopFontChooseFont(hDlg,&mycache[idDlg].font.logfont))
            {
                iUserChoice=ID_USE_ANOTHER_FONT;
                HWND hwndTarget=GetParent(hDlg);
                hwndTarget=GetDlgItem(hwndTarget,ID_ASPECT);
                hwndTarget=GetDlgItem(hwndTarget,1);
                if(!hwndTarget)
                {
                    MyPerror(" soisirowir");
                    MessageBox(hDlg,"An error occured","ERROR!!!",MB_ICONERROR|
                               MB_OK|MB_TASKMODAL);

                    SendMessage(hDlg,WM_COMMAND,
                                MAKELONG(IDCANCEL,BN_CLICKED),
                    (LPARAM)GetDlgItem(hDlg,IDCANCEL));
                    return TRUE;
                }
                mycache[idDlg].font.isFontParent=FALSE;//A placer avant le refresh
                MyRefreshAspect(GetParent(hDlg),idDlg);

                hwndObjAspect=GetDlgItem(GetDlgItem(GetParent(hDlg),ID_ASPECT),1);
                if(!hwndObjAspect)
                {
                    MyPerror("glf;glpr");
                    MyMsgError();
                    return TRUE;
                }
                mycache[idDlg].cxChar=LOWORD(
                    MyGetCharDim(hwndObjAspect,
                    mycache[idDlg].font.hFont));
                mycache[idDlg].cyChar=HIWORD(
                    MyGetCharDim(hwndObjAspect,
                    mycache[idDlg].font.hFont));

                MyRefreshAspect(GetParent(hDlg),idDlg);
                EnableWindow(GetDlgItem(hDlg,ID_APPLY),TRUE);

            }
            return TRUE;

        case ID_USE_PARENT_FONT:
        {
            iUserChoice=ID_USE_PARENT_FONT;
            int iIndexParent=MyGetIindexObjWithHwnd(g_hwndParentObjToSet);
            if(iIndexParent==-1)
            {
                mycache[idDlg].font=g_fontSys;
                if(isChildGB(g_hwndParentObjToSet))
                {
                    HWND hwndParentObj=GetParent(g_hwndParentObjToSet);
                    if(!hwndParentObj)
                    {
                        MyPerror("\n s odifosdf");
                    }
                    else
                    {
                        iIndexParent=MyGetIindexObjWithHwnd(hwndParentObj);
                        if(iIndexParent!=-1)
                        {
                            mycache[idDlg].font=uObjet[iIndexParent].font;
                        }
                    }
                }
            }
            else
                mycache[idDlg].font=uObjet[iIndexParent].font;

            mycache[idDlg].font.isFontParent=TRUE;//A placer avant le refresh
            MyRefreshAspect(GetParent(hDlg),idDlg);

            hwndObjAspect=GetDlgItem(GetDlgItem(GetParent(hDlg),ID_ASPECT),1);
            if(!hwndObjAspect)
            {
                MyPerror("geroioi");
                MyMsgError();
                return TRUE;
            }
            mycache[idDlg].cxChar=LOWORD(
                    MyGetCharDim(hwndObjAspect,
                    mycache[idDlg].font.hFont));
            mycache[idDlg].cyChar=HIWORD(
                    MyGetCharDim(hwndObjAspect,
                    mycache[idDlg].font.hFont));

            MyRefreshAspect(GetParent(hDlg),idDlg);
            EnableWindow(GetDlgItem(hDlg,ID_APPLY),TRUE);
        }
            return TRUE;

        case IDCANCEL:
            //Restaure l'ancien font
            mycache[idDlg].font=g_lastFont;
            MyRefreshAspect(GetParent(hDlg),idDlg);

            hwndObjAspect=GetDlgItem(GetDlgItem(GetParent(hDlg),ID_ASPECT),1);
            if(!hwndObjAspect)
            {
                MyPerror("kgdfkgkf");
                MyMsgError();
                return TRUE;
            }
            mycache[idDlg].cxChar=LOWORD(
                    MyGetCharDim(hwndObjAspect,
                    mycache[idDlg].font.hFont));
            mycache[idDlg].cyChar=HIWORD(
                    MyGetCharDim(hwndObjAspect,
                    mycache[idDlg].font.hFont));
            MyRefreshAspect(GetParent(hDlg),idDlg);

        case ID_APPLY:
            EndDialog(hDlg,LOWORD(wParam));
            return TRUE;

        case ID_SET_DEFAULT:
            {
            if(iUserChoice!=-1)
            CheckRadioButton(hDlg,ID_USE_PARENT_FONT,
                ID_USE_ANOTHER_FONT,iUserChoice);

            mycache[idDlg].font=g_fontSys;
            MyRefreshAspect(GetParent(hDlg),idDlg);

            hwndObjAspect=GetDlgItem(GetDlgItem(GetParent(hDlg),ID_ASPECT),1);
            if(!hwndObjAspect)
            {
                MyPerror("bdflpeg");
                MyMsgError();
                return TRUE;
            }
            mycache[idDlg].cxChar=LOWORD(
                    MyGetCharDim(hwndObjAspect,
                    mycache[idDlg].font.hFont));
            mycache[idDlg].cyChar=HIWORD(
                    MyGetCharDim(hwndObjAspect,
                    mycache[idDlg].font.hFont));

            MyRefreshAspect(GetParent(hDlg),idDlg);
            }
            return TRUE;
        }
        break;
    }
    return FALSE;
}


BOOL PopFontChooseFont (HWND hwnd,LOGFONT *logfont)
{
    /*Cette fonction affiche une boîte de dialogue permettant
    à l'utilisateur de choisir une font de caractères.
    Déclare une fonction nommée PopFontChooseFont qui
     retourne une valeur booléenne (TRUE si l'utilisateur a
    sélectionné une font, FALSE sinon) et prend en
    paramètre un handle de fenêtre parent hwnd.*/
     CHOOSEFONT cf ;
     cf.lStructSize = sizeof (CHOOSEFONT) ;
     cf.hwndOwner = hwnd ;
     cf.hDC = NULL ;
     cf.lpLogFont = logfont ;//initialise dans popFontInitialize
     cf.iPointSize = 0 ;
     /*CF_INITTOLOGFONTSTRUCT : Initialise la structure LOGFONT avec les valeurs par défaut.
    CF_SCREENFONTS : Affiche uniquement les fonts disponibles à l'écran.
    CF_EFFECTS : Affiche les options d'effets de font (gras, italique, souligné, etc.).*/
     cf.Flags = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS /*| CF_EFFECTS*/ ;
     cf.rgbColors = 0;//Aucune couleur de font par defaut
     cf.lCustData = 0 ;
     cf.lpfnHook = NULL ;
     cf.lpTemplateName = NULL ;
     cf.hInstance = NULL ;
     cf.lpszStyle = NULL ;
     cf.nFontType = 0 ; // Returned from ChooseFont
     cf.nSizeMin = 0 ;
     cf.nSizeMax = 0 ;
     /*Appelle la fonction ChooseFont avec l'adresse de la
     structure CHOOSEFONT comme paramètre et retourne
      le résultat de la fonction (TRUE si l'utilisateur a sélectionné
        une font, FALSE sinon).*/
     return ChooseFont (&cf) ;
}

void PopFontInitialize (HWND hwndEdit,HFONT *hFont,LOGFONT *logfont)
{
    //Recupere les informations sur le font system
     GetObject (GetStockObject (SYSTEM_FONT), sizeof (LOGFONT),
     (PTSTR) logfont) ;
     //cree un font copie du system font
     *hFont = CreateFontIndirect (logfont) ;
     //Applique le font a hwndedit
     SendMessage (hwndEdit, WM_SETFONT, (WPARAM) (*hFont), 0) ;
}

void PopFontSetFont (HWND hwndEdit,HFONT *hFont,LOGFONT *logfont)
{
     HFONT hFontNew ;
     RECT rect ;
     hFontNew = CreateFontIndirect (logfont) ;

     SendMessage (hwndEdit, WM_SETFONT, (WPARAM) hFontNew, 0) ;
     DeleteObject (*hFont) ;
     *hFont = hFontNew ;
     GetClientRect (hwndEdit, &rect) ;
     InvalidateRect (hwndEdit, &rect, TRUE) ;
}

void PopFontDeinitialize (HFONT *hFont)
{
    DeleteObject (*hFont) ;
}

void MyUpDateDimChar(HWND hwndParent)
{
    HWND hwndFont;
    HFONT hFont;

    int iIndex=MyGetIindexObjWithHwnd(hwndParent);
    if(iIndex==-1)
    {
        hFont=g_fontSys.hFont;
        hwndFont=hwndZC;
    }
    else
    {
        hwndFont=hwndParent;
        hFont=CreateFontIndirect(&uObjet[iIndex].font.logfont);
        if(!hFont)
        {
            MyPerror("vffffv");
            hFont=g_fontSys.hFont;
        }
    }

    cxChar=LOWORD(MyGetCharDim(hwndFont,hFont));
    cyChar=HIWORD(MyGetCharDim(hwndFont,hFont));
    if(hFont!=g_fontSys.hFont)
        DeleteObject(hFont);

}

void MySetFontParent(HWND hwndParent,MYFONT *myf)
{
    //le role de cette fonction est e parfaitement set myf sur le font
    //de son parent hwndParnet

    if(myf->isFontParent==TRUE)
    {
        int iIndexParent=MyGetIindexObjWithHwnd(hwndParent);
        if(iIndexParent==-1)
        {
            *myf=g_fontSys;
            if(isChildGB(hwndParent))
            {
                HWND hwndParentObj=GetParent(hwndParent);
                if(!hwndParentObj)
                {
                    MyPerror("\n jsjdfif");
                }
                else
                {
                    iIndexParent=MyGetIindexObjWithHwnd(hwndParentObj);
                    if(iIndexParent!=-1)
                    {
                        *myf=uObjet[iIndexParent].font;
                    }
                }
            }
        }
        else
        {
            *myf=uObjet[iIndexParent].font;
        }
        myf->isFontParent=TRUE;
    }

}

