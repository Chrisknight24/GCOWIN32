/*Nom de File:OpenmENUDialogBox.c
Role:File qui gere la CREATION DES BOITES de dialogues
utilisees dans la gestion des menus pour les obj de type DIALOG_BOX
,e File gere la creation des boites qui gere les taches situees
dans le groupBox Option de la fenetre de parametrage
BY CHIRSTIAN EDOUARDO LE 12/11/2024 a 12h53*/

#include <windows.h>
#include "resource.h"
# include "OpenDialogBox.h"

BOOL OpenMenuDialogBox(int idDlg,HWND hwndOwner)
{
    if(!hwndOwner) return FALSE;

    //Obtenir le iIndex et le nom du menu a modifier
    char szNameMenu[1024];
    int iIndexMenu;
    HWND hwndComboMod;
    HWND hDlgMenu=hwndOwner;

    hwndComboMod=GetDlgItem(hDlgMenu,IDC_MOD_MENU);
    if(!hwndComboMod)
    {
        MyPerror("sdgg546");
        exit(EXIT_FAILURE);
    }

    iIndexMenu=SendMessage(hwndComboMod,CB_GETCURSEL,0,0);
    if(iIndexMenu==CB_ERR ||iIndexMenu<0)
    {
        MyPerror("dskdskp");
        exit(EXIT_FAILURE);
    }

    GetWindowText(hwndComboMod,szNameMenu,1023);
    if(!(*szNameMenu))
    {
        MyPerror("kljsmvp");
        exit(EXIT_FAILURE);
    }

    switch(idDlg)
    {
    case IDR_MOD_ADD_ITEM:
        MyAddItemToMenu(iIndexMenu,hDlgMenu);
        break;
    case IDR_MOD_DEL_ITEM:
        MyDelItemToMenu(iIndexMenu,hDlgMenu);
        break;
    case IDR_MOD_MOD_ITEM:
        MyModifyItem(iIndexMenu,hDlgMenu);
        break;
    case IDR_MOD_MOD_MENU:
        MyModMenu(iIndexMenu,hDlgMenu);
        break;

    case IDR_MOD_DEL_MENU:
        {
            int iResult;
            char szRep[2048];
            wsprintf(szRep,"Do you want to delete menu :<%s>?\n",szNameMenu);
            iResult=MessageBox(hwndOwner,szRep,TEXT("QUESTION?"),
                               MB_TASKMODAL|MB_YESNO|MB_ICONQUESTION);
            if(iResult==IDYES)
            {
                fShouldInotify=FALSE;
                if(!MyDelMenu(menuTemp,iIndexMenu,menuTemp[iIndexMenu].idMenu,hDlgMenu))
                {
                    MyPerror("vanoojier");
                    exit(EXIT_FAILURE);
                }

                MessageBox(hDlgMenu,"Success!! Menu deleted!",TEXT("INFORMATION!!"),
                           MB_TASKMODAL|MB_ICONINFORMATION);

            }

        }
        break;

    }
    return TRUE;
}

