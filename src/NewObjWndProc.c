/*Nom de File:NewObjWndProc.c
Role:procedure de fenetre sous classees des objects nouvellements
crees par User
BY CHIRSTIAN EDOUARDO LE 14/09/2024 a 9h33*/

#include <windows.h>
#include "resource.h"
#include "OpenDialogBox.h"
#include <tchar.h>
#include "manageFont.h"

RECT rcAlign;
BOOL isCapsOn=FALSE;
FRAMELINK g_link;
FRAMEUNLINK g_unlink;
COLORREF couleur;
HBRUSH hBrushGb;

LRESULT CALLBACK NewObjWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    int iIndex;

    //determine l'index dans le tableau uObjet correspondant a cet objet
    iIndex=MyGetIindexObjWithHwnd(hwnd);
    if(iIndex==-1)
    {
        MessageBox(NULL,"GetObjIndex Error",NULL,MB_ICONERROR);
        exit(EXIT_FAILURE);
    }

    if(g_link.isFrameLinkOn)
    {
        if(hwnd==g_hwndParentObj)
            CallWindowProc(uObjet[iIndex].oldObjWndProc,hwnd,msg,wParam,lParam);
       switch(msg)
       {
       case WM_LBUTTONDOWN:
            if(uObjet[iIndex].hwnd !=g_link.hwndObjToLink)
            {
                g_link.isFrameLinkOn=FALSE;
                MyApproveLink(g_link.hwndObjToLink,uObjet[iIndex].hwnd);
                MyShowAllObj();
            }
            return 0;

       case WM_RBUTTONUP:
            g_link.isFrameLinkOn=FALSE;
            InvalidateRect(GetParent(g_link.hwndObjToLink),NULL,TRUE);
            MyShowAllObj();
            return 0;

        case WM_KEYDOWN:
            if(wParam==VK_ESCAPE)
            {
                g_link.isFrameLinkOn=FALSE;
                InvalidateRect(GetParent(g_link.hwndObjToLink),NULL,TRUE);
                MyShowAllObj();
            }
            return 0;

       default:
            if( uObjet[iIndex].hwnd==g_link.hwndObjToLink)
                return 0;
            else
                return CallWindowProc(uObjet[iIndex].oldObjWndProc,hwnd,msg,wParam,lParam);
       }
    }
    else if(g_unlink.isMaskUnLinkOn)
    {
        if(hwnd==g_hwndParentObj)
            CallWindowProc(uObjet[iIndex].oldObjWndProc,hwnd,msg,wParam,lParam);

        switch(msg)
       {
       case WM_LBUTTONDOWN:
            if(uObjet[iIndex].hwnd !=g_unlink.hwndObjToUnLink)
            {
                g_unlink.isMaskUnLinkOn=FALSE;
                MyApproveUnlink(g_unlink.hwndObjToUnLink,uObjet[iIndex].hwnd);
                MyShowAllObj();
            }
            return 0;

       case WM_RBUTTONUP:
            g_unlink.isMaskUnLinkOn=FALSE;
            InvalidateRect(GetParent(g_unlink.hwndObjToUnLink),NULL,TRUE);
            DestroyWindow(hwndMask);
            MyShowAllObj();
            return 0;

       case WM_KEYDOWN:
            if(wParam==VK_ESCAPE)
            {
                g_unlink.isMaskUnLinkOn=FALSE;
                InvalidateRect(GetParent(g_unlink.hwndObjToUnLink),NULL,TRUE);
                DestroyWindow(hwndMask);
                MyShowAllObj();
            }
            return 0;

       default:
            if( uObjet[iIndex].hwnd==g_unlink.hwndObjToUnLink)
                return 0;
            else
                return CallWindowProc(uObjet[iIndex].oldObjWndProc,hwnd,msg,wParam,lParam);
       }
    }

    static BOOL fBlocking=FALSE, isInitDone=FALSE;
    static HWND hwndParent;
    static POINT posCursor,posBegParent,posCursorOrg;
    static RECT rcChild,rcParent;
    static int pas=1;
    static POINT ptChildBeg,ptChildEnd;
    static BOOL isFirstEnter=FALSE;
    static HMENU hmenu;
    static HINSTANCE hinst;
    static int nbOccWmTime=0;
    POSOBJ poTemp;
    BOOL hasPosChanged=FALSE;
    static BOOL shouldIGetKeyDir=TRUE;
    switch(msg)
    {

    case WM_TIMER:
        switch(wParam)
        {
        case ID_TIMER_CLIGNOTE:
            if(nbOccWmTime<4)
            {
                nbOccWmTime++;
                if(nbOccWmTime%2)
                    Myborder(hwnd);
                else
                    MyMaskBorder(TRUE);
            }
            else
            {
                nbOccWmTime=0;
                KillTimer(hwnd,wParam);
            }
            break;//fin case ID_TIMER_CLIGNOTE

        case ID_TIMER_ALIGNOBJ:

            InvalidateRect(GetParent(hwnd),&rcAlign,TRUE);
            shouldIGetKeyDir=TRUE;
            KillTimer(hwnd,wParam);

            break;//fin case ID_TIMER_ALIGNOBJ
        }
        return 0;

    case WM_SETFOCUS:
        if(isFirstEnter==FALSE)
        {
            hinst=(HINSTANCE)GetWindowLongPtr(hwnd,GWLP_HINSTANCE);
            hmenu=LoadMenu(hinst,TEXT("OBJ_OPT_GCO"));
            if(hmenu)
                hmenu=GetSubMenu(hmenu,0);
            else
            {
                MessageBox(hwnd,"Unable to load hmenu",NULL,MB_ICONERROR);
                isFirstEnter=FALSE;
                break;
            }
        }
        isFirstEnter=TRUE;

        //Obtenir la postion actuelle du childs

        GetWindowRect(hwnd,&rcChild);
        hwndParent=GetParent(hwnd);
        GetClientRect(hwndParent,&rcParent);
        posBegParent.x=0;
        posBegParent.y=0;
        ClientToScreen(hwndParent,&posBegParent);
        OffsetRect(&rcChild,-posBegParent.x,-posBegParent.y);
        ptChildBeg.x=rcChild.left;
        ptChildBeg.y=rcChild.top;
        ptChildEnd.x=rcChild.right;
        ptChildEnd.y=rcChild.bottom;
        if(uObjet[iIndex].iTypeObj==OBJ_EDITBOX ||
           uObjet[iIndex].iTypeObj==OBJ_COMBOBOX)
        {
            HideCaret(hwnd);
            return 0;
        }
        break;

    case WM_COMMAND:

        if(!lParam && hwnd!=g_hwndParentObj)//Its concerning menu items
        {
            switch(LOWORD(wParam))//nature of item selected
            {
            case IDM_DEL_OBJ:
                if(hwnd!=g_hwndParentObj)
                {//on ne peut supprimer le mainWindow
                    if(isCapsOn)
                    {
                        MyMaskBorder(TRUE);
                        isCapsOn=FALSE;
                    }
                    MyDeleteObj(GetDlgCtrlID(hwnd));
                }
                return 0;

            case IDM_DUP_OBJ:
                //Duppliquer cet objet
                if(isCapsOn)
                {
                    MyMaskBorder(TRUE);
                    isCapsOn=FALSE;
                }
                MyDuplicateObj(GetDlgCtrlID(hwnd));
                return 0;

            case IDM_LINK_OBJ:
                MyLinkObj(GetDlgCtrlID(hwnd));
                return 0;

            case IDM_UNLINK_OBJ:
                MyUnLinkObj(GetDlgCtrlID(hwnd));
                return 0;

            case IDM_MOD_OBJ:
                //Afficher la boite de dialogue correspondante pour modifier
                //cet Objet
                if(isCapsOn)
                {
                    MyMaskBorder(TRUE);
                    isCapsOn=FALSE;
                }
                MyModifyObj(GetDlgCtrlID(hwnd));
                return 0;

            case IDM_ADD_OBJ:
                if(!OpenDialogBox(MODAL,DLG_CHOOSETYPEOBJET,
                    hwnd,(HINSTANCE)GetWindowLongPtr(hwnd,GWLP_HINSTANCE),NULL,0))
                    {
                        MessageBox(hwnd,TEXT("Unable to open dialog\n wich has ID=DLG_CHOOSETYPEOBJECT"),
                                TEXT("ERROR"),MB_ICONERROR);
                    }
                    return 0;

            case IDM_AUTOSIZE_RIGHT:
            case IDM_AUTOSIZE_LEFT:
            case IDM_AUTOSIZE_BOTTOM:
            case IDM_AUTOSIZE_TOP:
                //MySetAutoSizeOpt(hmenu,iIndex,LOWORD(wParam));
                return 0;

            }
        }
        break;

    case WM_SIZE:
        //Obtenir la postion actuelle du childs
        GetWindowRect(hwnd,&rcChild);
        hwndParent=GetParent(hwnd);
        posBegParent.x=0;
        posBegParent.y=0;
        ClientToScreen(hwndParent,&posBegParent);
        OffsetRect(&rcChild,-posBegParent.x,-posBegParent.y);
        ptChildBeg.x=rcChild.left;
        ptChildBeg.y=rcChild.top;
        ptChildEnd.x=rcChild.right;
        ptChildEnd.y=rcChild.bottom;
        if(isCapsOn)
            Myborder(hwnd);

        //Ajuster la position du child
        if(isGroupBoxObj(iIndex))
        {
            HWND hwndChildGB=uObjet[iIndex].hwndChildGB;
            MyUpDateDimChar(uObjet[iIndex].hwnd);
            int y=cyChar;
            if(!hwndChildGB)
            {
                MyPerror("\n unable to get hwndChildGb in WM_SIZE of parent");
            }
            MoveWindow(hwndChildGB,1,y,LOWORD(lParam)-4,
                       max(y,HIWORD(lParam)-y-2),TRUE);
        }
        break;

    case WM_LBUTTONDOWN:
        SetFocus(hwnd);
        fBlocking=TRUE;//indique que le deplacement est possible
        //Stocke l'ecart qu'il devra toujours exister entre la position du curseur
        //du curseur et le coin superieur gauche du child
        posCursorOrg.x=GET_X_LPARAM(lParam);//Retrieve in client coordinates based un childs upper left corner's
        posCursorOrg.y=GET_Y_LPARAM(lParam);

        //Obtenir la postion actuelle du childs
        GetWindowRect(hwnd,&rcChild);
        hwndParent=GetParent(hwnd);
        posBegParent.x=0;
        posBegParent.y=0;
        ClientToScreen(hwndParent,&posBegParent);
        OffsetRect(&rcChild,-posBegParent.x,-posBegParent.y);
        ptChildBeg.x=rcChild.left;
        ptChildBeg.y=rcChild.top;
        ptChildEnd.x=rcChild.right;
        ptChildEnd.y=rcChild.bottom;
        SetCapture(hwnd);

        break;

    case WM_LBUTTONUP:
        fBlocking=FALSE;
        isInitDone=FALSE;
        if(isCapsOn)
            Myborder(hwnd);
        ReleaseCapture();
        break;

    case WM_RBUTTONUP:
        if(hwnd==g_hwndParentObj) break;
        /*//Gerer le cochage dans autosize
        if(!MyCheckAutosizeOpt(hmenu,iIndex))
        {
            MyPerror("gkdkglfkg;sd");
        }
        */
        posCursor.x=LOWORD(lParam);
        posCursor.y=HIWORD(lParam);
        ptWhereCreate=posCursor;
        ClientToScreen(hwnd,&posCursor);

        if(uObjet[iIndex].iTypeObj==OBJ_DIALOGBOX ||
           isGroupBoxObj(iIndex))
        {
             EnableMenuItem(hmenu, IDM_ADD_OBJ, MF_ENABLED);
        }
        else
        {
             EnableMenuItem(hmenu, IDM_ADD_OBJ, MF_GRAYED);
        }

        if(uObjet[iIndex].linkopt.iLastIndex)
        {
            EnableMenuItem(hmenu, IDM_UNLINK_OBJ, MF_ENABLED);
        }
        else
        {
            EnableMenuItem(hmenu, IDM_UNLINK_OBJ, MF_GRAYED);
        }
        EnableMenuItem(hmenu, IDM_LINK_OBJ, MF_ENABLED);


        if(!isCapsOn)
        {
            Myborder(hwnd);
        }
        TrackPopupMenu(hmenu,TPM_RIGHTBUTTON,
                       posCursor.x,posCursor.y,0,hwnd,NULL);

        if(!isCapsOn)
        {
            MyMaskBorder(TRUE);
        }
        return 0;

    case WM_MOUSEMOVE:
        {
            //Donne la capacite a chaque child de dectecter le WM_MOUSELEAVE
            TRACKMOUSEEVENT tme = { 0 };
            tme.cbSize = sizeof(tme);
            tme.dwFlags = TME_LEAVE|TME_HOVER;
            tme.hwndTrack = hwnd;
            tme.dwHoverTime = 1500; //
            TrackMouseEvent(&tme);
        }

        if(fBlocking==TRUE)
        {
            //Debut de quelques initialisations
            if(isInitDone==FALSE)
            {
                //pour ne pas recalculer a chaque MouseMove
                //Le calcul ne se fera desormais qu'au premier MouseMove Next le
                //LBUTTONDOWN
                isInitDone=TRUE;
                MyMaskBorder(TRUE);
                hwndParent=GetParent(hwnd);
                GetClientRect(hwndParent,&rcParent);
                posBegParent.x=rcParent.left;
                posBegParent.y=rcParent.top;
                ClientToScreen(hwndParent,&posBegParent);
                GetWindowRect(hwnd,&rcChild);//screen coordinates of window child
                BringWindowToTop(hwnd);

             }

            //Recupere les coordonnes du curseur de souris
            posCursor.x=GET_X_LPARAM(lParam);//Retrieve in client coordinates based un childs upper left corner's
            posCursor.y=GET_Y_LPARAM(lParam);

            //Obtiens les coordonnees du coins superieurs gauche
            //Grace la lecart connu depuis le LBUTTONDOWN
            posCursor.x-=posCursorOrg.x;
            posCursor.y-=posCursorOrg.y;

            //Transforme les coordonnees du coins superieur gauche en client
            ClientToScreen(hwnd,&posCursor);

            //Les coordonnes du coins du childs c actuelles sont relatives au
            //aux coordonnees screen du coins du parent
            //il faut effecteur une translation, pour qu'elles soient en fonction
            //du coins de lecran du pc, puisque cest ce que MoveWindows realise
            ptChildBeg.x=posCursor.x-posBegParent.x;
            ptChildBeg.y=posCursor.y-posBegParent.y;
            ptChildEnd.x=ptChildBeg.x+(rcChild.right-rcChild.left);
            ptChildEnd.y=ptChildBeg.y+(rcChild.bottom-rcChild.top);

            ptChildBeg.x=max(0,ptChildBeg.x);
            ptChildBeg.x=min(rcParent.right-(rcChild.right-rcChild.left),ptChildBeg.x);
            ptChildBeg.y=max(0,ptChildBeg.y);
            ptChildBeg.y=min(rcParent.bottom-(rcChild.bottom-rcChild.top),ptChildBeg.y);

            MyMoveWindow(iIndex,ptChildBeg.x,ptChildBeg.y,rcChild.right-rcChild.left,rcChild.bottom-rcChild.top,TRUE);

        }
        break;

    case WM_MOUSEHOVER:
        MyDisPlayInfoObj(GetDlgCtrlID(hwnd));
        return 0;

    /*case WM_MOUSELEAVE:
        fBlocking=FALSE;
        isInitDone=FALSE;

        return 0;
*/
    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
        GetWindowRect(hwnd,&rcChild);
        hwndParent=GetParent(hwnd);
        GetClientRect(hwndParent,&rcParent);
        posBegParent.x=0;
        posBegParent.y=0;
        ClientToScreen(hwndParent,&posBegParent);
        OffsetRect(&rcChild,-posBegParent.x,-posBegParent.y);
        ptChildBeg.x=rcChild.left;
        ptChildBeg.y=rcChild.top;
        ptChildEnd.x=rcChild.right;
        ptChildEnd.y=rcChild.bottom;

        switch(wParam)
        {
        case VK_F9:
            MyTestProjet();
            return 0;

        case VK_TAB:
            if(isCapsOn)
            {//Change attribue le border au prochain obj
                HWND hwndNext;
                int nbObj=uObjet[0].nbObjCreated;
                int iIndexObjActu=MyGetIindexObjWithHwnd(hwnd);
                int iIndexSuiv=(iIndexObjActu+1)%nbObj;

                hwndNext=uObjet[iIndexSuiv].hwnd;
                //Masque l'objet actuel
                MyMaskBorder(TRUE);
                //Bodre hwndNext
                Myborder(hwndNext);
                SetFocus(hwndNext);
                return 0;
            }
            break;

        case VK_BACK://Supprimer l'objet
        {
            int idObj=GetDlgCtrlID(hwnd);
            int nbObj=uObjet[0].nbObjCreated;
            if(hwnd!=g_hwndParentObj)
            {

                if(isCapsOn)
                {
                    MyMaskBorder(TRUE);
                    isCapsOn=FALSE;
                }

                MyDeleteObj(idObj);

                nbObj=uObjet[0].nbObjCreated;

                if(nbObj>0)
                {
                    int iIndex=GetObjIndex(idObj);

                    iIndex=(iIndex+1)%nbObj;
                    if(uObjet[iIndex].hwnd==g_hwndParentObj)
                        iIndex=(iIndex+1)%nbObj;
                    SetFocus(uObjet[iIndex].hwnd);
                    //Clignotage qui indique le nouvel obj qui a le focus
                    SetTimer(uObjet[iIndex].hwnd,1,100,NULL);
                }
            }
            else
            {
                if(uObjet[0].nbObjCreated>0)
                {
                    int iIndex=GetObjIndex(idObj);

                    iIndex=(iIndex+1)%nbObj;
                    SetFocus(uObjet[iIndex].hwnd);
                    //Clignotage qui indique le nouvel obj qui a le focus
                    SetTimer(uObjet[iIndex].hwnd,1,100,NULL);
                }
            }

        }
            return 0;

        case VK_SUBTRACT://pour retrancher
            pas=(abs(pas)*-1);
            return 0;

        case VK_ADD://pour ajouter
            pas=(abs(pas));
            return 0;

        case VK_UP:
            if(!shouldIGetKeyDir)
                return 0;
            g_fShouldIAskSave=TRUE;
            if(isCapsOn)
            {
                //Pas de deplacement juste aggrandiseement ou diminution
                if(ptChildBeg.y>=BOR_H)
                    ptChildBeg.y-=pas;
                else
                    ptChildBeg.y=BOR_H;

                if(ptChildEnd.y-ptChildBeg.y<BOR_H)
                {
                    ptChildBeg.y=ptChildEnd.y-BOR_H;
                }
            }
            else
            {
                //deplacement
                if(ptChildBeg.y>0)
                {
                    ptChildBeg.y-=abs(pas);
                    ptChildEnd.y-=abs(pas);
                }
                else
                {
                    ptChildEnd.y=ptChildEnd.y-ptChildBeg.y;
                    ptChildBeg.y=0;
                }
            }

            //Verifier l'alignement
            //Si l'obj est aligne alors une ligne rouge sera dessinee
            //Puis un retardement sera effectue
            poTemp._beg=ptChildBeg;
            poTemp._end=ptChildEnd;
            MyCheckAlignObj(UP,hwnd,&poTemp,-abs(pas),&hasPosChanged);
            ptChildBeg=poTemp._beg;
            ptChildEnd=poTemp._end;
            MyMoveWindow(iIndex,ptChildBeg.x,ptChildBeg.y,ptChildEnd.x-ptChildBeg.x,
                         ptChildEnd.y-ptChildBeg.y,TRUE);

            if(isCapsOn)
                Myborder(hwnd);

            pas+=(pas>=0)?1:-1;
            if(hasPosChanged==TRUE)
            {//Si un alignement s'est realiser, faut patienter
                shouldIGetKeyDir=FALSE;
                SetTimer(hwnd,ID_TIMER_ALIGNOBJ,500,NULL);
                pas=(pas>=0)?1:-1;

            }
            return 0;


        case VK_RIGHT:
            if(!shouldIGetKeyDir)
                return 0;
            g_fShouldIAskSave=TRUE;
            if(isCapsOn)
            {
                //Pas de deplacement juste aggrandiseement ou diminution
                if(ptChildEnd.x<=rcParent.right-BOR_H)
                    ptChildEnd.x+=pas;
                else
                    ptChildEnd.x=rcParent.right-BOR_H;

                if(ptChildEnd.x-ptChildBeg.x<BOR_H)
                {
                    ptChildEnd.x=ptChildBeg.x+BOR_H;
                }
            }
            else
            {
                //deplacement
                if(ptChildEnd.x<rcParent.right)
                {
                    ptChildBeg.x+=abs(pas);
                    ptChildEnd.x+=abs(pas);
                }
                else
                {
                    ptChildBeg.x=rcParent.right-(ptChildEnd.x-ptChildBeg.x);
                    ptChildEnd.x=rcParent.right;
                }
            }
            //Verifier l'alignement
            //Si l'obj est aligne alors une ligne rouge sera dessinee
            //Puis un retardement sera effectue
            poTemp._beg=ptChildBeg;
            poTemp._end=ptChildEnd;
            MyCheckAlignObj(RIGHT,hwnd,&poTemp,abs(pas),&hasPosChanged);
            ptChildBeg=poTemp._beg;
            ptChildEnd=poTemp._end;

            MyMoveWindow(iIndex,ptChildBeg.x,ptChildBeg.y,ptChildEnd.x-ptChildBeg.x,
                         ptChildEnd.y-ptChildBeg.y,TRUE);

            if(isCapsOn)
                Myborder(hwnd);

            pas+=(pas>=0)?1:-1;
            if(hasPosChanged==TRUE)
            {//Si un alignement s'est realiser, faut patienter
                shouldIGetKeyDir=FALSE;
                SetTimer(hwnd,ID_TIMER_ALIGNOBJ,500,NULL);
                pas=(pas>=0)?1:-1;
            }
            return 0;

        case VK_DOWN:
            if(!shouldIGetKeyDir)
                return 0;

            g_fShouldIAskSave=TRUE;
            if(isCapsOn)
            {
                //Pas de deplacement juste aggrandiseement ou diminution
                if(ptChildEnd.y<=rcParent.bottom-BOR_H)
                    ptChildEnd.y+=pas;
                else
                    ptChildEnd.y=rcParent.bottom-BOR_H;

                if(ptChildEnd.y-ptChildBeg.y<BOR_H)
                {
                    ptChildEnd.y=ptChildBeg.y+BOR_H;
                }
            }
            else
            {
                //deplacement
                if(ptChildEnd.y<rcParent.bottom)
                {
                    ptChildBeg.y+=abs(pas);
                    ptChildEnd.y+=abs(pas);
                }
                else
                {
                    ptChildBeg.y=rcParent.bottom-(ptChildEnd.y-ptChildBeg.y);
                    ptChildEnd.y=rcParent.bottom;
                }
            }
            //Verifier l'alignement
            //Si l'obj est aligne alors une ligne rouge sera dessinee
            //Puis un retardement sera effectue
            poTemp._beg=ptChildBeg;
            poTemp._end=ptChildEnd;
            MyCheckAlignObj(DOWN,hwnd,&poTemp,pas,&hasPosChanged);
            ptChildBeg=poTemp._beg;
            ptChildEnd=poTemp._end;

            MyMoveWindow(iIndex,ptChildBeg.x,ptChildBeg.y,ptChildEnd.x-ptChildBeg.x,
                ptChildEnd.y-ptChildBeg.y,TRUE);

            if(isCapsOn)
                Myborder(hwnd);

            pas+=(pas>=0)?1:-1;
            if(hasPosChanged==TRUE)
            {//Si un alignement s'est realiser, faut patienter
                shouldIGetKeyDir=FALSE;
                SetTimer(hwnd,ID_TIMER_ALIGNOBJ,500,NULL);
                pas=(pas>=0)?1:-1;

            }
            return 0;


        case VK_LEFT:
            if(!shouldIGetKeyDir)
                return 0;

            g_fShouldIAskSave=TRUE;
            if(isCapsOn)
            {
                //Pas de deplacement juste aggrandiseement ou diminution
                if(ptChildBeg.x>=BOR_H)
                    ptChildBeg.x-=pas;
                else ptChildBeg.x=BOR_H;

                if(ptChildEnd.x-ptChildBeg.x<BOR_H)
                {
                    ptChildBeg.x=ptChildEnd.x-BOR_H;
                }
            }
            else
            {
                //deplacement
                if(ptChildBeg.x>0)
                {
                    ptChildBeg.x-=abs(pas);
                    ptChildEnd.x-=abs(pas);
                }
                else
                {
                    ptChildEnd.x=ptChildEnd.x-ptChildBeg.x;
                    ptChildBeg.x=0;
                }
            }
            //Verifier l'alignement
            //Si l'obj est aligne alors une ligne rouge sera dessinee
            //Puis un retardement sera effectue
            poTemp._beg=ptChildBeg;
            poTemp._end=ptChildEnd;
            MyCheckAlignObj(LEFT,hwnd,&poTemp,-abs(pas),&hasPosChanged);
            ptChildBeg=poTemp._beg;
            ptChildEnd=poTemp._end;


            MyMoveWindow(iIndex,ptChildBeg.x,ptChildBeg.y,ptChildEnd.x-ptChildBeg.x,
            ptChildEnd.y-ptChildBeg.y,TRUE);

            if(isCapsOn)
                Myborder(hwnd);
            pas+=(pas>=0)?1:-1;
            if(hasPosChanged==TRUE)
            {//Si un alignement s'est realiser, faut patienter
                shouldIGetKeyDir=FALSE;
                SetTimer(hwnd,ID_TIMER_ALIGNOBJ,500,NULL);
                pas=(pas>=0)?1:-1;
            }
            return 0;

        case VK_CAPITAL:
            isCapsOn=(isCapsOn+1)%2;

            if(isCapsOn)
            {
                //Si capsLock est allume, alors on active le Border au child
                //au child
                Myborder(hwnd);
            }
            else
            {
                //Si capsLock est allume, alors on
                //masque les bordures
                MyMaskBorder(TRUE);
            }
            return 0;
        }
        return 0;

    case WM_SYSCHAR:
    case WM_CHAR:

        return 0;

    case WM_SYSKEYUP:
    case WM_KEYUP:
        switch(wParam)
        {
        case VK_UP:
        case VK_DOWN:
        case VK_LEFT:
        case VK_RIGHT:
            pas=(pas>=0)?1:-1;
            return 0;
        }
        return 0;

    case WM_CLOSE:
        if(hwnd==g_hwndParentObj)
            break;
        SendMessage(hwnd,WM_COMMAND,(WPARAM)IDM_DEL_OBJ,0);
        return 0;
    }

    return CallWindowProc(uObjet[iIndex].oldObjWndProc,hwnd,msg,wParam,lParam);
}

