/*Nom de File:DialogProc.c
Role:File qui gere les boites de diqlogues qui permettent la creation
de control child
BY CHIRSTIAN EDOUARDO LE 01/09/2024 a 11h04*/

#include <windows.h>
#include "resource.h"
#include "OpenDialogBox.h"
#include <tchar.h>
#include "manageProjet.h"
#include "toolBar.h"
#include "manageFont.h"

LRESULT CALLBACK DimWindowProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
WNDPROC oldWndProc;
HWNDMODELESS *hDlgModeless;//Tableau des handles des boites de dialogues modeless crees
MYCACHE_DLG myValues;
BOOL isFirstLoad[NBOBJECT];

LRESULT CALLBACK DialogProc(HWND hDlg,UINT msg, WPARAM wParam, LPARAM lParam)
{
    static MY_DLGDATA *infoDlg;
    HWND hwndObjet;//handle de l'objet qui sera cree par user
    HWND hwndOwner;//handle du parent qui recoit l'objet cree par user

    //Recuperer les caracteristiques de la boite de dialogue, type, nature,id etc
    if(msg==WM_INITDIALOG)
    {
        infoDlg=(MY_DLGDATA*)lParam;

    }
    hwndOwner=infoDlg->hWndOwner;
    //Il faut traiter msg ,Next sa nature ou son id de boite de dialogue

    //BOITE D'ID=DLG_CHOOSETYOEOBJET
    if(infoDlg->idDlg==DLG_CHOOSETYPEOBJET)
    {
       static  int iUserchoice;
        switch(msg)
        {
        case WM_INITDIALOG:
            SetFocus(GetDlgItem(hDlg,ID_SUIV));
            return FALSE;//Force le systeme a ne pas attribuer le focus lui meme

        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
            case ID_SUIV:
                if(!OpenDialogBox(MODAL,DLG_BUTTON+iUserchoice-ID_BUTTON,
                        infoDlg->hWndOwner,infoDlg->hinst,hDlg,infoDlg->iTypeDlg))
                {
                    MessageBox(NULL,TEXT("This Dialog isn't yet available..."),
                    TEXT("ERROR"),MB_ICONERROR|MB_TASKMODAL);
                }

                return TRUE;

            case IDCANCEL:

                if(infoDlg->iTypeDlg==MODAL)
                    EndDialog(hDlg,TRUE);
                else
                    DeleteWindow(hDlg,hDlgModeless);


                return TRUE;

            default:
                iUserchoice=LOWORD(wParam);
                EnableWindow(GetDlgItem(hDlg,ID_SUIV),TRUE);
                return TRUE;

            }
            break;

        }
    }//fIN if idDlg==DLG_CHOSSETYPEOBJET

    //Gestion DLG_BUTTON

    else if(infoDlg->idDlg==DLG_BUTTON)
    {


        int idDlg=infoDlg->idDlg;


        if(isProjetLoadedWithOpenItem && isFirstTime[idDlg]&&isTypeModDlg)
            isFirstTime[idDlg]=FALSE;
        switch(msg)
        {
            case WM_INITDIALOG:
                if(!isTypeModDlg&& !isFirstTime[idDlg])
                {
                    mycache[idDlg].iLastX=ptWhereCreate.x;
                    mycache[idDlg].iLastY=ptWhereCreate.y;

                    MyUpDateDimChar(g_hwndParentObjToSet);
                    mycache[idDlg].cxChar=cxChar;
                    mycache[idDlg].cyChar=cyChar;
                    mycache[idDlg].font.isFontParent=TRUE;
                    //defini une chaine automatiquement et unique pour le nouveau item a creer
                    MySetNameId(mycache[idDlg].szNameId,idDlg);
                    MyInitFshouldAutosize(&mycache[idDlg].fShouldAutosize,FALSE);
                }
                if(isFirstTime[idDlg]==TRUE)
               {    isFirstTime[idDlg]=FALSE;
                        //Valeurs de depart du bouton a creer
                    //Utilie, pour adapter les dimensions a la taille de la font
                    mycache[idDlg].iLastX=ptWhereCreate.x;
                    mycache[idDlg].iLastY=ptWhereCreate.y;
                    mycache[idDlg].iLastH=2;
                    mycache[idDlg].iLastW=8;
                    mycache[idDlg].cxRest=0;
                    mycache[idDlg].cyRest=0;

                    MyUpDateDimChar(g_hwndParentObjToSet);
                    mycache[idDlg].cxChar=cxChar;
                    mycache[idDlg].cyChar=cyChar;
                    mycache[idDlg].font.isFontParent=TRUE;

                    mycache[idDlg].mycolorobj[BK].iIndexDefColor=1;//BK_PARENT
                    mycache[idDlg].mycolorobj[TXT].iIndexDefColor=0;//TXT_PARENT
                    mycache[idDlg].mycolorobj[BK].isDefChoosen=TRUE;
                    mycache[idDlg].mycolorobj[TXT].isDefChoosen=TRUE;
                    mycache[idDlg].mycolorobj[BK].defColor=COLOR_BK_PARENT;
                    mycache[idDlg].mycolorobj[TXT].defColor=COLOR_TXT_PARENT;

                    mycache[idDlg].dwLastStylesChosen=WS_VISIBLE|
                    WS_CHILD|BS_DEFPUSHBUTTON;
                    strcpy(mycache[idDlg].szTitle,"BUTTON");
                    //defini une chaine automatiquement et unique pour le nouveau item a creer
                    MySetNameId(mycache[idDlg].szNameId,idDlg);
                    strcpy(mycache[idDlg].szLastStylesChosen,
                        "WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON|");

                    MyInitFshouldAutosize(&mycache[idDlg].fShouldAutosize,FALSE);
               }
                //Indique qu'il s'agit du premier chargement
                isFirstLoad[idDlg]=TRUE;
            //Charger les donnees contenues dans le cache
            if(!MyLoadCache(idDlg,&myValues,hDlg))
                MessageBox(hDlg,TEXT("Unable to Load last datas"),NULL,0);
            SetFocus(GetDlgItem(hDlg,ID_CREATE));

            return FALSE;//Force le systeme a ne pas attribuer le focus lui meme

        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
            case ID_COLOR:
                MyManageColor(idDlg,hDlg);
                return TRUE;
            case  ID_FONT:
                MyManageFont(idDlg,hDlg);
                return TRUE;
            case ID_APPLY:
            case IDCANCEL:
            case ID_CREATE:

                if(LOWORD(wParam)==ID_APPLY ||
                   LOWORD(wParam)==ID_CREATE)
                {
                    //verifier la validite de szNameId
                    if(!isValidNameId(mycache[idDlg].szNameId,
                            (LOWORD(wParam)==ID_CREATE)?-1:idObjToDelete))
                    {
                        return TRUE;
                    }
                }
                //Creation de l'item

            //Fall through pour fermeture


            if(!MySaveCache(idDlg,myValues,hDlg))
                MessageBox(hDlg,TEXT("Unable to save theses datas"),NULL,0);
            //sortie
                if(infoDlg->iTypeDlg==MODAL)
                   EndDialog(hDlg,TRUE);
                else
                    DeleteWindow(hDlg,hDlgModeless);

                if(LOWORD(wParam)==ID_CREATE  ||
                   LOWORD(wParam)==ID_APPLY)
                {

                    //ici hDlg n'est plus valide car deja supprime
                    //Creer l'objet
                    //CreateObjet se chargera automatiquement d'ajouter l'objet
                    //a l'univers si la creation a reussi.

                    if(LOWORD(wParam)==ID_APPLY)
                    {
                        int iIndex=GetObjIndex(idObjToDelete);
                        if(iIndex==-1)
                        {
                            MyPerror("UnkNoewn error:iIndex==-1 in DIALOGproc CASE DLG_BUTTON");
                            exit(EXIT_FAILURE);
                        }

                        if(isGroupBoxObj(iIndex))
                        {

                            g_hwndLastChildGB=uObjet[iIndex].hwndChildGB;
                            if(!g_hwndLastChildGB)
                            {
                                MyPerror("\n error Unknown:!g_hwndLastChildGB in dialogProc");
                                exit(EXIT_FAILURE);
                            }

                        }
                        MyUpdateDimObjUniv();
                        MyDeleteObj(idObjToDelete);
                    }

                    hwndObjet=CreateObjet("button",DLG_BUTTON,hwndOwner);
                    if(!hwndObjet)
                    {
                        MessageBox(hwndOwner,"Unable to create this object",NULL,
                                   MB_ICONERROR);
                    }

                }

                return TRUE;


            case ID_PREV:

                if(!MySaveCache(idDlg,myValues,hDlg))
                    MessageBox(hDlg,TEXT("Unable to save theses datas"),NULL,0);

                if(!OpenDialogBox(MODAL,DLG_CHOOSETYPEOBJET,
                infoDlg->hWndOwner,infoDlg->hinst,hDlg,infoDlg->iTypeDlg))
                {
                    MessageBox(NULL,TEXT("Unable to open dialog"),
                    TEXT("ERROR"),MB_ICONERROR);
                }

                return TRUE;

            case ID_EDIT_TITLE:
                if(HIWORD(wParam)==EN_CHANGE)
                {
                    char buf[MAXDLGCHAR];
                    GetWindowText((HWND)lParam,buf,MAXDLGCHAR);
                    SetWindowText(GetDlgItem(GetDlgItem(hDlg,ID_ASPECT),1),buf);
                    strcpy(mycache[idDlg].szTitle,buf);
                }
                return TRUE;

            case IDE_NAMEID:
                if(HIWORD(wParam)==EN_CHANGE)
                {
                    char buf[MAXDLGCHAR];
                    GetWindowText((HWND)lParam,buf,MAXDLGCHAR);
                    strcpy(mycache[idDlg].szNameId,buf);
                }
                return TRUE;

            case ID_X_DEC:

                Decrement(idDlg,ID_STATIC_X,&myValues,hDlg);
                MyApplyCache(idDlg,&myValues,hDlg);
                return  TRUE;
            case ID_Y_DEC:
                Decrement(idDlg,ID_STATIC_Y,&myValues,hDlg);
                MyApplyCache(idDlg,&myValues,hDlg);
                return  TRUE;
            case ID_W_DEC:
                Decrement(idDlg,ID_STATIC_W,&myValues,hDlg);
               MyApplyCache(idDlg,&myValues,hDlg);
                return  TRUE;
            case ID_H_DEC:
                Decrement(idDlg,ID_STATIC_H,&myValues,hDlg);
                MyApplyCache(idDlg,&myValues,hDlg);
                return  TRUE;

            case ID_X_INC:
                Increment(idDlg,ID_STATIC_X,&myValues,hDlg);
                MyApplyCache(idDlg,&myValues,hDlg);
                return TRUE;
            case ID_Y_INC:
                Increment(idDlg,ID_STATIC_Y,&myValues,hDlg);
                 MyApplyCache(idDlg,&myValues,hDlg);
                return TRUE;
            case ID_W_INC:
                Increment(idDlg,ID_STATIC_W,&myValues,hDlg);
                 MyApplyCache(idDlg,&myValues,hDlg);
                return TRUE;
            case ID_H_INC:
                Increment(idDlg,ID_STATIC_H,&myValues,hDlg);
                 MyApplyCache(idDlg,&myValues,hDlg);
                return TRUE;

            }
            break;//fin case WM_COMMAND

            case WM_CTLCOLOREDIT:
            case WM_CTLCOLORSTATIC:
            {
                HWND hwndObjet=(HWND)lParam;
                if(!hwndObjet)
                {
                    MyPerror("mdfoh");
                    break;
                }
                static HBRUSH hBrush;
                if(GetDlgCtrlID(hwndObjet)==IDE_STYLE_DESC)
                {
                    //Gestion des colors de la zone edit qui affiche la description des styles
                    DeleteObject(hBrush);
                    hBrush=CreateSolidBrush(RGB(255,255,255));

                    return (LRESULT)hBrush;
                }
                else if(GetDlgCtrlID(hwndObjet)==IDS_STYLE_TO_DESC)
                {
                    //Gestion des colors de la zone static qui affiche
                    //Le nom du style a decrire
                    SetTextColor((HDC)wParam,RGB(100,100,255));
                    SetBkColor((HDC)wParam,RGB(240,240,240));
                    DeleteObject(hBrush);
                    hBrush=CreateSolidBrush(RGB(240,240,240));//gris dlg
                    return (LRESULT)hBrush;
                    //break;
                }


            }
                break;//fin case WM_COLOREDIT

        }//FIN switch(msg)
    }//fin if(infoDlg->idDlg==DLG_BUTTON

    //Gestion DLG_LISTBOX

    else if(infoDlg->idDlg==DLG_LISTBOX)
    {

        int idDlg=infoDlg->idDlg;


        if(isProjetLoadedWithOpenItem && isFirstTime[idDlg]&&isTypeModDlg)
            isFirstTime[idDlg]=FALSE;

        switch(msg)
        {
            case WM_INITDIALOG:
                if(!isTypeModDlg&& !isFirstTime[idDlg])
                {
                    mycache[idDlg].iLastX=ptWhereCreate.x;
                    mycache[idDlg].iLastY=ptWhereCreate.y;

                    MyUpDateDimChar(g_hwndParentObjToSet);
                    mycache[idDlg].cxChar=cxChar;
                    mycache[idDlg].cyChar=cyChar;
                    mycache[idDlg].font.isFontParent=TRUE;
                    //defini une chaine automatiquement et unique pour le nouveau item a creer
                    MySetNameId(mycache[idDlg].szNameId,idDlg);
                    MyInitFshouldAutosize(&mycache[idDlg].fShouldAutosize,FALSE);
                }

                if(isFirstTime[idDlg]==TRUE)
               {    isFirstTime[idDlg]=FALSE;
                        //Valeurs de depart du bouton a creer
                    //Utilie, pour adapter les dimensions a la taille de la font
                    mycache[idDlg].iLastX=ptWhereCreate.x;
                    mycache[idDlg].iLastY=ptWhereCreate.y;
                    mycache[idDlg].iLastH=5;
                    mycache[idDlg].iLastW=6;
                    mycache[idDlg].cxRest=0;
                    mycache[idDlg].cyRest=0;

                    MyUpDateDimChar(g_hwndParentObjToSet);

                    mycache[idDlg].cxChar=cxChar;
                    mycache[idDlg].cyChar=cyChar;

                    mycache[idDlg].font.isFontParent=TRUE;
                    mycache[idDlg].mycolorobj[BK].iIndexDefColor=1;//BK_PARENT
                    mycache[idDlg].mycolorobj[TXT].iIndexDefColor=0;//TXT_PARENT
                    mycache[idDlg].mycolorobj[BK].isDefChoosen=TRUE;
                    mycache[idDlg].mycolorobj[TXT].isDefChoosen=TRUE;
                    mycache[idDlg].mycolorobj[BK].defColor=COLOR_BK_PARENT;
                    mycache[idDlg].mycolorobj[TXT].defColor=COLOR_TXT_PARENT;

                    mycache[idDlg].dwLastStylesChosen=WS_VISIBLE|
                    WS_CHILD|LBS_STANDARD;

                    strcpy(mycache[idDlg].szTitle,"HELLO");
                    MySetNameId(mycache[idDlg].szNameId,idDlg);

                    strcpy(mycache[idDlg].szLastStylesChosen,
                        "WS_VISIBLE|WS_CHILD|LBS_STANDARD|");
                    MyInitFshouldAutosize(&mycache[idDlg].fShouldAutosize,FALSE);
               }
                    //Indique qu'il s'agit du premier chargement
                isFirstLoad[idDlg]=TRUE;
            //Charger les donnees contenues dans le cache
            if(!MyLoadCache(idDlg,&myValues,hDlg))
                MessageBox(hDlg,TEXT("Unable to Load last datas"),NULL,0);
            SetFocus(GetDlgItem(hDlg,ID_CREATE));

            return FALSE;//Force le systeme a ne pas attribuer le focus lui meme

        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
            case ID_COLOR:
                MyManageColor(idDlg,hDlg);
                return TRUE;
            case  ID_FONT:
                MyManageFont(idDlg,hDlg);
                return TRUE;

            case ID_APPLY:
            case IDCANCEL:
            case ID_CREATE:
                if(LOWORD(wParam)==ID_APPLY ||
                   LOWORD(wParam)==ID_CREATE)
                {
                    //verifier la validite de szNameId
                    if(!isValidNameId(mycache[idDlg].szNameId,
                            (LOWORD(wParam)==ID_CREATE)?-1:idObjToDelete))
                    {
                        return TRUE;
                    }
                }

                //Creation de l'item

            //Fall through pour fermeture


            if(!MySaveCache(idDlg,myValues,hDlg))
                MessageBox(hDlg,TEXT("Unable to save theses datas"),NULL,0);
            //sortie
                if(infoDlg->iTypeDlg==MODAL)
                   EndDialog(hDlg,TRUE);
                else
                    DeleteWindow(hDlg,hDlgModeless);

                if(LOWORD(wParam)==ID_CREATE  ||
                   LOWORD(wParam)==ID_APPLY)
                {

                    //ici hDlg n'est plus valide car deja supprime
                    //Creer l'objet
                    //CreateObjet se chargera automatiquement d'ajouter l'objet
                    //a l'univers si la creation a reussi.

                    if(LOWORD(wParam)==ID_APPLY)
                    {
                        MyDeleteObj(idObjToDelete);
                    }
                    hwndObjet=CreateObjet("listbox",DLG_LISTBOX,hwndOwner);
                    if(!hwndObjet)
                    {
                        MessageBox(hwndOwner,"Unable to create this object",NULL,
                                   MB_ICONERROR);
                    }

                }

                return TRUE;

            case ID_PREV:

                if(!MySaveCache(idDlg,myValues,hDlg))
                    MessageBox(hDlg,TEXT("Unable to save theses datas"),NULL,0);

                if(!OpenDialogBox(MODAL,DLG_CHOOSETYPEOBJET,
                infoDlg->hWndOwner,infoDlg->hinst,hDlg,infoDlg->iTypeDlg))
                {
                    MessageBox(NULL,TEXT("Unable to open dialog"),
                    TEXT("ERROR"),MB_ICONERROR);
                }

                return TRUE;

            case ID_EDIT_TITLE:
                if(HIWORD(wParam)==EN_CHANGE)
                {
                    char buf[MAXDLGCHAR];
                    GetWindowText((HWND)lParam,buf,MAXDLGCHAR);
                    SetWindowText(GetDlgItem(GetDlgItem(hDlg,ID_ASPECT),1),buf);
                    strcpy(mycache[idDlg].szTitle,buf);
                }
                return TRUE;

            case IDE_NAMEID:
                if(HIWORD(wParam)==EN_CHANGE)
                {
                    char buf[MAXDLGCHAR];
                    GetWindowText((HWND)lParam,buf,MAXDLGCHAR);
                    strcpy(mycache[idDlg].szNameId,buf);
                }
                return TRUE;

            case ID_X_DEC:

                Decrement(idDlg,ID_STATIC_X,&myValues,hDlg);
                MyApplyCache(idDlg,&myValues,hDlg);
                return  TRUE;
            case ID_Y_DEC:
                Decrement(idDlg,ID_STATIC_Y,&myValues,hDlg);
                MyApplyCache(idDlg,&myValues,hDlg);
                return  TRUE;
            case ID_W_DEC:
                Decrement(idDlg,ID_STATIC_W,&myValues,hDlg);
               MyApplyCache(idDlg,&myValues,hDlg);
                return  TRUE;
            case ID_H_DEC:
                Decrement(idDlg,ID_STATIC_H,&myValues,hDlg);
                MyApplyCache(idDlg,&myValues,hDlg);
                return  TRUE;

            case ID_X_INC:
                Increment(idDlg,ID_STATIC_X,&myValues,hDlg);
                MyApplyCache(idDlg,&myValues,hDlg);
                return TRUE;
            case ID_Y_INC:
                Increment(idDlg,ID_STATIC_Y,&myValues,hDlg);
                 MyApplyCache(idDlg,&myValues,hDlg);
                return TRUE;
            case ID_W_INC:
                Increment(idDlg,ID_STATIC_W,&myValues,hDlg);
                 MyApplyCache(idDlg,&myValues,hDlg);
                return TRUE;
            case ID_H_INC:
                Increment(idDlg,ID_STATIC_H,&myValues,hDlg);
                 MyApplyCache(idDlg,&myValues,hDlg);
                return TRUE;

            }
            break;//fin case WM_COMMAND

            case WM_CTLCOLOREDIT:
            case WM_CTLCOLORSTATIC:
            {
                HWND hwndObjet=(HWND)lParam;
                if(!hwndObjet)
                {
                    MyPerror("mdfoh");
                    break;
                }
                static HBRUSH hBrush;
                if(GetDlgCtrlID(hwndObjet)==IDE_STYLE_DESC)
                {
                    //Gestion des colors de la zone edit qui affiche la description des styles
                    DeleteObject(hBrush);
                    hBrush=CreateSolidBrush(RGB(255,255,255));

                    return (LRESULT)hBrush;
                }
                else if(GetDlgCtrlID(hwndObjet)==IDS_STYLE_TO_DESC)
                {
                    //Gestion des colors de la zone static qui affiche
                    //Le nom du style a decrire
                    SetTextColor((HDC)wParam,RGB(100,100,255));
                    SetBkColor((HDC)wParam,RGB(240,240,240));
                    DeleteObject(hBrush);
                    hBrush=CreateSolidBrush(RGB(240,240,240));//gris dlg
                    return (LRESULT)hBrush;
                    //break;
                }

            }
                break;//fin case WM_COLOREDIT

        }//FIN switch(msg)
    }//fin if(infoDlg->idDlg==DLG_LISTBOX)


    //Gestion DLG_EDITBOX

    else if(infoDlg->idDlg==DLG_EDITBOX)
    {
        int idDlg=infoDlg->idDlg;


        if(isProjetLoadedWithOpenItem && isFirstTime[idDlg]&&isTypeModDlg)
            isFirstTime[idDlg]=FALSE;

        switch(msg)
        {
            case WM_INITDIALOG:

                if(!isTypeModDlg&& !isFirstTime[idDlg])
                {
                    mycache[idDlg].iLastX=ptWhereCreate.x;
                    mycache[idDlg].iLastY=ptWhereCreate.y;

                    MyUpDateDimChar(g_hwndParentObjToSet);
                    mycache[idDlg].cxChar=cxChar;
                    mycache[idDlg].cyChar=cyChar;

                    mycache[idDlg].font.isFontParent=TRUE;
                    //defini une chaine automatiquement et unique pour le nouveau item a creer
                    MySetNameId(mycache[idDlg].szNameId,idDlg);
                    MyInitFshouldAutosize(&mycache[idDlg].fShouldAutosize,FALSE);
                }

                if(isFirstTime[idDlg]==TRUE)
               {    isFirstTime[idDlg]=FALSE;
                        //Valeurs de depart du bouton a creer
                    //Utilie, pour adapter les dimensions a la taille de la font

                    mycache[idDlg].iLastX=ptWhereCreate.x;
                    mycache[idDlg].iLastY=ptWhereCreate.y;
                    mycache[idDlg].iLastH=2;
                    mycache[idDlg].iLastW=6;
                    mycache[idDlg].cxRest=0;
                    mycache[idDlg].cyRest=0;

                    MyUpDateDimChar(g_hwndParentObjToSet);

                    mycache[idDlg].cxChar=cxChar;
                    mycache[idDlg].cyChar=cyChar;

                    mycache[idDlg].font.isFontParent=TRUE;
                    mycache[idDlg].mycolorobj[BK].iIndexDefColor=1;//BK_PARENT
                    mycache[idDlg].mycolorobj[TXT].iIndexDefColor=0;//TXT_PARENT
                    mycache[idDlg].mycolorobj[BK].isDefChoosen=TRUE;
                    mycache[idDlg].mycolorobj[TXT].isDefChoosen=TRUE;
                    mycache[idDlg].mycolorobj[BK].defColor=COLOR_BK_PARENT;
                    mycache[idDlg].mycolorobj[TXT].defColor=COLOR_TXT_PARENT;

                    mycache[idDlg].dwLastStylesChosen=WS_VISIBLE|
                    WS_CHILD|WS_BORDER;

                    strcpy(mycache[idDlg].szTitle,"EDIT");
                    MySetNameId(mycache[idDlg].szNameId,idDlg);

                    strcpy(mycache[idDlg].szLastStylesChosen,
                        "WS_VISIBLE|WS_CHILD|WS_BORDER|");
                    MyInitFshouldAutosize(&mycache[idDlg].fShouldAutosize,FALSE);
               }
                    //Indique qu'il s'agit du premier chargement
                isFirstLoad[idDlg]=TRUE;
            //Charger les donnees contenues dans le cache
            if(!MyLoadCache(idDlg,&myValues,hDlg))
                MessageBox(hDlg,TEXT("Unable to Load last datas"),NULL,0);
            SetFocus(GetDlgItem(hDlg,ID_CREATE));

            return FALSE;//Force le systeme a ne pas attribuer le focus lui meme

        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
            case ID_COLOR:
                MyManageColor(idDlg,hDlg);
                return TRUE;
            case  ID_FONT:
                MyManageFont(idDlg,hDlg);
                return TRUE;

            case ID_APPLY:
            case IDCANCEL:
            case ID_CREATE:
                if(LOWORD(wParam)==ID_APPLY ||
                   LOWORD(wParam)==ID_CREATE)
                {
                    //verifier la validite de szNameId
                    if(!isValidNameId(mycache[idDlg].szNameId,
                            (LOWORD(wParam)==ID_CREATE)?-1:idObjToDelete))
                    {
                        return TRUE;
                    }
                }

                //Creation de l'item

            //Fall through pour fermeture


            if(!MySaveCache(idDlg,myValues,hDlg))
                MessageBox(hDlg,TEXT("Unable to save theses datas"),NULL,0);
            //sortie
                if(infoDlg->iTypeDlg==MODAL)
                   EndDialog(hDlg,TRUE);
                else
                    DeleteWindow(hDlg,hDlgModeless);

                if(LOWORD(wParam)==ID_CREATE  ||
                   LOWORD(wParam)==ID_APPLY)
                {

                    //ici hDlg n'est plus valide car deja supprime
                    //Creer l'objet
                    //CreateObjet se chargera automatiquement d'ajouter l'objet
                    //a l'univers si la creation a reussi.

                    if(LOWORD(wParam)==ID_APPLY)
                    {
                        MyDeleteObj(idObjToDelete);
                    }
                    hwndObjet=CreateObjet("edit",DLG_EDITBOX,hwndOwner);
                    if(!hwndObjet)
                    {
                        MessageBox(hwndOwner,"Unable to create this object",NULL,
                                   MB_ICONERROR);
                    }

                }

                return TRUE;


            case ID_PREV:

                if(!MySaveCache(idDlg,myValues,hDlg))
                    MessageBox(hDlg,TEXT("Unable to save theses datas"),NULL,0);

                if(!OpenDialogBox(MODAL,DLG_CHOOSETYPEOBJET,
                infoDlg->hWndOwner,infoDlg->hinst,hDlg,infoDlg->iTypeDlg))
                {
                    MessageBox(NULL,TEXT("Unable to open dialog"),
                    TEXT("ERROR"),MB_ICONERROR);
                }

                return TRUE;

            case ID_EDIT_TITLE:
                if(HIWORD(wParam)==EN_CHANGE)
                {
                    char buf[MAXDLGCHAR];
                    GetWindowText((HWND)lParam,buf,MAXDLGCHAR);
                    SetWindowText(GetDlgItem(GetDlgItem(hDlg,ID_ASPECT),1),buf);
                    strcpy(mycache[idDlg].szTitle,buf);
                }
                return TRUE;


            case IDE_NAMEID:
                if(HIWORD(wParam)==EN_CHANGE)
                {
                    char buf[MAXDLGCHAR];
                    GetWindowText((HWND)lParam,buf,MAXDLGCHAR);
                    strcpy(mycache[idDlg].szNameId,buf);
                }
                return TRUE;

            case ID_X_DEC:

                Decrement(idDlg,ID_STATIC_X,&myValues,hDlg);
                MyApplyCache(idDlg,&myValues,hDlg);
                return  TRUE;
            case ID_Y_DEC:
                Decrement(idDlg,ID_STATIC_Y,&myValues,hDlg);
                MyApplyCache(idDlg,&myValues,hDlg);
                return  TRUE;
            case ID_W_DEC:
                Decrement(idDlg,ID_STATIC_W,&myValues,hDlg);
               MyApplyCache(idDlg,&myValues,hDlg);
                return  TRUE;
            case ID_H_DEC:
                Decrement(idDlg,ID_STATIC_H,&myValues,hDlg);
                MyApplyCache(idDlg,&myValues,hDlg);
                return  TRUE;

            case ID_X_INC:
                Increment(idDlg,ID_STATIC_X,&myValues,hDlg);
                MyApplyCache(idDlg,&myValues,hDlg);
                return TRUE;
            case ID_Y_INC:
                Increment(idDlg,ID_STATIC_Y,&myValues,hDlg);
                 MyApplyCache(idDlg,&myValues,hDlg);
                return TRUE;
            case ID_W_INC:
                Increment(idDlg,ID_STATIC_W,&myValues,hDlg);
                 MyApplyCache(idDlg,&myValues,hDlg);
                return TRUE;
            case ID_H_INC:
                Increment(idDlg,ID_STATIC_H,&myValues,hDlg);
                 MyApplyCache(idDlg,&myValues,hDlg);
                return TRUE;

            }
            break;//fin case WM_COMMAND
            case WM_CTLCOLOREDIT:
            case WM_CTLCOLORSTATIC:
            {
                HWND hwndObjet=(HWND)lParam;
                if(!hwndObjet)
                {
                    MyPerror("mdfoh");
                    break;
                }
                static HBRUSH hBrush;
                if(GetDlgCtrlID(hwndObjet)==IDE_STYLE_DESC)
                {
                    //Gestion des colors de la zone edit qui affiche la description des styles
                    DeleteObject(hBrush);
                    hBrush=CreateSolidBrush(RGB(255,255,255));

                    return (LRESULT)hBrush;
                }
                else if(GetDlgCtrlID(hwndObjet)==IDS_STYLE_TO_DESC)
                {
                    //Gestion des colors de la zone static qui affiche
                    //Le nom du style a decrire
                    SetTextColor((HDC)wParam,RGB(100,100,255));
                    SetBkColor((HDC)wParam,RGB(240,240,240));
                    DeleteObject(hBrush);
                    hBrush=CreateSolidBrush(RGB(240,240,240));//gris dlg
                    return (LRESULT)hBrush;
                    //break;
                }


            }
                break;//fin case WM_COLOREDIT


        }//FIN switch(msg)
    }//fin if(infoDlg->idDlg==DLG_EDITBOX)


    //Gestion DLG_COMBOBOX

    else if(infoDlg->idDlg==DLG_COMBOBOX)
    {

        int idDlg=infoDlg->idDlg;


        if(isProjetLoadedWithOpenItem && isFirstTime[idDlg]&&isTypeModDlg)
            isFirstTime[idDlg]=FALSE;

        switch(msg)
        {
            case WM_INITDIALOG:

                if(!isTypeModDlg&& !isFirstTime[idDlg])
                {
                    mycache[idDlg].iLastX=ptWhereCreate.x;
                    mycache[idDlg].iLastY=ptWhereCreate.y;

                    MyUpDateDimChar(g_hwndParentObjToSet);
                    mycache[idDlg].cxChar=cxChar;
                    mycache[idDlg].cyChar=cyChar;
                    mycache[idDlg].font.isFontParent=TRUE;
                    //defini une chaine automatiquement et unique pour le nouveau item a creer
                    MySetNameId(mycache[idDlg].szNameId,idDlg);
                    MyInitFshouldAutosize(&mycache[idDlg].fShouldAutosize,FALSE);
                }
                if(isFirstTime[idDlg]==TRUE)
               {    isFirstTime[idDlg]=FALSE;
                        //Valeurs de depart du bouton a creer
                    //Utilie, pour adapter les dimensions a la taille de la font
                    mycache[idDlg].iLastX=ptWhereCreate.x;
                    mycache[idDlg].iLastY=ptWhereCreate.y;
                    mycache[idDlg].iLastH=1;
                    mycache[idDlg].iLastW=10;
                    mycache[idDlg].cxRest=0;
                    mycache[idDlg].cyRest=0;

                    MyUpDateDimChar(g_hwndParentObjToSet);

                    mycache[idDlg].cxChar=cxChar;
                    mycache[idDlg].cyChar=5*cyChar;

                    mycache[idDlg].font.isFontParent=TRUE;
                    mycache[idDlg].mycolorobj[BK].iIndexDefColor=1;//BK_PARENT
                    mycache[idDlg].mycolorobj[TXT].iIndexDefColor=0;//TXT_PARENT
                    mycache[idDlg].mycolorobj[BK].isDefChoosen=TRUE;
                    mycache[idDlg].mycolorobj[TXT].isDefChoosen=TRUE;
                    mycache[idDlg].mycolorobj[BK].defColor=COLOR_BK_PARENT;
                    mycache[idDlg].mycolorobj[TXT].defColor=COLOR_TXT_PARENT;

                    mycache[idDlg].dwLastStylesChosen=WS_VISIBLE|
                    WS_CHILD|WS_BORDER|CBS_DROPDOWN |
                CBS_HASSTRINGS|CBS_SORT|
                WS_VSCROLL;

                    strcpy(mycache[idDlg].szTitle,"HELLO");
                    MySetNameId(mycache[idDlg].szNameId,idDlg);

                    strcpy(mycache[idDlg].szLastStylesChosen,
                        "WS_VISIBLE|WS_CHILD|WS_BORDER|CBS_DROPDOWN |CBS_HASSTRINGS|CBS_SORT|WS_VSCROLL|");
                    MyInitFshouldAutosize(&mycache[idDlg].fShouldAutosize,FALSE);
               }
                    //Indique qu'il s'agit du premier chargement
                isFirstLoad[idDlg]=TRUE;
            //Charger les donnees contenues dans le cache
            if(!MyLoadCache(idDlg,&myValues,hDlg))
                MessageBox(hDlg,TEXT("Unable to Load last datas"),NULL,0);
            SetFocus(GetDlgItem(hDlg,ID_CREATE));

            return FALSE;//Force le systeme a ne pas attribuer le focus lui meme

        case WM_COMMAND:

            switch(LOWORD(wParam))
            {
            case ID_COLOR:
                MyManageColor(idDlg,hDlg);
                return TRUE;
            case  ID_FONT:
                MyManageFont(idDlg,hDlg);
                return TRUE;

            case ID_APPLY:
            case IDCANCEL:
            case ID_CREATE:
                if(LOWORD(wParam)==ID_APPLY ||
                   LOWORD(wParam)==ID_CREATE)
                {
                    //verifier la validite de szNameId
                    if(!isValidNameId(mycache[idDlg].szNameId,
                            (LOWORD(wParam)==ID_CREATE)?-1:idObjToDelete))
                    {
                        return TRUE;
                    }
                }

                //Creation de l'item

            //Fall through pour fermeture


            if(!MySaveCache(idDlg,myValues,hDlg))
                MessageBox(hDlg,TEXT("Unable to save theses datas"),NULL,0);
            //sortie
                if(infoDlg->iTypeDlg==MODAL)
                   EndDialog(hDlg,TRUE);
                else
                    DeleteWindow(hDlg,hDlgModeless);

                if(LOWORD(wParam)==ID_CREATE  ||
                   LOWORD(wParam)==ID_APPLY)
                {

                    //ici hDlg n'est plus valide car deja supprime
                    //Creer l'objet
                    //CreateObjet se chargera automatiquement d'ajouter l'objet
                    //a l'univers si la creation a reussi.

                    if(LOWORD(wParam)==ID_APPLY)
                    {
                        MyDeleteObj(idObjToDelete);
                    }
                    hwndObjet=CreateObjet(WC_COMBOBOX,DLG_COMBOBOX,hwndOwner);
                    if(!hwndObjet)
                    {
                        MessageBox(hwndOwner,"Unable to create this object",NULL,
                                   MB_ICONERROR);
                    }

                }

                return TRUE;

            case ID_PREV:

                if(!MySaveCache(idDlg,myValues,hDlg))
                    MessageBox(hDlg,TEXT("Unable to save theses datas"),NULL,0);

                if(!OpenDialogBox(MODAL,DLG_CHOOSETYPEOBJET,
                infoDlg->hWndOwner,infoDlg->hinst,hDlg,infoDlg->iTypeDlg))
                {
                    MessageBox(NULL,TEXT("Unable to open dialog"),
                    TEXT("ERROR"),MB_ICONERROR);
                }

                return TRUE;

            case ID_EDIT_TITLE:
                if(HIWORD(wParam)==EN_CHANGE)
                {
                    char buf[MAXDLGCHAR];
                    GetWindowText((HWND)lParam,buf,MAXDLGCHAR);
                    SetWindowText(GetDlgItem(GetDlgItem(hDlg,ID_ASPECT),1),buf);
                    strcpy(mycache[idDlg].szTitle,buf);
                }
                return TRUE;


            case IDE_NAMEID:
                if(HIWORD(wParam)==EN_CHANGE)
                {
                    char buf[MAXDLGCHAR];
                    GetWindowText((HWND)lParam,buf,MAXDLGCHAR);
                    strcpy(mycache[idDlg].szNameId,buf);
                }
                return TRUE;

            case ID_X_DEC:

                Decrement(idDlg,ID_STATIC_X,&myValues,hDlg);
                MyApplyCache(idDlg,&myValues,hDlg);
                return  TRUE;
            case ID_Y_DEC:
                Decrement(idDlg,ID_STATIC_Y,&myValues,hDlg);
                MyApplyCache(idDlg,&myValues,hDlg);
                return  TRUE;
            case ID_W_DEC:
                Decrement(idDlg,ID_STATIC_W,&myValues,hDlg);
               MyApplyCache(idDlg,&myValues,hDlg);
                return  TRUE;
            case ID_H_DEC:
                Decrement(idDlg,ID_STATIC_H,&myValues,hDlg);
                MyApplyCache(idDlg,&myValues,hDlg);
                return  TRUE;

            case ID_X_INC:
                Increment(idDlg,ID_STATIC_X,&myValues,hDlg);
                MyApplyCache(idDlg,&myValues,hDlg);
                return TRUE;
            case ID_Y_INC:
                Increment(idDlg,ID_STATIC_Y,&myValues,hDlg);
                 MyApplyCache(idDlg,&myValues,hDlg);
                return TRUE;
            case ID_W_INC:
                Increment(idDlg,ID_STATIC_W,&myValues,hDlg);
                 MyApplyCache(idDlg,&myValues,hDlg);
                return TRUE;
            case ID_H_INC:
                Increment(idDlg,ID_STATIC_H,&myValues,hDlg);
                 MyApplyCache(idDlg,&myValues,hDlg);
                return TRUE;

            }
            break;//fin case WM_COMMAND

        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORSTATIC:
            {
                HWND hwndObjet=(HWND)lParam;
                if(!hwndObjet)
                {
                    MyPerror("mdfoh");
                    break;
                }
                static HBRUSH hBrush;
                if(GetDlgCtrlID(hwndObjet)==IDE_STYLE_DESC)
                {
                    //Gestion des colors de la zone edit qui affiche la description des styles
                    DeleteObject(hBrush);
                    hBrush=CreateSolidBrush(RGB(255,255,255));

                    return (LRESULT)hBrush;
                }
                else if(GetDlgCtrlID(hwndObjet)==IDS_STYLE_TO_DESC)
                {
                    //Gestion des colors de la zone static qui affiche
                    //Le nom du style a decrire
                    SetTextColor((HDC)wParam,RGB(100,100,255));
                    SetBkColor((HDC)wParam,RGB(240,240,240));
                    DeleteObject(hBrush);
                    hBrush=CreateSolidBrush(RGB(240,240,240));//gris dlg
                    return (LRESULT)hBrush;
                    //break;
                }

            }
            break;//fin case WM_COLOREDIT


        }//FIN switch(msg)
    }//fin if(infoDlg->idDlg==DLG_COMBOBOX)


    //Gestion DLG_STATIC

    else if(infoDlg->idDlg==DLG_STATIC)
    {
        int idDlg=infoDlg->idDlg;


        if(isProjetLoadedWithOpenItem && isFirstTime[idDlg]&&isTypeModDlg)
            isFirstTime[idDlg]=FALSE;

        switch(msg)
        {
            case WM_INITDIALOG:
                if(!isTypeModDlg&& !isFirstTime[idDlg])
                {
                    mycache[idDlg].iLastX=ptWhereCreate.x;
                    mycache[idDlg].iLastY=ptWhereCreate.y;

                    MyUpDateDimChar(g_hwndParentObjToSet);
                    mycache[idDlg].cxChar=cxChar;
                    mycache[idDlg].cyChar=cyChar;
                    mycache[idDlg].font.isFontParent=TRUE;
                    //defini une chaine automatiquement et unique pour le nouveau item a creer
                    MySetNameId(mycache[idDlg].szNameId,idDlg);
                    MyInitFshouldAutosize(&mycache[idDlg].fShouldAutosize,FALSE);
                }
               if(isFirstTime[idDlg]==TRUE)
               {    isFirstTime[idDlg]=FALSE;

                        //Valeurs de depart du static a creer
                    //Utilie, pour adapter les dimensions a la taille de la font

                    mycache[idDlg].iLastX=ptWhereCreate.x;
                    mycache[idDlg].iLastY=ptWhereCreate.y;
                    mycache[idDlg].iLastH=2;
                    mycache[idDlg].iLastW=8;
                    mycache[idDlg].cxRest=0;
                    mycache[idDlg].cyRest=0;

                    MyUpDateDimChar(g_hwndParentObjToSet);

                    mycache[idDlg].cxChar=cxChar;
                    mycache[idDlg].cyChar=cyChar;

                    mycache[idDlg].font.isFontParent=TRUE;
                    mycache[idDlg].mycolorobj[BK].iIndexDefColor=1;//BK_PARENT
                    mycache[idDlg].mycolorobj[TXT].iIndexDefColor=0;//TXT_PARENT
                    mycache[idDlg].mycolorobj[BK].isDefChoosen=TRUE;
                    mycache[idDlg].mycolorobj[TXT].isDefChoosen=TRUE;
                    mycache[idDlg].mycolorobj[BK].defColor=COLOR_BK_PARENT;
                    mycache[idDlg].mycolorobj[TXT].defColor=COLOR_TXT_PARENT;
                    mycache[idDlg].dwLastStylesChosen=WS_VISIBLE|
                    WS_CHILD|WS_BORDER;

                    strcpy(mycache[idDlg].szTitle,"STATIC");
                    MySetNameId(mycache[idDlg].szNameId,idDlg);

                    strcpy(mycache[idDlg].szLastStylesChosen,
                        "WS_VISIBLE|WS_CHILD|WS_BORDER|");
                    MyInitFshouldAutosize(&mycache[idDlg].fShouldAutosize,FALSE);
               }
                    //Indique qu'il s'agit du premier chargement
                isFirstLoad[idDlg]=TRUE;
            //Charger les donnees contenues dans le cache

            if(!MyLoadCache(idDlg,&myValues,hDlg))
                MessageBox(hDlg,TEXT("Unable to Load last datas"),NULL,0);
            SetFocus(GetDlgItem(hDlg,ID_CREATE));

            return FALSE;//Force le systeme a ne pas attribuer le focus lui meme

        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
            case ID_COLOR:
                MyManageColor(idDlg,hDlg);
                return TRUE;
            case  ID_FONT:
                MyManageFont(idDlg,hDlg);
                return TRUE;

            case ID_APPLY:
            case IDCANCEL:
            case ID_CREATE:
                if(LOWORD(wParam)==ID_APPLY ||
                   LOWORD(wParam)==ID_CREATE)
                {
                    //verifier la validite de szNameId
                    if(!isValidNameId(mycache[idDlg].szNameId,
                            (LOWORD(wParam)==ID_CREATE)?-1:idObjToDelete))
                    {
                        return TRUE;
                    }
                }

                //Creation de l'item

            //Fall through pour fermeture


            if(!MySaveCache(idDlg,myValues,hDlg))
                MessageBox(hDlg,TEXT("Unable to save theses datas"),NULL,0);
            //sortie
                if(infoDlg->iTypeDlg==MODAL)
                   EndDialog(hDlg,TRUE);
                else
                    DeleteWindow(hDlg,hDlgModeless);

                if(LOWORD(wParam)==ID_CREATE  ||
                   LOWORD(wParam)==ID_APPLY)
                {

                    //ici hDlg n'est plus valide car deja supprime
                    //Creer l'objet
                    //CreateObjet se chargera automatiquement d'ajouter l'objet
                    //a l'univers si la creation a reussi.

                    if(LOWORD(wParam)==ID_APPLY)
                    {
                        MyDeleteObj(idObjToDelete);
                    }
                    hwndObjet=CreateObjet("static",DLG_STATIC,hwndOwner);
                    if(!hwndObjet)
                    {
                        MessageBox(hwndOwner,"Unable to create this object",NULL,
                                   MB_ICONERROR);
                    }

                }

                return TRUE;

            case ID_PREV:

                if(!MySaveCache(idDlg,myValues,hDlg))
                    MessageBox(hDlg,TEXT("Unable to save theses datas"),NULL,0);

                if(!OpenDialogBox(MODAL,DLG_CHOOSETYPEOBJET,
                infoDlg->hWndOwner,infoDlg->hinst,hDlg,infoDlg->iTypeDlg))
                {
                    MessageBox(NULL,TEXT("Unable to open dialog"),
                    TEXT("ERROR"),MB_ICONERROR);
                }

                return TRUE;

            case ID_EDIT_TITLE:
                if(HIWORD(wParam)==EN_CHANGE)
                {
                    char buf[MAXDLGCHAR];
                    GetWindowText((HWND)lParam,buf,MAXDLGCHAR);
                    SetWindowText(GetDlgItem(GetDlgItem(hDlg,ID_ASPECT),1),buf);
                    strcpy(mycache[idDlg].szTitle,buf);
                }
                return TRUE;

            case IDE_NAMEID:
                if(HIWORD(wParam)==EN_CHANGE)
                {
                    char buf[MAXDLGCHAR];
                    GetWindowText((HWND)lParam,buf,MAXDLGCHAR);
                    strcpy(mycache[idDlg].szNameId,buf);
                }
                return TRUE;

            case ID_X_DEC:

                Decrement(idDlg,ID_STATIC_X,&myValues,hDlg);
                MyApplyCache(idDlg,&myValues,hDlg);
                return  TRUE;
            case ID_Y_DEC:
                Decrement(idDlg,ID_STATIC_Y,&myValues,hDlg);
                MyApplyCache(idDlg,&myValues,hDlg);
                return  TRUE;
            case ID_W_DEC:
                Decrement(idDlg,ID_STATIC_W,&myValues,hDlg);
               MyApplyCache(idDlg,&myValues,hDlg);
                return  TRUE;
            case ID_H_DEC:
                Decrement(idDlg,ID_STATIC_H,&myValues,hDlg);
                MyApplyCache(idDlg,&myValues,hDlg);
                return  TRUE;

            case ID_X_INC:
                Increment(idDlg,ID_STATIC_X,&myValues,hDlg);
                MyApplyCache(idDlg,&myValues,hDlg);
                return TRUE;
            case ID_Y_INC:
                Increment(idDlg,ID_STATIC_Y,&myValues,hDlg);
                 MyApplyCache(idDlg,&myValues,hDlg);
                return TRUE;
            case ID_W_INC:
                Increment(idDlg,ID_STATIC_W,&myValues,hDlg);
                 MyApplyCache(idDlg,&myValues,hDlg);
                return TRUE;
            case ID_H_INC:
                Increment(idDlg,ID_STATIC_H,&myValues,hDlg);
                 MyApplyCache(idDlg,&myValues,hDlg);
                return TRUE;

            }
            break;//fin case WM_COMMAND
        case WM_CTLCOLOREDIT:
            case WM_CTLCOLORSTATIC:
            {
                HWND hwndObjet=(HWND)lParam;
                if(!hwndObjet)
                {
                    MyPerror("mdfoh");
                    break;
                }
                static HBRUSH hBrush;
                if(GetDlgCtrlID(hwndObjet)==IDE_STYLE_DESC)
                {
                    //Gestion des colors de la zone edit qui affiche la description des styles
                    DeleteObject(hBrush);
                    hBrush=CreateSolidBrush(RGB(255,255,255));

                    return (LRESULT)hBrush;
                }
                else if(GetDlgCtrlID(hwndObjet)==IDS_STYLE_TO_DESC)
                {
                    //Gestion des colors de la zone static qui affiche
                    //Le nom du style a decrire
                    SetTextColor((HDC)wParam,RGB(100,100,255));
                    SetBkColor((HDC)wParam,RGB(240,240,240));
                    DeleteObject(hBrush);
                    hBrush=CreateSolidBrush(RGB(240,240,240));//gris dlg
                    return (LRESULT)hBrush;
                    //break;
                }


            }
            break;//fin case WM_COLOREDIT

        }//FIN switch(msg)
    }//fin if(infoDlg->idDlg==DLG_STATIC)

    //Gestion DLG_DIALOGBOX
    else if(infoDlg->idDlg==DLG_DIALOGBOX)
    {

        int idDlg=infoDlg->idDlg;

        char *pszClassName;

        if(isProjetLoadedWithOpenItem && isFirstTime[idDlg]&&isTypeModDlg)
            isFirstTime[idDlg]=FALSE;
        //Seul le dialogue ne met pas a jour son mycache.cychar
        switch(msg)
        {
            case WM_INITDIALOG:
                if(!isParentWnd && !isTypeModDlg)
                {
                    //Utilie, pour adapter les dimensions a la taille de la font
                    MyUpDateDimChar(hwndZC);

                    mycache[idDlg].font.isFontParent=TRUE;
                    mycache[idDlg].iLastX=ptWhereCreate.x;
                    mycache[idDlg].iLastY=ptWhereCreate.y;
                    mycache[idDlg].iLastH=150/cyChar;
                    mycache[idDlg].iLastW=150/cxChar;
                    mycache[idDlg].cxChar=cxChar;
                    mycache[idDlg].cyChar=cyChar;

                    if(!MyAllocMenuTab(&mycache[DLG_DIALOGBOX].menu))
                    {
                        MyPerror(", pieroiw");
                        exit(EXIT_FAILURE);
                    }
                    //defini une chaine automatiquement et unique pour le nouveau item a creer
                    MySetNameId(mycache[idDlg].szNameId,idDlg);
                    MyInitFshouldAutosize(&mycache[idDlg].fShouldAutosize,FALSE);
                }
               if(isFirstTime[idDlg]==TRUE  )
               {    isFirstTime[idDlg]=FALSE;

                    //Utilie, pour adapter les dimensions a la taille de la font
                    MyUpDateDimChar(hwndZC);

                    mycache[idDlg].mycolorobj[BK].iIndexDefColor=1;//BK_PARENT
                    mycache[idDlg].mycolorobj[TXT].iIndexDefColor=0;//TXT_PARENT
                    mycache[idDlg].mycolorobj[BK].isDefChoosen=TRUE;
                    mycache[idDlg].mycolorobj[TXT].isDefChoosen=TRUE;
                    mycache[idDlg].mycolorobj[BK].defColor=COLOR_BK_PARENT;
                    mycache[idDlg].mycolorobj[TXT].defColor=COLOR_TXT_PARENT;
                    mycache[idDlg].dwLastStylesChosen=WS_OVERLAPPEDWINDOW|
                    WS_VISIBLE|WS_CHILD;
                    mycache[idDlg].iLastH=150/cyChar;
                    mycache[idDlg].iLastW=150/cxChar;
                    mycache[idDlg].cxChar=cxChar;
                    mycache[idDlg].cyChar=cyChar;
                    mycache[idDlg].cxRest=0;
                    mycache[idDlg].cyRest=0;

                    strcpy(mycache[idDlg].szTitle,"No Title");
                    MySetNameId(mycache[idDlg].szNameId,idDlg);

                    strcpy(mycache[idDlg].szLastStylesChosen,
                        "WS_OVERLAPPEDWINDOW|WS_VISIBLE|WS_CHILD|");

                    MyInitFshouldAutosize(&mycache[idDlg].fShouldAutosize,FALSE);
               }
               else if(isParentWnd && !isTypeModDlg)
               {
                    //Utilie, pour adapter les dimensions a la taille de la font
                    MyUpDateDimChar(hwndZC);

                    mycache[idDlg].font.isFontParent=TRUE;
                    mycache[idDlg].mycolorobj[BK].iIndexDefColor=1;//BK_PARENT
                    mycache[idDlg].mycolorobj[TXT].iIndexDefColor=0;//TXT_PARENT
                    mycache[idDlg].mycolorobj[BK].isDefChoosen=TRUE;
                    mycache[idDlg].mycolorobj[TXT].isDefChoosen=TRUE;
                    mycache[idDlg].mycolorobj[BK].defColor=COLOR_BK_PARENT;
                    mycache[idDlg].mycolorobj[TXT].defColor=COLOR_TXT_PARENT;
                    mycache[idDlg].dwLastStylesChosen=WS_OVERLAPPEDWINDOW|
                    WS_VISIBLE|WS_CHILD;
                    mycache[idDlg].iLastH=150/cyChar;
                    mycache[idDlg].iLastW=150/cxChar;
                    mycache[idDlg].cxChar=cxChar;
                    mycache[idDlg].cyChar=cyChar;

                    strcpy(mycache[idDlg].szLastStylesChosen,
                        "WS_OVERLAPPEDWINDOW|WS_VISIBLE|WS_CHILD|");
                    MyInitFshouldAutosize(&mycache[idDlg].fShouldAutosize,FALSE);
               }

               if(!isParentWnd)
               {
                   EnableWindow(GetDlgItem(hDlg,ID_ADD_MENU),FALSE);
                   if(!strstr(mycache[idDlg].szLastStylesChosen,"WS_CHILD|"))
                   {
                       strcat(mycache[idDlg].szLastStylesChosen,"WS_CHILD|");
                       mycache[idDlg].dwLastStylesChosen|=WS_CHILD;
                   }

                   if(!strstr(mycache[idDlg].szLastStylesChosen,"WS_VISIBLE|"))
                   {
                       strcat(mycache[idDlg].szLastStylesChosen,"WS_VISIBLE|");
                    mycache[idDlg].dwLastStylesChosen|=WS_VISIBLE;
                   }
               }
                    //Indique qu'il s'agit du premier chargement
                isFirstLoad[idDlg]=TRUE;
            //Charger les donnees contenues dans le cache

            if(!MyLoadCache(idDlg,&myValues,hDlg))
                MessageBox(hDlg,TEXT("Unable to Load last datas"),NULL,0);
            SetFocus(GetDlgItem(hDlg,ID_CREATE));

            return FALSE;//Force le systeme a ne pas attribuer le focus lui meme

        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
            case ID_COLOR:
                MyManageColor(idDlg,hDlg);
                return TRUE;
            case  ID_FONT:
                MyManageFont(idDlg,hDlg);
                return TRUE;

            case ID_ADD_MENU:
            {
                int cpNbStyles=g_nbStyle;
                MyManageMenu(hDlg);
                g_nbStyle=cpNbStyles;//restaure,g_nbStyle qui peut etre affcecte par manageMenu(AddItem)
                MyRefreshAspect(hDlg,idDlg);
            }
                return TRUE;

            case ID_APPLY:
            case IDCANCEL:
            case ID_CREATE:
                if(LOWORD(wParam)==ID_APPLY ||
                   LOWORD(wParam)==ID_CREATE)
                {
                    //verifier la validite de szNameId
                    if(!isValidNameId(mycache[idDlg].szNameId,
                            (LOWORD(wParam)==ID_CREATE)?-1:idObjToDelete))
                    {
                        return TRUE;
                    }
                }

                //Creation de l'item

            //Fall through pour fermeture

            if(!MySaveCache(idDlg,myValues,hDlg))
                MessageBox(hDlg,TEXT("Unable to save theses datas"),NULL,0);
            //sortie
                if(LOWORD(wParam)==IDCANCEL && isParentWnd)
                {
                    if(MessageBox(hDlg,TEXT("\n Do you really want to cancel?"),
                        TEXT("Question"),MB_ICONQUESTION|MB_YESNO)==IDYES)
                    {
                        if(infoDlg->iTypeDlg==MODAL)
                            EndDialog(hDlg,TRUE);
                        else
                            DeleteWindow(hDlg,hDlgModeless);
                    }
                }
                else
                {
                    if(infoDlg->iTypeDlg==MODAL)
                        EndDialog(hDlg,TRUE);
                    else
                        DeleteWindow(hDlg,hDlgModeless);
                }

                if(LOWORD(wParam)==ID_CREATE  ||
                   LOWORD(wParam)==ID_APPLY)
                {
                    //ici hDlg n'est plus valide car deja supprime
                    //Creer l'objet
                    //CreateObjet se chargera automatiquement d'ajouter l'objet
                    //a l'univers si la creation a reussi.

                    if(isParentWnd)
                    {//For mainWindowParentObj
                        if(isTypeModDlg)
                        {
                            g_hwndPrevParentObj=g_hwndParentObj;
                            g_hwndParentObj=NULL;

                        }
                        pszClassName=szClassZC;
                    }
                    else
                    {//For other parent windows but not main
                        pszClassName=szClassDlgObj;
                        if(isTypeModDlg)
                        {
                            MySetLastParentObj(GetDlgItem(g_hwndParentObjToSet,idObjToDelete),
                            NULL);
                        }
                    }

                    if(LOWORD(wParam)==ID_APPLY)
                    {
                        MyUpdateDimObjUniv();
                        MyDeleteObj(idObjToDelete);
                    }
                    hwndObjet=CreateObjet(pszClassName,DLG_DIALOGBOX,hwndOwner);
                    if(!hwndObjet)
                    {
                        MessageBox(hwndOwner,"Unable to create this object",NULL,
                                   MB_ICONERROR);
                    }

                }

                return TRUE;

            case ID_PREV:

                if(!MySaveCache(idDlg,myValues,hDlg))
                    MessageBox(hDlg,TEXT("Unable to save theses datas"),NULL,0);

                if(!OpenDialogBox(MODAL,DLG_CHOOSETYPEOBJET,
                infoDlg->hWndOwner,infoDlg->hinst,hDlg,infoDlg->iTypeDlg))
                {
                    MessageBox(NULL,TEXT("Unable to open dialog"),
                    TEXT("ERROR"),MB_ICONERROR);
                }
                return TRUE;

            case ID_EDIT_TITLE:
                if(HIWORD(wParam)==EN_CHANGE)
                {
                    char buf[MAXDLGCHAR];
                    GetWindowText((HWND)lParam,buf,MAXDLGCHAR);
                    SetWindowText(GetDlgItem(GetDlgItem(hDlg,ID_ASPECT),1),buf);
                    strcpy(mycache[idDlg].szTitle,buf);
                }
                return TRUE;

            case IDE_NAMEID:
                if(HIWORD(wParam)==EN_CHANGE)
                {
                    char buf[MAXDLGCHAR];
                    GetWindowText((HWND)lParam,buf,MAXDLGCHAR);
                    strcpy(mycache[idDlg].szNameId,buf);
                }
                return TRUE;

            case ID_X_DEC:

                Decrement(idDlg,ID_STATIC_X,&myValues,hDlg);
                MyApplyCache(idDlg,&myValues,hDlg);
                return  TRUE;
            case ID_Y_DEC:
                Decrement(idDlg,ID_STATIC_Y,&myValues,hDlg);
                MyApplyCache(idDlg,&myValues,hDlg);
                return  TRUE;
            case ID_W_DEC:
                Decrement(idDlg,ID_STATIC_W,&myValues,hDlg);
               MyApplyCache(idDlg,&myValues,hDlg);
                return  TRUE;
            case ID_H_DEC:
                Decrement(idDlg,ID_STATIC_H,&myValues,hDlg);
                MyApplyCache(idDlg,&myValues,hDlg);
                return  TRUE;

            case ID_X_INC:
                Increment(idDlg,ID_STATIC_X,&myValues,hDlg);
                MyApplyCache(idDlg,&myValues,hDlg);
                return TRUE;
            case ID_Y_INC:
                Increment(idDlg,ID_STATIC_Y,&myValues,hDlg);
                 MyApplyCache(idDlg,&myValues,hDlg);
                return TRUE;
            case ID_W_INC:
                Increment(idDlg,ID_STATIC_W,&myValues,hDlg);
                 MyApplyCache(idDlg,&myValues,hDlg);
                return TRUE;
            case ID_H_INC:
                Increment(idDlg,ID_STATIC_H,&myValues,hDlg);
                 MyApplyCache(idDlg,&myValues,hDlg);
                return TRUE;

            }
            break;//fin case WM_COMMAND

        case WM_CTLCOLOREDIT:
            case WM_CTLCOLORSTATIC:
            {
                HWND hwndObjet=(HWND)lParam;
                if(!hwndObjet)
                {
                    MyPerror("mdfoh");
                    break;
                }
                static HBRUSH hBrush;
                if(GetDlgCtrlID(hwndObjet)==IDE_STYLE_DESC)
                {
                    //Gestion des colors de la zone edit qui affiche la description des styles
                    DeleteObject(hBrush);
                    hBrush=CreateSolidBrush(RGB(255,255,255));

                    return (LRESULT)hBrush;
                }
                else if(GetDlgCtrlID(hwndObjet)==IDS_STYLE_TO_DESC)
                {
                    //Gestion des colors de la zone static qui affiche
                    //Le nom du style a decrire
                    SetTextColor((HDC)wParam,RGB(100,100,255));
                    SetBkColor((HDC)wParam,RGB(240,240,240));
                    DeleteObject(hBrush);
                    hBrush=CreateSolidBrush(RGB(240,240,240));//gris dlg
                    return (LRESULT)hBrush;
                    //break;
                }


            }
            break;//fin case WM_COLOREDIT
        }//FIN switch(msg)
    }//fin if(infoDlg->idDlg==DLG_DIALOGBOX)

    return FALSE;
}

