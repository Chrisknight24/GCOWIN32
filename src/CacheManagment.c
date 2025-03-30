/*Name File:CacheManagement.c
Rule:Manage mycache[] structure .
*/
#include <windows.h>
#include <tchar.h>
#include "resource.h"
#include "OpenDialogBox.h"
#include "manageProjet.h"
#include "manageFont.h"


char szStylesChosen[MAXLENTGHT_OF_SZSTYLECHOSEN]="";
BOOL MyLoadCache(int idDlg,MYCACHE_DLG *myValues,HWND hDlg)
{
    //Load on dlgBox all the datas contained in mycache[idDlg]
    if(!hDlg) return FALSE;
    int i,nbStyles;
    DWORD dwStyleGroup,dwStyle;
    HWND hwndStatic,hwndEdit,hwndChildDlg,hwndChildChildDlg;
    TCHAR buf[MAXDLGCHAR],szCopie[MAXLENTGHT_OF_SZSTYLECHOSEN];
    int valeur=1;

    switch(idDlg)
    {
    case DLG_BUTTON:
        //X
        valeur=mycache[idDlg].iLastX;
        hwndStatic=GetDlgItem(hDlg,ID_STATIC_X);
        if(!hwndStatic) return FALSE;
        wsprintf(buf,"%d",valeur);
        SetWindowText(hwndStatic,buf);
        //Y
        valeur=mycache[idDlg].iLastY;
        hwndStatic=GetDlgItem(hDlg,ID_STATIC_Y);
        if(!hwndStatic) return FALSE;
        wsprintf(buf,"%d",valeur);
        SetWindowText(hwndStatic,buf);
        //W
        valeur=mycache[idDlg].iLastW;
        hwndStatic=GetDlgItem(hDlg,ID_STATIC_W);
        if(!hwndStatic) return FALSE;

        if(mycache[idDlg].isPixelUnit)
            valeur*=mycache[idDlg].cxChar;
        wsprintf(buf,"%d",valeur);
        SetWindowText(hwndStatic,buf);
        //H
        valeur=mycache[idDlg].iLastH;
        hwndStatic=GetDlgItem(hDlg,ID_STATIC_H);
        if(!hwndStatic) return FALSE;
        if(mycache[idDlg].isPixelUnit)
            valeur*=mycache[idDlg].cyChar;
        wsprintf(buf,"%d",valeur);
        SetWindowText(hwndStatic,buf);

        //button title
        hwndEdit=GetDlgItem(hDlg,ID_EDIT_TITLE);
        if(!hwndEdit) return FALSE;
        SetWindowText(hwndEdit,mycache[idDlg].szTitle);

        //Load  a nameId
        hwndEdit=GetDlgItem(hDlg,IDE_NAMEID);
        if(!hwndEdit) return FALSE;
        SetWindowText(hwndEdit,mycache[idDlg].szNameId);

        //Load styles
        hwndChildDlg=GetDlgItem(hDlg,ID_STYLE_LIST);
        if(!hwndChildDlg) return FALSE;
        nbStyles=mycache[idDlg].valueStyle[0].dwValuesStyles;
        dwStyleGroup=mycache[idDlg].dwLastStylesChosen;

        //Do a copy, cause originale is global and could be update in further calls
        strcpy(szCopie,mycache[idDlg].szLastStylesChosen);
        szStylesChosen[0]='\0';
        for(i=1;i<=nbStyles;i++)
        {
            dwStyle=mycache[idDlg].valueStyle[i].dwValuesStyles;
            if(( !dwStyle || dwStyleGroup&dwStyle) && (strstr(szCopie,mycache[idDlg].valueStyle[i].szValuesStyles)) )
            {
                //retrieve
                if(dwStyle)
                    dwStyleGroup&=~dwStyle;

                hwndChildChildDlg=GetDlgItem(hwndChildDlg,i);
                if(!hwndChildChildDlg) return FALSE;
                //Simulate a click on child ckeckbutton
                CheckDlgButton(hwndChildDlg,i,1);
                SendMessage(hwndChildDlg,WM_COMMAND,
                    (WPARAM)MAKELONG(i,BN_CLICKED),(LPARAM)hwndChildChildDlg);

            }

        }
        //Refresh aspect of object
        MyRefreshAspect(hDlg,idDlg);

        //Update myvalues
        *myValues=mycache[idDlg];

        //Inform that the next entry wont be a first
        if(isFirstLoad[idDlg]==TRUE) isFirstLoad[idDlg]=FALSE;
        break;//fin case DLG_BUTTON


    case DLG_LISTBOX:

        //X
        valeur=mycache[idDlg].iLastX;
        hwndStatic=GetDlgItem(hDlg,ID_STATIC_X);
        if(!hwndStatic) return FALSE;
        wsprintf(buf,"%d",valeur);
        SetWindowText(hwndStatic,buf);

        //Y
        valeur=mycache[idDlg].iLastY;
        hwndStatic=GetDlgItem(hDlg,ID_STATIC_Y);
        if(!hwndStatic) return FALSE;
        wsprintf(buf,"%d",valeur);
        SetWindowText(hwndStatic,buf);
        //W
        valeur=mycache[idDlg].iLastW;
        hwndStatic=GetDlgItem(hDlg,ID_STATIC_W);
        if(!hwndStatic) return FALSE;

        if(mycache[idDlg].isPixelUnit)
            valeur*=mycache[idDlg].cxChar;
        wsprintf(buf,"%d",valeur);
        SetWindowText(hwndStatic,buf);
        //H
        valeur=mycache[idDlg].iLastH;
        hwndStatic=GetDlgItem(hDlg,ID_STATIC_H);
        if(!hwndStatic) return FALSE;
        if(mycache[idDlg].isPixelUnit)
            valeur*=mycache[idDlg].cyChar;
        wsprintf(buf,"%d",valeur);
        SetWindowText(hwndStatic,buf);

        hwndEdit=GetDlgItem(hDlg,ID_EDIT_TITLE);
        if(!hwndEdit) return FALSE;
        SetWindowText(hwndEdit,mycache[idDlg].szTitle);

        // nameId
        hwndEdit=GetDlgItem(hDlg,IDE_NAMEID);
        if(!hwndEdit) return FALSE;
        SetWindowText(hwndEdit,mycache[idDlg].szNameId);

        // style
        hwndChildDlg=GetDlgItem(hDlg,ID_STYLE_LIST);
        if(!hwndChildDlg) return FALSE;
        nbStyles=mycache[idDlg].valueStyle[0].dwValuesStyles;
        dwStyleGroup=mycache[idDlg].dwLastStylesChosen;

        strcpy(szCopie,mycache[idDlg].szLastStylesChosen);
        szStylesChosen[0]='\0';
        for(i=1;i<=nbStyles;i++)
        {
            dwStyle=mycache[idDlg].valueStyle[i].dwValuesStyles;
            if(( !dwStyle || dwStyleGroup&dwStyle) && (strstr(szCopie,mycache[idDlg].valueStyle[i].szValuesStyles)) )
            {
                if(dwStyle)
                    dwStyleGroup&=~dwStyle;

                hwndChildChildDlg=GetDlgItem(hwndChildDlg,i);
                if(!hwndChildChildDlg) return FALSE;

                CheckDlgButton(hwndChildDlg,i,1);
                SendMessage(hwndChildDlg,WM_COMMAND,
                    (WPARAM)MAKELONG(i,BN_CLICKED),(LPARAM)hwndChildChildDlg);

            }

        }

        MyRefreshAspect(hDlg,idDlg);

        *myValues=mycache[idDlg];

        if(isFirstLoad[idDlg]==TRUE) isFirstLoad[idDlg]=FALSE;
        break;//fin case DLG_LISTBOX

    case DLG_EDITBOX:
        valeur=mycache[idDlg].iLastX;
        hwndStatic=GetDlgItem(hDlg,ID_STATIC_X);
        if(!hwndStatic) return FALSE;
        wsprintf(buf,"%d",valeur);
        SetWindowText(hwndStatic,buf);
        //Y
        valeur=mycache[idDlg].iLastY;
        hwndStatic=GetDlgItem(hDlg,ID_STATIC_Y);
        if(!hwndStatic) return FALSE;
        wsprintf(buf,"%d",valeur);
        SetWindowText(hwndStatic,buf);
        //W
        valeur=mycache[idDlg].iLastW;
        hwndStatic=GetDlgItem(hDlg,ID_STATIC_W);
        if(!hwndStatic) return FALSE;

        if(mycache[idDlg].isPixelUnit)
            valeur*=mycache[idDlg].cxChar;
        wsprintf(buf,"%d",valeur);
        SetWindowText(hwndStatic,buf);
        //H
        valeur=mycache[idDlg].iLastH;
        hwndStatic=GetDlgItem(hDlg,ID_STATIC_H);
        if(!hwndStatic) return FALSE;
        if(mycache[idDlg].isPixelUnit)
            valeur*=mycache[idDlg].cyChar;
        wsprintf(buf,"%d",valeur);
        SetWindowText(hwndStatic,buf);

        //title
        hwndEdit=GetDlgItem(hDlg,ID_EDIT_TITLE);
        if(!hwndEdit) return FALSE;
        SetWindowText(hwndEdit,mycache[idDlg].szTitle);

        //nameid
        hwndEdit=GetDlgItem(hDlg,IDE_NAMEID);
        if(!hwndEdit) return FALSE;
        SetWindowText(hwndEdit,mycache[idDlg].szNameId);

        //style
        hwndChildDlg=GetDlgItem(hDlg,ID_STYLE_LIST);
        if(!hwndChildDlg) return FALSE;
        nbStyles=mycache[idDlg].valueStyle[0].dwValuesStyles;
        dwStyleGroup=mycache[idDlg].dwLastStylesChosen;

        strcpy(szCopie,mycache[idDlg].szLastStylesChosen);
        szStylesChosen[0]='\0';
        for(i=1;i<=nbStyles;i++)
        {
            dwStyle=mycache[idDlg].valueStyle[i].dwValuesStyles;
            if(( !dwStyle || dwStyleGroup&dwStyle) && (strstr(szCopie,mycache[idDlg].valueStyle[i].szValuesStyles)) )
            {
                if(dwStyle)
                    dwStyleGroup&=~dwStyle;

                hwndChildChildDlg=GetDlgItem(hwndChildDlg,i);
                if(!hwndChildChildDlg) return FALSE;

                CheckDlgButton(hwndChildDlg,i,1);
                SendMessage(hwndChildDlg,WM_COMMAND,
                    (WPARAM)MAKELONG(i,BN_CLICKED),(LPARAM)hwndChildChildDlg);

            }

        }

        MyRefreshAspect(hDlg,idDlg);

        //Update
        *myValues=mycache[idDlg];

        if(isFirstLoad[idDlg]==TRUE) isFirstLoad[idDlg]=FALSE;
        break;//fin case DLG_EDITBOX


    case DLG_COMBOBOX:

        //X
        valeur=mycache[idDlg].iLastX;
        hwndStatic=GetDlgItem(hDlg,ID_STATIC_X);
        if(!hwndStatic) return FALSE;
        wsprintf(buf,"%d",valeur);
        SetWindowText(hwndStatic,buf);
        //Y
        valeur=mycache[idDlg].iLastY;
        hwndStatic=GetDlgItem(hDlg,ID_STATIC_Y);
        if(!hwndStatic) return FALSE;
        wsprintf(buf,"%d",valeur);
        SetWindowText(hwndStatic,buf);
        //W
        valeur=mycache[idDlg].iLastW;
        hwndStatic=GetDlgItem(hDlg,ID_STATIC_W);
        if(!hwndStatic) return FALSE;

        if(mycache[idDlg].isPixelUnit)
            valeur*=mycache[idDlg].cxChar;
        wsprintf(buf,"%d",valeur);
        SetWindowText(hwndStatic,buf);
        //H
        valeur=mycache[idDlg].iLastH;
        hwndStatic=GetDlgItem(hDlg,ID_STATIC_H);
        if(!hwndStatic) return FALSE;
        if(mycache[idDlg].isPixelUnit)
            valeur*=mycache[idDlg].cyChar;
        wsprintf(buf,"%d",valeur);
        SetWindowText(hwndStatic,buf);

        hwndEdit=GetDlgItem(hDlg,ID_EDIT_TITLE);
        if(!hwndEdit) return FALSE;
        SetWindowText(hwndEdit,mycache[idDlg].szTitle);

        hwndEdit=GetDlgItem(hDlg,IDE_NAMEID);
        if(!hwndEdit) return FALSE;
        SetWindowText(hwndEdit,mycache[idDlg].szNameId);

        hwndChildDlg=GetDlgItem(hDlg,ID_STYLE_LIST);
        if(!hwndChildDlg) return FALSE;
        nbStyles=mycache[idDlg].valueStyle[0].dwValuesStyles;
        dwStyleGroup=mycache[idDlg].dwLastStylesChosen;

        strcpy(szCopie,mycache[idDlg].szLastStylesChosen);
        szStylesChosen[0]='\0';
        for(i=1;i<=nbStyles;i++)
        {
            dwStyle=mycache[idDlg].valueStyle[i].dwValuesStyles;
            if(( !dwStyle || dwStyleGroup&dwStyle) && (strstr(szCopie,mycache[idDlg].valueStyle[i].szValuesStyles)) )
            {
                if(dwStyle)
                    dwStyleGroup&=~dwStyle;

                hwndChildChildDlg=GetDlgItem(hwndChildDlg,i);
                if(!hwndChildChildDlg) return FALSE;

                CheckDlgButton(hwndChildDlg,i,1);
                SendMessage(hwndChildDlg,WM_COMMAND,
                    (WPARAM)MAKELONG(i,BN_CLICKED),(LPARAM)hwndChildChildDlg);

            }

        }
        MyRefreshAspect(hDlg,idDlg);

        *myValues=mycache[idDlg];

        if(isFirstLoad[idDlg]==TRUE) isFirstLoad[idDlg]=FALSE;

        break;//fin case DLG_COMBOBOX



    case DLG_STATIC:
        valeur=mycache[idDlg].iLastX;
        hwndStatic=GetDlgItem(hDlg,ID_STATIC_X);
        if(!hwndStatic) return FALSE;
        wsprintf(buf,"%d",valeur);
        SetWindowText(hwndStatic,buf);

        valeur=mycache[idDlg].iLastY;
        hwndStatic=GetDlgItem(hDlg,ID_STATIC_Y);
        if(!hwndStatic) return FALSE;
        wsprintf(buf,"%d",valeur);
        SetWindowText(hwndStatic,buf);

        valeur=mycache[idDlg].iLastW;
        hwndStatic=GetDlgItem(hDlg,ID_STATIC_W);
        if(!hwndStatic) return FALSE;

        if(mycache[idDlg].isPixelUnit)//Si les mesures doivent etre en pixel, par defaut elles sont en unite de caractere
            valeur*=mycache[idDlg].cxChar;
        wsprintf(buf,"%d",valeur);
        SetWindowText(hwndStatic,buf);

        valeur=mycache[idDlg].iLastH;
        hwndStatic=GetDlgItem(hDlg,ID_STATIC_H);
        if(!hwndStatic) return FALSE;
        if(mycache[idDlg].isPixelUnit)
            valeur*=mycache[idDlg].cyChar;
        wsprintf(buf,"%d",valeur);
        SetWindowText(hwndStatic,buf);


        hwndEdit=GetDlgItem(hDlg,ID_EDIT_TITLE);
        if(!hwndEdit) return FALSE;
        SetWindowText(hwndEdit,mycache[idDlg].szTitle);


        hwndEdit=GetDlgItem(hDlg,IDE_NAMEID);
        if(!hwndEdit) return FALSE;
        SetWindowText(hwndEdit,mycache[idDlg].szNameId);

        hwndChildDlg=GetDlgItem(hDlg,ID_STYLE_LIST);
        if(!hwndChildDlg) return FALSE;
        nbStyles=mycache[idDlg].valueStyle[0].dwValuesStyles;
        dwStyleGroup=mycache[idDlg].dwLastStylesChosen;

        strcpy(szCopie,mycache[idDlg].szLastStylesChosen);
        szStylesChosen[0]='\0';
        for(i=1;i<=nbStyles;i++)
        {
            dwStyle=mycache[idDlg].valueStyle[i].dwValuesStyles;
            if(( !dwStyle || dwStyleGroup&dwStyle) && (strstr(szCopie,mycache[idDlg].valueStyle[i].szValuesStyles)) )
            {
                if(dwStyle)
                    dwStyleGroup&=~dwStyle;

                hwndChildChildDlg=GetDlgItem(hwndChildDlg,i);
                if(!hwndChildChildDlg) return FALSE;

                CheckDlgButton(hwndChildDlg,i,1);
                SendMessage(hwndChildDlg,WM_COMMAND,
                    (WPARAM)MAKELONG(i,BN_CLICKED),(LPARAM)hwndChildChildDlg);

            }

        }
        MyRefreshAspect(hDlg,idDlg);

        *myValues=mycache[idDlg];

        if(isFirstLoad[idDlg]==TRUE) isFirstLoad[idDlg]=FALSE;

        break;//fin case DLG_STATIC


        case DLG_DIALOGBOX:
        valeur=mycache[idDlg].iLastX;
        hwndStatic=GetDlgItem(hDlg,ID_STATIC_X);
        if(!hwndStatic) return FALSE;
        wsprintf(buf,"%d",valeur);
        SetWindowText(hwndStatic,buf);

        valeur=mycache[idDlg].iLastY;
        hwndStatic=GetDlgItem(hDlg,ID_STATIC_Y);
        if(!hwndStatic) return FALSE;
        wsprintf(buf,"%d",valeur);
        SetWindowText(hwndStatic,buf);

        valeur=mycache[idDlg].iLastW;
        hwndStatic=GetDlgItem(hDlg,ID_STATIC_W);
        if(!hwndStatic) return FALSE;

        if(mycache[idDlg].isPixelUnit)//Si les mesures doivent etre en pixel, par defaut elles sont en unite de caractere
            valeur*=mycache[idDlg].cxChar;
        wsprintf(buf,"%d",valeur);
        SetWindowText(hwndStatic,buf);

        valeur=mycache[idDlg].iLastH;
        hwndStatic=GetDlgItem(hDlg,ID_STATIC_H);
        if(!hwndStatic) return FALSE;
        if(mycache[idDlg].isPixelUnit)
            valeur*=mycache[idDlg].cyChar;
        wsprintf(buf,"%d",valeur);
        SetWindowText(hwndStatic,buf);

        hwndEdit=GetDlgItem(hDlg,ID_EDIT_TITLE);
        if(!hwndEdit) return FALSE;
        SetWindowText(hwndEdit,mycache[idDlg].szTitle);


        hwndEdit=GetDlgItem(hDlg,IDE_NAMEID);
        if(!hwndEdit) return FALSE;
        SetWindowText(hwndEdit,mycache[idDlg].szNameId);

        hwndChildDlg=GetDlgItem(hDlg,ID_STYLE_LIST);
        if(!hwndChildDlg) return FALSE;
        nbStyles=mycache[idDlg].valueStyle[0].dwValuesStyles;
        dwStyleGroup=mycache[idDlg].dwLastStylesChosen;

        if(isParentWnd&& !isTypeModDlg)
            strcpy(szCopie,"WS_OVERLAPPEDWINDOW|");
        else
            strcpy(szCopie,mycache[idDlg].szLastStylesChosen);

        szStylesChosen[0]='\0';
        for(i=1;i<=nbStyles;i++)
        {
            dwStyle=mycache[idDlg].valueStyle[i].dwValuesStyles;
            if(( !dwStyle || dwStyleGroup&dwStyle) && (strstr(szCopie,mycache[idDlg].valueStyle[i].szValuesStyles)) )
            {
                if(dwStyle)
                    dwStyleGroup&=~dwStyle;

                hwndChildChildDlg=GetDlgItem(hwndChildDlg,i);
                if(!hwndChildChildDlg) return FALSE;

                CheckDlgButton(hwndChildDlg,i,1);
                SendMessage(hwndChildDlg,WM_COMMAND,
                    (WPARAM)MAKELONG(i,BN_CLICKED),(LPARAM)hwndChildChildDlg);

            }

        }
        MyRefreshAspect(hDlg,idDlg);

        *myValues=mycache[idDlg];

        if(isFirstLoad[idDlg]==TRUE) isFirstLoad[idDlg]=FALSE;

        break;//fin case DLG_DIALOGBOX

    case -1:
        //Here with are dealing with menus display
        hwndChildDlg=GetDlgItem(hDlg,ID_FLAGITEM_LIST);
        if(!hwndChildDlg)
        {
            MyPerror("v merieo");
            return FALSE;
        }

        nbStyles=g_mycacheItem.tabStyleItemMenu[0].dwValuesStyles;
        dwStyleGroup=(DWORD)(g_mycacheItem.uLastStylesChosen);

         //Uncheck All
         for(int j=1;j<=nbStyles;j++)
        {
            hwndChildChildDlg=GetDlgItem(hwndChildDlg,j);
            CheckDlgButton(hwndChildDlg,j,0);
        }

        //Do a copie of a global variable which could change.
        strcpy(szCopie,g_mycacheItem.szLastStylesChosen);
        strcat(szCopie,"|");

        g_mycacheItem.szLastStylesChosen[0]='\0';
        for(i=1;i<=nbStyles;i++)
        {
            dwStyle=g_mycacheItem.tabStyleItemMenu[i].dwValuesStyles;

            if(( !dwStyle || dwStyleGroup&dwStyle) && (strstr(szCopie,g_mycacheItem.tabStyleItemMenu[i].szValuesStyles)) )
            {
                if(dwStyle)
                    dwStyleGroup&=~dwStyle;

                hwndChildChildDlg=GetDlgItem(hwndChildDlg,i);
                if(!hwndChildChildDlg) return FALSE;

                //Simulate a click
                CheckDlgButton(hwndChildDlg,i,1);
                SendMessage(hwndChildDlg,WM_COMMAND,
                    (WPARAM)MAKELONG(i,BN_CLICKED),(LPARAM)hwndChildChildDlg);

            }

        }
        //Restore
        strcpy(g_mycacheItem.szLastStylesChosen,szCopie);
        //del |
        g_mycacheItem.szLastStylesChosen[strlen(szCopie)-1]='\0';
        break;//FIN CASE itemMenu
    }
    return TRUE;
}