int GetObjIndex(int idObj)
{
    //cette fonction retourne l'index du tableau dont l'objet a l'id ==idObj

    int i,nbObj;
    nbObj=uObjet[0].nbObjCreated;
    for(i=0;i<nbObj;i++)
    {
        if(idObj==uObjet[i].id)
        {
            return i;
        }
    }
    return -1;
}

BOOL MyDeleteObj(int idObj)
{

    if(!uObjet)
        return TRUE;
    //cette fonction permet de supprimer un objet de l'univers connaissant
    //Son id

    if(idObj!=-1)
    {
        int iIndex,i,nbObjet;

        iIndex=GetObjIndex(idObj);
        if(iIndex==-1)
        {
            MessageBox(NULL,TEXT("Unable to Delete this Objet"),
                   NULL,MB_ICONERROR);
            return FALSE;
        }
        if(!isTypeModDlg && uObjet[iIndex].iTypeObj==OBJ_DIALOGBOX)
        {
            //sI DLG A des child, supprimer dabord ces childs
            for(i=0;i<uObjet[0].nbObjCreated;i++)
            {
                if(uObjet[i].hwndParent==uObjet[iIndex].hwnd)
                {
                    MyDeleteObj(uObjet[i].id);
                    i=-1;
                    iIndex=GetObjIndex(idObj);
                    if(iIndex==-1)
                    {
                        MessageBox(NULL,TEXT("Unable to Delete this Objet2"),
                         NULL,MB_ICONERROR);
                        return FALSE;
                    }
                }
            }

            iIndex=GetObjIndex(idObj);
            if(iIndex==-1)
            {
                MessageBox(NULL,TEXT("Unable to Delete this Objet2"),
                NULL,MB_ICONERROR);
                return FALSE;
            }
        }

        if((!isTypeModDlg && isGroupBoxObj(iIndex)) ||
            (isTypeModDlg && isGroupBoxObj(iIndex)&&
             !strstr(mycache[DLG_BUTTON].szLastStylesChosen,
                     "BS_GROUPBOX|")
             )
          )
        {

            //sI GB A des child, supprimer dabord ces childs
            HWND hwndParentObj=uObjet[iIndex].hwndChildGB;
            if(!hwndParentObj)
            {
                MyPerror("\n error unknown in ModifyObj:!hwndParentObj");
                exit(EXIT_FAILURE);
            }

            for(i=0;i<uObjet[0].nbObjCreated;i++)
            {
                if(uObjet[i].hwndParent==hwndParentObj)
                {
                    MyDeleteObj(uObjet[i].id);
                    i=-1;
                    iIndex=GetObjIndex(idObj);// mise a jour due au decalage
                    if(iIndex==-1)
                    {
                        MessageBox(NULL,TEXT("Unable to Delete this Objet2"),
                         NULL,MB_ICONERROR);
                        return FALSE;
                    }
                }
            }

            iIndex=GetObjIndex(idObj);
            if(iIndex==-1)
            {
                MessageBox(NULL,TEXT("Unable to Delete this Objet2"),
                NULL,MB_ICONERROR);
                return FALSE;
            }
        }


        iIndex=GetObjIndex(idObj);
        if(iIndex==-1)
        {
            MessageBox(NULL,TEXT("Unable to Delete this Objet"),
                   NULL,MB_ICONERROR);
            return FALSE;
        }
       if(uObjet[iIndex].iTypeObj==OBJ_DIALOGBOX &&
           uObjet[iIndex].hwndParent==hwndZC)
        {
            if(!MyFreeMenuTab(&uObjet[iIndex].menu))
            {
                MyPerror("m aaod");
                return FALSE;
            }

        }
        DeleteObject(uObjet[iIndex].font.hFont);
        /*if(!MyFreeLinkOpt(&uObjet[iIndex].linkopt))
        {
            MyPerror("mpdofiof");
        }
        */
        DestroyWindow(uObjet[iIndex].hwnd);
        //Enlever cet element du tableau uObjet
        nbObjet=uObjet[0].nbObjCreated;

        //Decalage
        for(i=iIndex;i<nbObjet-1;i++)
        {
            uObjet[i]=uObjet[i+1];
            po[i]=po[i+1];//tableau des positions dynamiques de chaque pbj
        }
        uObjet[0].nbObjCreated=nbObjet-1;

        if(!isTypeModDlg)
            MyUpdateLinksUniv();
    }
    else if(idObj==-1)
    {
        int i;

        for(i=0;i<uObjet[0].nbObjCreated;i++)
        {
            MyDeleteObj(uObjet[i].id);
            i=-1;
        }
        iActualIdForObjet=100;


    }
    return TRUE;
}