BOOL DeleteWindow(HWND hDlg,HWNDMODELESS *hDlgModeless)
{
    //Cette fonction supprime hDlg via destroy window,
    DestroyWindow(hDlg);

    //On va gerer la suite apres :) noublie pas le IsDialog du While du main
    return TRUE;
}

HWND CreateObjet(PSTR pClassName,int idDlg,HWND hwndOwner)
{
    if(!pClassName || idDlg<0)
    {
        MyPerror("vldkkhtr");
        MyMsgError();
        exit(EXIT_FAILURE);
    }
    g_fShouldIAskSave=TRUE;
    HWND hwndParent=hwndOwner;
    int iIndexParent=MyGetIindexObjWithHwnd(hwndParent);
    UNIVERS uTemp;
    HWND hwnd;
    TCHAR szTitle[MAXDLGCHAR];
    TCHAR szNameId[MAXDLGCHAR];
    strcpy(szTitle,mycache[idDlg].szTitle);
    strcpy(szNameId,mycache[idDlg].szNameId);
    int cxObj=mycache[idDlg].cxChar;
    int cyObj=mycache[idDlg].cyChar;
    if(idDlg==DLG_DIALOGBOX)
    {
        //les cxChar de dlgBox restent ceux du systeme
        MyUpDateDimChar(hwndZC);
        cxObj=cxChar;
        cyObj=cyChar;

    }
    int cxRest=mycache[idDlg].cxRest;
    int cyRest=mycache[idDlg].cyRest;
    int x=mycache[idDlg].iLastX;
    int y=mycache[idDlg].iLastY;
    int w=mycache[idDlg].iLastW*cxObj+cxRest;
    int h=mycache[idDlg].iLastH*cyObj+cyRest;


    DWORD dwStyle=mycache[idDlg].dwLastStylesChosen;
    HINSTANCE hinst=(HINSTANCE)GetWindowLongPtr(hwndOwner,GWLP_HINSTANCE);
    INT_PTR id;
    int i;

    if(!idObjToDelete)
    {
        id=iActualIdForObjet; iActualIdForObjet++;
        uTemp.isVisible=TRUE;
        uTemp.linkopt.hwndLinkWith=malloc(sizeof(HWND)*10);
        if(!uTemp.linkopt.hwndLinkWith)
        {
            MyPerror("error malloc");
            exit(EXIT_FAILURE);
        }
        uTemp.linkopt.id=malloc(sizeof(int)*10);
        if(!uTemp.linkopt.id)
        {
            MyPerror("error malloc");
            exit(EXIT_FAILURE);
        }
        uTemp.linkopt.iLastIndex=0;
        uTemp.linkopt.nbLink=10;
        memset(uTemp.linkopt.hwndLinkWith,0,sizeof(HWND)*10);
        memset(uTemp.linkopt.id,0,sizeof(int)*10);
        uTemp.linkopt.isLinked=FALSE;

    }
    else
    {
        id=idObjToDelete;
        MYLINKOBJ link;
        link=mycache[idDlg].linkopt;
        uTemp.isVisible=TRUE;
        uTemp.linkopt=link;
    }

    if(!strcmp(pClassName,"static"))
        dwStyle|=SS_NOTIFY;

    dwStyle|=WS_CLIPSIBLINGS;
    if(dwStyle&WS_DISABLED)
        dwStyle&=~WS_DISABLED;//pour pouvoir modifier un disabled


    if(dwStyle&BS_GROUPBOX)
        dwStyle|=BS_NOTIFY;//pour pouvoir modifier un disabled

    uTemp.isMainParent=FALSE;
    if(isParentWnd)
    {
        dwStyle|=WS_CHILD|WS_VISIBLE;
        uTemp.isMainParent=TRUE;
    }

    strcpy(uTemp.szLastStylesChosen,mycache[idDlg].szLastStylesChosen);
    uTemp.dwStyle=mycache[idDlg].dwLastStylesChosen;
    uTemp.mycolorobj[BK]=mycache[idDlg].mycolorobj[BK];
    uTemp.mycolorobj[TXT]=mycache[idDlg].mycolorobj[TXT];

    uTemp.font=mycache[idDlg].font;

    //case Menu....
    if(idDlg==DLG_DIALOGBOX && isParentWnd)
    {
        //S'il s'agit de la creation du mainParent
        uTemp.menu=NULL;
        if(!MyAllocMenuTab(&uTemp.menu))
        {
            MyPerror("mverieriwepo");
            return FALSE;
        }
        if(!MyCopieMenuTab(mycache[DLG_DIALOGBOX].menu,
                           &uTemp.menu))
        {
            MyPerror("amjvaij");
            return FALSE;
        }
    }

    if(iIndexParent!=-1 && isGroupBoxObj(iIndexParent))
    {
        hwndParent=GetDlgItem(hwndParent,ID_DLG_GB);
        if(!hwndParent)
        {
            MyPerror("\n error unknown in createObj");
            hwndParent=hwndOwner;
        }
    }

    uTemp.fShouldAutosize=mycache[idDlg].fShouldAutosize;

    hwnd=CreateWindow(pClassName,szTitle,dwStyle,x,y,w,h,
                      hwndParent,(HMENU)id,hinst,NULL);


    if(hwnd)
    {
         /*  //Set le font
        PopFontSetFont(hwnd,&uTemp.font.hFont,&uTemp.font.logfont);
        */
        PopFontSetFont(hwnd,&mycache[idDlg].font.hFont,
                        &mycache[idDlg].font.logfont);


        //Si la creation a reussi, alors ajouter le nouvel object cree a
        //la l'univers des objets crees
        uTemp.nbObjCreated=0;
        uTemp.hwnd=hwnd;
        uTemp.hinst=hinst;
        uTemp.hwndParent=hwndParent;
        uTemp.id=id;
        _tcscpy(uTemp.szTitle,szTitle);
        _tcscpy(uTemp.szNameId,szNameId);
        uTemp.x=x;
        uTemp.y=y;
        uTemp.w=w/cxObj;
        uTemp.h=h/cyObj;
        uTemp.cxChar=cxObj;
        uTemp.cyChar=cyObj;
        uTemp.cxRest=cxRest;
        uTemp.cyRest=cyRest;

        uTemp.newObjWndProc=NewObjWndProc;
        uTemp.oldObjWndProc=(WNDPROC) SetWindowLongPtr(hwnd,
                                            GWLP_WNDPROC,(LONG_PTR)NewObjWndProc);
        if(!uTemp.oldObjWndProc)
        {
            MessageBox(hwndParent,"Unable to Subclass this object",
                       NULL,MB_ICONERROR);
            exit(EXIT_FAILURE);
        }
        //Enregistrer le type de l'objet
        int iTypeObj=0;
        switch(idDlg)
        {
            case DLG_BUTTON: iTypeObj=OBJ_BUTTON; break;
            case DLG_EDITBOX: iTypeObj=OBJ_EDITBOX; break;
            case DLG_LISTBOX: iTypeObj=OBJ_LISTBOX; break;
            case DLG_COMBOBOX: iTypeObj=OBJ_COMBOBOX; break;
            case DLG_STATIC: iTypeObj=OBJ_STATIC; break;
            case DLG_DIALOGBOX: iTypeObj=OBJ_DIALOGBOX; break;
            case DLG_IMGSTATIC: iTypeObj=OBJ_IMGSTATIC; break;
        }
        uTemp.iTypeObj=iTypeObj;
        //Obtenir la szDescription de l'objet
        MyGetDescOfObj(&uTemp);

        //Ajouter uTemp a l'univer  tableau uObjet
        //a uObjet[0], nbObjCreated defini le nombre actuel d'objets deja crees
        if(uObjet[0].nbObjCreated>=maxObjectCreable)
        {
            int n=maxObjectCreable;
            //Si le nombre maximale d'objets creable dans l'univers est atteint
            //Alors, il faut reallouer le tableau avant d'ajouter uTemp;
            maxObjectCreable+=MIN_OBJ_CREABLE;
            uObjet=realloc(uObjet,sizeof(UNIVERS)*maxObjectCreable);
            po=realloc(po,sizeof(POSOBJ)*maxObjectCreable);
            if(!uObjet || !po)
            {
                MessageBox(NULL,"Unable to realloc uObjet/po before adding new objetc",NULL,
                           MB_ICONERROR);
                           exit(EXIT_FAILURE);
            }
            memset(uObjet+n,0,sizeof(UNIVERS)*MIN_OBJ_CREABLE);
            memset(po+n,0,sizeof(POSOBJ)*MIN_OBJ_CREABLE);

        }
        i=uObjet[0].nbObjCreated;
        uObjet[i]=uTemp;

        //cOPIE DU MENU,SI ON LAISSE COMME ca, menu de obj pointe directement sur uTemp LOCAL
        if(uObjet[i].iTypeObj==OBJ_DIALOGBOX && isParentWnd)
        {

            uObjet[i].menu=NULL;
            if(!MyAllocMenuTab(&uObjet[i].menu))
            {
                MyPerror("cm ksdj");
                return FALSE;
            }

            if(!MyCopieMenuTab(uTemp.menu,&uObjet[i].menu))
            {
                MyPerror("c nmajijd");
                return FALSE;
            }

        }
        else
            uObjet[i].menu=NULL;//important

        uObjet[0].nbObjCreated=i+1;
        po[i]._beg.x=uObjet[i].x;
        po[i]._beg.y=uObjet[i].y;
        po[i]._end.x=po[i]._beg.x+uObjet[i].w*uObjet[i].cxChar;
        po[i]._end.y=po[i]._beg.y+uObjet[i].h*uObjet[i].cyChar;
        BringWindowToTop(hwnd);
        SetFocus(hwnd);
        if(isParentWnd)
        {
            //Si l'objet cree est la fenetre sensee porter tous les autres
            po[i]._beg.x=0;
            po[i]._beg.y=0;
            po[i]._end.x=po[i]._beg.x;
            po[i]._end.y=po[i]._beg.y;

            g_hwndParentObj=hwnd;
            MyChangeParentBorder(hwnd);
            MyApplyMenuCache(hwnd,uObjet[i].menu);
            //Enable or not, ID_STOP_TOOL,SAVE and other
            HMENU hMenu=GetMenu(GetParent(hwndZC));
            if(!hMenu)
            {
                MyPerror("m pdooit");
            }

            if(!IsWindow(g_hwndParentTest))
            {
                //Si aucun test n'est en cours

                MyEnableButtonToolBar(g_hwndToolBar,ID_STOP_TOOL,TRUE,FALSE);

                MyEnableButtonToolBar(g_hwndToolBar,IDM_SAVE,TRUE,FALSE);
                EnableMenuItem(hMenu, IDM_SAVE, MF_ENABLED);
                EnableMenuItem(hMenu, IDM_SAVEAS, MF_ENABLED);
                EnableMenuItem(hMenu, IDM_CLOSE_PROJECT, MF_ENABLED);
            }
            else
            {
                //Si test est en cours
                MyEnableButtonToolBar(g_hwndToolBar,ID_STOP_TOOL,FALSE,FALSE);

                MyEnableButtonToolBar(g_hwndToolBar,IDM_SAVE,TRUE,TRUE);
                EnableMenuItem(hMenu, IDM_SAVE, MF_GRAYED);
                EnableMenuItem(hMenu, IDM_SAVEAS, MF_GRAYED);
                EnableMenuItem(hMenu, IDM_CLOSE_PROJECT, MF_GRAYED);
            }
            //anyway Enable set define .h
            EnableMenuItem(hMenu, IDM_SET_DEF_OUTPUT, MF_ENABLED);

        }
        else if(!g_hwndParentObj && idDlg==DLG_DIALOGBOX)
        {
            //Sil s'agit dune dlgbox mais pas parent,et qu'il ya pas
            //de parent
            MyEnableButtonToolBar(g_hwndToolBar,
                                  IDM_SAVE,TRUE,FALSE);

        }
        //Au cas ou createWindow ait cree des childs DLG_DIALOG
        //Il faut restaurer le cache par les donnees du MainParent
        //Sinon le mainParent sera charge avec les donnees du child Dlg
        strcpy(mycache[idDlg].szLastStylesChosen,uTemp.szLastStylesChosen);
        mycache[idDlg].dwLastStylesChosen=uTemp.dwStyle;
        mycache[idDlg].mycolorobj[BK]=uTemp.mycolorobj[BK];
        mycache[idDlg].mycolorobj[TXT]=uTemp.mycolorobj[TXT];
        mycache[idDlg].font=uTemp.font;


        //GroupBox setting ne pas modifier i plus haut
        uObjet[i].hwndChildGB=NULL;
        if(isGroupBoxObj(i))
        {
            MYFONT fontParent=uObjet[i].font;
            int cxParent=uObjet[i].cxChar;
            int cyParent=uObjet[i].cyChar;
             //creer un childs dlg qui va rediriger tous les msg
            //du GB etant donne que sa structure naturelle ne
            //gere pas les message de souris
            HWND hwndChildGB;
            MyUpDateDimChar(uObjet[i].hwnd);
            int y=cyChar-2;
            int wGb=(uObjet[i].w*uObjet[i].cxChar+uObjet[i].cxRest)-4;
            int hGb=max(y,(uObjet[i].h*uObjet[i].cyChar+uObjet[i].cyRest)-y-2);
            hwndChildGB=CreateWindow(szClassGroupBoxChild,
            NULL,WS_OVERLAPPED|WS_VISIBLE|WS_CHILD,
            2,y,wGb,hGb,uObjet[i].hwnd,(HMENU)ID_DLG_GB,
            uObjet[i].hinst,NULL);
            if(!hwndChildGB)
            {
                MyPerror("\n error in creating childGropBox");
                exit(EXIT_FAILURE);
            }
            uObjet[i].hwndChildGB=hwndChildGB;
            MyAddToolTipToObj(hwndChildGB,hwndParent);
            //Charger des childs a hwndChild si il en a
            if(g_hwndLastChildGB)
            {
                //NB OOOH: Ici on utilise idDlgChild
                HWND hwndLastChildGB=g_hwndLastChildGB;
                int i,idObj,idDlgChild=0;
                PTCHAR pszClassName=NULL;
               int nbObj=uObjet[0].nbObjCreated;
                for(i=0;i<nbObj;i++)
                {
                    if(uObjet[i].hwndParent==hwndLastChildGB)
                    {
                         switch(uObjet[i].iTypeObj)
                        {
                        case OBJ_BUTTON:
                            pszClassName=TEXT("button");
                            idDlgChild=DLG_BUTTON;

                            if(isGroupBoxObj(i))
                            {
                                g_hwndLastChildGB=uObjet[i].hwndChildGB;
                            }
                            break;
                        case OBJ_EDITBOX:
                            pszClassName=TEXT("edit");
                            idDlgChild=DLG_EDITBOX;
                            break;
                        case OBJ_LISTBOX:
                            pszClassName=TEXT("listbox");
                            idDlgChild=DLG_LISTBOX;
                            break;
                        case OBJ_COMBOBOX:
                            pszClassName=WC_COMBOBOX;
                            idDlgChild=DLG_COMBOBOX;
                            break;
                        case OBJ_STATIC:
                            pszClassName=TEXT("static");
                            idDlgChild=DLG_STATIC;
                            break;

                        case OBJ_DIALOGBOX:
                            pszClassName=szClassDlgObj;
                            idDlgChild=DLG_DIALOGBOX;
                            g_hwndParentObjToSet=hwnd;
                            MySetLastParentObj(uObjet[i].hwnd,NULL);
                            break;

                        case OBJ_IMGSTATIC:
                            pszClassName=NULL;
                            idDlgChild=DLG_IMGSTATIC;
                            break;
                        }

                        //Rempli mycache avec les styles de l'objet a dupliquer
                        //Car ce cache sera utiliser dans createObjet
                        strcpy(mycache[idDlgChild].szLastStylesChosen,uObjet[i].szLastStylesChosen);
                        mycache[idDlgChild].dwLastStylesChosen=uObjet[i].dwStyle;
                        mycache[idDlgChild].iLastX=uObjet[i].x;
                        mycache[idDlgChild].iLastY=uObjet[i].y;
                        mycache[idDlgChild].iLastW=uObjet[i].w;
                        mycache[idDlgChild].iLastH=uObjet[i].h;
                        mycache[idDlgChild].cxChar=uObjet[i].cxChar;
                        mycache[idDlgChild].cyChar=uObjet[i].cyChar;
                        mycache[idDlgChild].cxRest=uObjet[i].cxRest;
                        mycache[idDlgChild].cyRest=uObjet[i].cyRest;
                        mycache[idDlgChild].mycolorobj[BK]=uObjet[i].mycolorobj[BK];
                        mycache[idDlgChild].mycolorobj[TXT]=uObjet[i].mycolorobj[TXT];
                        _tcscpy(mycache[idDlgChild].szTitle,uObjet[i].szTitle);
                        _tcscpy(mycache[idDlgChild].szNameId,uObjet[i].szNameId);
                        mycache[idDlgChild].linkopt=uObjet[i].linkopt;
                        mycache[idDlgChild].fShouldAutosize=uObjet[i].fShouldAutosize;

                        mycache[idDlgChild].font=uObjet[i].font;
                        if(uObjet[i].font.isFontParent==TRUE)
                        {
                            mycache[idDlgChild].font=fontParent;
                            mycache[idDlgChild].font.isFontParent=TRUE;
                            if(g_fShouldApplyFontOnChilds)
                            {
                                mycache[idDlgChild].cxChar=cxParent;
                                mycache[idDlgChild].cyChar=cyParent;
                            }
                        }

                        if(idDlgChild==DLG_DIALOGBOX && isParentWnd)
                        {
                            if(!MyCopieMenuTab(uObjet[i].menu,&mycache[idDlgChild].menu))
                            {
                                MyPerror(" ssdktk");
                                exit(EXIT_FAILURE);
                            }
                        }

                        idObj=uObjet[i].id;

                        isTypeModDlg=TRUE;//Impacte dans MyDeleteObj pour pas checker les childs
                        idObjToDelete=idObj;

                        CreateObjet(pszClassName,idDlgChild,hwnd);
                        idObjToDelete=0;

                        MyDeleteObj(idObj);

                        isTypeModDlg=FALSE;//Renitialise
                        i--;
                        nbObj--;
                    }
                }

            }
        }
        else
            MyAddToolTipToObj(hwnd,hwndParent);

        //Clignotage qui indique celui qui a le focus
        SetTimer(hwnd,ID_TIMER_CLIGNOTE,100,NULL);
    }
    else
    {
        MessageBox(NULL,"Unable to create this objet",NULL,0);
    }

    //free


    //menu of dialog in Utem
    if(idDlg==DLG_DIALOGBOX && isParentWnd)
    {
        if(!MyFreeMenuTab(&uTemp.menu))
        {
            MyPerror("m aaod");
            return FALSE;
        }
    }

    return hwnd;
}