BOOL MySaveCache(int idDlg,MYCACHE_DLG myValues,HWND hDlg)
{
    //This func add datas of myValues to myCache(global).
    //these datas are mainly dimensions
    if(!hDlg) return FALSE;

    //Get Title
    GetWindowText(GetDlgItem(hDlg,ID_EDIT_TITLE),myValues.szTitle,MAXDLGCHAR);
    //get NameId
    GetWindowText(GetDlgItem(hDlg,IDE_NAMEID),myValues.szNameId,MAXDLGCHAR);

    //get color
    myValues.mycolorobj[BK]=mycache[idDlg].mycolorobj[BK];
    myValues.mycolorobj[TXT]=mycache[idDlg].mycolorobj[TXT];

    //get font
    myValues.font=mycache[idDlg].font;

    //get style
    myValues.dwLastStylesChosen=mycache[idDlg].dwLastStylesChosen;
    strcpy(myValues.szLastStylesChosen,mycache[idDlg].szLastStylesChosen);
    for(int i=0;i<=mycache[idDlg].valueStyle[0].dwValuesStyles;i++)
    myValues.valueStyle[i]=mycache[idDlg].valueStyle[i];

    myValues.cxChar=mycache[idDlg].cxChar;
    myValues.cyChar=mycache[idDlg].cyChar;
    myValues.cxRest=mycache[idDlg].cxRest;
    myValues.cyRest=mycache[idDlg].cyRest;

    //At the beginning myValues has the dimension, so we have added
    //mycache to myvalues and  re-add myvalues to mycache for copie dimensions
    mycache[idDlg]=myValues;
    return TRUE;
}

