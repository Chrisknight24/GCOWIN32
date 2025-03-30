/*Nom du File:MyTestProjet.c
Rôle du programme:  Permet d'apliquer l'environnement
que User vient de creer ,en temp reel, comme une vraie apk
de bureau ouverte,tout ceci grace aux donnees de la structure Univers*/
#include <windows.h>
#include "OpenDialogBox.h"
#include "toolBar.h"
#include "manageProjet.h"
#include "manageFont.h"

COUPLEHWND *g_coupHwnd=NULL;
BOOL MyTestProjet(void)
{
    if(!g_hwndParentObj || g_hwndParentObj==hwndZC)
        return FALSE;
    if(!g_fisTestRunAble)//Si une autre instance est deja creee
    {
        /*//mettre la dite fenetre en foreground
        int iIndexParent=MyGetIindexObjWithHwnd(g_hwndParentObj);
        if(iIndexParent==-1)
        {
            MyPerror(" oidigi");
            return FALSE;
        }
        HWND hwndParent=g_coupHwnd[iIndexParent].hwndNew;
        if(!hwndParent || !IsWindow(hwndParent))
        {
            MyPerror("jlsdklkgg");
            return FALSE;
        }
        SetForegroundWindow(hwndParent);
        */
        return FALSE;
    }

    MyEnableButtonToolBar(g_hwndToolBar,ID_STOP_TOOL,FALSE,FALSE);
    //desactiver, save et New
    MyEnableButtonToolBar(g_hwndToolBar,IDM_NEW,TRUE,TRUE);
    MyEnableButtonToolBar(g_hwndToolBar,IDM_OPEN,TRUE,TRUE);
    MyEnableButtonToolBar(g_hwndToolBar,IDM_SAVE,TRUE,TRUE);

    EnableMenuItem(GetMenu(GetParent(hwndZC)),
                IDM_CLOSE_PROJECT,MF_GRAYED);
    EnableMenuItem(GetMenu(GetParent(hwndZC)),
                IDM_SAVE,MF_GRAYED);
    EnableMenuItem(GetMenu(GetParent(hwndZC)),
                IDM_SAVEAS,MF_GRAYED);
    EnableMenuItem(GetMenu(GetParent(hwndZC)),
                IDM_NEW,MF_GRAYED);
    EnableMenuItem(GetMenu(GetParent(hwndZC)),
                IDM_OPEN,MF_GRAYED);

    //uObjet[0] contient les infos sur le premier elt
    int nbObj=uObjet[0].nbObjCreated;
    if(nbObj<=0)
    {
        MyPerror(" kgfdgj");
        return FALSE;
    }

    //tableau qui contient les handles des nouveau obj crees dans lapk test
    //le iiNDEX du tableau correspond avec le iIndex de uObjet
    if(g_coupHwnd)
    {
        free(g_coupHwnd);
        g_coupHwnd=NULL;
    }

    g_coupHwnd=malloc(sizeof(COUPLEHWND)*nbObj);
    if(!g_coupHwnd)
    {
        MyPerror(" rjdfjg");
        return FALSE;
    }
    memset(g_coupHwnd,0,sizeof(COUPLEHWND)*nbObj);

    int i,iIndexParent;

    iIndexParent=MyGetIindexObjWithHwnd(g_hwndParentObj);
    if(iIndexParent==-1)
    {
        MyPerror(" fdkfksld");
        return FALSE;
    }

    MyUpdateDimObjUniv();

    g_coupHwnd[iIndexParent].hwndLast=uObjet[iIndexParent].hwnd;
    //first create MainParent, le champ hwndNew de g_coup[iIndexParent] sera rempli
    MyCreateObjTest(iIndexParent,g_coupHwnd);
    g_hwndParentTest=g_coupHwnd[iIndexParent].hwndNew;

    MyApplyMenuCache(g_coupHwnd[iIndexParent].hwndNew,
        uObjet[iIndexParent].menu);
    for(i=0;i<nbObj;i++)
    {
        if(i==iIndexParent) continue;
        g_coupHwnd[i].hwndLast=uObjet[i].hwnd;
        MyCreateObjTest(i,g_coupHwnd);
    }
    return TRUE;
}