BOOL MyAddToolTipToObj(HWND hwndObj,HWND hwndParentObj)
{
    if(!hwndObj || !hwndParentObj||!g_hToolTip)
        return FALSE;

    TOOLINFO ti={0};

    ti.cbSize = sizeof(TOOLINFO);
    ti.uFlags = TTF_SUBCLASS|TTF_IDISHWND;
    ti.hwnd = hwndParentObj;
    ti.uId = (UINT_PTR)hwndObj;
    ti.lpszText = LPSTR_TEXTCALLBACK; // Texte dynamique
    SendMessage(g_hToolTip, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);

    return TRUE;
}

BOOL MyGetDescOfObj(UNIVERS* uTemp)
{
    //Cette fonction uTilise l'ensemble des donnees de Utemps,
    //pour creer une chaine qui affiche l'id,le szId,la classe,et le
    //style de l'objet uTemp
    char *pszData,*pszDesc,buf[1024];
    int iActuelLenght=0;
    //Initialiser le tampon de stockage
    uTemp->szDescription[0]='\0';
    pszDesc=uTemp->szDescription;
    //Cas du nom de classe de l'objet
    pszData="Class: ";
    strcat(pszDesc,pszData);
    iActuelLenght+=strlen(pszData);
    switch(uTemp->iTypeObj)
    {
        case OBJ_BUTTON: pszData="button;\n"; break;
        case OBJ_COMBOBOX: pszData="combobox;\n"; break;
        case OBJ_DIALOGBOX: pszData="DialogBox;\n"; break;
        case OBJ_EDITBOX: pszData="edit;\n"; break;
        case OBJ_LISTBOX: pszData="listbox;\n"; break;
        case OBJ_IMGSTATIC: pszData="GROUPBOX;\n"; break;
        case OBJ_STATIC: pszData="static;\n"; break;
    }
    if(iActuelLenght<=MAXLENTGHT_OF_DESCRIPTION-1-strlen(pszData))
    {
        strcat(pszDesc,pszData);
        iActuelLenght+=strlen(pszData);
    }
    else
    {
        MessageBox(NULL,TEXT("Max Space for szDescription has been reached"),NULL,
                             MB_ICONERROR);
        return TRUE;
    }

    //Cas de l'ID
    pszData="ID: ";
    strcat(pszDesc,pszData);
    iActuelLenght+=strlen(pszData);
    wsprintf(buf,"%d;\n",uTemp->id);
    pszData=buf;
    if(iActuelLenght<=MAXLENTGHT_OF_DESCRIPTION-1-strlen(pszData))
    {
        strcat(pszDesc,pszData);
        iActuelLenght+=strlen(pszData);
    }
    else
    {
        MessageBox(NULL,TEXT("Max Space for szDescription has been reached"),NULL,
                             MB_ICONERROR);
        return TRUE;
    }

    //Cas du nom de l'id
    pszData="NAMEID: ";
    strcat(pszDesc,pszData);
    iActuelLenght+=strlen(pszData);
    pszData=uTemp->szNameId;
    if(iActuelLenght<=MAXLENTGHT_OF_DESCRIPTION-1-strlen(pszData))
    {
        strcat(pszDesc,pszData);
        iActuelLenght+=strlen(pszData);
    }
    else
    {
        MessageBox(NULL,TEXT("Max Space for szDescription has been reached"),NULL,
                             MB_ICONERROR);
        return TRUE;
    }

    return TRUE;
}

