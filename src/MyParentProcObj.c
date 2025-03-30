/*Nom de File:MyParentProc.c
Role:procedure de fenetre de la fenetre qui sera le parent
des objets que user va creer
BY CHIRSTIAN EDOUARDO LE 11/10/2024 a 19h00*/

#include <windows.h>
#include <tchar.h>
#include "resource.h"
#include "OpenDialogBox.h"
#include "mystrtok.h"

LRESULT CALLBACK MyClrDlgObjProc(HWND hwnd ,UINT msg,WPARAM wParam,LPARAM lParam)
{
    //Pour l'instant, cette fonction n'est utilisee que par Aspect
    //et lors de la creation proprement dite
    //Dans Aspect, il faut juste gerer l'affichage, tous les autres actions
    //comme les clis ou autres seront negliges
    int iIndexObj=0,i,idObj;
    int idDlg=DLG_DIALOGBOX;
    HWND hwndLastParentObj;
    char *pszClassName=NULL;

    //traitement de la fentre parente des objets,situe dans la zone aspect
    switch(msg)
    {
    case WM_CREATE:
        {

            HWND hwndParent=GetParent(hwnd);
            int iIndexObj=MyGetIindexObjWithHwnd(hwndParent);

            if(iIndexObj)
            {
                /*printf("\n jappique le cache dans aspect");
                MyApplyMenuCache(hwnd,mycache[DLG_DIALOGBOX].menu);
*/

            }

            if(!isChildGB(hwndParent)&&iIndexObj==-1 && hwndParent!=g_hwndParentObjToSet && hwndZC!=hwndParent)
            {
                return 0;//C'est qu'il s'agit des Obj de ID_ASPECT
            }

        }
        //Cas des obj crees dans le g_hwndParentObj
        //Charge les enfants de hwnd si il y'en a
        hwndLastParentObj=MyGetLastParentObj(NULL);
        if(!hwndLastParentObj)
        {
            return 0;
        }
        //Modifie le NULL par hwnd
        MyChangeHwndNewParent(NULL,hwnd);

        MYFONT fontParent=mycache[DLG_DIALOGBOX].font;
        int cxParent=mycache[DLG_DIALOGBOX].cxChar;
        int cyParent=mycache[DLG_DIALOGBOX].cyChar;
        int nbObj=uObjet[0].nbObjCreated;
        for(i=0;i<nbObj;i++)
        {
            if(uObjet[i].hwndParent==hwndLastParentObj)
            {
                switch(uObjet[i].iTypeObj)
                    {
                    case OBJ_BUTTON:
                        pszClassName=TEXT("button");
                        idDlg=DLG_BUTTON;
                        if(isGroupBoxObj(i))
                        {
                            g_hwndLastChildGB=uObjet[i].hwndChildGB;
                        }
                        break;
                    case OBJ_EDITBOX:
                        pszClassName=TEXT("edit");
                        idDlg=DLG_EDITBOX;
                        break;
                    case OBJ_LISTBOX:
                        pszClassName=TEXT("listbox");
                        idDlg=DLG_LISTBOX;
                        break;
                    case OBJ_COMBOBOX:
                        pszClassName=WC_COMBOBOX;
                        idDlg=DLG_COMBOBOX;
                        break;
                    case OBJ_STATIC:
                        pszClassName=TEXT("static");
                        idDlg=DLG_STATIC;
                        break;

                    case OBJ_DIALOGBOX:
                        pszClassName=szClassDlgObj;
                        idDlg=DLG_DIALOGBOX;
                        //copieHwnd=g_hwndParentObjToSet;
                        g_hwndParentObjToSet=hwnd;
                        MySetLastParentObj(uObjet[i].hwnd,NULL);
                        break;

                    case OBJ_IMGSTATIC:
                        pszClassName=NULL;
                        idDlg=DLG_IMGSTATIC;
                        break;
                    }

                    //Rempli mycache avec les styles de l'objet a dupliquer
                    //Car ce cache sera utiliser dans createObjet
                    strcpy(mycache[idDlg].szLastStylesChosen,uObjet[i].szLastStylesChosen);
                    mycache[idDlg].dwLastStylesChosen=uObjet[i].dwStyle;
                    mycache[idDlg].iLastX=uObjet[i].x;
                    mycache[idDlg].iLastY=uObjet[i].y;
                    mycache[idDlg].iLastW=uObjet[i].w;
                    mycache[idDlg].iLastH=uObjet[i].h;
                    mycache[idDlg].cxChar=uObjet[i].cxChar;
                    mycache[idDlg].cyChar=uObjet[i].cyChar;
                    mycache[idDlg].cxRest=uObjet[i].cxRest;
                    mycache[idDlg].cyRest=uObjet[i].cyRest;
                    mycache[idDlg].mycolorobj[BK]=uObjet[i].mycolorobj[BK];
                    mycache[idDlg].mycolorobj[TXT]=uObjet[i].mycolorobj[TXT];
                    mycache[idDlg].font=uObjet[i].font;
                    _tcscpy(mycache[idDlg].szTitle,uObjet[i].szTitle);
                    _tcscpy(mycache[idDlg].szNameId,uObjet[i].szNameId);
                    mycache[idDlg].linkopt=uObjet[i].linkopt;
                    mycache[idDlg].fShouldAutosize=uObjet[i].fShouldAutosize;
                    mycache[idDlg].font=uObjet[i].font;
                    if(uObjet[i].font.isFontParent==TRUE)
                    {
                        mycache[idDlg].font=fontParent;
                        mycache[idDlg].font.isFontParent=TRUE;
                        if(g_fShouldApplyFontOnChilds)
                        {
                            mycache[idDlg].cxChar=cxParent;
                            mycache[idDlg].cyChar=cyParent;
                        }
                    }

                  /*  if(idDlg==DLG_DIALOGBOX)
                    {
                        if(!MyCopieMenuTab(uObjet[i].menu,&mycache[idDlg].menu))
                        {
                            MyPerror(",soerpwo");
                            exit(EXIT_FAILURE);
                        }
                    }
*/
                    idObj=uObjet[i].id;

                    isTypeModDlg=TRUE;//Impacte dans MyDeleteObj pour pas checker les childs
                    idObjToDelete=idObj;
                    CreateObjet(pszClassName,idDlg,hwnd);
                    idObjToDelete=0;
                    MyDeleteObj(idObj);
                    isTypeModDlg=FALSE;//Renitialise
                    //g_hwndParentObjToSet=copieHwnd;
                    i=-1;
                    //nbObj--;
                }
            }
            return 0;

    case WM_PAINT:
        {
        iIndexObj=MyGetIindexObjWithHwnd(hwnd);
        HWND hwndParentObj;
        int iIndexParentObj;

        if(iIndexObj==-1)//Nous sommes dans ID_ASPECT
        {
            hwndParentObj=g_hwndParentObjToSet;

        }

        else
        {
            hwndParentObj=GetParent(hwnd);
        }

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
                    hwndParentObj=GetParent(hwnd);
                }
                else
                {
                    iIndexParentObj=MyGetIindexObjWithHwnd(hwndParentObj);
                }
            }
        }
        if(iIndexObj==-1)
        {//Image dans ASPECT
            //For Obj Created
            HDC hdc;
            PAINTSTRUCT ps;


            hdc=BeginPaint(hwnd,&ps);
            COLORREF couleur;
            static HBRUSH hBrushStatic;

            //TXT
            if(mycache[idDlg].mycolorobj[TXT].isDefChoosen)
            {
                int iIndex=mycache[idDlg].mycolorobj[TXT].defColor;
                if(iIndex==COLOR_TXT_PARENT)
                {
                    //obtenir la couleur de texte du parent
                    //ici parent,=g_hwndParentObj
                    if(iIndexParentObj==-1 )
                    {
                        HDC hdc=GetDC(hwndParentObj);
                        couleur=GetTextColor(hdc);
                        ReleaseDC(hwndParentObj,hdc);
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
                        HDC hdc=GetDC(hwndParentObj);
                        couleur=GetBkColor(hdc);
                        ReleaseDC(hwndParentObj,hdc);
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
            SetTextColor(hdc,couleur);
            //BK
            if(mycache[idDlg].mycolorobj[BK].isDefChoosen)
            {
                int iIndex=mycache[idDlg].mycolorobj[BK].defColor;
                if(iIndex==COLOR_TXT_PARENT)
                {
                    //obtenir la couleur de texte du parent
                    if(iIndexParentObj==-1)
                    {
                        HDC hdc=GetDC(hwndParentObj);
                        couleur=GetTextColor(hdc);
                        ReleaseDC(hwndParentObj,hdc);
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
                        HDC hdc=GetDC(hwndParentObj);
                        couleur=GetBkColor(hdc);
                        ReleaseDC(hwndParentObj,hdc);
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

            DeleteObject(hBrushStatic);
            hBrushStatic=CreateSolidBrush(couleur);
            SetBkColor(hdc,couleur);
            FillRect(hdc,&ps.rcPaint,hBrushStatic);

            //TextOut(hdc,0,0,"BONSOIR",7);
            EndPaint(hwnd,&ps);
        }
        else if(iIndexObj!=-1)
        {//For non ID8ASPECT obj
            //For Obj Created
            HDC hdc;
            PAINTSTRUCT ps;

            hdc=BeginPaint(hwnd,&ps);
            COLORREF couleur;
            static HBRUSH hBrushStatic;

            //TXT
            if(uObjet[iIndexObj].mycolorobj[TXT].isDefChoosen)
            {
                int iIndex=uObjet[iIndexObj].mycolorobj[TXT].defColor;
                if(iIndex==COLOR_TXT_PARENT)
                {
                    //obtenir la couleur de texte du parent
                    //ici parent,=g_hwndParentObj
                    if(iIndexParentObj==-1)
                    {
                        HDC hdc=GetDC(hwndParentObj);
                        couleur=GetTextColor(hdc);
                        ReleaseDC(hwndParentObj,hdc);
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
                        HDC hdc=GetDC(hwndParentObj);
                        couleur=GetBkColor(hdc);
                        ReleaseDC(hwndParentObj,hdc);
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
                couleur=uObjet[iIndexObj].mycolorobj[TXT].rgbColor;
            }
            SetTextColor(hdc,couleur);

            //BK
            if(uObjet[iIndexObj].mycolorobj[BK].isDefChoosen)
            {
                int iIndex=uObjet[iIndexObj].mycolorobj[BK].defColor;
                if(iIndex==COLOR_TXT_PARENT)
                {
                    //obtenir la couleur de texte du parent
                    if(iIndexParentObj==-1)
                    {
                        HDC hdc=GetDC(hwndParentObj);
                        couleur=GetTextColor(hdc);
                        ReleaseDC(hwndParentObj,hdc);
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
                        HDC hdc=GetDC(hwndParentObj);
                        couleur=GetBkColor(hdc);
                        ReleaseDC(hwndParentObj,hdc);
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
                couleur=uObjet[iIndexObj].mycolorobj[BK].rgbColor;
            }

            DeleteObject(hBrushStatic);
            hBrushStatic=CreateSolidBrush(couleur);
            SetBkColor(hdc,couleur);
            FillRect(hdc,&ps.rcPaint,hBrushStatic);

            //TextOut(hdc,0,0,"BONSOIR",7);
            EndPaint(hwnd,&ps);
        }

        }
        return 0;

    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORMSGBOX:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
    {
        COLORREF couleur;
        static HBRUSH hBrush;
        UNIVERS uTemp;
        HWND hwndObjet,hwndParentObj;
        int iIndex,iIndexParentObj;

        hwndObjet=(HWND)lParam;
        hwndParentObj=hwnd;

        iIndexParentObj=MyGetIindexObjWithHwnd(hwndParentObj);
        iIndex=MyGetIindexObjWithHwnd(hwndObjet);
        if(iIndex==-1) break;


        uTemp=uObjet[iIndex];
        //TXT
        if(uTemp.mycolorobj[TXT].isDefChoosen)
        {
            int iIndex=uTemp.mycolorobj[TXT].defColor;
            if(iIndex==COLOR_TXT_PARENT)
            {
                //obtenir la couleur de texte du parent

                if(iIndexParentObj==-1)
                {
                    HDC hdc=GetDC(hwndParentObj);
                    couleur=GetTextColor(hdc);
                    ReleaseDC(hwndParentObj,hdc);
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
                    HDC hdc=GetDC(hwndParentObj);
                    couleur=GetBkColor(hdc);
                    ReleaseDC(hwndParentObj,hdc);
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
            couleur=uTemp.mycolorobj[TXT].rgbColor;
        }
        SetTextColor((HDC)wParam,couleur);

        //BK
        if(uTemp.mycolorobj[BK].isDefChoosen)
        {
            int iIndex=uTemp.mycolorobj[BK].defColor;
            if(iIndex==COLOR_TXT_PARENT)
            {
                //obtenir la couleur de texte du parent
                if(iIndexParentObj==-1)
                {
                    HDC hdc=GetDC(hwndParentObj);
                    couleur=GetTextColor(hdc);
                    ReleaseDC(hwndParentObj,hdc);
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
                    HDC hdc=GetDC(hwndParentObj);
                    couleur=GetBkColor(hdc);
                    ReleaseDC(hwndParentObj,hdc);
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
            couleur=uTemp.mycolorobj[BK].rgbColor;
        }

        DeleteObject(hBrush);
        hBrush=CreateSolidBrush(couleur);
        SetBkColor((HDC)wParam,couleur);
        return (LRESULT)  hBrush;

        }
        break;//Unreacheable

        //Gestion de l'affichage de l'infobulle
        case WM_NOTIFY:
        {
            switch (((LPNMHDR)lParam)->code)
            {
            case TTN_GETDISPINFO:
                {
                int iIndex,idObj;
                TOOLINFO ti={0};
                LPNMTTDISPINFO pInfo = (LPNMTTDISPINFO)lParam;
               //Obtient les inifos sur la structure TOOLINFO
                ti.cbSize = sizeof(TOOLINFO);
                ti.hwnd = pInfo->hdr.hwndFrom;

                // Obtenir l'outil courant
                SendMessage(pInfo->hdr.hwndFrom, TTM_GETCURRENTTOOL, 0, (LPARAM)&ti);

                // ti.uId contient l'ID du bouton associé
                //Mais c'est en fait le HWND de l'objet a cause du TTF_IDISHWND
                idObj=GetDlgCtrlID((HWND)ti.uId);
                iIndex=GetObjIndex(idObj);
                if(iIndex==-1)
                {
                    if(isChildGB((HWND)ti.uId))
                    {
                        idObj=GetDlgCtrlID(GetParent((HWND)ti.uId));
                        iIndex=GetObjIndex(idObj);
                        if(iIndex==-1)
                        {
                            MyPerror("Unknown error, at MyParentOBj,:iIndex==-1 :WM_NOTIFY");
                            break;
                        }

                    }
                    else
                        break;
                }

                SendMessage(pInfo->hdr.hwndFrom, TTM_SETMAXTIPWIDTH, 0, 150);
                pInfo->lpszText=uObjet[iIndex].szDescription;

                }
                break;//fin case TTN_GETDISPINFO
            }//fin switch(code)
        }
        break;//fin case WM_NOTIFY

    case WM_DESTROY:

        return 0;
     }
    return DefWindowProc(hwnd,msg,wParam,lParam);
}