BOOL MyCreateObjTest(int iIndex,COUPLEHWND *tabHwnd)
{
    if(tabHwnd[iIndex].hwndNew)
    {
        return TRUE;//ALERADY Created
    }

    //cette fonction cree chaque objet, uObjet[iIndex]
    PTCHAR pszClassName,pszTitle;
    HWND hwndParent=NULL;
    BOOL fIsChildGB=FALSE;
    pszTitle=uObjet[iIndex].szTitle;

    switch(uObjet[iIndex].iTypeObj)
    {
    case OBJ_BUTTON:
        pszClassName="button";
        break;
    case OBJ_COMBOBOX:
        pszClassName=WC_COMBOBOX;
        break;
    case OBJ_LISTBOX:
        pszClassName="listbox";
        break;
    case OBJ_EDITBOX:
        pszClassName="edit";
        break;
    case OBJ_DIALOGBOX:
        pszClassName=szClassTestProjet;
        break;
    case OBJ_STATIC:
        pszClassName="static";
        break;
    }
    int i;
    //search NewHwnParent
    if(uObjet[iIndex].hwnd!=g_hwndParentObj)
    {
        //Test GB
        if(isChildGB(uObjet[iIndex].hwndParent))
        {   //Si iIndex est unChild de Gb dans GCO
            HWND hwndGB=GetParent(uObjet[iIndex].hwndParent);
            int iIndexGb;

            if(!hwndGB)
            {
                MyPerror(" kodkfodf");
                return FALSE;
            }
            iIndexGb=MyGetIindexObjWithHwnd(hwndGB);
            if(iIndexGb==-1)
            {
                MyPerror("m pkfo");
                return FALSE;
            }

            if(!tabHwnd[iIndexGb].hwndNew)
            {
                tabHwnd[iIndexGb].hwndLast=uObjet[iIndexGb].hwnd;
                MyCreateObjTest(iIndexGb,tabHwnd);

            }
            if(!tabHwnd[iIndexGb].hwndNew)
            {
                MyPerror("m afdkd");
                return FALSE;
            }
            hwndParent=tabHwnd[iIndexGb].hwndNew;
            if(!hwndParent)
            {
                MyPerror("lksdgkfdgki");
                return FALSE;
            }

            //Ordonner conversion des coordonnees de l'objet dans child Gb
            //a celle dans parentGb
            fIsChildGB=TRUE;
        }
        else
        {
            for(i=0;i<uObjet[0].nbObjCreated;i++)
            {
                if(uObjet[i].hwnd==uObjet[iIndex].hwndParent)
                {

                    hwndParent=tabHwnd[i].hwndNew;
                    if(!hwndParent)
                    {
                        tabHwnd[i].hwndLast=uObjet[i].hwnd;
                        MyCreateObjTest(i,tabHwnd);
                    }
                    hwndParent=tabHwnd[i].hwndNew;
                    if(!hwndParent)
                    {
                        MyPerror("nuodifoei");
                        return FALSE;
                    }
                }
            }
        }
    }

    //dwStyle, ID
    DWORD dwStyle;
    HINSTANCE hInst=uObjet[iIndex].hinst;
    int id;

    id=uObjet[iIndex].id;
    dwStyle=uObjet[iIndex].dwStyle;

    //x,y,w,h
    int x,y,w,h;
    x=uObjet[iIndex].x;
    y=uObjet[iIndex].y;
    w=uObjet[iIndex].w*uObjet[iIndex].cxChar+uObjet[iIndex].cxRest;
    h=uObjet[iIndex].h*uObjet[iIndex].cyChar+uObjet[iIndex].cyRest;

    if(uObjet[iIndex].hwnd==g_hwndParentObj)
    {
        //mainParent
        dwStyle&=~WS_CHILD;
        hInst=NULL;
        dwStyle|=WS_VISIBLE;
        id=0;
        if(uObjet[iIndex].menu[0].iLastIndexMenu>0)
            h+=GetSystemMetrics(SM_CYMENU);
        x=0;
        y=0;
    }

    if(fIsChildGB)
    {
        fIsChildGB=FALSE;

        //convertir les coordonnees du childs de Gb a celle du parent de Gb
        RECT rcGB;
        TCHAR szClassParent[1024];

        if(!hwndParent)
        {
            MyPerror("mv apsk");
            return FALSE;
        }

        do
        {
            GetWindowRect(hwndParent,&rcGB);

            hwndParent=GetParent(hwndParent);
            if(!hwndParent)
            {
                MyPerror("m pokoidfg");
                return FALSE;
            }

            MyScreenRcToClientRc(hwndParent,&rcGB);
            x=rcGB.left+x;
            y=rcGB.top+y;

            GetClassName(hwndParent,szClassParent,1023);

        }while(strcmp(szClassParent,szClassTestProjet));

        MyUpDateDimChar(GetParent(uObjet[iIndex].hwndParent));
        y+=cyChar;
    }
    //Creation
    HWND hwnd;
    hwnd=CreateWindow(pszClassName,pszTitle,
                      dwStyle,x,y,w,h,hwndParent,(HMENU)((INT_PTR)id),
    hInst,NULL);

    if(!hwnd)
    {
        MyPerror("bkdlfkkg");
        return FALSE;
    }
    tabHwnd[iIndex].hwndNew=hwnd;
    PopFontSetFont(hwnd,&uObjet[iIndex].font.hFont,&uObjet[iIndex].font.logfont);

    return TRUE;
}