BOOL MyUpdateDimObjUniv(void)
{
    //Cette fonction met a jour les champs x,y,w,h des
    //Objets de l'univers
    int i,nbObj;
    int diff=0;
    RECT rcObj;
    nbObj=uObjet[0].nbObjCreated;
    for(i=0;i<nbObj;i++)
    {
        if(uObjet[i].iTypeObj==OBJ_LISTBOX)
            diff=2;
        else
            diff=0;

        GetWindowRect(uObjet[i].hwnd,&rcObj);
        MyScreenRcToClientRc(uObjet[i].hwndParent,&rcObj);
        uObjet[i].x=rcObj.left;
        uObjet[i].y=rcObj.top;
        uObjet[i].w=(rcObj.right -rcObj.left)/uObjet[i].cxChar;
        uObjet[i].h=(rcObj.bottom-rcObj.top-diff)/uObjet[i].cyChar;
        uObjet[i].cxRest=(rcObj.right-rcObj.left)-(uObjet[i].w*uObjet[i].cxChar);
        uObjet[i].cyRest=(rcObj.bottom-rcObj.top)-(uObjet[i].h*uObjet[i].cyChar);

        if(uObjet[i].iTypeObj==OBJ_LISTBOX && uObjet[i].cyChar!=HIWORD(GetDialogBaseUnits()) )
        {
            //Mettre a jour rc, en fonction des coordonnees systemes
            uObjet[i].h+=1;
        }

     }
     return TRUE;
}

