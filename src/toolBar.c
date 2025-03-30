/*nOM DU File: toolbar.c
Ce File se charge de gerer tout ce qui concerne le toolBar
de la premiere fenetre parent
BY CHRISTIAN EDOUARDO LE 20/12/2024  a 12h57
*/
#include <windows.h>
#include <commctrl.h>
#include "manageProjet.h"
#include "toolBar.h"

int g_iIndexRunTB;//Positon du bmpRun(triangle vert) dans l'image list
int g_iIndexStopTB;//

LRESULT RelabelButton(HWND hWndToolbar,PSTR pszNew,int idButton)
{//Change le nom d'un bouton de ToolBar existant

    TBBUTTONINFO tbInfo;

    tbInfo.cbSize = sizeof(TBBUTTONINFO);
    tbInfo.dwMask = TBIF_TEXT;
    tbInfo.pszText = pszNew;

    return SendMessage(hWndToolbar, TB_SETBUTTONINFO, (WPARAM)idButton,
    (LPARAM)&tbInfo);
}

// Fonction pour Create la barre d'outils
HWND CreateToolBar(HWND hwndParent)
 {
     HINSTANCE hinst=(HINSTANCE)GetWindowLongPtr(hwndParent,GWLP_HINSTANCE);

    // Create la barre d'outils
    HWND hwndToolbar = CreateWindowEx(0,
                TOOLBARCLASSNAME, NULL,
                WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT |
                 CCS_TOP,0, 0, 0, 0,hwndParent, NULL,hinst,
                  NULL);

    // Create une liste d'images pour les boutons de la barre d'outils
    HIMAGELIST hImageList = ImageList_Create(16, 16,
                            ILC_COLOR16|ILC_MASK, 1, 0);
    if (hImageList)
    {
        // Charger l'image bitmap à partir du File .bmp (ou de la ressource)

         // Associer la liste d'images à la barre d'outils
        SendMessage(hwndToolbar, TB_SETIMAGELIST, 0, (LPARAM)hImageList);

        HBITMAP hBmpStop = LoadBitmap(hinst,"IDB_STOP");
        HBITMAP hBmpRun = LoadBitmap(hinst,"IDB_RUN");

        if (hBmpStop && hBmpRun)
        {

            // Ajouter l'image à la liste d'images
            SendMessage(hwndToolbar,TB_LOADIMAGES,
                        (WPARAM)IDB_STD_SMALL_COLOR,
            (LPARAM)HINST_COMMCTRL);

            g_iIndexStopTB=ImageList_Add(hImageList, hBmpStop, NULL);
            g_iIndexRunTB=ImageList_Add(hImageList, hBmpRun, NULL);

        }
        else
        {
            MessageBox(NULL, "Error when loading images", "Error",
                        MB_OK | MB_ICONERROR|MB_TASKMODAL);
        }
    }

    // Ajouter un bouton avec l'image (croix rouge) à la barre d'outils
    TBBUTTON tbButton = {0};
    tbButton.iBitmap =g_iIndexRunTB ;  // Index de l'image
    tbButton.idCommand = ID_STOP_TOOL;  // Identifiant du bouton
    tbButton.fsState = TBSTATE_INDETERMINATE;
    tbButton.fsStyle = TBSTYLE_BUTTON;
    tbButton.iString=(INT_PTR)"Run";

    TBBUTTON tbButtons[3] =
    {
    { MAKELONG(STD_FILENEW, 0), IDM_NEW, TBSTATE_ENABLED,
        BTNS_AUTOSIZE, {0}, 0, (INT_PTR)"New" },
        { MAKELONG(STD_FILEOPEN, 0),IDM_OPEN, TBSTATE_ENABLED,
        BTNS_AUTOSIZE, {0}, 0, (INT_PTR)"Open" },
        { MAKELONG(STD_FILESAVE, 0), IDM_SAVE, 0,
        BTNS_AUTOSIZE, {0}, 0, (INT_PTR)"Save" },

 };
        // Add buttons.
        SendMessage(hwndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON),
                0);
        SendMessage(hwndToolbar, TB_ADDBUTTONS, (WPARAM)3,
        (LPARAM)&tbButtons);


        SendMessage(hwndToolbar, TB_ADDBUTTONS, 1, (LPARAM)&tbButton);

        // Ajuster la taille de la barre d'outils
        SendMessage(hwndToolbar, TB_AUTOSIZE, 0, 0);

    return hwndToolbar;
}


BOOL MyEnableButtonToolBar(HWND hwndToolBar,int idButton,BOOL fEnable,BOOL fShouldGray)
{
    //Cette fonction active ou desactive le bouton stop
    PSTR pszName;
    int posIindex;
    BYTE fState;

    TBBUTTONINFO tb;

    if(idButton==ID_STOP_TOOL)
    {
                if(fEnable)
        {
            //Si user souhaite activer
            posIindex=g_iIndexRunTB;
            fState=TBSTATE_ENABLED;
            pszName="Run";
        }
        else
        {
            posIindex=g_iIndexStopTB;
            fState=TBSTATE_ENABLED;
            pszName="Stop";
        }
        tb.cbSize=sizeof(TBBUTTONINFO);
        tb.fsState=(fShouldGray)?TBSTATE_INDETERMINATE:fState;
        tb.idCommand=idButton;
        tb.iImage=posIindex;
        tb.pszText=pszName;
        tb.dwMask=TBIF_STATE|TBIF_IMAGE|TBIF_TEXT;

        if(fShouldGray || !fEnable)
            g_fisTestRunAble=FALSE;
        else
            g_fisTestRunAble=TRUE;

        if(SendMessage(hwndToolBar,TB_SETBUTTONINFO,
                    (WPARAM) idButton,(LPARAM)&tb))
            return TRUE;

    }
    else
    {
        //pour les autres boutons
        tb.cbSize=sizeof(TBBUTTONINFO);
        tb.fsState=(fShouldGray)?TBSTATE_INDETERMINATE:TBSTATE_ENABLED;
        tb.idCommand=idButton;
        tb.dwMask=TBIF_STATE;

        if(SendMessage(hwndToolBar,TB_SETBUTTONINFO,
                (WPARAM) idButton,(LPARAM)&tb))
        return TRUE;

    }

    MyPerror("m okglkg");
    MyMsgError();
    return FALSE;
}
