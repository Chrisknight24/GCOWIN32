/*Nom de File:MyManageCheckStyle.c
Role:File qui gere les choix de styles de User sur les differents
types d'objet
BY CHIRSTIAN EDOUARDO LE 20/09/2024 a 13h43*/

#include <windows.h>
#include <tchar.h>
#include "resource.h"
#include "OpenDialogBox.h"
#include "mystrtok.h"


BOOL MyManageCheckStyle(int idDlg, WPARAM wParam,LPARAM lParam)
{
    if(idDlg!=-1)
    {
        //cette fonction gere le clique sur les styles en mettant a jour laspect
        //Du coche, et du style, et l'affichage des styles actuellement choisi par User
        HWND hwndChild,hwnd;
        int idCheckButton;

        TCHAR szStyleGroup[MAXLENTGHT_OF_SZSTYLECHOSEN];
        TCHAR szStyleChild[MAXLENTGHT_OF_SZSTYLECHOSEN];
        char buf2[1024]="|",buf3[1024],buf[1024]="",*c;
        int i;

        hwndChild=(HWND)lParam;
        hwnd=GetParent(hwndChild);
        idCheckButton=LOWORD(wParam);
        strcpy(szStyleGroup,mycache[idDlg].szLastStylesChosen);
        strcpy(szStyleChild,mycache[idDlg].valueStyle[idCheckButton].szValuesStyles);
        //Lorsque un style est clique ou selectionne
        if(IsDlgButtonChecked(hwnd,idCheckButton))
        {

            //Si le bouton vient detre coché
            //faut l'ajouter
            mycache[idDlg].dwLastStylesChosen|=mycache[idDlg].valueStyle[idCheckButton].dwValuesStyles;
            if(szStylesChosen[0]=='\0')
            {
                //Si cest le tout premier style a ajouter
                GetWindowText(hwndChild,buf,1024);
                strcpy(szStylesChosen,buf);
                SetWindowText(GetDlgItem(GetParent(hwnd),
                ID_EDIT_STYLES_CHOOSEN),szStylesChosen);
            }
            else
            {
                i=strlen(szStylesChosen);
                szStylesChosen[i]='|';
                if(++i>MAXLENTGHT_OF_SZSTYLECHOSEN)
                {
                    MessageBox(hwnd,"MAX SPACE FOR SZSTYLECHOOSEN has been reached",NULL,0);
                    exit(EXIT_FAILURE);
                }
                szStylesChosen[i]='\0';
                GetWindowText(hwndChild,buf,1024);
                strcat(szStylesChosen,buf);
                SetWindowText(GetDlgItem(GetParent(hwnd),
                ID_EDIT_STYLES_CHOOSEN),szStylesChosen);
            }
            //ajouter le nom du style a la liste des styles choisis
            strcpy(mycache[idDlg].szLastStylesChosen,szStylesChosen);
            //Ajouter le | pour delimiter chaque nom de syles
            strcat(mycache[idDlg].szLastStylesChosen,"|");
        }
        else
        {
            //Si le bouton vient detre décoché, faut l'enlever
            GetWindowText(hwndChild,buf,1024);
            strcat(buf2,buf);
            strcpy(buf3,buf);
            strcat(buf3,"|");

            //Au finish buf2=|buf et buf3=buf|
            //1ere tentative avec | au debut
            BOOL fIsValid=FALSE;
            char *p=strstr(szStylesChosen,buf2);

            if(p && (*(p+strlen(buf2))=='|'  || *(p+strlen(buf2))=='\0' ))
                fIsValid=TRUE;
            //FIsvalid est utilise pour les styles de meme debut de nom
            //present tous deux dans la chaine de style:Exemple WS_OVERLAPPED|WS_VISIBLE|WS_OVERLAPPEDWINDOW
            c=strrep(szStylesChosen,buf2,"");
            if(!c || (c && fIsValid==FALSE))
            {
                //2eme tentative avec | a la fin
                c=strrep(szStylesChosen,buf3,"");
                if(!c)
                {
                    //3e tentative sans |
                    c=strrep(szStylesChosen,buf,"");
                    if(!c)
                    {
                        MessageBox(hwnd,"Error while deleting style",NULL,0);
                        exit(EXIT_FAILURE);
                    }
                }
            }
            //mettre a jour l'affochage du edit qui specifie la liste des styles choisis
            strcpy(szStylesChosen,c);
            SetWindowText(GetDlgItem(GetParent(hwnd),
            ID_EDIT_STYLES_CHOOSEN),szStylesChosen);
            //Sauvegarde la liste des styles choisis
            strcpy(mycache[idDlg].szLastStylesChosen,szStylesChosen);
            //Ajouter le | pour delimiter chaque nom de syles
            strcat(mycache[idDlg].szLastStylesChosen,"|");
            //Mettre a jour le nouveau dwStyle
            MyUpdateDwStyle(idDlg);

        }
    }
    else if(idDlg==-1)//for itemMenu (mf_rand)
    {
        if(!(g_mycacheItem.tabStyleItemMenu[0].dwValuesStyles))
        {
            MyPerror("m tiosiyp");
            exit(EXIT_FAILURE);
        }
      /*  if(!(g_mycacheItem.tabStyleItemMenu[0].dwValuesStyles))
        {
            for(int i=1;i<=tabStyleItemMenu[0].dwValuesStyles;i++)
            {
                g_mycacheItem.tabStyleItemMenu[i]=tabStyleItemMenu[i];
            }
        }
*/
        HWND hwndChild,hwnd;
        int idCheckButton;

        TCHAR szStyleGroup[MAXLENTGHT_OF_SZSTYLECHOSEN];
        TCHAR szStyleChild[MAXLENTGHT_OF_SZSTYLECHOSEN];
        char buf2[1024]="|",buf3[1024],buf[1024]="",*c;
        int i;

        hwndChild=(HWND)lParam;
        hwnd=GetParent(hwndChild);
        idCheckButton=LOWORD(wParam);
        strcpy(szStyleGroup,g_mycacheItem.szLastStylesChosen);
        strcpy(szStyleChild,g_mycacheItem.tabStyleItemMenu[idCheckButton].szValuesStyles);
        //Lorsque un style est clique ou selectionne
        if(IsDlgButtonChecked(hwnd,idCheckButton))
        {

            //Si le bouton vient detre coché
            //faut l'ajouter
            g_mycacheItem.uLastStylesChosen|=(UINT)(g_mycacheItem.tabStyleItemMenu[idCheckButton].dwValuesStyles);
            if(g_mycacheItem.szLastStylesChosen[0]=='\0')
            {
                //Si cest le tout premier style a ajouter
                GetWindowText(hwndChild,buf,1024);
                strcpy(g_mycacheItem.szLastStylesChosen,buf);
                SetWindowText(GetDlgItem(GetParent(hwnd),
                ID_EDIT_STYLES_CHOOSEN),g_mycacheItem.szLastStylesChosen);
            }
            else
            {
                i=strlen(g_mycacheItem.szLastStylesChosen);
                g_mycacheItem.szLastStylesChosen[i]='|';
                if(++i>MAXLENTGHT_OF_SZSTYLECHOSEN)
                {
                    MessageBox(hwnd,"MAX SPACE FOR SZSTYLECHOOSEN has been reached",NULL,0);
                    exit(EXIT_FAILURE);
                }
                g_mycacheItem.szLastStylesChosen[i]='\0';
                GetWindowText(hwndChild,buf,1024);
                strcat(g_mycacheItem.szLastStylesChosen,buf);
                SetWindowText(GetDlgItem(GetParent(hwnd),
                ID_EDIT_STYLES_CHOOSEN),g_mycacheItem.szLastStylesChosen);
            }
        }
        else
        {
            //Si le bouton vient detre décoché, faut l'enleve
            GetWindowText(hwndChild,buf,1024);
            strcat(buf2,buf);
            strcpy(buf3,buf);
            strcat(buf3,"|");

            //Au finish buf2=|buf et buf3=buf|
            //1ere tentative avec | au debut
            c=strrep(g_mycacheItem.szLastStylesChosen,buf2,"");
            if(!c)
            {
                //2eme tentative avec | a la fin
                c=strrep(g_mycacheItem.szLastStylesChosen,buf3,"");
                if(!c)
                {
                    //3e tentative sans |
                    c=strrep(g_mycacheItem.szLastStylesChosen,buf,"");
                    if(!c)
                    {
                        MessageBox(hwnd,"Error while deleting style",NULL,0);
                        exit(EXIT_FAILURE);
                    }
                }
            }
            //mettre a jour l'affochage du edit qui specifie la liste des styles choisis
            strcpy(g_mycacheItem.szLastStylesChosen,c);
            SetWindowText(GetDlgItem(GetParent(hwnd),
            ID_EDIT_STYLES_CHOOSEN),g_mycacheItem.szLastStylesChosen);
            //Mettre a jour le nouveau dwStyle
            MyUpdateDwStyle(-1);

        }
    }//fin if idDlg==-1
    return TRUE;
}