HWND MyGetLastParentObj(HWND hwndNewParentObj)
{
    //Cette fonction retourne le precedenet handle que hwndNewParentObj
    //Vient de remplacer
    //La premiere case contient les infos sur la taille
   HWND hwndOut=NULL;
   int i;

   //Allocation
   if(!g_tabHwnd)
   {
        return NULL;
    }

   //Rechercher hwndNewParent dans tabHwnd.New ,si trouv,
   //retourne .last
   for(i=0;i<g_tabHwnd[0].iIndexValid;i++)
   {
       if(g_tabHwnd[i].hwndNew==hwndNewParentObj)
       {
           hwndOut=g_tabHwnd[i].hwndLast;
           break;
       }
    }
   return hwndOut;
}

BOOL MySetLastParentObj(HWND hwndLastParentObj,HWND hwndNewParentObj)
{
    //Cette fonction ajoute je couple des hwnd a la strucutre globale
   int i;

   //Allocation
   if(!g_tabHwnd)
   {
       g_tabHwnd=malloc(sizeof(COUPLEHWND)*100);
       if(!g_tabHwnd)
       {
           MessageBox(NULL," error in alloc tabHwnd",NULL,0);
           exit(EXIT_FAILURE);
       }
       g_tabHwnd[0].tailleTab=100;
       g_tabHwnd[0].iIndexValid=0;
   }
   else if(g_tabHwnd[0].iIndexValid==g_tabHwnd[0].tailleTab)
   {
       //Reallocation car valeur max atteinte
       g_tabHwnd[0].tailleTab+=100;
       g_tabHwnd=realloc(g_tabHwnd,sizeof(COUPLEHWND)*g_tabHwnd[0].tailleTab);
       if(!g_tabHwnd)
       {
           MessageBox(NULL," error in alloc tabHwnd",NULL,0);
           exit(EXIT_FAILURE);
       }
   }
   //Ajoute
   i=g_tabHwnd[0].iIndexValid;
   g_tabHwnd[i].hwndLast=hwndLastParentObj;
   g_tabHwnd[i].hwndNew=hwndNewParentObj;
   i++;
   g_tabHwnd[0].iIndexValid=i;
   return TRUE;
}