BOOL MyDuplicateObj(int idObj)
{
    PSTR pszClassName=NULL;
    int iIndex;
    int idDlg=0;

    HWND hwnd;
    iIndex=GetObjIndex(idObj);
    if(iIndex==-1) return FALSE;

    switch(uObjet[iIndex].iTypeObj)
    {
        case OBJ_BUTTON:
            pszClassName=TEXT("button");
            idDlg=DLG_BUTTON;
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
              break;

        case OBJ_IMGSTATIC:
             pszClassName=NULL;
             idDlg=DLG_IMGSTATIC;
              break;
    }
    MyUpdateDimObjUniv();
    //Rempli mycache avec les styles de l'objet a dupliquer
    //Car ce cache sera utiliser dans createObjet
    strcpy(mycache[idDlg].szLastStylesChosen,uObjet[iIndex].szLastStylesChosen);
    mycache[idDlg].dwLastStylesChosen=uObjet[iIndex].dwStyle;
    mycache[idDlg].iLastX=uObjet[iIndex].x+5;//Leger decalage par rapport a loriginal
    mycache[idDlg].iLastY=uObjet[iIndex].y+5;//leger dealage par rapport a l'original
    mycache[idDlg].iLastW=uObjet[iIndex].w;
    mycache[idDlg].iLastH=uObjet[iIndex].h;
    mycache[idDlg].cxChar=uObjet[iIndex].cxChar;
    mycache[idDlg].cyChar=uObjet[iIndex].cyChar;
    mycache[idDlg].cxRest=uObjet[iIndex].cxRest;
    mycache[idDlg].cyRest=uObjet[iIndex].cyRest;

    mycache[idDlg].mycolorobj[BK]=uObjet[iIndex].mycolorobj[BK];
    mycache[idDlg].mycolorobj[TXT]=uObjet[iIndex].mycolorobj[TXT];
    mycache[idDlg].font=uObjet[iIndex].font;//Temporaire pb, use popFontInit pour creer un new hFont
    _tcscpy(mycache[idDlg].szTitle,uObjet[iIndex].szTitle);
    mycache[idDlg].fShouldAutosize=uObjet[iIndex].fShouldAutosize;
    MySetNameId(mycache[idDlg].szNameId,idDlg);
    if(!(hwnd=CreateObjet(pszClassName,idDlg,uObjet[iIndex].hwndParent)))
    {
        MessageBox(uObjet[iIndex].hwndParent,"Unable to Duplicate this Object",
                   NULL,MB_ICONERROR);
        return FALSE;
    }

    if(uObjet[iIndex].iTypeObj==OBJ_DIALOGBOX)
    {
        //Si uObjet[iIndex] a des childs, il faut aussi le dupliquer
        int nbObj=uObjet[0].nbObjCreated;
        int i;
        HWND hwndCopie;

        for(i=0;i<nbObj;i++)
        {
            if(uObjet[i].hwndParent==uObjet[iIndex].hwnd)
            {
                hwndCopie=uObjet[i].hwndParent;
                uObjet[i].hwndParent=hwnd;
                MyDuplicateObj(uObjet[i].id);
                uObjet[i].hwndParent=hwndCopie;
            }
        }

    }
    else if(isGroupBoxObj(iIndex))
    {
        //Si uObjet[iIndex] a des childs, il faut aussi le dupliquer
        int nbObj=uObjet[0].nbObjCreated;
        int i;
        HWND hwndCopie;
        HWND hwndParentChild=uObjet[iIndex].hwndChildGB;
        if(!hwndParentChild)
        {
            MyPerror("\n errror unknown in DuplicateObj :!hwndParentChild");
            hwndParentChild=uObjet[iIndex].hwnd;
        }
        for(i=0;i<nbObj;i++)
        {
            if(uObjet[i].hwndParent==hwndParentChild)
            {
                hwndCopie=uObjet[i].hwndParent;//pour dupliquer avec les meme proprietes
                uObjet[i].hwndParent=hwnd;
                uObjet[i].hwndParent=GetDlgItem(hwnd,ID_DLG_GB);
                if(!uObjet[i].hwndParent)
                {
                    MyPerror("\n another unKnowError at Duplicate func:!uObjet[i].hwndParent");
                    MyMsgError();
                    uObjet[i].hwndParent=hwnd;
                    exit(EXIT_FAILURE);
                }
                MyDuplicateObj(uObjet[i].id);
                uObjet[i].hwndParent=hwndCopie;//restoration
            }
        }

    }
    BringWindowToTop(hwnd);
    return TRUE;
}