BOOL Decrement(int idDlg,int idStaticChildDlg,MYCACHE_DLG *myValues,HWND hDlg)
{
    //For Decrementing values of x,y,w,h on dialogBoxes
    if(!hDlg || !myValues) return FALSE;

    HWND hwndStatic;
    int iLastValue;
    TCHAR buf[MAXDLGCHAR];

    switch(idStaticChildDlg)
    {
    case ID_STATIC_X:
        hwndStatic=GetDlgItem(hDlg,ID_STATIC_X);
        if(!hwndStatic) return FALSE;
        iLastValue=myValues->iLastX;
        myValues->iLastX=max(1,myValues->iLastX-1);
        if(iLastValue!=myValues->iLastX)
        {
            wsprintf(buf,"%d",myValues->iLastX);
            SetWindowText(hwndStatic,buf);
        }
        break;

    case ID_STATIC_Y:
        hwndStatic=GetDlgItem(hDlg,ID_STATIC_Y);
        if(!hwndStatic) return FALSE;
        iLastValue=myValues->iLastY;
        myValues->iLastY=max(1,myValues->iLastY-1);
        if(iLastValue!=myValues->iLastY)
        {
            wsprintf(buf,"%d",myValues->iLastY);
            SetWindowText(hwndStatic,buf);
        }
        break;

    case ID_STATIC_W:
        hwndStatic=GetDlgItem(hDlg,ID_STATIC_W);
        if(!hwndStatic) return FALSE;
        iLastValue=myValues->iLastW;
        myValues->iLastW=max(1,myValues->iLastW-1);
        if(iLastValue!=myValues->iLastW)
        {
            wsprintf(buf,"%d",myValues->iLastW);
            SetWindowText(hwndStatic,buf);
        }
        break;

    case ID_STATIC_H:
        hwndStatic=GetDlgItem(hDlg,ID_STATIC_H);
        if(!hwndStatic) return FALSE;
        iLastValue=myValues->iLastH;
        myValues->iLastH=max(1,myValues->iLastH-1);
        if(iLastValue!=myValues->iLastH)
        {
            wsprintf(buf,"%d",myValues->iLastH);
            SetWindowText(hwndStatic,buf);
        }
        break;
    }


    return TRUE;
}