BOOL MyChangeHwndNewParent(HWND hwndLastNewParent,HWND hwndNewParent)
{
    //Cette fonction remplace le champs New de g_tabHWND
    //par hwndNewParent.
    int i;
    if(!g_tabHwnd) return FALSE;

    for(i=0;i<g_tabHwnd[0].iIndexValid;i++)
    {
        if(g_tabHwnd[i].hwndNew==hwndLastNewParent)
        {
            g_tabHwnd[i].hwndNew=hwndNewParent;
            return TRUE;
        }
    }
    return FALSE;
}

BOOL MyApplyMenuCache(HWND hwndObj,MYMENULIST *menu)
{
    //cette fonction ,permet dApply le menu a hwndObj qui est sense
    //etre de typeDlg
    if(!hwndObj)
    {
        MyPerror("m eirto");
        return FALSE;
    }

    HMENU hMenuMain = CreateMenu();
    int i;
/*
    //Creer les menuParents
    for(i=0;i<menu[0].iLastIndexMenu;i++)
    {
        if(menu[i].iIndexParent==-1)
        {
            if(!MyAppendPopupElt(i,hMenuMain,menu))
            {
                MyPerror(" deoiowq");
                return FALSE;
            }
        }
    }

    //Creer les autres
    for(i=0;i<menu[0].iLastIndexMenu;i++)
    {
        if(menu[i].iIndexParent!=-1)
        {
            if(!MyAppendPopupElt(i,hMenuMain,menu))
            {
                MyPerror("m sadifosi");
                return FALSE;
            }
        }
    }
*/

    //Creer le menuParent, puis les sous items/et menus
    //Du menuParent Next lordre etabli
    for(i=0;i<menu[0].iLastIndexMenu;i++)
    {
        if(menu[i].iIndexParent==-1)
        {
            //Creer le main parent
            //QUI SE CHARGERA DE LA CREATION DES ELTS
            //DE SON ORDRE ET PAR ORDRE :)
            if(!MyAppendPopupElt(i,hMenuMain,menu))
            {
                MyPerror(" moioweiw");
                MyMsgError();
                return FALSE;
            }
        }
    }

    SetMenu(hwndObj,hMenuMain);
    return TRUE;
}