BOOL MyModifyObj(int idObj)
{
    //Cette fonction reaffiche la boite de dialogue ou obj
    //a ete parametrer, pour effectuer certaines modifications
    // si necessaires

    int iIndex;
    int idDlg=0;


    iIndex=GetObjIndex(idObj);
    if(iIndex==-1) return FALSE;

    switch(uObjet[iIndex].iTypeObj)
    {
        case OBJ_BUTTON:
            idDlg=DLG_BUTTON;
            g_hwndLastChildGB=NULL;
            break;
        case OBJ_EDITBOX:
            idDlg=DLG_EDITBOX;
            break;
        case OBJ_LISTBOX:
            idDlg=DLG_LISTBOX;
             break;
        case OBJ_COMBOBOX:
             idDlg=DLG_COMBOBOX;
              break;
        case OBJ_STATIC:
             idDlg=DLG_STATIC;
              break;

        case OBJ_DIALOGBOX:
             idDlg=DLG_DIALOGBOX;
              break;

        case OBJ_IMGSTATIC:
            idDlg=DLG_IMGSTATIC;
            break;
    }
    MyUpdateDimObjUniv();
    //Rempli mycache avec les styles de l'objet a dupliquer
    //Car ce cache sera utiliser dans createObjet
    strcpy(mycache[idDlg].szLastStylesChosen,uObjet[iIndex].szLastStylesChosen);
    mycache[idDlg].dwLastStylesChosen=uObjet[iIndex].dwStyle;
    mycache[idDlg].iLastX=uObjet[iIndex].x;
    mycache[idDlg].iLastY=uObjet[iIndex].y;
    mycache[idDlg].iLastW=uObjet[iIndex].w;
    mycache[idDlg].iLastH=uObjet[iIndex].h;
    mycache[idDlg].cxChar=uObjet[iIndex].cxChar;
    mycache[idDlg].cyChar=uObjet[iIndex].cyChar;
    mycache[idDlg].cxRest=uObjet[iIndex].cxRest;
    mycache[idDlg].cyRest=uObjet[iIndex].cyRest;

    mycache[idDlg].mycolorobj[BK]=uObjet[iIndex].mycolorobj[BK];
    mycache[idDlg].mycolorobj[TXT]=uObjet[iIndex].mycolorobj[TXT];
    mycache[idDlg].font=uObjet[iIndex].font;
    _tcscpy(mycache[idDlg].szTitle,uObjet[iIndex].szTitle);
    _tcscpy(mycache[idDlg].szNameId,uObjet[iIndex].szNameId);
    mycache[idDlg].linkopt=uObjet[iIndex].linkopt;

    if(idDlg==DLG_DIALOGBOX && isParentWnd)
    {
        if(!MyCopieMenuTab(uObjet[iIndex].menu,&mycache[idDlg].menu))
        {
            MyPerror("mvfsdfdd");
            exit(EXIT_FAILURE);
        }

    }

    isTypeModDlg=TRUE;
    idObjToDelete=idObj;
    g_fShouldApplyFontOnChilds=FALSE;//important
    OpenDialogBox(MODAL,idDlg,uObjet[iIndex].hwndParent,
        uObjet[iIndex].hinst,NULL,0);
    isTypeModDlg=FALSE;
    idObjToDelete=0;
    MyUpdateLinksUniv();
    return TRUE;
}

