/*Nom du File:MyLoadBorderOpt.c
Role:File qui gere la creation des bordures, permettant a user
,lorsqu'il aura cree un nouvel objet, en appuyant sur CapsLock, de redimensionner
son bouton
BY CHRISTIAN EDOUARDO LE 19/09/2024 a 17h34*/
#include <windows.h>
#include <windowsx.h>//pour GET_X_LPARAM etc
#include "OpenDialogBox.h"
LRESULT CALLBACK BorderProc(HWND hwnd, UINT msg,WPARAM wParam, LPARAM lParam  );
BOOL MyMaskBorder(BOOL choice);
BOOL Myborder(HWND hwndTarget);
BOOL MyChangeParentBorder(HWND hwndNewParentBorder);
TCHAR *szClassBorder[]={TEXT("bcUP"),TEXT("bcDOWN"),TEXT("bcRIGHT"),
TEXT("bcLEFT"),TEXT("bcLU"),TEXT("bcLD"),TEXT("bcRU"),TEXT("bcRD")
    };
static HWND hwndBorder[8];
static HWND hwndToSize;


BOOL MyLoadBorderOpt(HWND hwndParentBorder)
{
   if(!hwndParentBorder) return FALSE;

    //cette fonction cree juste les fenetres de bordures
    static int nbOccur=0;
    int i;
    HCURSOR hCursor=NULL;
    WNDCLASS wc;
    HINSTANCE hinstance=(HINSTANCE)GetWindowLongPtr(hwndParentBorder,
                                      GWLP_HINSTANCE);
    wc.cbClsExtra=0;
    wc.cbWndExtra=0;
    wc.hCursor=LoadCursor(NULL,IDC_ARROW);
    wc.hIcon=LoadIcon(NULL,IDI_APPLICATION);
    wc.hInstance=hinstance;
    wc.lpszMenuName=NULL;
    wc.style=CS_VREDRAW|CS_HREDRAW;
    hBrushGlobal=CreateSolidBrush(RGB(236,153,20));
    wc.hbrBackground=hBrushGlobal;
    wc.lpfnWndProc=BorderProc;
    //enregistrer les classes pour les fenetres bordures

    nbOccur++;
    if(nbOccur==1)//juste une fois
    {
        //Cette boucle for ne doit s'executer que la premiere fois
        for(int i=0;i<8;i++)
        {
             wc.lpszClassName=szClassBorder[i];
            //Resgistering Border class
            if(!RegisterClass(&wc))
            {
                MyPerror("Error in registering wc border");
                exit(EXIT_FAILURE);
            }
        }
    }
    else
    {
        //Supprimer les anciennes fenetres Border
        for(i=0;i<8;i++)
        {
            DestroyWindow(hwndBorder[i]);
        }
    }
    //creer les 8bordures
    for(i=0;i<8;i++)
    {
        hwndBorder[i]=CreateWindow(szClassBorder[i],NULL,
                    WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS,
                    0,0,0,0,hwndParentBorder,(HMENU)((INT_PTR)i),
            (HINSTANCE)GetWindowLongPtr(hwndParentBorder,GWLP_HINSTANCE),
                NULL);

            if(!hwndBorder[i])
            {
                MessageBox(hwndParentBorder,"Unable to create Border Child",
                           NULL,MB_ICONERROR);

                exit(EXIT_FAILURE);
            }

            switch(i)
            {
            case UP:
                hCursor=LoadCursor(NULL,IDC_SIZENS);
                break;
            case DOWN:
                hCursor=LoadCursor(NULL,IDC_SIZENS);
                break;
            case LEFT:
                hCursor=LoadCursor(NULL,IDC_SIZEWE);
                break;
            case RIGHT:
                hCursor=LoadCursor(NULL,IDC_SIZEWE);
                break;
            case COR_LU:
                hCursor=LoadCursor(NULL,IDC_SIZENWSE);
                break;
            case COR_LD:
                hCursor=LoadCursor(NULL,IDC_SIZENESW);
                break;
            case COR_RD:
                hCursor=LoadCursor(NULL,IDC_SIZENWSE);
                break;
            case COR_RU:
                hCursor=LoadCursor(NULL,IDC_SIZENESW);
                break;
            }

            //Charger les curseurs des differentes bordures
            SetClassLongPtr(hwndBorder[i],GCLP_HCURSOR,
                (LONG_PTR)hCursor);
        }

        return TRUE;

}