BOOL MyAppendPopupElt(int iIndexMenu,HMENU hMenuMain,MYMENULIST *menu)
{//cette fonction ajoute un elt de type popup menu soit
    //directement a hMenuMain, si il na pas de parent direct
    //ou alors a son parent direct

    if(!hMenuMain)
    {
        MyPerror(" ,OIRGOG");
        return FALSE;
    }
    if(!menu)
    {
        MyPerror(" erit");
        return FALSE;
    }
    if(iIndexMenu<0 || iIndexMenu>=menu[0].iLastIndexMenu)
    {
        MyPerror(" erioit");
        return FALSE;
    }

    if(!menu[iIndexMenu].tabElt[0].isPopPupMenu)
    {
        MyPerror(" a,oritoe");
        return FALSE;
    }

    HMENU hMenuParent=NULL;
    int iIndexParent=-2;
    //Determiner le hMenu ou append menu[iIndexMenu].tab[iIndexElt].popup
    if((iIndexParent=menu[iIndexMenu].iIndexParent)==-1)
        hMenuParent=hMenuMain;
    else
    {
        if(iIndexParent<0 || iIndexParent>=menu[0].iLastIndexMenu)
        {
            MyPerror(" ,eritoe");
            return FALSE;
        }
        if(menu[iIndexParent].tabElt[0].isPopPupMenu)
        {
            hMenuParent=menu[iIndexParent].tabElt[0].dataPop.hMenu;
            if(!hMenuParent)
            {
                MyPerror(" ,ortii");
                return FALSE;
            }
        }
        else
        {
            MyPerror(" aeroitr");
            return FALSE;
        }

    }

    //ici on devrait avoir un hMenuParent valide
    UINT uFlag;
    UINT_PTR idPop;
    LPCSTR pszNamePop;
    HMENU hMenuToAppend=CreatePopupMenu();
    if(!hMenuToAppend)
    {
        MyPerror(", aokoasdk");
        return FALSE;
    }
    uFlag=menu[iIndexMenu].tabElt[0].dataPop.uFlag;
    idPop=(UINT_PTR)hMenuToAppend;
    pszNamePop=menu[iIndexMenu].tabElt[0].dataPop.szNameMenu;

    //mettre a jour (important)
    menu[iIndexMenu].tabElt[0].dataPop.hMenu=hMenuToAppend;

    //Creer/Coller/Ajouter tous les childs de lordre de IindexMenu
    MYMENUORDER *mno=menu[iIndexMenu].tabOrder;
    for(int k=0;k<mno[0].nbOrder;k++)
    {
        if(mno[k].isMenuChild)
        {
            //Append un popmenu
            int iIndexChild=mno[k].iIndexChild;
            if(!MyAppendPopupElt(iIndexChild,hMenuToAppend,menu))
            {
                MyPerror("m ofoisdg");
                MyMsgError();
                return FALSE;
            }
        }
        else
        {
            //Append un item
            int iIndexChild=mno[k].iIndexChild;
            if(!MyAppendItemElt(iIndexMenu,iIndexChild,hMenuToAppend,menu))
            {
                MyPerror("mjkjdfgkjfk");
                MyMsgError();
                return FALSE;
            }
        }
    }


    if(!AppendMenu(hMenuParent,uFlag,idPop,pszNamePop))
     {
         //mettre a jour (important)
        menu[iIndexMenu].tabElt[0].dataPop.hMenu=NULL;
         return FALSE;
     }

    return TRUE;
}