BOOL MyDisPlayInfoObj(int idObj)
{
    return TRUE;
}
BOOL MyCheckAlignObj(int dir,HWND hwndObj,POSOBJ *poTemp,int pas,BOOL *hasPosChanged)
{
    //Cette fonctionne verfiie si hwndObj est aligne dans n'importe quelle
    //direction avec n'importe quelle objet dont po a la positon
    //ce n'importe quel obj doit avoir le meme parent que hwndObj
    if(!po || !hwndObj || !uObjet)
        return FALSE;

    if(hwndObj==g_hwndParentObj)
        return TRUE;

    int nbObj=uObjet[0].nbObjCreated;
    int i,iIndex=MyGetIindexObjWithHwnd(hwndObj);
    int  x1,y1,x2,y2,a1,b1,a2,b2;
    int lastX1,lastY1,lastX2,lastY2;
    HWND hwndParent=GetParent(hwndObj);
    static int sA1=-1,sB1=-1,sA2=-1,sB2=-1,lastPas=-12345;
    if(iIndex==-1)
        return FALSE;

    if(lastPas==-12345) lastPas=pas;
    *hasPosChanged=FALSE;
    x1=poTemp->_beg.x;
    y1=poTemp->_beg.y;
    x2=poTemp->_end.x;
    y2=poTemp->_end.y;

    //pas=(pas>=0?pas-1:pas+1);
    lastX1=x1+pas;
    lastY1=y1+pas;
    lastX2=x2+pas;
    lastY2=y2+pas;

    for(i=0;i<nbObj;i++)
    {
        if(i==iIndex || isLinkedWith(i,iIndex)) continue;
        if(uObjet[i].hwndParent!=hwndParent) continue;
        a1=po[i]._beg.x;
        b1=po[i]._beg.y;
        a2=po[i]._end.x;
        b2=po[i]._end.y;

        if(dir==LEFT)
        {
            //Test avec a1=beg.x
            if(isPassedTrough(a1,lastX1,x1) )
            {
                if(sA1==a1)
                {
                    sA1=-1;
                    break;
                }
                sA1=a1;

                poTemp->_beg.x=a1;
                poTemp->_end.x=a1+x2-x1;
                *hasPosChanged=TRUE;
                DrawLine(hwndParent,po[i]._beg,poTemp->_beg);
                break;
            }

            if(!isCapsOn && isPassedTrough(a1,lastX2,x2) )
            {
                if(sA1==a1)
                {
                    sA1=-1;
                    break;
                }
                sA1=a1;

                poTemp->_end.x=a1;
                poTemp->_beg.x=a1-(x2-x1);
                *hasPosChanged=TRUE;
                DrawLine(hwndParent,po[i]._beg,poTemp->_end);
                break;
            }

            //Test avec a2=End.x
            if(isPassedTrough(a2,lastX1,x1) )
            {
                if(sA2==a2)
                {
                    sA2=-1;
                    break;
                }
                sA2=a2;

                poTemp->_beg.x=a2;
                poTemp->_end.x=a2+x2-x1;
                *hasPosChanged=TRUE;
                DrawLine(hwndParent,po[i]._end,poTemp->_beg);
                break;
            }

            if( !isCapsOn && isPassedTrough(a2,lastX2,x2) )
            {
                if(sA2==a2)
                {
                    sA2=-1;
                    break;
                }
                sA2=a2;

                poTemp->_end.x=a2;
                poTemp->_beg.x=a2-(x2-x1);
                *hasPosChanged=TRUE;
                DrawLine(hwndParent,po[i]._end,poTemp->_end);
                break;
            }


        }
        else if(dir==RIGHT)
        {
            //Test avec a1=beg.x
            if( !isCapsOn && isPassedTrough(a1,lastX1,x1) )
            {
                if(sA1==a1)
                {
                    sA1=-1;
                    break;
                }
                sA1=a1;

                poTemp->_beg.x=a1;
                poTemp->_end.x=a1+x2-x1;
                *hasPosChanged=TRUE;
                DrawLine(hwndParent,po[i]._beg,poTemp->_beg);
                break;
            }

            if( isPassedTrough(a1,lastX2,x2) )
            {
                if(sA1==a1)
                {
                    sA1=-1;
                    break;
                }
                sA1=a1;

                poTemp->_end.x=a1;
                poTemp->_beg.x=a1-(x2-x1);
                *hasPosChanged=TRUE;
                DrawLine(hwndParent,po[i]._beg,poTemp->_end);
                break;
            }


            //Test avec a2=End.x
            if(!isCapsOn && isPassedTrough(a2,lastX1,x1) )
            {
                if(sA2==a2)
                {
                    sA2=-1;
                    break;
                }
                sA2=a2;

                poTemp->_beg.x=a2;
                poTemp->_end.x=a2+x2-x1;
                *hasPosChanged=TRUE;
                DrawLine(hwndParent,po[i]._end,poTemp->_beg);
                break;
            }

            if(isPassedTrough(a2,lastX2,x2) )
            {
                if(sA2==a2)
                {
                    sA2=-1;
                    break;
                }
                sA2=a2;

                poTemp->_end.x=a2;
                poTemp->_beg.x=a2-(x2-x1);
                *hasPosChanged=TRUE;
                DrawLine(hwndParent,po[i]._end,poTemp->_end);
                break;
            }

        }
        else if(dir==UP)
        {

            //Test avec b1=beg.y

            if(isPassedTrough(b1,lastY1,y1))
            {
                if(sB1==b1)
                {
                    sB1=-1;
                    break;
                }
                sB1=b1;
                poTemp->_beg.y=b1;
                poTemp->_end.y=b1+y2-y1;

                *hasPosChanged=TRUE;
                DrawLine(hwndParent,po[i]._beg,poTemp->_beg);
                break;
            }

            if(!isCapsOn && isPassedTrough( b1,lastY2,y2) )
            {
                if(sB1==b1)
                {
                    sB1=-1;
                    break;
                }
                sB1=b1;

                poTemp->_end.y=b1;
                poTemp->_beg.y=b1-(y2-y1);
                *hasPosChanged=TRUE;
                DrawLine(hwndParent,po[i]._beg,poTemp->_end);
                break;
            }

            //Test avec b2=End.y

            if(isPassedTrough(b2,lastY1,y1) )
            {
                if(sB2==b2)
                {
                    sB2=-1;
                    break;
                }
                sB2=b2;

                poTemp->_beg.y=b2;
                poTemp->_end.y=b2+y2-y1;
                *hasPosChanged=TRUE;
                DrawLine(hwndParent,po[i]._end,poTemp->_beg);
                break;

            }

            if(!isCapsOn &&isPassedTrough(b2,lastY2,y2) )
            {
                if(sB2==b2)
                {
                    sB2=-1;
                    break;
                }
                sB2=b2;

                poTemp->_end.y=b2;
                poTemp->_beg.y=b2-(y2-y1);
                *hasPosChanged=TRUE;
                DrawLine(hwndParent,po[i]._end,poTemp->_end);
                break;

            }

        }
        else if(dir==DOWN)
        {

            //Test avec b1=beg.y

            if(!isCapsOn && isPassedTrough(b1,lastY1,y1))
            {
                if(sB1==b1)
                {
                    sB1=-1;
                    break;
                }
                sB1=b1;
                poTemp->_beg.y=b1;
                poTemp->_end.y=b1+y2-y1;

                *hasPosChanged=TRUE;
                DrawLine(hwndParent,po[i]._beg,poTemp->_beg);
                break;
            }

            if(isPassedTrough( b1,lastY2,y2) )
            {
                if(sB1==b1)
                {
                    sB1=-1;
                    break;
                }
                sB1=b1;

                poTemp->_end.y=b1;
                poTemp->_beg.y=b1-(y2-y1);
                *hasPosChanged=TRUE;
                DrawLine(hwndParent,po[i]._beg,poTemp->_end);
                break;
            }

            //Test avec b2=End.y

            if(!isCapsOn && isPassedTrough(b2,lastY1,y1) )
            {
                if(sB2==b2)
                {
                    sB2=-1;
                    break;
                }
                sB2=b2;

                poTemp->_beg.y=b2;
                poTemp->_end.y=b2+y2-y1;
                *hasPosChanged=TRUE;
                DrawLine(hwndParent,po[i]._end,poTemp->_beg);
                break;

            }

            if(isPassedTrough(b2,lastY2,y2) )
            {
                if(sB2==b2)
                {
                    sB2=-1;
                    break;
                }
                sB2=b2;

                poTemp->_end.y=b2;
                poTemp->_beg.y=b2-(y2-y1);
                *hasPosChanged=TRUE;
                DrawLine(hwndParent,po[i]._end,poTemp->_end);
                break;

            }
        }

    }
    return TRUE;
}
BOOL isPassedTrough(int target,int before,int after)
{
    //cette fonction verifie si target est compris entre before et after
    //Sauf que ici, before n'est pas forcement plus petit que after
    //on va donc devoir faire un petit changement de variable pour
    //respecter cela, ce qui rendra les calculs de verification simple
    if(before>after)
    {
        int temp=before;
        before=after;
        after=temp;
    }
    if(before<=target && target<=after)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
BOOL DrawLine(HWND hwnd,POINT ptBeg,POINT ptEnd)
{
    //cette fonction dessine une ligne qui relie ptBeg a ptEnd sur
    //la zone cliente de hwnd
    if(!hwnd)
        return FALSE;
    HDC hdc;
    PAINTSTRUCT ps;


    rcAlign.left=ptBeg.x-2;
    rcAlign.top=ptBeg.y-2;
    rcAlign.right=ptEnd.x+2;
    rcAlign.bottom=ptEnd.y+2;

    InvalidateRect(hwnd,&rcAlign,TRUE);
    hdc=BeginPaint(hwnd,&ps);
    SelectObject(hdc,CreatePen(PS_SOLID,0,RGB(255,0,0)));
    MoveToEx(hdc,ptBeg.x,ptBeg.y,NULL);
    LineTo(hdc,ptEnd.x,ptEnd.y);
    DeleteObject(SelectObject(hdc,GetStockObject(BLACK_PEN)));
    EndPaint(hwnd,&ps);
    return TRUE;
}


BOOL MyLinkObj(int idObj)
{
    int i,iIndexObj=GetObjIndex(idObj);
    if(iIndexObj==-1)
    {
        MyPerror("error iIndex");
        return FALSE;
    }
    //Afficher juste les obj de meme parent que obj
    for(i=0;i<uObjet[0].nbObjCreated;i++)
    {
        if(iIndexObj==i) continue;
        if(uObjet[i].hwndParent!=uObjet[iIndexObj].hwndParent &&
            !isAncestor(uObjet[i].hwnd,uObjet[iIndexObj].hwnd))
        {
            //Si uObjet[i].hwnd est un ancedre de iIndexObj
            uObjet[i].isVisible=FALSE;
            ShowWindow(uObjet[i].hwnd,SW_HIDE);
        }
    }
    MyFrameLink(iIndexObj,TRUE);
    SetFocus(g_hwndParentObj);
    return TRUE;
}

BOOL MyUnLinkObj(int idObj)
{
    int i,iIndexObj=GetObjIndex(idObj);
    if(iIndexObj==-1)
    {
        MyPerror("error iIndex");
        return FALSE;
    }

    //Afficher juste les obj de meme parent que obj
    for(i=0;i<uObjet[0].nbObjCreated;i++)
    {
        if(iIndexObj==i || uObjet[i].hwnd==g_hwndParentObj) continue;
        if(uObjet[i].hwndParent!=uObjet[iIndexObj].hwndParent &&
            !isAncestor(uObjet[i].hwnd,uObjet[iIndexObj].hwnd))
        {
            uObjet[i].isVisible=FALSE;
            ShowWindow(uObjet[i].hwnd,SW_HIDE);
        }
    }
    MyFrameLink(iIndexObj,FALSE);
    SetFocus(g_hwndParentObj);
    return TRUE;
}

BOOL MyFrameLink(int iIndexObj,BOOL isLink)
{
    if(isLink==TRUE)
    {
        //Cette fonction frameRect avec une couleur aleatoire
        DeleteObject(g_link.hBrushLink);
        couleur=RGB(rand()%256,rand()%256,rand()%256);
        g_link.hBrushLink=CreateSolidBrush(couleur);
        HWND hwndObj=g_link.hwndObjToLink=uObjet[iIndexObj].hwnd;
        RECT rcFrame;

        HDC hdc=GetDC(GetParent(hwndObj));
        HPEN hPen=CreatePen(PS_SOLID,10,couleur);
        SelectObject(hdc,hPen);
        SelectObject(hdc,GetStockObject(NULL_BRUSH));
        GetWindowRect(hwndObj,&rcFrame);
        MyScreenRcToClientRc(GetParent(hwndObj),&rcFrame);
        g_link.rcFrame=rcFrame;
        Rectangle(hdc,rcFrame.left,rcFrame.top,rcFrame.right,rcFrame.bottom);
        DeleteObject(SelectObject(hdc,GetStockBrush(BLACK_PEN)));
        ReleaseDC(GetParent(hwndObj),hdc);

        //Important
        g_link.isFrameLinkOn=TRUE;
    }
    else
    {
        //Masquer  hwndToUnlink
        //Framer tous les autres
        RECT rcMask;
        int i;
        DeleteObject(g_unlink.hBrushUnLink);
        couleur=RGB(rand()%256,rand()%256,rand()%256);
        g_unlink.hBrushUnLink=CreateSolidBrush(couleur);
        g_unlink.hwndObjToUnLink=uObjet[iIndexObj].hwnd;
        GetWindowRect(uObjet[iIndexObj].hwnd,&rcMask);
        MyScreenRcToClientRc(uObjet[iIndexObj].hwndParent,
                         &rcMask);

        hwndMask=CreateWindow(szClassLinkMask,"NULL",
                          WS_OVERLAPPED|WS_BORDER|WS_CHILD|WS_VISIBLE,
                          rcMask.left,rcMask.top,rcMask.right-rcMask.left,rcMask.bottom-rcMask.top,
                          uObjet[iIndexObj].hwndParent,(HMENU)123,
                          (HINSTANCE)GetWindowLongPtr(uObjet[iIndexObj].hwnd,GWLP_HINSTANCE),
                          NULL);
        if(!hwndMask)
        {
            MyPerror("error loading hwndMask");
            return FALSE;
        }
        SetClassLongPtr(hwndMask,GCLP_HBRBACKGROUND,
                 (LONG_PTR)g_unlink.hBrushUnLink);
        InvalidateRect(hwndMask,NULL,TRUE);
        BringWindowToTop(hwndMask);

        //Afficher les autres rectangles frames
         for(i=0;i<uObjet[iIndexObj].linkopt.iLastIndex;i++)
        {

             HWND hwndToFrame;
            hwndToFrame=uObjet[iIndexObj].linkopt.hwndLinkWith[i];
            RECT rcFrame;

            //NE PAS TOUCHER!!!!! ;)
            HDC hdc=GetDC(GetParent(hwndToFrame));
            HPEN hPen=CreatePen(PS_SOLID,10,couleur);
            SelectObject(hdc,hPen);
            SelectObject(hdc,GetStockObject(NULL_BRUSH));
            GetWindowRect(hwndToFrame,&rcFrame);
            MyScreenRcToClientRc(GetParent(hwndToFrame),&rcFrame);
            Rectangle(hdc,rcFrame.left,rcFrame.top,rcFrame.right,rcFrame.bottom);
            DeleteObject(SelectObject(hdc,GetStockBrush(BLACK_PEN)));
            ReleaseDC(GetParent(hwndToFrame),hdc);

        }
         //Important
        g_unlink.isMaskUnLinkOn=TRUE;

     }
    return TRUE;
}

BOOL MyApproveLink(HWND hwndToLink,HWND hwndLinkTo)
{
    //Masquer hwndLinkTo
    RECT rcMask;
    int iResult;
    int iIndex,i;

    //Test validity

    if(isAncestor(hwndToLink,hwndLinkTo) ||
       isAncestor(hwndLinkTo,hwndToLink))
    {
        InvalidateRect(GetParent(hwndLinkTo),NULL,TRUE);
        return FALSE;
    }

    iIndex=MyGetIindexObjWithHwnd(hwndLinkTo);
    if(iIndex==-1)
    {
        InvalidateRect(GetParent(hwndLinkTo),NULL,TRUE);
        return FALSE;
    }


    iIndex=MyGetIindexObjWithHwnd(hwndToLink);
    if(iIndex==-1)
    {
        InvalidateRect(GetParent(hwndLinkTo),NULL,TRUE);
        return FALSE;
    }

    //Ne rien faire sI les hwnd Sont deja lies
    for(i=0;i<uObjet[iIndex].linkopt.iLastIndex;i++)
    {
        if(hwndLinkTo==uObjet[iIndex].linkopt.hwndLinkWith[i])
        {
            MessageBox(NULL,TEXT("Already linked"),TEXT("INFORMATION"),
                       MB_ICONINFORMATION|MB_TASKMODAL);
            InvalidateRect(GetParent(hwndLinkTo),NULL,TRUE);
            return FALSE;
        }
    }
    GetWindowRect(hwndLinkTo,&rcMask);
    MyScreenRcToClientRc(GetParent(hwndLinkTo),
                         &rcMask);

    hwndMask=CreateWindow(szClassLinkMask,"NULL",
                          WS_OVERLAPPED|WS_BORDER|WS_CHILD|WS_VISIBLE,
                          rcMask.left,rcMask.top,rcMask.right-rcMask.left,rcMask.bottom-rcMask.top,
                          GetParent(hwndLinkTo),(HMENU)123,
                          (HINSTANCE)GetWindowLongPtr(hwndLinkTo,GWLP_HINSTANCE),
                          NULL);
    if(!hwndMask)
    {
        MyPerror("error loading hwndMask");
        return FALSE;
    }
    SetClassLongPtr(hwndMask,GCLP_HBRBACKGROUND,
                 (LONG_PTR)g_link.hBrushLink);
    InvalidateRect(hwndMask,NULL,TRUE);

    iResult=MessageBox(NULL,"Do you accept this linkage?","QUESTION?",
                       MB_ICONQUESTION|MB_YESNO|
                       MB_TASKMODAL);

    if(iResult==IDYES)
    {
        //Ajouter hwndLinKto a hwndToLink
        if(!MyAddLink(hwndToLink,hwndLinkTo))
        {
            MyPerror("error myaddLink");
            return FALSE;
        }
        DestroyWindow(hwndMask);
        InvalidateRect(GetParent(hwndToLink),NULL,TRUE);
    }
    else if(iResult==IDNO)
    {
        DestroyWindow(hwndMask);
        InvalidateRect(GetParent(hwndToLink),NULL,TRUE);
    }
    return TRUE;
}

BOOL MyApproveUnlink(HWND hwndToUnlink,HWND hwndUnlinkTo)
{
    int iResult,i;
    int iIndex=MyGetIindexObjWithHwnd(hwndToUnlink);
    BOOL hasFound=FALSE;
    //S'assurer que hwndUnlinkTo est bien present
    if(iIndex==-1)
    {
        MyPerror("iIndex==-1");
        DestroyWindow(hwndMask);
        InvalidateRect(GetParent(hwndToUnlink),NULL,TRUE);
        return FALSE;
    }
    for(i=0;i<uObjet[iIndex].linkopt.iLastIndex;i++)
    {
        if(uObjet[iIndex].linkopt.hwndLinkWith[i]==hwndUnlinkTo)
        {
            hasFound=TRUE;
            break;
        }
    }
    if(hasFound==FALSE)
    {
        DestroyWindow(hwndMask);
        InvalidateRect(GetParent(hwndToUnlink),NULL,TRUE);
        return FALSE;
    }

    iResult=MessageBox(NULL,"Delete this linkage?","QUESTION?",
                       MB_ICONQUESTION|MB_YESNO|MB_TASKMODAL);

    if(iResult==IDYES)
    {
        //Delier hwndUnlinkTo a hwndToUnlink
        //Ajouter hwndLinKto a hwndToLink
        if(!MyRemLink(hwndToUnlink,hwndUnlinkTo))
        {
            MyPerror("error myRemlink");
            return FALSE;
        }
        DestroyWindow(hwndMask);
        InvalidateRect(GetParent(hwndToUnlink),NULL,TRUE);
    }
    else if(iResult==IDNO)
    {
        DestroyWindow(hwndMask);
        InvalidateRect(GetParent(hwndToUnlink),NULL,TRUE);
    }
    return TRUE;
}

LRESULT CALLBACK MyLinkMaskProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
    case WM_LBUTTONDOWN:

        return 0;

    }
    return DefWindowProc(hwnd,message,wParam,lParam);
}