BOOL Increment(int idDlg,int idStaticChildDlg,MYCACHE_DLG *myValues,HWND hDlg)
{
    //For incrementing values of x,y,w,h on dialogBoxes
    if(!hDlg || !myValues) return FALSE;

    HWND hwndStatic;
    int iLastValue;
    TCHAR buf[MAXDLGCHAR];

    switch(idStaticChildDlg)
    {
    case ID_STATIC_X:
        hwndStatic=GetDlgItem(hDlg,ID_STATIC_X);
        if(!hwndStatic) return FALSE;
        iLastValue=myValues->iLastX;
        myValues->iLastX=min(MAXDIM_X,myValues->iLastX+1);
        if(iLastValue!=myValues->iLastX)
        {
            wsprintf(buf,"%d",myValues->iLastX);
            SetWindowText(hwndStatic,buf);
        }
        break;

    case ID_STATIC_Y:
        hwndStatic=GetDlgItem(hDlg,ID_STATIC_Y);
        if(!hwndStatic) return FALSE;
        iLastValue=myValues->iLastY;
        myValues->iLastY=min(MAXDIM_Y,myValues->iLastY+1);
        if(iLastValue!=myValues->iLastY)
        {
            wsprintf(buf,"%d",myValues->iLastY);
            SetWindowText(hwndStatic,buf);
        }
        break;

    case ID_STATIC_W:
        hwndStatic=GetDlgItem(hDlg,ID_STATIC_W);
        if(!hwndStatic) return FALSE;
        iLastValue=myValues->iLastW;
        myValues->iLastW=min(MAXDIM_W,myValues->iLastW+1);
        if(iLastValue!=myValues->iLastW)
        {
            wsprintf(buf,"%d",myValues->iLastW);
            SetWindowText(hwndStatic,buf);
        }
        break;

    case ID_STATIC_H:
        hwndStatic=GetDlgItem(hDlg,ID_STATIC_H);
        if(!hwndStatic) return FALSE;
        iLastValue=myValues->iLastH;
        myValues->iLastH=min(MAXDIM_H,myValues->iLastH+1);
        if(iLastValue!=myValues->iLastH)
        {
            wsprintf(buf,"%d",myValues->iLastH);
            SetWindowText(hwndStatic,buf);
        }
        break;
    }
    return TRUE;
}