LRESULT CALLBACK TestProjetProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{//Procedure pour tous objets de types DlgDialog

    static HBRUSH hBrushDlg=NULL;
    static HBRUSH hBrushCTL=NULL;

    switch(message)
    {
    case WM_PAINT:
        //Il s'agit ici de colorier hwnd avec sa couleur dans uObjet
    {
        //Determiner a quel objet de l'univers correspond ce hwnd
        //en utilisant tabHwnd
        int iIndex=MyGetIIndexWithhwndNew(hwnd);
        if(iIndex==-1)
        {
            MyPerror(" pidosisodg");
            iIndex=0;
        }
        COLORREF couleurBk,couleurTxt;
        if(hBrushDlg)
            DeleteObject(hBrushDlg);
        couleurBk=MyGetSysColor(uObjet[iIndex].mycolorobj[BK],
                                uObjet[iIndex].hwnd);
        couleurTxt=MyGetSysColor(uObjet[iIndex].mycolorobj[TXT],
                                 uObjet[iIndex].hwnd);
        hBrushDlg=CreateSolidBrush(couleurBk);
        PAINTSTRUCT ps;
        HDC hdc=BeginPaint(hwnd,&ps);
        FillRect(hdc,&ps.rcPaint,hBrushDlg);
        SetBkColor(hdc,couleurBk);
        SetTextColor(hdc,couleurTxt);
        EndPaint(hwnd,&ps);
    }
        return 0;//fin WM_PAINT
    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORMSGBOX:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
    {
        HWND hwndObjet=(HWND)lParam;
        int iIndex=MyGetIIndexWithhwndNew(hwndObjet);
        if(iIndex==-1)
        {
            break;
        }

        COLORREF couleurBk,couleurTxt;
        couleurBk=MyGetSysColor(uObjet[iIndex].mycolorobj[BK],
                                uObjet[iIndex].hwnd);
        couleurTxt=MyGetSysColor(uObjet[iIndex].mycolorobj[TXT],
                                 uObjet[iIndex].hwnd);

        SetBkColor((HDC)wParam,couleurBk);
        SetTextColor((HDC)wParam,couleurTxt);
        if(hBrushCTL)
            DeleteObject(hBrushCTL);
        hBrushCTL=CreateSolidBrush(couleurBk);
        return (LRESULT)hBrushCTL;
    }
        break;//Unreacheable...fin WM_CTLCOLOR...


    case WM_CLOSE:
        if(hwnd!=g_hwndParentTest) break;
        //activer sur Run
        MyEnableButtonToolBar(g_hwndToolBar,ID_STOP_TOOL,TRUE,FALSE);
        //reactiver, save et New et open
        MyEnableButtonToolBar(g_hwndToolBar,IDM_NEW,TRUE,FALSE);
        MyEnableButtonToolBar(g_hwndToolBar,IDM_OPEN,TRUE,FALSE);
        MyEnableButtonToolBar(g_hwndToolBar,IDM_SAVE,TRUE,FALSE);

        EnableMenuItem(GetMenu(GetParent(hwndZC)),
                IDM_CLOSE_PROJECT,MF_ENABLED);
        EnableMenuItem(GetMenu(GetParent(hwndZC)),
                IDM_SAVE,MF_ENABLED);
        EnableMenuItem(GetMenu(GetParent(hwndZC)),
                IDM_SAVEAS,MF_ENABLED);
        EnableMenuItem(GetMenu(GetParent(hwndZC)),
                IDM_NEW,MF_ENABLED);
        EnableMenuItem(GetMenu(GetParent(hwndZC)),
                IDM_OPEN,MF_ENABLED);

        DeleteObject(hBrushDlg);
        DeleteObject(hBrushCTL);
        free(g_coupHwnd);
        g_coupHwnd=NULL;
        hBrushCTL=NULL;
        hBrushDlg=NULL;
        g_hwndParentTest=NULL;//important


        break;

    case WM_DESTROY:

        return 0;

    }
    return DefWindowProc(hwnd,message,wParam,lParam);
}

int  MyGetIIndexWithhwndNew(HWND hwnd)
{
    if(!hwnd) return -1;
    int i;
    for(i=0;i<uObjet[0].nbObjCreated;i++)
    {
        if(g_coupHwnd[i].hwndNew==hwnd)
            return i;
    }
    return -1;
}