LRESULT CALLBACK BorderProc(HWND hwnd, UINT msg,WPARAM wParam, LPARAM lParam  )
{
    static BOOL fBlocking=FALSE;
    static RECT rcTarget;
    LONG cxTarget,cyTarget;
    static HWND hwndParent;
    POINT ptBeg,ptEnd;
    switch(msg)
    {
    case WM_LBUTTONDOWN:
        fBlocking=TRUE;
        SetCapture(hwnd);
        GetWindowRect(hwndToSize,&rcTarget);
        hwndParent=GetParent(hwnd);

        //conversions screen rctarget vers client rctarget
        ptBeg.x=rcTarget.left;
        ptBeg.y=rcTarget.top;
        ptEnd.x=rcTarget.right;
        ptEnd.y=rcTarget.bottom;
        ScreenToClient(hwndParent,&ptBeg);
        ScreenToClient(hwndParent,&ptEnd);
        rcTarget.left=ptBeg.x;
        rcTarget.top=ptBeg.y;
        rcTarget.right=ptEnd.x;
        rcTarget.bottom=ptEnd.y;


        return 0;

    case WM_LBUTTONUP:
        fBlocking=FALSE;
        ReleaseCapture();
        return 0;

    case WM_MOUSEMOVE:
        if(fBlocking)
        {
            //Client coordinnates in border window
            POINT posCursor={GET_X_LPARAM(lParam),
                                            GET_Y_LPARAM(lParam)};

            ClientToScreen(hwnd,&posCursor);
            ScreenToClient(hwndParent,&posCursor);
            switch(GetDlgCtrlID(hwnd))
            {
            case UP:
                rcTarget.top=posCursor.y;
                break;
            case DOWN:
                rcTarget.bottom=posCursor.y;
                break;
            case RIGHT:
                rcTarget.right=posCursor.x;
                break;
            case LEFT:
                rcTarget.left=posCursor.x;
                break;
            case COR_RU:
                rcTarget.right=posCursor.x;
                rcTarget.top=posCursor.y;
                break;
            case COR_RD:
                rcTarget.right=posCursor.x;
                rcTarget.bottom=posCursor.y;
                break;
            case COR_LU:
                rcTarget.left=posCursor.x;
                rcTarget.top=posCursor.y;
                break;
            case COR_LD:
                rcTarget.left=posCursor.x;
                rcTarget.bottom=posCursor.y;
                break;
            }

            cxTarget=rcTarget.right-rcTarget.left;
            cyTarget=rcTarget.bottom-rcTarget.top;
            MoveWindow(hwndToSize,rcTarget.left,rcTarget.top,
                       cxTarget,cyTarget,TRUE);

            Myborder(hwndToSize);

        }
        return 0;
    }
   return DefWindowProc(hwnd,msg,wParam,lParam);
}

BOOL Myborder(HWND hwndTarget)
{

    //Cette fonction applique les bordures a hwndTarget
    if(!hwndTarget ) return FALSE;

    HWND hwndParentTarget=GetParent(hwndTarget);
    if(!hwndParentTarget) return FALSE;

    if(GetParent(hwndBorder[0])!=hwndParentTarget)
    {
        MyLoadBorderOpt(hwndParentTarget);
        MyChangeParentBorder(hwndParentTarget);
    }
    hwndToSize=hwndTarget;

    RECT rcTarget;
    LONG width,height;
    POINT posBegBorder;


    MyMaskBorder(FALSE);
    //Screen cordinates
    GetWindowRect(hwndTarget,&rcTarget);

    MyScreenRcToClientRc(hwndParentTarget,&rcTarget);
    width=rcTarget.right-rcTarget.left;
    height=rcTarget.bottom-rcTarget.top;

    // Positionnement du hwndUp
    posBegBorder.y=rcTarget.top-BOR_H;
    posBegBorder.x=rcTarget.left;
    MoveWindow(hwndBorder[UP],posBegBorder.x,posBegBorder.y,
               width,BOR_H,TRUE);

    // Positionnement du COR_LU
    posBegBorder.y=posBegBorder.y;
    posBegBorder.x=posBegBorder.x-BOR_H;
    MoveWindow(hwndBorder[COR_LU],posBegBorder.x,posBegBorder.y,
               BOR_H,BOR_H,TRUE);

    // Positionnement du hwndDown
    posBegBorder.y=rcTarget.bottom;
    posBegBorder.x=rcTarget.left;
    MoveWindow(hwndBorder[DOWN],posBegBorder.x,posBegBorder.y,
               width,BOR_H,TRUE);

    // Positionnement du COR_LD
    posBegBorder.y=posBegBorder.y;
    posBegBorder.x=posBegBorder.x-BOR_H;
    MoveWindow(hwndBorder[COR_LD],posBegBorder.x,posBegBorder.y,
               BOR_H,BOR_H,TRUE);



    // Positionnement du hwndLeft
    posBegBorder.y=rcTarget.top;
    posBegBorder.x=rcTarget.left-BOR_H;
    MoveWindow(hwndBorder[LEFT],posBegBorder.x,posBegBorder.y,
               BOR_H,height,TRUE);


     // Positionnement du COR_RD
    posBegBorder.y=posBegBorder.y+height;
    posBegBorder.x=posBegBorder.x+BOR_H+width;
    MoveWindow(hwndBorder[COR_RD],posBegBorder.x,posBegBorder.y,
               BOR_H,BOR_H,TRUE);


    // Positionnement du hwndRight
    posBegBorder.y=rcTarget.top;
    posBegBorder.x=rcTarget.right;
    MoveWindow(hwndBorder[RIGHT],posBegBorder.x,posBegBorder.y,
               BOR_H,height,TRUE);

    // Positionnement du COR_RU
    posBegBorder.y=posBegBorder.y-BOR_H;
    posBegBorder.x=posBegBorder.x;
    MoveWindow(hwndBorder[COR_RU],posBegBorder.x,posBegBorder.y,
               BOR_H,BOR_H,TRUE);


    return TRUE;
}

BOOL MyMaskBorder(BOOL choice)
{
    //cette fonction permet de masquer les bordures presentes
    //sur lecran
    int i;
    int nCmdShow;

    if(choice==TRUE) nCmdShow=SW_HIDE;
    else nCmdShow=SW_NORMAL;

    for(i=0;i<8;i++)
    {
        ShowWindow(hwndBorder[i],nCmdShow);
        if(nCmdShow==SW_NORMAL)
            BringWindowToTop(hwndBorder[i]);

    }
    return TRUE;
}
BOOL MyChangeParentBorder(HWND hwndNewParentBorder)
{
    //Cette fonction change le parent des bordures, afin que
    //les objets qu'il encadrent le soit en correlation avec la nature de
    //leur parent
    int i;
    for(i=0;i<8;i++)
    {
        SetParent(hwndBorder[i],hwndNewParentBorder);
    }
    return TRUE;
}