BOOL MyUpdateDwStyle(int idDlg)
{
    if(idDlg!=-1)
    {
        //cette fonction permet de reconstruire la valeur dw du mycache dwStyle
        //Cest une technique plus sure, on ne passe pas par &~ parceque tous
        //les styles ne sont pas forcement des multiples de 2
        PDWORD pdw;
        int i,nbStyle;
        PTCHAR pszStyleGroup,pszStyleChild;

        pszStyleGroup=mycache[idDlg].szLastStylesChosen;

        pdw=&mycache[idDlg].dwLastStylesChosen;
        *pdw=0;
        nbStyle=mycache[idDlg].valueStyle[0].dwValuesStyles;
        for(i=1;i<=nbStyle;i++)
        {
            pszStyleChild=mycache[idDlg].valueStyle[i].szValuesStyles;
            if(strstr(pszStyleGroup,pszStyleChild))
            {
                *pdw|=mycache[idDlg].valueStyle[i].dwValuesStyles;
            }

        }
    }
    else if(idDlg==-1)//for ItemMenu cache(MF_RAND)
    {

        PUINT pu;
        int i,nbStyle;
        PTCHAR pszStyleGroup,pszStyleChild;

        pszStyleGroup=g_mycacheItem.szLastStylesChosen;

        pu=&g_mycacheItem.uLastStylesChosen;
        *pu=0;
        nbStyle=g_mycacheItem.tabStyleItemMenu[0].dwValuesStyles;
        for(i=1;i<=nbStyle;i++)
        {
            pszStyleChild=g_mycacheItem.tabStyleItemMenu[i].szValuesStyles;
            if(strstr(pszStyleGroup,pszStyleChild))
            {
                *pu|=(UINT)(g_mycacheItem.tabStyleItemMenu[i].dwValuesStyles);
            }

        }
    }
    return TRUE;

}