BOOL MyAddLink(HWND hwnd1,HWND hwnd2)
{
    if(!hwnd1 || !hwnd2) return FALSE;
    int iIndex,i;
    MYLINKOBJ *link;

    //Add hwnd1 to hwnd2
    iIndex=MyGetIindexObjWithHwnd(hwnd2);
    if(iIndex==-1) return FALSE;

    link=&uObjet[iIndex].linkopt;

    if(link->iLastIndex && !((link->iLastIndex)%10))
    {
        //reallocation
        int n=link->iLastIndex;
        link->nbLink+=10;

        link->hwndLinkWith=realloc(link->hwndLinkWith,sizeof(HWND)*link->nbLink);
        link->id=realloc(link->id,sizeof(int)*link->nbLink);

        if(!(link->hwndLinkWith) || !(link->id))
        {
            MyPerror("error realloc link->hwndLinkWith");
            exit(EXIT_FAILURE);
        }
        memset(link->hwndLinkWith +n,0,sizeof(HWND)*10);
        memset(link->id +n,0,sizeof(HWND)*10);

    }
    i=link->iLastIndex;
    if(!(link->hwndLinkWith))
    {
        exit(EXIT_FAILURE);
    }
    link->hwndLinkWith[i]=hwnd1;
    link->id[i]=GetDlgCtrlID(hwnd1);
    link->iLastIndex+=1;
    link->isLinked=TRUE;

    //Add hwnd2 to hwnd1
    iIndex=MyGetIindexObjWithHwnd(hwnd1);
    if(iIndex==-1) return FALSE;

    link=&uObjet[iIndex].linkopt;

    if(link->iLastIndex && !((link->iLastIndex)%10))
    {
        //reallocation
        int n=link->iLastIndex;
        link->nbLink+=10;
        link->hwndLinkWith=realloc(link->hwndLinkWith,sizeof(HWND)*link->nbLink);
        link->id=realloc(link->id,sizeof(int)*link->nbLink);

        if(!(link->hwndLinkWith) || !(link->id))
        {
            MyPerror("error realloc link->hwndLinkWith");
            exit(EXIT_FAILURE);
        }
        memset(link->hwndLinkWith +n,0,sizeof(HWND)*10);
        memset(link->id +n,0,sizeof(HWND)*10);

    }
    i=link->iLastIndex;
    link->hwndLinkWith[i]=hwnd2;
    link->id[i]=GetDlgCtrlID(hwnd2);
    link->iLastIndex+=1;
    link->isLinked=TRUE;

    return TRUE;
}

BOOL MyRemLink(HWND hwnd1,HWND hwnd2)
{
    if(!hwnd1 || !hwnd2)
    {
        MyPerror(" spoiodifoi");
        return FALSE;
    }
    int iIndex,i,iIndexToDel;
    MYLINKOBJ *link;

    //Remove hwnd1 to hwnd2

    iIndex=MyGetIindexObjWithHwnd(hwnd2);
    //iIndex=GetObjIndex(GetDlgCtrlID(hwnd2));
    if(iIndex!=-1)
    {
        link=&uObjet[iIndex].linkopt;

        iIndexToDel=-1;
        for(i=0;i<link->iLastIndex;i++)
        {
            if(link->hwndLinkWith[i]==hwnd1)
            {
                iIndexToDel=i;
                break;
            }
        }
        if(iIndexToDel==-1)
        {
            MyPerror("2error iIndexToDel=-1");
            return FALSE;
        }
        for(i=iIndexToDel;i<-1+link->iLastIndex;i++)
        {
            link->hwndLinkWith[i]=link->hwndLinkWith[i+1];
            link->id[i]=link->id[i+1];
        }
        link->iLastIndex-=1;
        if(link->iLastIndex)
            link->isLinked=TRUE;
        else
            link->iLastIndex=FALSE;
        link->iLastIndex=max(0,link->iLastIndex);

    }

    //Remove hwnd2 to hwnd1
    iIndex=MyGetIindexObjWithHwnd(hwnd1);
    //iIndex=GetObjIndex(GetDlgCtrlID(hwnd1));
    if(iIndex!=-1)
    {

        link=&uObjet[iIndex].linkopt;

        iIndexToDel=-1;
        for(i=0;i<link->iLastIndex;i++)
        {
            if(link->hwndLinkWith[i]==hwnd2)
            {
                iIndexToDel=i;
                break;
            }
        }
        if(iIndexToDel==-1)
        {
            MyPerror("1error iIndexToDel=-1");
            return FALSE;
        }
        for(i=iIndexToDel;i<-1+link->iLastIndex;i++)
        {
            link->hwndLinkWith[i]=link->hwndLinkWith[i+1];
            link->id[i]=link->id[i+1];
        }
        link->iLastIndex-=1;
        if(link->iLastIndex)
            link->isLinked=TRUE;
        else
            link->iLastIndex=FALSE;

        link->iLastIndex=max(0,link->iLastIndex);
    }
    return TRUE;
}
BOOL MyShowAllObj(void)
{
    int i;
    //Afficher les obj qui etaient cachees
    for(i=0;i<uObjet[0].nbObjCreated;i++)
    {
        if(uObjet[i].isVisible==FALSE)
        {
            uObjet[i].isVisible=TRUE;
            ShowWindow(uObjet[i].hwnd,SW_NORMAL);
        }
    }
    return TRUE;
}