BOOL MyApplyCache(int idDlg,MYCACHE_DLG *myValues,HWND hDlg)
{
    //Saves and applies mycache[idDlg] on hDlg

    if(!MySaveCache(idDlg,*myValues,hDlg))
    {
        MessageBox(hDlg,TEXT("Unable to save theses datas"),NULL,0);
        return FALSE;
    }

    //Load datas contained in mycache
    isFirstLoad[idDlg]=TRUE;
    if(!MyLoadCache(idDlg,myValues,hDlg))
    {
        MessageBox(hDlg,TEXT("Unable to Load last datas"),NULL,0);
        return FALSE;
    }

        return TRUE;
}

BOOL MyRefreshAspect(HWND hDlg,int idDlg)
{

    //Refresh aspect of object created in aspect's zone
    HWND hwndChildDlg,hwndChildChildDlg;
    TCHAR *szClassName;
    DWORD dwStyle=0;
    SCROLLINFO si;
    int posH,posV;

    //Delete last aspect of object
    //1 is a ID of this aspect
    DestroyWindow(GetDlgItem(GetDlgItem(hDlg,ID_ASPECT),1));

    switch(idDlg)
    {
        case DLG_BUTTON : szClassName=TEXT("button"); break;
        case DLG_EDITBOX : szClassName=TEXT("edit"); break;
        case DLG_LISTBOX : szClassName=TEXT("listbox"); break;
        case DLG_STATIC : szClassName=TEXT("static"); break;
        case DLG_COMBOBOX: szClassName=WC_COMBOBOX; break;
        case DLG_DIALOGBOX : szClassName=szClassDlgObj; break;
    }

    hwndChildDlg=GetDlgItem(hDlg,ID_ASPECT);
    if(!hwndChildDlg) return FALSE;

    si.cbSize=sizeof(SCROLLINFO);
    si.fMask=SIF_ALL;
    GetScrollInfo(hwndChildDlg,SB_HORZ,&si);
    posH=si.nPos;

    si.cbSize=sizeof(SCROLLINFO);
    si.fMask=SIF_ALL;
    GetScrollInfo(hwndChildDlg,SB_VERT,&si);
    posV=si.nPos;

    SendMessage(hwndChildDlg,WM_HSCROLL,
            MAKELONG(SB_TOP,mycache[idDlg].iLastX/cxChar),0);
        SendMessage(hwndChildDlg,WM_VSCROLL,
            MAKELONG(SB_TOP,mycache[idDlg].iLastY/cyChar),0);

    if(isParentWnd &&
        (mycache[idDlg].dwLastStylesChosen ||
          strstr(mycache[idDlg].szLastStylesChosen,
                  "WS_OVERLAPPED|")))
        dwStyle=WS_CHILD|WS_VISIBLE;

    int cxObj=mycache[idDlg].cxChar;
    int cyObj=mycache[idDlg].cyChar;
    if(idDlg==DLG_DIALOGBOX)
    {
        //for DLG_DIALOGBOX, cxChar and cyChar are for a system
        MyUpDateDimChar(hwndZC);
        cxObj=cxChar;
        cyObj=cyChar;

    }
    cxChar=LOWORD(GetDialogBaseUnits());
    cyChar=HIWORD(GetDialogBaseUnits());

    //recreate button with new aspect
    hwndChildChildDlg=CreateWindow(szClassName,
    mycache[idDlg].szTitle,
    mycache[idDlg].dwLastStylesChosen|dwStyle,
    mycache[idDlg].iLastX/cxChar,
    mycache[idDlg].iLastY/cyChar,
    mycache[idDlg].iLastW*cxObj+mycache[idDlg].cxRest,
    mycache[idDlg].iLastH*cyObj+mycache[idDlg].cyRest,
    hwndChildDlg,
    (HMENU)1,
    (HINSTANCE)GetWindowLongPtr(hwndChildDlg,GWLP_HINSTANCE),
    NULL
    );

    RedrawWindow(hwndChildChildDlg,NULL,NULL,RDW_ERASE|RDW_UPDATENOW);
    int idTarget;

    if(hwndChildChildDlg)
    {
        MySetFontParent(g_hwndParentObjToSet,&mycache[idDlg].font);
        PopFontSetFont(hwndChildChildDlg,&mycache[idDlg].font.hFont,
                               &mycache[idDlg].font.logfont);

        //Make object created always visible on aspect 's zone.
        SendMessage(hwndChildDlg,WM_HSCROLL,
            MAKELONG(SB_THUMBPOSITION,posH),0);
        SendMessage(hwndChildDlg,WM_VSCROLL,
            MAKELONG(SB_THUMBPOSITION,posV),0);

        //Activate apply or create 's button
        if(isTypeModDlg)
            idTarget=ID_APPLY;
        else
            idTarget=ID_CREATE;


        hwndChildDlg=GetDlgItem(hDlg,idTarget);
        if(!hwndChildDlg) return FALSE;
        EnableWindow(hwndChildDlg,TRUE);
    }
    else
    {
        //Disable ID_APPLY or ID_CREATE 's button
        if(isTypeModDlg)
            idTarget=ID_APPLY;
        else
            idTarget=ID_CREATE;

        hwndChildDlg=GetDlgItem(hDlg,idTarget);
        if(!hwndChildDlg) return FALSE;
        EnableWindow(hwndChildDlg,FALSE);
    }

    return TRUE;
}

BOOL MyInitCacheDlg(void)
{

    if(!MyFreeMenuTab(&mycache[DLG_DIALOGBOX].menu))
    {
        MyPerror(" aosioirwp");
        return FALSE;
    }
    memset(mycache,0,sizeof(MYCACHE_DLG)*NBOBJECT);
    if(!MyAllocMenuTab(&mycache[DLG_DIALOGBOX].menu))
    {
        MyPerror("ovjkoekre");
        return FALSE;
    }

    for(int i=0;i<NBOBJECT;i++)
    {
        isFirstTime[i]=TRUE;

    }
    return TRUE;
}
