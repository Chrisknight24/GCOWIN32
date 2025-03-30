/*Nom de File:MyChildProcDlgComboBox.c
Role:File qui gere l les fenetres childs personnalisees de la boite
de dialogue DLG_COMBOBOX
BY CHIRSTIAN EDOUARDO LE 13/09/2024 a 15h14*/

#include <windows.h>
#include <tchar.h>
#include "resource.h"
#include "OpenDialogBox.h"
#include "mystrtok.h"

static int idDlg;
WNDPROC OldCheckProc;
LRESULT CALLBACK NewCheckProc(HWND hwnd,UINT msg,WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MyChildProcDlgComboBox(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    //Cette procedure de fenetre est sensee gerer tous les childs present
    //dans la boite de dialogue de DLG_COMBOBOX, mais uniquement ceux crees avec la classe personnalisee
    //Ceci signifie que cette procedure geres plusieurs childs different, et son comportement
    //depend donc de la nature du childs, qui ne peut etre connue que par son id
    int idChildDlgComBoBox;
    idDlg=DLG_COMBOBOX;

    idChildDlgComBoBox=GetDlgCtrlID(hwnd);

    if(idChildDlgComBoBox==ID_STYLE_LIST)
    {
        FILE *File=NULL;
        char buf[1024],*c,*szName;
        static HWND hwndChild,hwndStatic[2];
        int i,j,iBegPaint,iEndPaint,pas;
        WPARAM direction;
        static HBRUSH hBrushStatic;
        SCROLLINFO si;
        static int cxClient,cyClient,idStatic2, nbStyles;

        PAINTSTRUCT ps;

        BOOL isStatic2DisPlayed;

        static int iVertPos;//Position courante du curseur de la barre verticale
        static int iHorzPos;//Position courante du curseur de la barre horizontale
        DWORD dwStyleChild=WS_CHILD|WS_VISIBLE|
            BS_AUTOCHECKBOX|BS_NOTIFY|WS_CLIPSIBLINGS;


        //Child ou est affiche la listes des tyles
        switch(msg)
        {
        case WM_CREATE:
            //Creer des checkBox childs referencant chaque style
            //Le nom des styles est situe dans styles.txt et ceux
            //etendus dans stylesEx.txt

            //Static Title d'affichage
            i=0;
            hwndStatic[0]=CreateWindow(TEXT("static"),
TEXT("---STANDARDS STYLES---"),WS_CHILD|WS_VISIBLE,0,i,
25*cxChar,cyChar,hwnd,(HMENU)IDC_STATIC,
(HINSTANCE)GetWindowLongPtr(hwnd,GWLP_HINSTANCE),NULL);
            if(!hwndStatic[0])
            {
                MessageBox(hwnd,TEXT("Unable to create Child Button style"),NULL,0);
                exit(EXIT_FAILURE);
            }
            hBrushStatic = CreateSolidBrush (
            GetSysColor (COLOR_WINDOW)) ;

            HRSRC hResInfo=FindResource((HINSTANCE)GetWindowLongPtr(hwndZC,GWLP_HINSTANCE),
                "GCO_STYLES","TEXT");
            TCHAR szNameFile[1024]="";
            MyCreateTempTxtFile(hResInfo,szNameFile);
            File=fopen(szNameFile,"r");
            if(!File)
            {
                MyPerror("dkflsklgk");
                MyMsgError();
                exit(EXIT_FAILURE);
            }
            //Affichages des styles standars commencant par WS et BS
            i=1;
            j=i;
            nbStyles=0;
            isStatic2DisPlayed=FALSE;//etat du static pour les styles etendus
            while(fgets(buf,1024,File))
            {
                //Ignorer les lignes vides
                c=buf;
                while(*c==' ' || *c=='\t') c++;
                if(*c=='\n') continue;
                else strcpy(buf,c);//Supprime les espaces du debut s'il y,en avait

                //Retire le nom du style
                szName=strtok(buf," ");
                if( ((c=strstr(buf,"WS_")) || (c=strstr(buf,"CBS_"))) && c==buf)//CBS car on traite les  comboBox
                {
                    if(i==1) dwStyleChild=dwStyleChild | WS_GROUP|WS_TABSTOP;
                    else if(i==2) dwStyleChild=dwStyleChild & ~WS_GROUP;
                    //Si  buf est valide(commence par ES ou par WS)
                    //Trouver la valeur du style ,tableau qui commence a 1
                    if(!StringToiD(szName,&mycache[idDlg].valueStyle[i].dwValuesStyles,szNameFile))
                        exit(EXIT_FAILURE);
                    //Ajouter le mot correspondant au syle
                    strcpy(mycache[idDlg].valueStyle[i].szValuesStyles,strcat(szName,"|"));
                    //Supprime le "|" de la fin du au strcat precedent
                    szName[strlen(szName)-1]='\0';
                    //Affichage du static pour styles etendus
                    if(strstr(buf,"WS_EX_")  && isStatic2DisPlayed==FALSE)
                    {
                        hwndStatic[1]=CreateWindow(TEXT("static"),
    TEXT("---EXTENDED STYLES---"),WS_CHILD|WS_VISIBLE,0,j*cyChar,
    25*cxChar,cyChar,hwnd,(HMENU)IDC_STATIC,
    (HINSTANCE)GetWindowLongPtr(hwnd,GWLP_HINSTANCE),NULL);
                        if(!hwndStatic[1])
                        {
                            MessageBox(hwnd,TEXT("Unable to create Child Button style"),NULL,0);
                            exit(EXIT_FAILURE);
                        }
                        idStatic2=j;
                        j++;

                        isStatic2DisPlayed=TRUE;
                    }

                    hwndChild=CreateWindow(TEXT("button"),
    szName,dwStyleChild,
    0,(j)*cyChar,3*25*cxChar/2,cyChar,hwnd,(HMENU)((INT_PTR)i),
    (HINSTANCE)GetWindowLongPtr(hwnd,GWLP_HINSTANCE),NULL);
                    if(!hwndChild)
                    {
                        MessageBox(hwnd,TEXT("Unable to create Child Button style"),NULL,0);
                        exit(EXIT_FAILURE);
                    }
                    //SubClassing
                    OldCheckProc=(WNDPROC)SetWindowLongPtr(hwndChild,GWLP_WNDPROC,
                                            (LONG_PTR)NewCheckProc);
                    if(!OldCheckProc)
                    {
                        MessageBox(hwnd,TEXT("Unable to SubClass check Button"),NULL,MB_ICONERROR);
                        exit(EXIT_FAILURE);
                    }
                    i++;
                    j++;
                    nbStyles++;

                }
            }
            fclose(File);
            //Remove
            remove(szNameFile);
            if(nbStyles<=0)
            {
                MyPerror("fdklflk");
                MyMsgError();
                exit(EXIT_FAILURE);
            }
            //La premiere case de dwValueStyle stocke le nombre de styles
            mycache[idDlg].valueStyle[0].dwValuesStyles=nbStyles;
            g_nbStyle=nbStyles;
            return 0;//fin case WM_CREATE

        case WM_SIZE:
            cxClient=LOWORD(lParam);
            cyClient=HIWORD(lParam);

            //Reglage barre verticale
            si.cbSize=sizeof(SCROLLINFO);
            si.fMask=SIF_RANGE|SIF_PAGE;
            si.nMax=nbStyles;
            si.nMin=0;
            si.nPage=cyClient/cyChar;
            SetScrollInfo(hwnd,SB_VERT,&si,TRUE);
        //Reglage barre Horizontale
            si.nMax=50;//temporaire a remplacer par le nb de caratere du plus long style
            si.nMin=0;
            si.nPage=cxClient/cxChar;
            SetScrollInfo(hwnd,SB_HORZ,&si,TRUE);
            return 0;//fin case WM_SIZE

        case WM_VSCROLL:
        //When user use a vertical's scroll bar
        //Get All infos concerning a current state of scroll bar

            si.cbSize=sizeof(SCROLLINFO);
            si.fMask=SIF_ALL;//Get all infos

            GetScrollInfo(hwnd,SB_VERT,&si);
            //Save the actual value of cursor's bar
            iVertPos=si.nPos;

            //Analyse more deeply a nature of action done by user
            //in scroll bar
            switch(LOWORD(wParam))
            {
            case SB_TOP:
                si.nPos=si.nMin;
                break;//fin case SB_TOP
            case SB_BOTTOM:
                si.nPos=si.nMax;
                break;//fin case SB_BOTTOM
            case SB_LINEUP:
                si.nPos-=1;
                break;//fin case SB_LINEUP
            case SB_LINEDOWN:
                si.nPos+=1;
                break;//fin case SB_LINEDOWN
            case SB_PAGEUP:
                si.nPos-=cyClient/cyChar;
                break;//fin case SB_PAGEUP
            case SB_PAGEDOWN:
                si.nPos+=cyClient/cyChar;
                break;//fin case SB_PAGEDOWN
            case SB_THUMBTRACK:
                si.nPos=si.nTrackPos;
                break;
            default:
                break;
            }//fin switch(LOWORD(wParam))

            //Now save and aply all the modification ,concerning
            //a state of a cursor's bar

            si.cbSize=sizeof(SCROLLINFO);
            si.fMask=SIF_POS;
            SetScrollInfo(hwnd,SB_VERT,&si,TRUE);
            //Now we need to know if the position of cursor
            //has changed
            GetScrollInfo(hwnd,SB_VERT,&si);
            if(iVertPos!=si.nPos)
            {   //if a position has changed
                ScrollWindow(hwnd,0,cyChar*(iVertPos-si.nPos),NULL,NULL);
                //force repainting
                UpdateWindow(hwnd);
            }//fin if iVertPos!=si.nPos
            return 0;//fin case WM_VSCROLL

        case WM_HSCROLL:
            //When user use a vertical's scroll bar
            //Get All infos concerning a current state of scroll bar
            si.cbSize=sizeof(SCROLLINFO);
            si.fMask=SIF_ALL;//Get all infos
            GetScrollInfo(hwnd,SB_HORZ,&si);
            //Save the actual value of cursor's bar
            iHorzPos=si.nPos;
            //Analyse more deeply a nature of action done by user
            //in scroll bar
            switch(LOWORD(wParam))
            {
            case SB_TOP:
                si.nPos=si.nMin;
                break;//fin case SB_TOP
            case SB_BOTTOM:
                si.nPos=si.nMax;
                break;//fin case SB_BOTTOM
            case SB_LINEUP:
                si.nPos-=1;
                break;//fin case SB_LINEUP
            case SB_LINEDOWN:
                si.nPos+=1;
                break;//fin case SB_LINEDOWN
            case SB_PAGEUP:
                si.nPos-=cxClient/cxChar;
                break;//fin case SB_PAGEUP
            case SB_PAGEDOWN:
                si.nPos+=cxClient/cxChar;
                break;//fin case SB_PAGEDOWN
            case SB_THUMBTRACK:
                si.nPos=si.nTrackPos;
                break;
            default:
                break;
            }//fin switch(LOWORD(wParam))

            //Now save and aply all the modification ,concerning
            //a state of a cursor's bar
            si.cbSize=sizeof(SCROLLINFO);
            si.fMask=SIF_POS;
            SetScrollInfo(hwnd,SB_HORZ,&si,TRUE);
            //Now we need to know if the position of cursor
            //has changed
            GetScrollInfo(hwnd,SB_HORZ,&si);
            if(iHorzPos!=si.nPos)
            {   //if a position has changed
                ScrollWindow(hwnd,cxChar*(iHorzPos-si.nPos),0,NULL,NULL);
                //force repainting
                UpdateWindow(hwnd);
            }//fin if iHorzPos!=si.nPos
            return 0;//fin case WM_HSCROLL

        case WM_KEYDOWN:
        {
            int idButton=GetDlgCtrlID(GetFocus());
            switch (wParam)
            {
            case VK_HOME:
                MyDisplayStyleDesc(mycache[idDlg].valueStyle,
                    MAKELONG(idButton,BN_CLICKED),
                    (LPARAM)GetFocus(),hwnd);
                SendMessage (hwnd, WM_VSCROLL, SB_TOP, 0) ;
                break ;
            case VK_END:

                MyDisplayStyleDesc(mycache[idDlg].valueStyle,
                    MAKELONG(idButton,BN_CLICKED),
                    (LPARAM)GetFocus(),hwnd);
                SendMessage (hwnd, WM_VSCROLL, SB_BOTTOM, 0) ;
                break ;
            case VK_PRIOR:

                MyDisplayStyleDesc(mycache[idDlg].valueStyle,
                    MAKELONG(idButton,BN_CLICKED),
                    (LPARAM)GetFocus(),hwnd);
                SendMessage (hwnd, WM_VSCROLL, SB_PAGEUP, 0) ;
                break ;
            case VK_NEXT:

                MyDisplayStyleDesc(mycache[idDlg].valueStyle,
                    MAKELONG(idButton,BN_CLICKED),
                    (LPARAM)GetFocus(),hwnd);
                SendMessage (hwnd, WM_VSCROLL, SB_PAGEDOWN, 0) ;
                break ;
            case VK_UP:

                MyDisplayStyleDesc(mycache[idDlg].valueStyle,
                    MAKELONG(idButton,BN_CLICKED),
                    (LPARAM)GetFocus(),hwnd);
                SendMessage (hwnd, WM_VSCROLL, SB_LINEUP, 0) ;
                break ;
            case VK_DOWN:

                MyDisplayStyleDesc(mycache[idDlg].valueStyle,
                    MAKELONG(idButton,BN_CLICKED),
                    (LPARAM)GetFocus(),hwnd);
                SendMessage (hwnd, WM_VSCROLL, SB_LINEDOWN, 0) ;
                break ;
            case VK_LEFT:

                MyDisplayStyleDesc(mycache[idDlg].valueStyle,
                    MAKELONG(idButton,BN_CLICKED),
                    (LPARAM)GetFocus(),hwnd);
                SendMessage (hwnd, WM_HSCROLL, SB_PAGEUP, 0) ;
                break ;
            case VK_RIGHT:

                MyDisplayStyleDesc(mycache[idDlg].valueStyle,
                    MAKELONG(idButton,BN_CLICKED),
                    (LPARAM)GetFocus(),hwnd);
                SendMessage (hwnd, WM_HSCROLL, SB_PAGEDOWN, 0) ;
                break ;
            }
        }
            return 0 ;//fin case WM_KEYDOWN

        case WM_PAINT:
            BeginPaint(hwnd,&ps);
            //GetInformations about state of scroll bars
            si.cbSize=sizeof(SCROLLINFO);
            si.fMask=SIF_POS;
            GetScrollInfo(hwnd,SB_VERT,&si);
            iVertPos=si.nPos;

            si.cbSize=sizeof(SCROLLINFO);
            si.fMask=SIF_POS;
            GetScrollInfo(hwnd,SB_HORZ,&si);
            iHorzPos=si.nPos;

            iBegPaint=max(0,iVertPos+ps.rcPaint .top/cyChar);
            iEndPaint=min(nbStyles,iVertPos+ps.rcPaint.bottom/cyChar);

            for(i=iBegPaint;i<=iEndPaint;i++)
            {
               // int  x=cxCaps*(1-iHorzPos);
                //int y=cyChar*(i-iVertPos);
                if(i==0)
                    ShowWindow(hwndStatic[0],SW_NORMAL);
                else if(i==idStatic2)
                    ShowWindow(hwndStatic[1],SW_NORMAL);
                if(i)//cAR a i==idStatic2 on a aussi un id valide de bouton
                    ShowWindow(GetDlgItem(hwnd,i),SW_NORMAL);
            }
            EndPaint(hwnd,&ps);
            return 0;//Fin case WM_PAINT

        case WM_CTLCOLORSTATIC:
            i = GetWindowLong ((HWND) lParam, GWL_ID) ;
            if(i==IDC_STATIC)
            SetTextColor ((HDC) wParam,RGB(0,0,128)) ;
            SetBkColor ((HDC) wParam, GetSysColor (COLOR_WINDOW));
            return (LRESULT) hBrushStatic ;//fin case WM_CTLCOLORSTATIC

        case WM_MOUSEWHEEL:
            //Recu soit quand le static a recu ou quand hwnd la recu
            pas=GET_WHEEL_DELTA_WPARAM(wParam)/WHEEL_DELTA;
            if(pas<0)direction=VK_DOWN;
            else direction=VK_UP;

            SendMessage (hwnd, WM_KEYDOWN, direction, 0) ;
            return 0;//fin case WM_MOUSEWHEEL

        case WM_COMMAND:
            if(HIWORD(wParam)==BN_CLICKED)
            {
                MyManageCheckStyle(idDlg,wParam,lParam);

                MyRefreshAspect(GetParent(hwnd),idDlg);
                MyDisplayStyleDesc(mycache[idDlg].valueStyle,
                    wParam,lParam,hwnd);
            }
            break;

        }//fin switch(msg)
    }//fin if idChildDlgComBoBox==ID_STYLE_LIST


    //Procedure du ID_ASPECT
    else if(idChildDlgComBoBox==ID_ASPECT)
    {
        //Child qui affiche l'aspect des boutons

        SCROLLINFO si;
        static int cxClient,cyClient;


        static int iVertPos;//Position courante du curseur de la barre verticale
        static int iHorzPos;//Position courante du curseur de la barre horizontale
        //Child ou est affiche la listes des tyles
        static HWND hwndParent;
        static RECT rcParentObj;
        switch(msg)
        {

        case WM_SIZE:
            hwndParent=GetParent(GetParent(hwnd));
            GetWindowRect(hwndParent,&rcParentObj);
            OffsetRect(&rcParentObj,-rcParentObj.left,-rcParentObj.top);
            cxClient=LOWORD(lParam);
            cyClient=HIWORD(lParam);

            //Reglage barre verticale
            si.cbSize=sizeof(SCROLLINFO);
            si.fMask=SIF_RANGE|SIF_PAGE;
            si.nMax=(rcParentObj.bottom)/cyChar -1;
            si.nMin=0;
            si.nPage=cyClient/cyChar;
            SetScrollInfo(hwnd,SB_VERT,&si,TRUE);
        //Reglage barre Horizontale
            si.nMax=(rcParentObj.right)/cxChar -1;
            si.nMin=0;
            si.nPage=cxClient/cxChar;
            SetScrollInfo(hwnd,SB_HORZ,&si,TRUE);
            return 0;//fin case WM_SIZE

        case WM_VSCROLL:
        //When user use a vertical's scroll bar
        //Get All infos concerning a current state of scroll bar

            si.cbSize=sizeof(SCROLLINFO);
            si.fMask=SIF_ALL;//Get all infos

            GetScrollInfo(hwnd,SB_VERT,&si);
            //Save the actual value of cursor's bar
            iVertPos=si.nPos;

            //Analyse more deeply a nature of action done by user
            //in scroll bar
            switch(LOWORD(wParam))
            {
            case SB_TOP:
                si.nPos=si.nMin;
                break;//fin case SB_TOP
            case SB_BOTTOM:
                si.nPos=si.nMax;
                break;//fin case SB_BOTTOM
            case SB_LINEUP:
                si.nPos-=1;
                break;//fin case SB_LINEUP
            case SB_LINEDOWN:
                si.nPos+=1;
                break;//fin case SB_LINEDOWN
            case SB_PAGEUP:
                si.nPos-=cyClient/cyChar;
                break;//fin case SB_PAGEUP
            case SB_PAGEDOWN:
                si.nPos+=cyClient/cyChar;
                break;//fin case SB_PAGEDOWN
            case SB_THUMBTRACK:
                si.nPos=si.nTrackPos;
                break;
            default:
                break;
            }//fin switch(LOWORD(wParam))

            //Now save and aply all the modification ,concerning
            //a state of a cursor's bar

            si.cbSize=sizeof(SCROLLINFO);
            si.fMask=SIF_POS;
            SetScrollInfo(hwnd,SB_VERT,&si,TRUE);
            //Now we need to know if the position of cursor
            //has changed
            GetScrollInfo(hwnd,SB_VERT,&si);
            if(iVertPos!=si.nPos)
            {   //if a position has changed
                ScrollWindow(hwnd,0,cyChar*(iVertPos-si.nPos),NULL,NULL);
                //force repainting
                UpdateWindow(hwnd);
            }//fin if iVertPos!=si.nPos
            return 0;//fin case WM_VSCROLL

        case WM_HSCROLL:
            //When user use a vertical's scroll bar
            //Get All infos concerning a current state of scroll bar
            si.cbSize=sizeof(SCROLLINFO);
            si.fMask=SIF_ALL;//Get all infos
            GetScrollInfo(hwnd,SB_HORZ,&si);
            //Save the actual value of cursor's bar
            iHorzPos=si.nPos;
            //Analyse more deeply a nature of action done by user
            //in scroll bar
            switch(LOWORD(wParam))
            {
            case SB_TOP:
                si.nPos=si.nMin;
                break;//fin case SB_TOP
            case SB_BOTTOM:
                si.nPos=si.nMax;
                break;//fin case SB_BOTTOM
            case SB_LINEUP:
                si.nPos-=1;
                break;//fin case SB_LINEUP
            case SB_LINEDOWN:
                si.nPos+=1;
                break;//fin case SB_LINEDOWN
            case SB_PAGEUP:
                si.nPos-=cxClient/cxChar;
                break;//fin case SB_PAGEUP
            case SB_PAGEDOWN:
                si.nPos+=cxClient/cxChar;
                break;//fin case SB_PAGEDOWN
            case SB_THUMBTRACK:
                si.nPos=si.nTrackPos;
                break;
            default:
                break;
            }//fin switch(LOWORD(wParam))

            //Now save and aply all the modification ,concerning
            //a state of a cursor's bar
            si.cbSize=sizeof(SCROLLINFO);
            si.fMask=SIF_POS;
            SetScrollInfo(hwnd,SB_HORZ,&si,TRUE);
            //Now we need to know if the position of cursor
            //has changed
            GetScrollInfo(hwnd,SB_HORZ,&si);
            if(iHorzPos!=si.nPos)
            {   //if a position has changed
                ScrollWindow(hwnd,cxChar*(iHorzPos-si.nPos),0,NULL,NULL);
                //force repainting
                UpdateWindow(hwnd);
            }//fin if iHorzPos!=si.nPos
            return 0;//fin case WM_HSCROLL

        case WM_MOUSEWHEEL:
            {

                //Recu soit quand le static a recu ou quand hwnd la recu
                int  pas=GET_WHEEL_DELTA_WPARAM(wParam)/WHEEL_DELTA;
                WPARAM direction;

                if(pas<0)direction=SB_LINEDOWN;
                else direction=SB_LINEUP;

                SendMessage (hwnd, WM_VSCROLL, direction, 0) ;
            }
            return 0;//fin case WM_MOUSEWHEEL

        case WM_KEYDOWN:
            switch (wParam)
            {
            case VK_HOME:
                SendMessage (hwnd, WM_VSCROLL, SB_TOP, 0) ;
                break ;
            case VK_END:
                SendMessage (hwnd, WM_VSCROLL, SB_BOTTOM, 0) ;
                break ;
            case VK_PRIOR:
                SendMessage (hwnd, WM_VSCROLL, SB_PAGEUP, 0) ;
                break ;
            case VK_NEXT:
                SendMessage (hwnd, WM_VSCROLL, SB_PAGEDOWN, 0) ;
                break ;
            case VK_UP:
                SendMessage (hwnd, WM_VSCROLL, SB_LINEUP, 0) ;
                break ;
            case VK_DOWN:
                SendMessage (hwnd, WM_VSCROLL, SB_LINEDOWN, 0) ;
                break ;
            case VK_LEFT:
                SendMessage (hwnd, WM_HSCROLL, SB_PAGEUP, 0) ;
                break ;
            case VK_RIGHT:
                SendMessage (hwnd, WM_HSCROLL, SB_PAGEDOWN, 0) ;
                break ;
            }
            return 0 ;//fin case WM_KEYDOWN


            //Gestion des colors du static
        case WM_CTLCOLORLISTBOX:
           {
            COLORREF couleur;
            static HBRUSH hBrush;

            HWND hwndParentObj;
            int iIndexParentObj;


            hwndParentObj=g_hwndParentObjToSet;
            if(!hwndParentObj)
            {
                printf("\n error :%s %d",__FILE__,__LINE__);
                break;
            }

            iIndexParentObj=MyGetIindexObjWithHwnd(hwndParentObj);
            //Teste si parent est unGroupBox
            if(iIndexParentObj==-1)
            {
                if(isChildGB(hwndParentObj))
                {
                    hwndParentObj=GetParent(hwndParentObj);
                    if(!hwndParentObj)
                    {
                        MyPerror("\n error unKnown in parentObjProc");
                        hwndParentObj=g_hwndParentObjToSet;
                    }
                    else
                    {
                        iIndexParentObj=MyGetIindexObjWithHwnd(hwndParentObj);
                    }
                }
            }
            //TXT
            if(mycache[idDlg].mycolorobj[TXT].isDefChoosen)
            {
                int iIndex=mycache[idDlg].mycolorobj[TXT].defColor;
                if(iIndex==COLOR_TXT_PARENT)
                {
                    //obtenir la couleur de texte du parent

                    if(iIndexParentObj==-1)
                    {
                        HDC hdc=GetDC(hwnd);
                        couleur=GetTextColor(hdc);
                        ReleaseDC(hwnd,hdc);
                    }
                    else if(iIndexParentObj!=-1)
                    {
                        couleur=MyGetSysColor(uObjet[iIndexParentObj].mycolorobj[TXT],hwndParentObj);
                    }

                }
                else if(iIndex==COLOR_BK_PARENT)
                {
                    //obtenir la couleur du Bk du parent
                    //c'est la meme que le BK du texte
                    if(iIndexParentObj==-1)
                    {
                        HDC hdc=GetDC(hwnd);
                        couleur=GetBkColor(hdc);
                        ReleaseDC(hwnd,hdc);
                    }
                    else if(iIndexParentObj!=-1)
                    {
                        couleur=MyGetSysColor(uObjet[iIndexParentObj].mycolorobj[BK],hwndParentObj);
                    }
                }
                else
                    couleur=GetSysColor(iIndex);
            }
            else
            {
                couleur=mycache[idDlg].mycolorobj[TXT].rgbColor;
            }
            SetTextColor((HDC)wParam,couleur);

            //BK
            if(mycache[idDlg].mycolorobj[BK].isDefChoosen)
            {
                int iIndex=mycache[idDlg].mycolorobj[BK].defColor;
                if(iIndex==COLOR_TXT_PARENT)
                {
                    //obtenir la couleur de texte du parent
                    if(iIndexParentObj==-1)
                    {
                        HDC hdc=GetDC(hwnd);
                        couleur=GetTextColor(hdc);
                        ReleaseDC(hwnd,hdc);
                    }
                    else if(iIndexParentObj!=-1)
                    {
                        couleur=MyGetSysColor(uObjet[iIndexParentObj].mycolorobj[TXT],hwndParentObj);
                    }

                }
                else if(iIndex==COLOR_BK_PARENT)
                {
                    //obtenir la couleur du Bk du parent
                    //c'est la meme que le BK du texte

                    if(iIndexParentObj==-1)
                    {
                        HDC hdc=GetDC(hwnd);
                        couleur=GetBkColor(hdc);
                        ReleaseDC(hwnd,hdc);
                    }
                    else if(iIndexParentObj!=-1)
                    {
                        couleur=MyGetSysColor(uObjet[iIndexParentObj].mycolorobj[BK],hwndParentObj);
                    }

                }
                else
                    couleur=GetSysColor(iIndex);
            }
            else
            {
                couleur=mycache[idDlg].mycolorobj[BK].rgbColor;
            }

            DeleteObject(hBrush);
            hBrush=CreateSolidBrush(couleur);
            SetBkColor((HDC)wParam,couleur);
            return (LRESULT)  hBrush;

        }
            break;//Unreacheable
        }//fin switch(msg)
    }//fin  if(idChildDlgComBoBox==ID_ASPECT)
    else if(idChildDlgComBoBox==ID_DESCRIPTION)
    {
        static HWND hwndEditDesc;



        switch(msg)
        {
        case WM_CREATE:
            hwndEditDesc=CreateWindow("edit","",WS_CHILD|
                WS_BORDER|WS_VSCROLL|
                ES_AUTOVSCROLL|ES_MULTILINE|
                WS_VISIBLE,0,0,0,0,hwnd,
                        (HMENU)IDE_STYLE_DESC,
                        ((LPCREATESTRUCT)lParam)->hInstance,
                                      NULL);
            if(!hwndEditDesc)
            {
                MyPerror("jgkdejgkdj");
                MyMsgError();
                exit(EXIT_FAILURE);
            }

            //Sous classer, pour simuler un readOnly coloriable
            g_oldEditStyleDescProc=(WNDPROC)SetWindowLongPtr(hwndEditDesc,
                        GWLP_WNDPROC,(LONG_PTR)NewEditStyleDescProc);

            if(!g_oldEditStyleDescProc)
            {
                MyPerror("mveijrhe");
                MyMsgError();
                exit(EXIT_FAILURE);
            }
            return 0;

        case WM_SIZE:
            MoveWindow(hwndEditDesc,0,0,LOWORD(lParam),
                       HIWORD(lParam),TRUE);
            return 0;

        }

    }
    return DefWindowProc(hwnd,msg,wParam,lParam);
}