BOOL isAncestor(HWND hwndAncestor,HWND hwndChild)
{
    //Cette fonction verifie si hwndAncestor est un parent de
    //hwndChild
    LPARAM_ENUMANCES lParamSup;

    lParamSup.response=FALSE;
    lParamSup.hwndChild=hwndChild;

    EnumChildWindows(hwndAncestor,EnumChildIsAncetorProc,
                     (LPARAM)&lParamSup);

    return lParamSup.response;
}

BOOL CALLBACK EnumChildIsAncetorProc(HWND hwndChild,LPARAM lParam)
{
    LPARAM_ENUMANCES *lParamSup;

    lParamSup=(LPARAM_ENUMANCES*)lParam;
    if(hwndChild==lParamSup->hwndChild)
    {
        lParamSup->response=TRUE;
        return FALSE;
    }

    EnumChildWindows(hwndChild,EnumChildIsAncetorProc,
                     (LPARAM)lParamSup);

    return TRUE;
}

BOOL MyUpdateLinksUniv(void)
{

    //Cette fonction met a jour tous le liens de
    //L'univers et verifie que tous les handles des objets
    //associes existent
    int i,j,nbLinkage,nbObj=uObjet[0].nbObjCreated;

    MYLINKOBJ *link;
    for(i=0;i<nbObj;i++)
    {
        link=&uObjet[i].linkopt;
        if(link->isLinked)
        {
            nbLinkage=link->iLastIndex;
            for(j=0;j<nbLinkage;j++)
            {
                HWND hwndCopie=link->hwndLinkWith[j];
                link->hwndLinkWith[j]=MyGetHwndObj(link->id[j]);

                if(!(link->hwndLinkWith[j]))
                {
                    //Alors supprimer ce lien et recommencer
                    link->hwndLinkWith[j]=hwndCopie;
                    if(MyRemLink(hwndCopie,uObjet[i].hwnd))
                    {
                        nbLinkage=link->iLastIndex;
                        j--;
                    }
                    else
                    {
                        MyPerror("mvpodrie");
                        return FALSE;
                    }
                }
            }
        }
    }
    return TRUE;
}

HWND MyGetHwndObj(int idObj)
{//Cette fonction se charge de parcourir l'univers
    //puis de retourner le handle de la fenetre qui a idObj comme id
    int i;
    for(i=0;i<uObjet[0].nbObjCreated;i++)
    {
        if(uObjet[i].id==idObj)
        {
            return uObjet[i].hwnd;
        }
    }
    return NULL;
}