BOOL MyAppendItemElt(int iIndexMenu,int iIndexElt,HMENU hMenuMain,MYMENULIST *menu)
{
    //cette fonction coolle item a tab[0] de iiNDEXmENU
    if(!menu)
    {
        MyPerror("cwetop");
        return FALSE;
    }
    if(iIndexMenu<0 || iIndexMenu>=menu[0].iLastIndexMenu)
    {
        MyPerror("  SArars");
        return FALSE;
    }
    if(iIndexElt<0 || iIndexElt>=menu[iIndexMenu].iLastIinexElt)
    {
        MyPerror("retesr");
        return FALSE;
    }
    if(!menu[iIndexMenu].tabElt[0].isPopPupMenu)
    {
        MyPerror(", verkokt");
        return FALSE;
    }
    if(!menu[iIndexMenu].tabElt[0].dataPop.hMenu)
    {
        MyPerror(" V,ERTOEI");
        return FALSE;
    }

    HMENU hMenuParent=menu[iIndexMenu].tabElt[0].dataPop.hMenu;
    UINT uFlag;
    UINT_PTR idItem;
    LPCSTR pszNameItem;

    uFlag=menu[iIndexMenu].tabElt[iIndexElt].dataItem.uFlag;
    idItem=(iIndexMenu)*(MIN_NBELT_PER_MENU-1)+(iIndexElt+iIndexMenu)+WM_USER;
    //We set a idItem calculated here.so that it will be defined in GCODEF.h
    menu[iIndexMenu].tabElt[iIndexElt].dataItem.idItem=idItem;
    pszNameItem=menu[iIndexMenu].tabElt[iIndexElt].dataItem.szNameItem;

    if(!AppendMenu(hMenuParent,uFlag,idItem,pszNameItem))
        return FALSE;

    return TRUE;
}

void MySetNameId(TCHAR *szNameId,int idDlg)
{
    //Le role de cette fonction est de creer un chaine unique
    //A partir de la classe + le numero actuel de lobjet de cette classe
    //1:Determiner la classe
    int nbObjSameType=0;
    static BOOL isRecursiveEntering=FALSE;//Specifie si cet appel est recursiv,(qu'il ya eu un premier appel) ou pas
    static int idUnique=1;//utilise en cas de isrecursiveEntering, pour reparcourir en commencant a 0

    switch(idDlg)
    {
    case DLG_BUTTON:
        strcpy(szNameId,"ID_BUTTON");
        if(!isRecursiveEntering)
            nbObjSameType=MyGetNbObjSameType(OBJ_BUTTON);
        break;
    case DLG_DIALOGBOX:
        strcpy(szNameId,"ID_DLG");
        if(!isRecursiveEntering)
            nbObjSameType=MyGetNbObjSameType(OBJ_DIALOGBOX);
        break;
    case DLG_STATIC:
        strcpy(szNameId,"ID_STATIC");
        if(!isRecursiveEntering)
            nbObjSameType=MyGetNbObjSameType(OBJ_STATIC);
        break;
    case DLG_COMBOBOX:
        strcpy(szNameId,"ID_COMBO");
        if(!isRecursiveEntering)
            nbObjSameType=MyGetNbObjSameType(OBJ_COMBOBOX);
        break;
    case DLG_EDITBOX:
        strcpy(szNameId,"ID_EDIT");
        if(!isRecursiveEntering)
            nbObjSameType=MyGetNbObjSameType(OBJ_EDITBOX);
        break;

    case DLG_LISTBOX:
        strcpy(szNameId,"ID_LIST");
        if(!isRecursiveEntering)
            nbObjSameType=MyGetNbObjSameType(OBJ_LISTBOX);
        break;

    case -1://cas des id de menu
        strcpy(szNameId,"IDM_");
        if(!isRecursiveEntering)
            nbObjSameType=MyGetNbObjSameType(-1);

        break;

    default:
        MyPerror("fkokog");
        return;
    }
    if(!isRecursiveEntering)
    {
        nbObjSameType++;
        char szNUm[1024];//10 est meme trop grand
        sprintf(szNUm,"%d",nbObjSameType);
        strcat(szNameId,szNUm);
    }
    else if(isRecursiveEntering)
    {
        //si il ya deja eu un autre appel de cette fonctionm, ca veut forcement
        //dire uqe szNameId ,davant est deja utilise, on va donc utiliser une autre methode
        char szNUm[10];//10 est meme trop grand
        sprintf(szNUm,"%d",idUnique);
        strcat(szNameId,szNUm);
        idUnique++;
    }

    //Tant que szNameId est deja utilise, il faudra trouver un nouveau
    while(isThisNameIdAlreadyUsed(szNameId,NULL))
    {
        isRecursiveEntering=TRUE;//Specifie quil s'agit d'une entree recursive
        MySetNameId(szNameId,idDlg);
    }
    idUnique=1;//Reinitialisation importante
    isRecursiveEntering=FALSE;//reinitialisation importante
    return ;
}

int MyGetNbObjSameType(int objType)
{
    //pARCOURS uObjet compte et retourne le nombre d'obj du
    //meme type
    //Si objtype==-1, alors la recherche se fait parmi les items de menu
    //de la boite de dialogue parente
    int nbObj=0;

    if(objType==-1)
    {
        //Chercher le nombre d'item de lunivers,
        //tout ca est possible si l'univers a un parent
        int iIndexParent=MyGetIindexObjWithHwnd(g_hwndParentObj);
        if(iIndexParent==-1)
        {
            return nbObj;
        }
        //Compter tous les item du parent,pas les pop, mais les items uniquement
        //Car les pop nont pas besoin detre identifies dans GCODEF.h
        for(int i=0;i<uObjet[iIndexParent].menu[0].iLastIndexMenu;i++)
        {
            for(int j=0;j<uObjet[iIndexParent].menu[i].iLastIinexElt;j++)
            {
                if(!uObjet[iIndexParent].menu[i].tabElt[j].isPopPupMenu)
                    nbObj++;
            }
        }
    }
    else if(objType!=-1)
    {
        //Compter le nombre dobjets de lunivers
        //qui ont le meme type=objtype
        for(int i=0;i<uObjet[0].nbObjCreated;i++)
            if(uObjet[i].iTypeObj==objType)
                nbObj++;

    }

    return nbObj;
}

BOOL isThisNameIdAlreadyUsed(TCHAR *szNameId,int *iIndexUsing)
{
    //cette fonction parcours uObje puis verifie si un obj
    //a un szNameId qui vaut celui en parametre
    //La recherche se fait aussi avec les items de menus
    //qui ont aussi des noms
    for(int i=0;i<uObjet[0].nbObjCreated;i++)
    {
        if(!strcmp(uObjet[i].szNameId,szNameId))
        {
            if(iIndexUsing)
                *iIndexUsing=i;
            return TRUE;
        }
    }

    if(!menuTemp)
    {
        return FALSE;
    }
    //Recherche dans les id de menus
    //recherche dans les id des items de menu
    for(int j=0;j<menuTemp[0].iLastIndexMenu;j++)
    {
        for(int k=0;k<menuTemp[j].iLastIinexElt;k++)
        {
            if(!menuTemp[j].tabElt[k].isPopPupMenu)
            {//Si on est en territoire popitem
                if(!strcmp(szNameId,menuTemp[j].tabElt[k].dataItem.szNameIdItem))
                {
                    //si on trouve un szNameId de meme nom
                    if(iIndexUsing)
                        *iIndexUsing=-1;
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}


BOOL isValidNameId(TCHAR *szNameId,int idObjExcluded)
{
    //retourne FALSE si szNameId n'est pas valide
    //ou est deja utilise
    char *c=szNameId;
    int taille1=0;
    if(!isalpha(*c) && *c!='_')
    {
        //Si le premier caratere n'est pas une lettre ou '_"
        MessageBox(GetForegroundWindow(),
        "Error: in <NAMEID>'s option,  first caracter should be a letter or '_' !","Error!!!",MB_ICONERROR|MB_OK|
        MB_TASKMODAL);
        return FALSE;
    }
    while(*c!='\0')
    {
        taille1++;
        if (!isalnum(*c) && *c != '_')
        {
            char buf[1024];

            sprintf(buf,"In <NAMEID>'s option, you use character '%c'.\n This styles Could Create problem when defining IDs in GCO.h.\nDo you still want to continue?  ",
                    *c);
            int iResult=MessageBox(GetForegroundWindow(),
                    buf,"Question:",MB_ICONQUESTION|MB_YESNO|
                    MB_TASKMODAL);
            if(iResult==IDNO)
                return FALSE;
        }
        c++;
    }

    if(*szNameId=='\0')
    {
        MessageBox(GetForegroundWindow(),"You need to Fill <NAMEID> 's Option",
                   "Error",MB_OK|MB_ICONERROR|MB_TASKMODAL);
        return FALSE;
    }

    //ON VA TESTER LA VALIDITE APRES: cARACTERES a utiliser
    int iIndexUsing=-1;
    if(isThisNameIdAlreadyUsed(szNameId,&iIndexUsing))
    {
        //Si ce nom est deja utilise
        if(idObjExcluded!=-1)
        {

            //Sil 'y'a actuellement un objet a modifier
            int iIndexEx=GetObjIndex(idObjExcluded);
            if(iIndexEx==-1)
            {
                MyPerror("kvokogk");
                MyMsgError();
                exit(EXIT_FAILURE);
            }
            if(iIndexEx!=iIndexUsing)
            {
                //si CE n'est pas l'indexEx(generalement celui qu'on modifie)
                //Qui a deja ce nom

                //On affiche un message D'erreur
                char buf[1024];
                wsprintf(buf,"Objet with id=%d has already <%s> as szNameId!!\n you need to change NAMEID for this objet",
                    uObjet[iIndexUsing].id,uObjet[iIndexUsing].szNameId);
                MessageBox(GetForegroundWindow(),buf,"Error:",
                    MB_OK|MB_ICONERROR|MB_TASKMODAL);
                return FALSE;
            }

        }
        else if(iIndexUsing!=-1)//Si lindex n'est pas un index de item menu, mais plutot d'uObjet
        {
            //Si szNameId est deja utilise et qu'il ya pas d'objet a modifier
            //On affiche un message D'erreur
            char buf[1024];
            wsprintf(buf,"Objet with id=%d has already <%s> as szNameId!!\n you need to change NAMEID for this objet",
                uObjet[iIndexUsing].id,uObjet[iIndexUsing].szNameId);
            MessageBox(GetForegroundWindow(),buf,"Error:",
                MB_OK|MB_ICONERROR|MB_TASKMODAL);
            return FALSE;
        }
        else if(iIndexUsing==-1)
        {//
            if(idObjToDelete)
            {
                //Si nous sommes en zone modify item menu
                //idObjToDelete, est le iIndexItem de menu

                //On sait deja que szNameId est deja duplique
                //On sait aussi que la chaine duplicante est dans les menuitems
                //Maintenant on doit verifier si la chaine correspond a l'index de iIndexItem
                for(int i=0;i<menuTemp[0].iLastIndexMenu;i++)
                {
                    for(int j=0;j<menuTemp[i].iLastIinexElt;j++)
                    {
                         if(!menuTemp[i].tabElt[j].isPopPupMenu)
                         {
                             if(!strcmp(szNameId,menuTemp[i].tabElt[j].dataItem.szNameIdItem))
                             {
                                 //Si lelement de meme id est le idObj=iIndexItem qui se modifie
                                 //Alors on ne considere pas ce cas
                                 return TRUE;
                             }
                         }
                    }
                }
            }
            MessageBox(GetForegroundWindow(),"Error: Another item of menu has a same NAMEID, you need to change This <NAMEID>'s option ","Error:",
                MB_OK|MB_ICONERROR|MB_TASKMODAL);
            return FALSE;
        }
    }
    return TRUE;
}

BOOL MyInitFshouldAutosize(FOUR_BOOL *tab,BOOL defValue)
{
    //initialise tab avec defVALUE
    if(!tab)
    {
        MyPerror("jksglfklhd");
        return FALSE;
    }
    tab->bottom=defValue;
    tab->left=defValue;
    tab->right=defValue;
    tab->top=defValue;

    return TRUE;
}