int MyGetIindexObjWithHwnd(HWND hwndObj)
{
  //Cette fonction se charge de parcourir lunivers puis,
  //de retourner le idObj de l'univers qui a hwndObj
    if(!hwndObj) return -1;
    int i;
    for(i=0;i<uObjet[0].nbObjCreated;i++)
    {
        if(uObjet[i].hwnd==hwndObj)
        {
            return i;
        }
    }
    return -1;
}
WINBOOL MyMoveWindow(int iIndexObj,int left,int top,int right,int bottom,BOOL fRedraw)
{
    g_fShouldIAskSave=TRUE;
    HWND hwndObj=uObjet[iIndexObj].hwnd;
    WINBOOL iResult=TRUE;
    RECT rcObjLast;

    GetWindowRect(hwndObj,&rcObjLast);
    MyScreenRcToClientRc(GetParent(hwndObj),&rcObjLast);

    iResult=MoveWindow(hwndObj,left,top,right,bottom,fRedraw);

    //Mettre a jour la positon de l'objet deplace
    //On a deja iIndex
    if(hwndObj!=g_hwndParentObj)
    {
        po[iIndexObj]._beg.x=left;
        po[iIndexObj]._beg.y=top;
        po[iIndexObj]._end.x=left+right;
        po[iIndexObj]._end.y=top+bottom;
    }

    //Gerer les fonctionnalites de link
    MYLINKOBJ *link;
    int i,iIndexObj2,left2,top2,right2,bottom2;
    RECT rcObj;
    POINT distance;


    link=&uObjet[iIndexObj].linkopt;
    if(link->isLinked)
    {
        for(i=0;i<link->iLastIndex;i++)
        {

            hwndObj=link->hwndLinkWith[i];
            GetWindowRect(hwndObj,&rcObj);
            MyScreenRcToClientRc(GetParent(hwndObj),&rcObj);
            distance.x=rcObj.left-rcObjLast.left;
            distance.y=rcObj.top-rcObjLast.top;

            right2=rcObj.right-rcObj.left;
            bottom2=rcObj.bottom-rcObj.top;
            left2=left+distance.x;
            top2=top+distance.y;

            MoveWindow(hwndObj,left2,top2,right2,bottom2,fRedraw);
            iIndexObj2=GetObjIndex(link->id[i]);
            if(iIndexObj2==-1)
            {
                MyPerror("d,sopof");
                continue;
            }
            //Mettre a jour la positon de l'objet deplace
            //On a deja iIndex
            if(hwndObj!=g_hwndParentObj)
            {
                po[iIndexObj2]._beg.x=left2;
                po[iIndexObj2]._beg.y=top2;
                po[iIndexObj2]._end.x=left2+right2;
                po[iIndexObj2]._end.y=top2+bottom2;
            }

        }

    }

    return iResult;
}
BOOL isLinkedWith(int iIndex1,int iIndex2)
{
    //cette fonction verifie si iIndex2 fait partie des liens de iIndex1
    //et vice versa
    int i;
    MYLINKOBJ *link;
    HWND hwndTemp;

    //Search iIndex2 dans iIndex1
    hwndTemp=uObjet[iIndex2].hwnd;
    link=&uObjet[iIndex1].linkopt;
    for(i=0;i<link->iLastIndex;i++)
    {
        if(link->hwndLinkWith[i]==hwndTemp)
        {
            return TRUE;
        }
    }


    //Search iIndex1 dans iIndex2
    hwndTemp=uObjet[iIndex1].hwnd;
    link=&uObjet[iIndex2].linkopt;
    for(i=0;i<link->iLastIndex;i++)
    {
        if(link->hwndLinkWith[i]==hwndTemp)
        {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL isGroupBoxObj(int iIndex)
{
    //Cette fonction verifie si uObjet[iIndex]
    //est de type GroupBox
    if(iIndex<0 || iIndex>=uObjet[0].nbObjCreated)
        return FALSE;
    if(strstr(uObjet[iIndex].szLastStylesChosen,"BS_GROUPBOX|"))
        return TRUE;

    return FALSE;
}

BOOL isChildGB(HWND hwnd)
{
    //Cette fonction determine si le parent de hwnd est unGroupBox
    HWND hwndParent=GetParent(hwnd);

    if(!hwndParent) return FALSE;
    if(GetDlgCtrlID(hwnd)!=ID_DLG_GB) return FALSE;

    int iIndexParent=MyGetIindexObjWithHwnd(hwndParent);

    if(iIndexParent!=-1 && isGroupBoxObj(iIndexParent))
        return TRUE;

    return FALSE;
}

LRESULT CALLBACK ChildGroupBoxProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
    case WM_PAINT:
        //colorier hwnd avec la couleur du parent GB
        {
            //Obtenir le Dlg directement parent de childGb
            //il prendra sa colors
            HWND hwndDlg;
            TCHAR szClassName[1024];

            hwndDlg=GetParent(hwnd);

            GetClassName(hwndDlg,szClassName,1023);

            while(strcmp(szClassName,szClassDlgObj) &&
                  strcmp(szClassName,szClassZC) )
            {
                if(!hwndDlg)
                {
                    hwndDlg=g_hwndParentObj;
                    MyPerror(" mpooti");
                    break;
                }
                hwndDlg=GetParent(hwndDlg);
                if(!hwndDlg)
                {
                    hwndDlg=g_hwndParentObj;
                    MyPerror("vmoiodiot");
                    break;
                }
                GetClassName(hwndDlg,szClassName,1023);
             }

            int iIndex=MyGetIindexObjWithHwnd(hwndDlg);
            if(iIndex==-1 && hwndDlg!=hwndZC)
            {
                MyPerror(",cieri");
                break;
            }
            PAINTSTRUCT ps;
            HDC hdc;
            //DeleteObject(hBrushGb);
            if(iIndex==-1 && hwndDlg==hwndZC)
            {
                HDC hdc1=GetDC(hwndDlg);
                hBrushGb=CreateSolidBrush(GetBkColor(hdc1));

                ReleaseDC(hwndDlg,hdc1);
            }
            else
                hBrushGb=CreateSolidBrush(MyGetSysColor(uObjet[iIndex].mycolorobj[BK],uObjet[iIndex].hwnd));

            hdc=BeginPaint(hwnd,&ps);
            FillRect(hdc,&ps.rcPaint,hBrushGb);
            EndPaint(hwnd,&ps);
        }
        return 0;

    case WM_LBUTTONDOWN:
        //Translate coordinates point to real coordinates for GB
        //Cause there is a shif between child and GB
        {
            MyUpDateDimChar(GetParent(hwnd));
            int vShift=cyChar-2;
            lParam=MAKELONG(GET_X_LPARAM(lParam)+2,
                            GET_Y_LPARAM(lParam)+vShift);
        }
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_KEYDOWN:
        SendMessage(GetParent(hwnd),message,wParam,lParam);
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
        hwndParentObj=GetParent(hwnd);

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

    case WM_MOUSEMOVE:
        {
            //Donne la capacite a chaque child de dectecter le WM_MOUSELEAVE
            TRACKMOUSEEVENT tme = { 0 };
            tme.cbSize = sizeof(tme);
            tme.dwFlags = TME_LEAVE|TME_HOVER;
            tme.hwndTrack = hwnd;
            tme.dwHoverTime = 1500; //
            TrackMouseEvent(&tme);
        }
        break;

    case WM_MOUSEHOVER:
        MyDisPlayInfoObj(GetDlgCtrlID(GetParent(hwnd)));
        return 0;

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


    }
    return DefWindowProc(hwnd,message,wParam,lParam);
}

BOOL MyFreeuObjet(UNIVERS **puObjet)
{

    //Libere proprement uObjet en commencant premierement
    //par les  champs pointeurs de UObjet

    if(!(*puObjet))
    {
        return TRUE;
    }

    UNIVERS *unvs=*puObjet;

    for(int i=0;i<unvs[0].nbObjCreated;i++)
    {
        if(unvs[i].iTypeObj==OBJ_DIALOGBOX)
        {
            if(unvs[i].menu)
            {
                if(!MyFreeMenuTab(&unvs[i].menu))
                {
                    MyPerror("n apodsiodsif");
                    return FALSE;
                }
            }
        }
        if(!MyFreeLinkOpt(&unvs[i].linkopt))
        {
            MyPerror(" asoifosdif");
            return FALSE;
        }
        DeleteObject(uObjet[i].font.hFont);

    }

    free(*puObjet);
    *puObjet=NULL;

    return TRUE;
}

BOOL MyFreeLinkOpt(MYLINKOBJ *link)
{
    //Permet de juste de free les champs *hwnd et *id;
    if(link->hwndLinkWith)
    {
        free(link->hwndLinkWith);
        link->hwndLinkWith=NULL;
    }
    if(link->id)
    {
        free(link->id);
        link->id=NULL;
    }
    return TRUE;
}

BOOL fHasChilds(int iIndex)
{
    //Cette fonction verifie si uObjet[iIndex].hwnd est le parent
    //d'un obj dans l'univers
    //On verifie au prealable si iIndex est une groupBox si oui
    //Alors, son le parent sera hwndChildGb
    HWND hwndParent;
    if(isGroupBoxObj(iIndex))
    {
        hwndParent=uObjet[iIndex].hwndChildGB;
        for(int i=0;i<uObjet[0].nbObjCreated;i++)
        {
            if(i==iIndex)continue;
            if(uObjet[i].hwndParent==hwndParent)
                return TRUE;
        }
    }
    else if(uObjet[iIndex].iTypeObj==OBJ_DIALOGBOX)
    {
        hwndParent=uObjet[iIndex].hwnd;
        for(int i=0;i<uObjet[0].nbObjCreated;i++)
        {
            if(i==iIndex)continue;
            if(uObjet[i].hwndParent==hwndParent)
                return TRUE;
        }
    }
    return FALSE;
}

BOOL MyCheckAutosizeOpt(HMENU hMenu,int iIndexObj)
{
    //cette fonction obtient le sous menu de hMenu
    //puis coche les item du sous menu(left,top,bottoom,right)
    //Next les indications situees dans uObjet[iIndexObj].fShouldAutosize
    if(!hMenu)
    {
        MyPerror("dkglkwe");
        return FALSE;
    }

    if(iIndexObj<0 || iIndexObj>=uObjet[0].nbObjCreated)
    {
        MyPerror("lkvlkeer");
        return FALSE;
    }
    HMENU hSubMenu=GetSubMenu(hMenu,6);//6 c'est la  0-positon actuelle apres unlink
    if(!hSubMenu)
    {
        MyPerror("kvlklgrg");
        return FALSE;
    }

    UINT uFlag;
    //top
    if(uObjet[iIndexObj].fShouldAutosize.top)
        uFlag=MF_CHECKED|MF_BYCOMMAND;
    else
        uFlag=MF_UNCHECKED|MF_BYCOMMAND;
    CheckMenuItem(hSubMenu,IDM_AUTOSIZE_TOP,uFlag);
    //bottom
    if(uObjet[iIndexObj].fShouldAutosize.bottom)
        uFlag=MF_CHECKED|MF_BYCOMMAND;
    else
        uFlag=MF_UNCHECKED|MF_BYCOMMAND;
    CheckMenuItem(hSubMenu,IDM_AUTOSIZE_BOTTOM,
                  uFlag);

    //right
    if(uObjet[iIndexObj].fShouldAutosize.right)
        uFlag=MF_CHECKED|MF_BYCOMMAND;
    else
        uFlag=MF_UNCHECKED|MF_BYCOMMAND;
    CheckMenuItem(hSubMenu,IDM_AUTOSIZE_RIGHT,uFlag);

    //left
    if(uObjet[iIndexObj].fShouldAutosize.left)
        uFlag=MF_CHECKED|MF_BYCOMMAND;
    else
        uFlag=MF_UNCHECKED|MF_BYCOMMAND;
    CheckMenuItem(hSubMenu,IDM_AUTOSIZE_LEFT,uFlag);

    return TRUE;
}


BOOL MySetAutoSizeOpt(HMENU hMenu,int iIndexObj,int idItem)
{
    //Cette fonction est appelee pour soit cocher ou decocher idItem
    //de subMenu de hMenu
    if(!hMenu)
    {
        MyPerror("djfgs5gsf");
        return FALSE;
    }

    if(iIndexObj<0 || iIndexObj>=uObjet[0].nbObjCreated)
    {
        MyPerror("hd54gjf");
        return FALSE;
    }
    HMENU hSubMenu=GetSubMenu(hMenu,6);//6 c'est la  0-positon actuelle apres unlink
    if(!hSubMenu)
    {
        MyPerror("trre565h");
        return FALSE;
    }
    BOOL *p=NULL;

    //Determiner la zone (TOP,BOTTOM,RIGHT,LEFT) a changer
    switch(idItem)
    {
    case IDM_AUTOSIZE_BOTTOM:
        p=&uObjet[iIndexObj].fShouldAutosize.bottom;
        break;

    case IDM_AUTOSIZE_TOP:
        p=&uObjet[iIndexObj].fShouldAutosize.top;
        break;

    case IDM_AUTOSIZE_RIGHT:
        p=&uObjet[iIndexObj].fShouldAutosize.right;
        break;

    case IDM_AUTOSIZE_LEFT:
        p=&uObjet[iIndexObj].fShouldAutosize.left;
        break;
    }
    UINT uState=GetMenuState(hSubMenu,idItem,MF_BYCOMMAND);
    UINT uNewFlag;

    if(!p)
    {
        MyPerror("vsakkf");
        MyMsgError();
        exit(EXIT_FAILURE);
    }
    if(uState & MF_CHECKED)
    {
        //Si l'item etait coche
        //On decoche
        *p=FALSE;
        uNewFlag=MF_UNCHECKED|MF_BYCOMMAND;
        CheckMenuItem(hSubMenu,idItem,uNewFlag);
    }
    else
    {
        //Si l'item etait decoche
        //On coche
        *p=TRUE;
        uNewFlag=MF_CHECKED|MF_BYCOMMAND;
        CheckMenuItem(hSubMenu,idItem,uNewFlag);
    }
    return TRUE;
}


BOOL MyApplyAutoSizeOnChild(HWND hwndParent,int cxLastClient,int cyLastClient)
{
    //le role de cette fonction  est de rediemsionner les childs de
    //hwndParent, Next la modificatin de taille qui vient detre effectuee
    //sur hwnd
    //en parametre on a les cxLast et cyLast qui contiennent les dimensions
    //avant le WM_SIZE, les dimenisions des childs actuels sont calibrees
    //sur ces anciennes dimension. On va juste recuperer le coef de
    //proportionnalite et lApply au dimesion actuelles(Next les orientatation
    //(left,right,bottom,top) defini par user pour chaque objet

    int nbObj=uObjet[0].nbObjCreated;
    if(nbObj<=1)
        return FALSE;

    //Obtenir le iIindex du prent
    int iIndexParent=MyGetIindexObjWithHwnd(hwndParent);
    if(iIndexParent==-1)
    {
        MyPerror("vdklhk;d");
        return FALSE;
    }
    //on travaille avec po[] et non les dimesion set dans uobjet,
    //car celles de po[] sont regulierment mise a jour a chaque move
    for(int i=0;i<nbObj;i++)
    {
        if(uObjet[i].hwndParent==hwndParent)
        {

            //On ajuste Next les preferences d'autosize definies par user
            if(uObjet[i].fShouldAutosize.bottom)
                MyAdjustDimChild(DOWN,i,iIndexParent,cxLastClient,cyLastClient);

            if(uObjet[i].fShouldAutosize.top)
                MyAdjustDimChild(UP,i,iIndexParent,cxLastClient,cyLastClient);

            if(uObjet[i].fShouldAutosize.left)
                MyAdjustDimChild(LEFT,i,iIndexParent,cxLastClient,cyLastClient);

            if(uObjet[i].fShouldAutosize.right)
                MyAdjustDimChild(RIGHT,i,iIndexParent,cxLastClient,cyLastClient);

        }
    }
    return TRUE;
}

BOOL MyAdjustDimChild(int side,int iIndexChild,int iIndexParent,int cxLastClient,int cyLastClient)
{
    //cette fonction ajuste les dimension de po[iIndexChild] Next le side
    //et en se basant sur la relation qui unissait po[iIndexChild] et
    //les cyLast et cxLast du parent
    if(iIndexChild==iIndexParent)
    {
        MyPerror("vlkls;lf");
        return FALSE;
    }
    if(iIndexChild<0 || iIndexChild>=uObjet[0].nbObjCreated)
    {
        MyPerror("kldk;dk");
        return FALSE;
    }
    if(iIndexParent<0 || iIndexParent>=uObjet[0].nbObjCreated)
    {
        MyPerror("kldk;dk");
        return FALSE;
    }

    RECT rcParent;
    //obtenir les nouvelles dimension du parent
    GetClientRect(uObjet[iIndexParent].hwnd,&rcParent);
    switch(side)
    {
    case UP:
        //User veut que le top soit proportionnel et non fige
        //Il faut juste Apply lancien coef de proportionnalite au dimesion actuelles du parent
        {
            int cyNewParent=rcParent.bottom-rcParent.top;
            int iLastTop=po[iIndexChild]._beg.y;
            //int height=po[iIndexChild]._end.y-iLastTop;

            po[iIndexChild]._beg.y=cyNewParent*iLastTop/cyLastClient;
            /*//Le height doit demeurrer constant
            po[iIndexChild]._end.y=po[iIndexChild]._beg.y+height;
            */
        }
        break;

    case DOWN:
        //User veut que le bottom soit proportionnel et non fige
        //Il faut juste Apply lancien coef de proportionnalite au dimesion actuelles du parent
        {
            int cyNewParent=rcParent.bottom-rcParent.top;
            int iLastBottom=po[iIndexChild]._end.y;

            po[iIndexChild]._end.y=cyNewParent*iLastBottom/cyLastClient;
        }
        break;

    case LEFT:
        //User veut que le left soit proportionnel et non fige
        //Il faut juste Apply lancien coef de proportionnalite aux dimesions actuelles du parent
        {
            int cxNewParent=rcParent.right-rcParent.left;
            int iLastLeft=po[iIndexChild]._beg.x;
            //int width=po[iIndexChild]._end.x-iLastLeft;

            po[iIndexChild]._beg.x=cxNewParent*iLastLeft/cxLastClient;
            /*//Le width doit demeurrer constant
            po[iIndexChild]._end.x=po[iIndexChild]._beg.x+width;
            */
        }
        break;

    case RIGHT:
        //User veut que le right soit proportionnel et non fige
        //Il faut juste Apply lancien coef de proportionnalite aux dimesions actuelles du parent
        {
            int cxNewParent=rcParent.right-rcParent.left;
            int iLastRight=po[iIndexChild]._end.x;

            po[iIndexChild]._end.x=cxNewParent*iLastRight/cxLastClient;
        }
        break;

    default:
        MyPerror("mbklkdsfl");
        return FALSE;
    }

    int x=po[iIndexChild]._beg.x;
    int y=po[iIndexChild]._beg.y;
    int w=po[iIndexChild]._end.x-x;
    int h=po[iIndexChild]._end.y-y;
    HWND hwndChild=uObjet[iIndexChild].hwnd;
    MoveWindow(hwndChild,x,y,w,h,TRUE);
    return TRUE;
}
