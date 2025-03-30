/*Name File:main.c
Rule:Entry point of gco.exe,and performs displaying of the
main window of gco*/

#include <windows.h>
#include <ctype.h>
#include "OpenDialogBox.h"
#include <time.h>
#include "resource.h"
#include "manageProjet.h"
#include "manageFont.h"
#include "toolBar.h"

#ifndef WS_EX_NOREDIRECTIONBITMAP
#define WS_EX_NOREDIRECTIONBITMAP 0x00200000
#endif


WNDCLASS myWc;//Variable used by func RegisterClass() for registering personalized classes

BOOL MyLoadBorderOpt(HWND hwnd);
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
BOOL CALLBACK AboutDlgProc (HWND, UINT, WPARAM, LPARAM) ;

BOOL MyRegisterOtherClass(WNDCLASS myWc);

//Global variables
int iActualIdForObjet;
UNIVERS *uObjet;
int maxObjectCreable;
HBRUSH hBrushGlobal;
BOOL isTypeModDlg=FALSE;//Define if a dialogBox is displayed on <<Modifying>>'s mode

BOOL isParentWnd=FALSE;//Define if a current obj to create is a mainWindow
int idObjToDelete;//Used when an obj will be deleted
HWND g_hToolTip;//Global handle for tooltips which decribe every obj, when mouse will be on
POSOBJ *po;//For save all the current positions of obj created in mainWindow
HWND g_hwndParentObj=NULL;//Global handle of the mainParent Window
HWND g_hwndPrevParentObj=NULL;//Last Global handle of MainParent before an new updating
HWND g_hwndParentObjToSet=NULL;//Global handle's parent of obj which need to be updated
HWND g_hwndLastChildGB=NULL;//Last Global handle of Dlg box set as child of a groupBox button(for ease moving :)
TCHAR* szClassZC=TEXT("ZC_CLASS");//Class Name of right'zone where obj will be created. ZC wh stands for zone creation of objects
TCHAR* szClassManage=TEXT("MANAGE_CLASS");//Class name of left's zone which will describe projet's tree
BOOL fShouldIdisplay=TRUE;//Not a good way, but permit to know if we should display a messageBox
POINT ptWhereCreate;//Generally the point in a mainWindow where user has clicked for creating obj
COUPLEHWND *g_tabHwnd;//Just a couple of last and New handles
HWND hwndZC,hwndManage;
HWND hwndMask;//Used in link options on displaying randomly colored windows on objects
MYCACHE_DLG mycache[NBOBJECT]={0};//Structure for saving datas concerning last modifications of objects
TCHAR szCurrentPath[MAX_PATH+1];
BOOL isProjetLoadedWithOpenItem=FALSE;
TCHAR g_szPathFolder[MAX_PATH];
MYFONT g_fontSys;
HTREEITEM g_htrProjects;
MYCACHE_TREEVIEW *g_mtv;//Save a descriptions of all projects atually opened
BOOL isFirstTime[NBOBJECT]={FALSE};
PSTR pszPathTV=NULL;
BOOL g_fShouldIAskSave=FALSE;
BOOL g_fIsProcessNewProject=FALSE;
HWND g_hwndTitleProj;//Windows which set the name of the current gco'files project activated
HWND g_hwndCloseProj;//Cross button for close projects
HWND g_hwndStaticManage;
HWND g_hwndToolBar;
BOOL g_fShouldApplyFontOnChilds;
int g_nbStyle;//number of styles checkables in dialog setting for objs
BOOL g_fisTestRunAble=FALSE;//definit si une app de test est actuellement cree
HWND g_hwndParentTest;//MainWindow handle on test(clicking Run button or F9)
STYLE_DESCRIPTION g_styleDescription[MAX_STYLES_DESC];
WNDPROC g_oldEditStyleDescProc;
BOOL g_IsDoubleClickedGcoFile;
BOOL g_fShowHowSize=TRUE;

TCHAR *szProjectAppExt=".gco";//namefile extension's.
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int nCmdShow)
{
    int a;int b;

    //Manage   Mutex
    // Create a  mutex for check if another instance of a same programm is actually running
    HANDLE hMutex = CreateMutex(NULL, TRUE, "MyAppMutex");

    if (hMutex == NULL)
    {
        MessageBox(NULL, "Unable to create mutex", "Error", MB_OK | MB_ICONERROR|MB_TASKMODAL);
        MyPerror("korgkpero");
        return -1;
    }

    // Check if a mutex has already been created
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        //if another instance is actually running
        //Get This windows and setForeground

        HWND hwnd=FindWindow("GCO","GCO");
        if(!hwnd)
        {
            perror("dlkflgd");
            MyMsgError();
            exit(EXIT_FAILURE);
        }
        ShowWindow(hwnd,SW_MAXIMIZE);
        SetForegroundWindow(hwnd);
        SetFocus(hwnd);

        CloseHandle(hMutex); // Free  mutex

        //Check if a GCO's file has been clicked
        //Inform an <already existing>'s instance that a gco File has been
        //clicked an that he needs to treat it.
        TCHAR *c;
        if(szCmdLine && _tcslen(szCmdLine)>0)
        {

            //Check if has doubleClicked a gco's File
            if(!MyIsThisExtFile(szCmdLine,szProjectAppExt))
            {
                //if user has'nt doubleclicked  gco's file
                char buf[1024]="";

                sprintf(buf,"Unable to Load <%s> on this app. Unknown extension!!!",szCmdLine);
                MessageBox(hwnd,buf,"INFORMATION",MB_ICONINFORMATION|MB_OK|MB_TASKMODAL);
                return 0;
            }
            MyInformRunningInstanceOF(szCmdLine);//OF=OPEN FILE
        }

        return 0; // Get out without launching.Cause there's already an instance launched
    }


    //Code for app  Launched begins here
    MyLoadG_Paths();
    srand(time(NULL));
    maxObjectCreable=MIN_OBJ_CREABLE;
    iActualIdForObjet=100;

    //Allocation of uObjet, po,mycache an initialisations
    uObjet=malloc(sizeof(UNIVERS)*maxObjectCreable);
    po=malloc(sizeof(POSOBJ)*maxObjectCreable);
    if(!uObjet || !po)
    {
        MessageBox(NULL,TEXT("Error when allocating uObjet in main.c"),NULL,
                   MB_ICONERROR);
        exit(EXIT_FAILURE);
    }
    memset(uObjet,0,sizeof(UNIVERS)*maxObjectCreable);
    memset(po,0,sizeof(POSOBJ)*maxObjectCreable);
    memset(mycache,0,sizeof(MYCACHE_DLG)*NBOBJECT);
    mycache[DLG_DIALOGBOX].menu=NULL;

    //g_mtv
    g_mtv=malloc(sizeof(MYCACHE_TREEVIEW)*NB_MIN_PROJ_TV);
    if(!g_mtv)
    {
        MyPerror("m dsoif");
        return -1;
    }
    memset(g_mtv,0,sizeof(MYCACHE_TREEVIEW)*NB_MIN_PROJ_TV);
    for(int i=0;i<NBOBJECT;i++)
        isFirstTime[i]=TRUE;

    //Initialise  datas  of styleDescription
    InitStyleDesc();

    TCHAR *szAppName=TEXT("GCO");
    MSG msg;
    HWND hwnd;
    WNDCLASS wc;
    HBRUSH hBrushBk;

    hBrushBk=CreateSolidBrush(GetSysColor(4));//4=COLOR_DLG
    //Creation d'une classe de fenetre
    wc.cbClsExtra=0;
    wc.cbWndExtra=0;
    wc.hbrBackground=hBrushBk;
    wc.hCursor=LoadCursor(NULL,IDC_ARROW);
    wc.hIcon=LoadIcon(hInstance,szAppName);
    wc.hInstance=hInstance;
    wc.lpszClassName=szAppName;
    wc.lpszMenuName=NULL;
    wc.style=CS_VREDRAW|CS_HREDRAW;
    wc.lpfnWndProc=WndProc;

    if(!RegisterClass(&wc))
    {
        MessageBox(NULL,TEXT("Error in Registering wc"),
                   NULL,MB_ICONERROR);

        exit(EXIT_FAILURE);
    }

    //Registering szClassZC
    wc.lpfnWndProc=ZcProc;
    wc.lpszClassName=szClassZC;
    wc.hbrBackground=(HBRUSH)GetStockObject(GRAY_BRUSH);

    if(!RegisterClass(&wc))
    {
        MyPerror("Error in registering wc");
        exit(EXIT_FAILURE);
    }

    //Registering szClassMange
    wc.lpfnWndProc=ManageProc;
    wc.lpszClassName=szClassManage;
    wc.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
    if(!RegisterClass(&wc))
    {
        MyPerror("Error in registering wc");
        exit(EXIT_FAILURE);
    }

    myWc=wc;
    //Save  another personalized classes
    MyRegisterOtherClass(myWc);

    hwnd=CreateWindow(szAppName,TEXT("GCO"),
            WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,
            CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
            NULL,NULL,hInstance,NULL);


    ShowWindow(hwnd,SW_MAXIMIZE);

    //Manage a doubleClicking GCO'File
    if(szCmdLine && _tcslen(szCmdLine)>0)
    {
        //Check if has doubleClicked a gco's File
        if(!MyIsThisExtFile(szCmdLine,szProjectAppExt))
        {
            //if user has'nt doubleclicked  gco's file
            char buf[1024]="";

            sprintf(buf,"Unable to Load <%s> on this app. Unknown extension!!!",szCmdLine);
            MessageBox(hwnd,buf,"INFORMATION",MB_ICONINFORMATION|MB_OK|MB_TASKMODAL);
            exit(EXIT_FAILURE);
        }
        //Just Fill projet.desc informations and send IDM_OPEN
        //To window
        TCHAR p[1024];
        _tcscpy(p,szCmdLine);

        //REMOVE   ""
        RemoveQuotes(p);

        //Get nameFile, path an other stuff, knowing a path's file doubleClicked
        if(!MyGetProjetDescWithPathFile(p,&projet.descProjet))
        {
            MyPerror("vmjeoe");
            MyMsgError();
            exit(EXIT_FAILURE);
        }
        //Informe we are in mode=DoubleClickedGcoFile
        g_IsDoubleClickedGcoFile=TRUE;

        //Now order to open file
        SendMessage(hwnd,WM_COMMAND,MAKELONG(IDM_OPEN,BN_CLICKED),
            0);
    }

    //Main Loop
    while(GetMessage(&msg,NULL,0,0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    DeleteObject(hBrushGlobal);
    DeleteObject(hBrushBk);

    MySaveG_Paths();

    //Free zone
    if(!MyFreeuObjet(&uObjet))
    {
        MyPerror("m podiog");
        return -1;
    }
    if(!MyFreeMenuTab(&mycache[DLG_DIALOGBOX].menu))
    {
        MyPerror(",adsa.sd");
        return -1;
    }
    free(g_tabHwnd);
    free(g_mtv);
    g_mtv=NULL;
    CloseHandle(hMutex);

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message,WPARAM wParam, LPARAM lParam  )
{

    static int cxClient,cyClient;
    int x,y,w,h;
    static HMENU hMenu;
    static HMENU hMenuFile;
    static HMENU hMenuHelp;
    static HMENU   hMenuSubTv;
    HTREEITEM htr;

    switch(message)
    {
    case WM_CREATE:
        g_hwndToolBar=CreateToolBar(hwnd);
        if(!g_hwndToolBar)
        {
            MyPerror(",voerkkie");
            MyMsgError();
            exit(EXIT_FAILURE);
        }
        hwndZC=CreateWindow(szClassZC,NULL,WS_OVERLAPPED|WS_CHILD|
                            WS_VSCROLL|WS_HSCROLL|WS_VISIBLE|
                            WS_BORDER|WS_DISABLED,
                            0,0,0,0,hwnd,(HMENU)ID_ZC,
                            ((LPCREATESTRUCT)lParam)->hInstance,NULL);
        if(!hwndZC)
        {
            MessageBox(hwnd,TEXT("Unable to Create hwndZC"),
                       NULL,MB_ICONERROR);
            exit(EXIT_FAILURE);
        }
        g_hwndParentObj=hwndZC;

        g_hwndTitleProj=CreateWindow("static","No project",WS_CHILD|
                                    WS_VISIBLE|WS_BORDER|SS_LEFT,
                                    0,0,0,0,hwnd,(HMENU)IDS_TITLE_PROJ,
                                    ((LPCREATESTRUCT)lParam)->hInstance,NULL);
        if(!g_hwndTitleProj)
        {
            MessageBox(hwnd,TEXT("Unable to Create_hwndTitleProj"),
                       NULL,MB_ICONERROR);
            exit(EXIT_FAILURE);
        }

        g_hwndCloseProj=CreateWindow("button","X",WS_CHILD|
                            WS_VISIBLE|
                            WS_BORDER|WS_DISABLED,
                            0,0,0,0,hwnd,(HMENU)IDB_CLOSE_PROJ,
                            ((LPCREATESTRUCT)lParam)->hInstance,NULL);
        if(!g_hwndCloseProj)
        {
            MessageBox(hwnd,TEXT("Unable to Create g_hwndCloseProj"),
                       NULL,MB_ICONERROR);
            exit(EXIT_FAILURE);
        }

        g_hwndStaticManage=CreateWindow("static","Management",WS_CHILD|
                            WS_VISIBLE|SS_LEFT|
                            WS_BORDER,
                            0,0,0,0,hwnd,(HMENU)IDS_MANAGE,
                            ((LPCREATESTRUCT)lParam)->hInstance,NULL);
        if(!g_hwndStaticManage)
        {
            MessageBox(hwnd,TEXT("Unable to Create g_hwndStaticManage"),
                       NULL,MB_ICONERROR);
            exit(EXIT_FAILURE);
        }

        //Scrolling initialisations
        SetScrollRange(hwndZC, SB_VERT, 0, MAX_VSCROLL_RANGE, FALSE);
        SetScrollRange(hwndZC, SB_HORZ, 0, MAX_HSCROLL_RANGE, FALSE);
        SetScrollPos(hwndZC, SB_VERT, 0, TRUE);
        SetScrollPos(hwndZC, SB_HORZ, 0, TRUE);

        //Tree view Section(On manage project
        INITCOMMONCONTROLSEX icex;
        icex.dwSize=sizeof(INITCOMMONCONTROLSEX);
        icex.dwICC=ICC_TREEVIEW_CLASSES;
        InitCommonControlsEx(&icex);

        hwndManage=CreateWindow(WC_TREEVIEW,TEXT("Tree view"),WS_OVERLAPPED|WS_CHILD|
                                WS_VSCROLL|WS_HSCROLL|WS_VISIBLE|TVS_SHOWSELALWAYS|
                                WS_BORDER|TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS,
                            0,0,0,0,hwnd,(HMENU)ID_MANAGE,
                            ((LPCREATESTRUCT)lParam)->hInstance,NULL);

        if(!hwndManage)
        {
            MessageBox(hwnd,TEXT("Unable to Create hwndManage"),
                       NULL,MB_ICONERROR);
            exit(EXIT_FAILURE);
        }

        g_htrProjects=AddTreeItem(hwndManage,TVI_ROOT,TEXT("PROJECTS"));
        if(!g_htrProjects)
        {
            MyPerror("m dogofgi");
        }

        //Set a system font on hwnd
       PopFontInitialize(hwnd,&g_fontSys.hFont,&g_fontSys.logfont);
        g_fontSys.isFontParent=-1;//-1 means nothing

        //Initialize font's data of caches of objects on system font
        //NBOBJECT a number of differents families(button, edit, listbox etc) that GCO can perform
        for(int i=0;i<NBOBJECT;i++)
        {
            PopFontInitialize(hwnd,&mycache[i].font.hFont,
                              &mycache[i].font.logfont);
            mycache[i].font.isFontParent=-1;
        }

        //MainMenu section
        {
            //Submenu
        hMenuSubTv = CreatePopupMenu();
        //MyLoadMenuTv(&hMenuSubTv);After
        AppendMenu(hMenuSubTv, MF_STRING,
            IDM_ACTIVATE_PROJ, "&Activate Projet");
        AppendMenu(hMenuSubTv, MF_SEPARATOR,
            0,NULL);
        AppendMenu(hMenuSubTv, MF_STRING,
            IDM_SAVE_TV, "&Save");
        AppendMenu(hMenuSubTv, MF_STRING,
            IDM_SAVEAS_TV, "Save &As");
        AppendMenu(hMenuSubTv, MF_SEPARATOR,
            0,NULL);
        AppendMenu(hMenuSubTv, MF_STRING,
            IDM_SET_DEF_OUTPUT_TV, "Set output of define's file");
        AppendMenu(hMenuSubTv, MF_SEPARATOR,
            0,NULL);

        AppendMenu(hMenuSubTv, MF_STRING,
            IDM_CLOSE_PROJECT_TV, "Close &Projet");
        AppendMenu(hMenuSubTv, MF_SEPARATOR,
            0,NULL);

            //menu
        hMenu = CreateMenu();
        hMenuFile = CreatePopupMenu();
        hMenuHelp = CreatePopupMenu();

        AppendMenu(hMenuFile, MF_STRING, IDM_NEW, "&New project");
        AppendMenu(hMenuFile, MF_SEPARATOR, 0, NULL);

        AppendMenu(hMenuFile, MF_STRING, IDM_OPEN, "&Open");
        AppendMenu(hMenuFile, MF_SEPARATOR, 0,NULL);

        AppendMenu(hMenuFile, MF_STRING|MF_GRAYED, IDM_SAVE, "&Save");
        AppendMenu(hMenuFile, MF_STRING|MF_GRAYED, IDM_SAVEAS, "Save &As");
        AppendMenu(hMenuFile, MF_SEPARATOR, 0,NULL);

        AppendMenu(hMenuFile,
                   MF_STRING|MF_GRAYED, IDM_SET_DEF_OUTPUT,
                   "Set Output of define's file");
        AppendMenu(hMenuFile, MF_SEPARATOR, 0,NULL);

        AppendMenu(hMenuFile, MF_STRING|MF_GRAYED, IDM_CLOSE_PROJECT, "Close &Projet");
        AppendMenu(hMenuFile, MF_SEPARATOR, 0,NULL);

        //Help
        AppendMenu(hMenuHelp, MF_STRING, IDM_ABOUT, "Abou&t");
        AppendMenu(hMenuHelp, MF_STRING, IDM_TUTORIAL, "&How To Use GCO?");

        // Add submenus on main menu
        AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hMenuFile, "&File");
        AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hMenuHelp, "&Help");

        // Associate main menu to hwnd
        SetMenu(hwnd,hMenu);

        }

        PopFileInitialize(hwnd);
        return 0;

    case WM_SIZE:
       {

        //Inform toolbars
        SendMessage(g_hwndToolBar,TB_AUTOSIZE,0,0);
        RECT rc;
        GetWindowRect(g_hwndToolBar,&rc);
        int heightTB=rc.bottom-rc.top;
        cxClient=LOWORD(lParam);
        cyClient=HIWORD(lParam)-5;

        //Display hwndZC
        x=21*cxClient/100;
        y=5*HIWORD(GetDialogBaseUnits())/4+2;
        y+=heightTB;
        w=cxClient-x;
        h=cyClient-y;

        MoveWindow(hwndZC,x,y,w,h,TRUE);

        //Display g_hwndTitleProj
        x=21*cxClient/100;
        y=0;
        y+=heightTB;
        w=w-15*(LOWORD(GetDialogBaseUnits()))/4;
        h=5*(HIWORD(GetDialogBaseUnits()))/4;
        MoveWindow(g_hwndTitleProj,x,y,w,h,TRUE);
        PopFontSetFont(g_hwndTitleProj,&g_fontSys.hFont,0);
        //Display g_hwndClose
        x=x+w+1;
        y=y;
        w=3*(LOWORD(GetDialogBaseUnits()));
        h=5*(HIWORD(GetDialogBaseUnits()))/4;
        MoveWindow(g_hwndCloseProj,x,y,w,h,TRUE);

        //Display hwndManage
        x=0;
        y=5*HIWORD(GetDialogBaseUnits())/4 +2;
        y+=heightTB;
        w=20*cxClient/100;
        h=cyClient-y;
        MoveWindow(hwndManage,x,y,w,h,TRUE);


        //Display g_hwndStaticManage
        x=0;
        y=0;
        y+=heightTB;
        w=w;
        h=5*(HIWORD(GetDialogBaseUnits))/4;
        MoveWindow(g_hwndStaticManage,x,y,w,h,TRUE);
        PopFontSetFont(g_hwndStaticManage,&g_fontSys.hFont,NULL);
       }
        return 0;

    case WM_INITMENUPOPUP:
    {
        UINT uFlag=(g_mtv[0].nbProjects>0 )?
                MF_ENABLED:MF_GRAYED;
        EnableMenuItem(hMenu,IDM_SET_DEF_OUTPUT,uFlag);

        uFlag=(g_mtv[0].nbProjects>0 && g_fisTestRunAble)?
                MF_ENABLED:MF_GRAYED;
        EnableMenuItem(hMenu,IDM_SAVE,uFlag);
        EnableMenuItem(hMenu,IDM_SAVEAS,uFlag);
        EnableMenuItem(hMenu,IDM_CLOSE_PROJECT,uFlag);
    }
        break;

    case WM_COPYDATA:
    {
        //Received when user has doubleclicked a gco's File outside
        //of the program

        COPYDATASTRUCT *cds = (COPYDATASTRUCT*)lParam;
        if (cds->dwData == PATH_OF_GCO_FILES)
        {
            //It mean loading a .gco's file
            char *szCmdLine=(char*)cds->lpData;
            if(szCmdLine && _tcslen(szCmdLine)>0)
            {

                if(!MyIsThisExtFile(szCmdLine,szProjectAppExt))
                {
                    //if user has'nt doubleclicked  gco's file
                    char buf[1024]="";

                    sprintf(buf,"Unable to Load <%s> on this app. Unknown extension!!!",szCmdLine);
                    MessageBox(hwnd,buf,"INFORMATION",MB_ICONINFORMATION|MB_OK|MB_TASKMODAL);
                    return 0;
                }
                TCHAR p[1024];
                _tcscpy(p,szCmdLine);

                RemoveQuotes(p);

                //Save ACTUAL activated's project
                if(uObjet[0].nbObjCreated>0)
                {
                    MyUpdateDimObjUniv();
                    projet.puObjet=uObjet;
                    MySaveProjet();
                }

                if(!MyGetProjetDescWithPathFile(p,&projet.descProjet))
                {
                    MyPerror("vmjeoe");
                    MyMsgError();
                    exit(EXIT_FAILURE);
                }

                g_IsDoubleClickedGcoFile=TRUE;
                SendMessage(hwnd,WM_COMMAND,MAKELONG(IDM_OPEN,BN_CLICKED),
                    0);
            }
        }
    }
        return 0;

    case WM_COMMAND:

        if(LOWORD(wParam==IDB_CLOSE_PROJ)&&
           HIWORD(wParam)==BN_CLICKED)
        {
            SendMessage(hwnd,WM_COMMAND,
                        IDM_CLOSE_PROJECT,0);
            return 0;
        }

        switch(wParam)
        {
        case IDM_ABOUT :
            //Creation et affichage le la boite de dialogue
            DialogBox ((HINSTANCE)GetWindowLongPtr(hwndZC,GWLP_HINSTANCE),
                TEXT ("AboutBox"), hwnd,(DLGPROC) AboutDlgProc) ;
            return 0 ;

        case IDM_TUTORIAL:
            MyDispLayTutorial();
            return 0;

        case ID_STOP_TOOL:
            if(g_fisTestRunAble)//if there is not already testing's instance
                MyTestProjet();
            else
            {
                //Then user wants to close an actual testing's instance
                if(!g_hwndParentTest || !IsWindow(g_hwndParentTest))
               {
                   MyPerror("\ndsgoi");
                   MyMsgError();
                   return 0;
               }
                SendMessage(g_hwndParentTest,WM_CLOSE,SC_CLOSE,0);
            }
            return 0;

        case IDM_SET_DEF_OUTPUT:
            //It's send when user want to specify a folder where create GCODEF.h
            if(!MyManageDefOutPut(projet.szPathDefFile))
            {
                MyPerror("kgokoeer");
                MyMsgError();
                exit(EXIT_FAILURE);
            }
            return 0;

        case IDM_NEW:
            if(HIWORD(wParam)!=BN_CLICKED)
                return 0;

            //User wants to create a new project

            if(!MyInitCacheDlg())
            {
                MyPerror("m oioi");
                MyMsgError();
                exit(EXIT_FAILURE);
            }

            //Save last activated's project
            if(uObjet[0].nbObjCreated>0)
                SendMessage(GetParent(hwndZC),WM_COMMAND,IDM_SAVE,0);

            //Display Dlg where user specify a path where create new project
            if(!MyCreateNewProjet())
                return TRUE;

            //Check if a "new project comming"  is already on project tree
            for(int i=0;i<g_mtv[0].nbProjects;i++)
            {
                if(!strcmp(g_mtv[i].desc.szFilePath,projet.descProjet.szFilePath))
                {
                    //If a file path specified by user is already created
                    //Delete this projet on project 's tree view
                    MyDelTreeItem(i);
                }
            }

            //Add a new project on tree view
            htr= AddTreeItem(hwndManage,g_htrProjects,projet.descProjet.szNameFolder);
            if(htr)
            {
                HTREEITEM hChild=AddTreeItem(hwndManage,htr,projet.descProjet.szNameFile);
                if(hChild)
                {
                    TreeView_EnsureVisible(hwndManage,hChild);
                    MyAddTreeItem(hChild,projet.descProjet);
                    g_fIsProcessNewProject=TRUE;
                    projet.szPathDefFile[0]='\0';//reinitialisation
                    //Activate this project, and making it visible
                    if(!MyActivateProjet(hChild))
                    {
                        MyPerror("whfgfd");
                        MyMsgError();
                        exit(EXIT_FAILURE);
                    }
                }
            }
            g_fIsProcessNewProject=FALSE;
            isProjetLoadedWithOpenItem=FALSE;
            g_fShouldIAskSave=FALSE;
            SetWindowText(g_hwndTitleProj,projet.descProjet.szFilePath);
            EnableWindow(g_hwndCloseProj,TRUE);


            EnableWindow(hwndZC,TRUE);
            g_hwndPrevParentObj=NULL;
            g_hwndParentObj=NULL;

            MyEnableButtonToolBar(g_hwndToolBar,ID_STOP_TOOL,TRUE,TRUE);

            isTypeModDlg=FALSE;
            isParentWnd=TRUE;//important
            OpenDialogBox(MODAL,DLG_DIALOGBOX,
            hwndZC,(HINSTANCE)GetWindowLongPtr(hwndZC,GWLP_HINSTANCE),
                NULL,0);
            isParentWnd=FALSE;//renitialisation

            //If used hadn't decided to cancel , we should have one
            //object created which is a mainParent.
            //If, not, then user has surely decided to cancel,So
            //We will close this project
            if(uObjet[0].nbObjCreated==0)
            {
                SendMessage(hwnd,WM_COMMAND,
                    MAKELONG(IDM_CLOSE_PROJECT,0),0);
                return 0;
            }

            //Dispaly how to size object
            if(g_fShowHowSize)
            {
                DLGPARAMINFO dlgInfo;
                dlgInfo.hwndParentDlg=hwnd;
                dlgInfo.idDlg=IDD_HOW_TO_RESIZE;

                DialogBoxParam((HINSTANCE)GetWindowLongPtr(hwnd,GWLP_HINSTANCE),
                    MAKEINTRESOURCE(IDD_HOW_TO_RESIZE),hwnd,(DLGPROC)DlgNotifProc,
                    (LPARAM)&dlgInfo);
            }
            return  0;

        case IDM_SAVE:
            if(HIWORD(wParam)!=BN_CLICKED)
                return 0;

            MyUpdateDimObjUniv();
            projet.puObjet=uObjet;
            MySaveProjet();
           return 0;

        case IDM_SAVEAS:
            if(HIWORD(wParam)!=BN_CLICKED)
                return 0;

            MyUpdateDimObjUniv();
            projet.puObjet=uObjet;
            MySaveAsProjet(hwnd);
            return 0;

        case IDM_OPEN:
            if(HIWORD(wParam)!=BN_CLICKED)
                return 0;

            MyManageOpenGCOfiles(hwnd);
            //Open dialog for set mainWindow if a .gco 's file
            //IF OPEN's file opened is empty
            if(g_mtv[0].nbProjects>0 && uObjet[0].nbObjCreated==0)
            {
                isTypeModDlg=FALSE;
                isParentWnd=TRUE;
                OpenDialogBox(MODAL,DLG_DIALOGBOX,
                    hwndZC,(HINSTANCE)GetWindowLongPtr(hwndZC,GWLP_HINSTANCE),
                    NULL,0);
                isParentWnd=FALSE;//renitialisation


                if(uObjet[0].nbObjCreated==0)
                {
                    SendMessage(hwnd,WM_COMMAND,
                    MAKELONG(IDM_CLOSE_PROJECT,0),0);
                }
            }
            return 0;

        case IDM_CLOSE_PROJECT:

            SendMessage((!g_hwndParentObj)?hwndZC:g_hwndParentObj,WM_CLOSE,SC_CLOSE,0);
            if(g_mtv[0].nbProjects==0)
            {
                SetWindowText(g_hwndTitleProj,"No project");
                EnableWindow(g_hwndCloseProj,FALSE);
                MyEnableButtonToolBar(g_hwndToolBar,ID_STOP_TOOL,TRUE,TRUE);
            }

            return 0;

        case IDM_CLOSE_PROJECT_TV:
            if(!pszPathTV)
            {
                MyPerror("m pgifig");
                MyMsgError();
                exit(EXIT_FAILURE);
            }
            //Delete  project on tv
            if(!strcmp(pszPathTV,projet.descProjet.szFilePath))
            {
                //if a project to del is also a activated's project
                SendMessage(hwnd,WM_COMMAND,(WPARAM)IDM_CLOSE_PROJECT,0);
            }
            else
            {
                //If it's not also a activated's project
                //just delete it to treew view
                for(int i=0;i<g_mtv[0].nbProjects;i++)
                {
                    if(!strcmp(g_mtv[i].desc.szFilePath,pszPathTV))
                    {
                        MyDelTreeItem(i);
                        pszPathTV=NULL;
                        break;
                    }
                }

              }
            return 0;

        case IDM_SAVE_TV:
            if(!pszPathTV)
            {
                MyPerror(",gfogpo");
                MyMsgError();
                exit(EXIT_FAILURE);
            }
            if(!strcmp(pszPathTV,projet.descProjet.szFilePath))
            {
                //if a project to save is also a activated's project
                SendMessage(hwnd,WM_COMMAND,(WPARAM)IDM_SAVE,0);
            }
            else
            {
                //if not
                //We will manage it later by using temporarly saving
                //For now, all the project opened an non activated
                //Are automatically saved.
            }

            return 0;

        case IDM_SAVEAS_TV:
            if(!pszPathTV)
            {
                MyPerror("saoioewp");
                MyMsgError();
                exit(EXIT_FAILURE);
            }
            if(!strcmp(pszPathTV,projet.descProjet.szFilePath))
            {
                //if a project to save is also a activate's project
                SendMessage(hwnd,WM_COMMAND,(WPARAM)IDM_SAVEAS,0);
            }
            else
            {
                //if not, we will manage this later
                //For now, all the project opened an non activated
                //Are automatically saved.
            }

            return 0;

        case IDM_ACTIVATE_PROJ:
            {
                //If user wants to activates an open project visible on tree view
                SendMessage(GetParent(hwndZC),
                WM_COMMAND,IDM_SAVE,0);

                for(int i=0;i<g_mtv[0].nbProjects;i++)
                {
                    if(!strcmp(pszPathTV,g_mtv[i].desc.szFilePath))
                    {
                        if(!MyActivateProjet(g_mtv[i].hItem))
                        {
                            MyPerror("gfioigofgi");
                            MyMsgError();
                            break;
                        }
                    }
                }

            }
            return 0;//fin case IDM_ACTIVATE_PROJ

        }
        break;//Fin case WM_COMMAND

    case WM_CLOSE:
        {
            if(!uObjet[0].nbObjCreated)
                break;

            int result;
            if(g_fShouldIAskSave)
            {
                result=MessageBox(hwnd,TEXT("Do you want to  save this project?"),
                TEXT("Question"),MB_ICONQUESTION|MB_YESNOCANCEL|MB_TASKMODAL);
            }
            else
                result=IDNO;

            if(result==IDYES)
            {
                //Save 's procedure
                SendMessage(GetParent(hwndZC),WM_COMMAND,
                IDM_SAVE,0);

                //Delete datas an obj of uObjet
                MyDeleteObj(-1);//-1 means all

                //Gray hwndZC
                MyEnableButtonToolBar(g_hwndToolBar,IDM_SAVE,TRUE,TRUE);
                EnableWindow(hwndZC,FALSE);
                EnableMenuItem(GetMenu(GetParent(hwndZC)),
                                IDM_CLOSE_PROJECT,MF_GRAYED);
                EnableMenuItem(GetMenu(GetParent(hwndZC)),
                                       IDM_SAVE,MF_GRAYED);
                EnableMenuItem(GetMenu(GetParent(hwndZC)),
                                       IDM_SAVEAS,MF_GRAYED);
                EnableMenuItem(GetMenu(GetParent(hwndZC)),
                                       IDM_SET_DEF_OUTPUT,MF_GRAYED);

                break;
            }
            else if(result==IDNO)
            {
                MyDeleteObj(-1);
                EnableWindow(hwndZC,FALSE);
                break;
            }
            else return 0;
        }
        break;//unreachable

    case WM_NOTIFY:
        {
            LPNMHDR lpnmhdr=(LPNMHDR)lParam;
            if(lpnmhdr->idFrom==ID_MANAGE)
            {
                //Manage project's tree view interactions with mouse
                switch(lpnmhdr->code)
                {
                case NM_DBLCLK://on double click
                    {
                        //user want ti activate a project which has been doubleclicked
                        TVHITTESTINFO ht={0};
                        GetCursorPos(&ht.pt);
                        ScreenToClient(hwndManage,&ht.pt);
                        if(TreeView_HitTest(hwndManage,&ht) &&(ht.flags&TVHT_ONITEM))
                        {
                            HTREEITEM hItem=ht.hItem;
                            for(int i=0;i<g_mtv[0].nbProjects;i++)
                            {
                                if(g_mtv[i].hItem==hItem && (strcmp(projet.descProjet.szFilePath,
                                g_mtv[i].desc.szFilePath)))
                                {
                                    //Note:Later, save of actual activated project wont be on a temporarly zone.
                                    //For now ,we are forcing saving, even if user dont want it.We will change it as soon
                                    SendMessage(GetParent(hwndZC),
                                        WM_COMMAND,IDM_SAVE,0);

                                    //Then activate project which has been doubleClicked
                                    //Simulate as if it was a openFile
                                    isProjetLoadedWithOpenItem=TRUE;
                                    MyActivateProjet(hItem);
                                    return 0;
                                }
                            }
                        }
                    }
                    break;//Fin case NM_DBCLK

                case NM_RCLICK://On righ click
                    {
                        //user wants to see a popupmenu and make a choice
                        TVHITTESTINFO ht={0};
                        GetCursorPos(&ht.pt);
                        ScreenToClient(hwndManage,&ht.pt);
                        if(TreeView_HitTest(hwndManage,&ht) &&(ht.flags&TVHT_ONITEM))
                        {
                            HTREEITEM hItem=ht.hItem;
                            POINT pt;
                            for(int i=0;i<g_mtv[0].nbProjects;i++)
                            {
                                if(g_mtv[i].hItem==hItem)
                                {
                                    GetCursorPos(&pt);
                                    EnableMenuItem(hMenuSubTv,IDM_ACTIVATE_PROJ,MF_GRAYED);

                                    pszPathTV=g_mtv[i].desc.szFilePath;
                                    if(!strcmp(pszPathTV,projet.descProjet.szFilePath))
                                        EnableMenuItem(hMenuSubTv,IDM_ACTIVATE_PROJ,MF_GRAYED);
                                    else
                                        EnableMenuItem(hMenuSubTv,IDM_ACTIVATE_PROJ,MF_ENABLED);

                                    if(!g_fisTestRunAble && g_hwndParentObj)
                                    {
                                        //We have decided to avoid user to open popupmenu if
                                        //an testing instance is running.
                                        //Then we need to gray options like save, open etc
                                        EnableMenuItem(hMenuSubTv,
                                            IDM_CLOSE_PROJECT_TV,MF_GRAYED);
                                        EnableMenuItem(hMenuSubTv,
                                            IDM_SAVE_TV,MF_GRAYED);
                                        EnableMenuItem(hMenuSubTv,
                                            IDM_SAVEAS_TV,MF_GRAYED);
                                    }
                                    else
                                    {
                                        //A popup will be correctly display without restrictions
                                        //when there is no testing's running instance

                                        EnableMenuItem(hMenuSubTv,
                                            IDM_CLOSE_PROJECT_TV,MF_ENABLED);
                                        EnableMenuItem(hMenuSubTv,
                                            IDM_SAVE_TV,MF_ENABLED);
                                        EnableMenuItem(hMenuSubTv,
                                            IDM_SAVEAS_TV,MF_ENABLED);
                                        }

                                    //No matter a state of running instance, IDM_SET_DEF_OUTPUT
                                    //will still be enabled
                                    EnableMenuItem(hMenuSubTv,
                                            IDM_SET_DEF_OUTPUT_TV,MF_ENABLED);

                                    //Then display popup menu with uptading on state(GRAY or ENABLEd)
                                    //done before
                                    TrackPopupMenu(hMenuSubTv,TPM_RIGHTBUTTON,
                                    pt.x,pt.y,0,hwnd,NULL);

                                    return 0;
                                }
                            }

                        }
                    }
                    break;//fin case NM_RCLICK
                }
            }

        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }//fin switch(message)

    return DefWindowProc(hwnd,message,wParam,lParam);
}//fin LRESULT CALLBACK WndProc()

LRESULT CALLBACK ZcProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

    //This procedure is for A ZC and a main parent obj of all
    //the other objects created on him.
    if(!g_hwndParentObj || hwnd==g_hwndParentObj)
    {
        //Here we are working with mainParent of all objs
        //Not a ZC
        static HMENU hmenu;
        static HINSTANCE hinst;
        POINT pt;

        int i,idDlg=0,idObj;
        PSTR pszClassName=NULL;




        switch(msg)
        {
        case WM_ENABLE:
            if(wParam)
            {//Window is enabled
                //Change background
                DeleteObject((HBRUSH)SetClassLongPtr(hwnd,GCLP_HBRBACKGROUND,
                                         (LONG_PTR)GetStockObject(WHITE_BRUSH)));
                InvalidateRect(hwnd,NULL,TRUE);
            }
            else
            {//Window is disabled
                DeleteObject((HBRUSH)SetClassLongPtr(hwnd,GCLP_HBRBACKGROUND,
                                         (LONG_PTR)GetStockObject(GRAY_BRUSH)));
                InvalidateRect(hwnd,NULL,TRUE);
            }
            break;

        case WM_CREATE:
            //Get hinst
            //Load menu, and Get the first submenu positionned a 0
            hinst=((LPCREATESTRUCT)lParam)->hInstance;

            hmenu=LoadMenu(hinst,TEXT("GCO"));
            hmenu=GetSubMenu(hmenu,0);


            //Load resizing options
            MyLoadBorderOpt(hwnd);
            DestroyWindow(g_hToolTip);
             // Recreate g_hToolTip
            g_hToolTip = CreateWindowEx(WS_EX_TOPMOST,
            TOOLTIPS_CLASS, NULL,WS_POPUP | TTS_NOPREFIX |
             TTS_BALLOON,CW_USEDEFAULT, CW_USEDEFAULT,
            CW_USEDEFAULT, CW_USEDEFAULT,hwnd, NULL,
             NULL, NULL);

            MYFONT fontParent=mycache[DLG_DIALOGBOX].font;
            int cxParent=mycache[DLG_DIALOGBOX].cxChar;
            int cyParent=mycache[DLG_DIALOGBOX].cyChar;

            //A WM_CREATE Can be recevied many times here
            //Sometimes, a g_hwndParent will already have a childs
            //an if user modify him, we will recreate it with the new
            //updates(size,color,styles, menus etc). in this case,
            //we will need to create all his chidls before finishing his own
            //WM_CREATE.

            //This section is applied when g_hwndParentObj has already
            //childs
            int nbObj=uObjet[0].nbObjCreated;
            for(i=0;i<nbObj;i++)
            {
                if(uObjet[i].hwndParent==g_hwndPrevParentObj)//prev cause, the new has change, but uObjet has still hwnd of lastParent
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
                         g_hwndParentObjToSet=hwnd;
                        MySetLastParentObj(uObjet[i].hwnd,NULL);
                        break;

                    case OBJ_IMGSTATIC:
                        pszClassName=NULL;
                        idDlg=DLG_IMGSTATIC;
                        break;
                    }

                    //Now mycache[idDlg] will be used on CreateObj func for reaload
                    //All the childs as the were
                    _tcscpy(mycache[idDlg].szLastStylesChosen,uObjet[i].szLastStylesChosen);
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
                    _tcscpy(mycache[idDlg].szTitle,uObjet[i].szTitle);
                    _tcscpy(mycache[idDlg].szNameId,uObjet[i].szNameId);
                    mycache[idDlg].linkopt=uObjet[i].linkopt;
                    mycache[idDlg].fShouldAutosize=uObjet[i].fShouldAutosize;
                    isParentWnd=FALSE;
                    idObj=uObjet[i].id;

                    isTypeModDlg=TRUE;//Important in MyDeleteObj for not checking and also delete childs
                    idObjToDelete=idObj;
                    //Then create childs
                    if(!CreateObjet(pszClassName,idDlg,hwnd))
                    {
                        MyPerror("ssfodifdo");
                        exit(EXIT_FAILURE);
                    }
                    idObjToDelete=0;
                    isParentWnd=TRUE;//important
                    MyDeleteObj(idObj);

                    isTypeModDlg=FALSE;//Renitialize
                    i=-1;

                }
            }

            return 0;

        case WM_SIZE:
            /*PLustard!!! :)
            //Applique les parametres de sizing aux childs
            if(g_hwndParentObj)
            {
                MyApplyAutoSizeOnChild(g_hwndParentObj,cxLastClient,cyLastClient);
                cxLastClient=LOWORD(lParam);
                cyLastClient=HIWORD(lParam);

             }
             */

            break;

        case WM_RBUTTONUP:
            if(g_link.isFrameLinkOn)
            {
                g_link.isFrameLinkOn=FALSE;
                InvalidateRect(GetParent(g_link.hwndObjToLink),NULL,TRUE);
                MyShowAllObj();
            }
            if(g_unlink.isMaskUnLinkOn)
            {
                g_unlink.isMaskUnLinkOn=FALSE;
                InvalidateRect(GetParent(g_unlink.hwndObjToUnLink),NULL,TRUE);
                DestroyWindow(hwndMask);
                MyShowAllObj();
            }

            //Get and convert to screen coordiantes the pos where user has clicked
            //Display menu context at the client coordinates pos

            pt.x=LOWORD(lParam);
            pt.y=HIWORD(lParam);
            ptWhereCreate=pt;
            ClientToScreen(hwnd,&pt);
            if(g_hwndParentObj==hwndZC || !g_hwndParentObj)
            {
                EnableMenuItem(hmenu, IDM_MOD_PAR, MF_GRAYED);
                EnableMenuItem(hmenu, IDM_DEL_PAR, MF_GRAYED);
            }
            else
             {

                EnableMenuItem(hmenu, IDM_DEL_PAR, MF_ENABLED);
                EnableMenuItem(hmenu, IDM_MOD_PAR, MF_ENABLED);
                Myborder(hwnd);
             }

            TrackPopupMenu(hmenu,TPM_RIGHTBUTTON,
            pt.x,pt.y,0,hwnd,NULL);
            if(g_hwndParentObj!=hwndZC)
            {
                MyMaskBorder(TRUE);
            }
            return 0;

        case WM_COMMAND:

            if(!lParam)//Its concerning menu items
            {
                switch(LOWORD(wParam))//nature of item selected
                {
                case IDM_ADD_OBJ:
                    // Open  MODAL ,Dialog Box which Creates Choose type's object to create
                    if(!OpenDialogBox(MODAL,DLG_CHOOSETYPEOBJET,
                                      (g_hwndParentObj==NULL?hwndZC:g_hwndParentObj),hinst,NULL,0))
                    {
                        MessageBox(hwnd,TEXT("Unable to open dialog\n wich has ID=DLG_CHOOSETYPEOBJECT"),
                                TEXT("ERROR"),MB_ICONERROR);
                    }
                    return 0;

                case IDM_MOD_PAR:
                    //So user wants to modify settings of g_hwndParentObj
                    isParentWnd=TRUE;//important
                    MyModifyObj(GetDlgCtrlID(hwnd));
                    isParentWnd=FALSE;//renitialization
                    return 0;

                case IDM_DEL_PAR:
                    //User whats to delete g_hwndParentObj
                    SendMessage(hwnd,WM_CLOSE,SC_CLOSE,0);
                    return 0;
                }
            }
            else if(lParam)//its concernig childs controls
            {
                int iIndex=GetObjIndex(LOWORD(wParam));
                if(iIndex==-1) break;

                if(HIWORD(wParam)==STN_CLICKED && uObjet[iIndex].iTypeObj==OBJ_STATIC)
                {
                    //We use it for manipulate static, which dont take focus
                    //by default
                    SetFocus((HWND)lParam);
                }
            }
            break;

        //Manage displaying of tool tip which dispaly description of objs
        case WM_NOTIFY:
        {
            switch (((LPNMHDR)lParam)->code)
            {
            case TTN_GETDISPINFO:
                {
                int iIndex,idObj;
                TOOLINFO ti={0};
                LPNMTTDISPINFO pInfo = (LPNMTTDISPINFO)lParam;
               //Get infos on  structure TOOLINFO
                ti.cbSize = sizeof(TOOLINFO);
                ti.hwnd = pInfo->hdr.hwndFrom;

                // Get current tool
                SendMessage(pInfo->hdr.hwndFrom, TTM_GETCURRENTTOOL, 0, (LPARAM)&ti);

                // ti.uId= ID OF of associates which is in reality
                //an hwnd because of TTF_IDISHWND specified before

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


        //Manage colors

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
            hwndParentObj=g_hwndParentObj;

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
                    //Get text color of parent

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
                    //Get a BK's color of parent
                    //it's the same than a BK's text color of parent :)
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
                couleur=uTemp.mycolorobj[TXT].rgbColor;
            }
            SetTextColor((HDC)wParam,couleur);

            //BK
            if(uTemp.mycolorobj[BK].isDefChoosen)
            {
                int iIndex=uTemp.mycolorobj[BK].defColor;
                if(iIndex==COLOR_TXT_PARENT)
                {
                    //Get the text color of parent
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
                    //Get a BK's color of parent
                    //it's the same than a BK's text color of parent :)

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
                couleur=uTemp.mycolorobj[BK].rgbColor;
            }

            DeleteObject(hBrush);
            hBrush=CreateSolidBrush(couleur);
            SetBkColor((HDC)wParam,couleur);
            return (LRESULT)  hBrush;

        }
            break;//Unreacheable

        case WM_KEYDOWN:
            if(wParam==VK_ESCAPE)
            {
                if(g_link.isFrameLinkOn)
                {
                    g_link.isFrameLinkOn=FALSE;
                    InvalidateRect(GetParent(g_link.hwndObjToLink),NULL,TRUE);
                    MyShowAllObj();
                }
                else if(g_unlink.isMaskUnLinkOn)
                {
                    g_unlink.isMaskUnLinkOn=FALSE;
                    InvalidateRect(GetParent(g_unlink.hwndObjToUnLink),NULL,TRUE);
                    DestroyWindow(hwndMask);
                    MyShowAllObj();
                }
            }
            break;

        case WM_PAINT:
            {

            //For Obj Created



            int iIndexObj=MyGetIindexObjWithHwnd(hwnd);
            if(iIndexObj!=-1)
            {

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
                        //Get the txt's color of parent
                        //here,parent=g_hwndParentObj
                        HDC hdc=GetDC(GetParent(hwnd));
                        couleur=GetTextColor(hdc);
                        ReleaseDC(GetParent(hwnd),hdc);
                    }
                    else if(iIndex==COLOR_BK_PARENT)
                    {
                        //Get a BK's color of parent
                        //it's the same than a BK's text color of parent :)

                        HDC hdc=GetDC(GetParent(hwnd));
                        couleur=GetBkColor(hdc);
                        ReleaseDC(GetParent(hwnd),hdc);
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
                        //Get txt's color of parent
                        HDC hdc=GetDC(GetParent(hwnd));
                        couleur=GetTextColor(hdc);
                        ReleaseDC(GetParent(hwnd),hdc);
                    }
                    else if(iIndex==COLOR_BK_PARENT)
                    {
                        //Get a BK's color of parent
                        //it's the same than a BK's text color of parent :)

                        HDC hdc=GetDC(GetParent(hwnd));
                        couleur=GetBkColor(hdc);
                        ReleaseDC(hwnd,hdc);
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
                EndPaint(hwnd,&ps);
            }
            else break;
            }
            return 0;

        case WM_CLOSE:
            {

                BOOL isG_parentDeletable=(!g_hwndParentObj || g_hwndParentObj==hwndZC)?FALSE:TRUE;//Tres important car apres lactivation du prochain prochain ,g_hwndParent peut changer :)

                pszPathTV=NULL;//RENITIALISATION IMPORTANTE
                int result;
                if(g_fShouldIAskSave)
                {
                    result=MessageBox(hwnd,TEXT("Do you want to save this project?"),
                          TEXT("Question"),MB_ICONQUESTION|MB_YESNOCANCEL|
                                      MB_TASKMODAL);

                }
                else
                    result=IDNO;

                if(result==IDYES)
                {

                    if(isG_parentDeletable && !g_fisTestRunAble)
                    {
                        //If a testing 's instance is running, we nedd to close
                        //it before continue
                        SendMessage(GetParent(hwndZC),WM_COMMAND,
                            ID_STOP_TOOL,0);
                    }
                    //Save procedure

                    SendMessage(GetParent(hwndZC),WM_COMMAND,
                    IDM_SAVE,0);
                    MyDeleteObj(-1);
                    MyEnableButtonToolBar(g_hwndToolBar,IDM_SAVE,TRUE,TRUE);
                    EnableWindow(hwndZC,FALSE);
                    EnableMenuItem(GetMenu(GetParent(hwndZC)),
                                   IDM_CLOSE_PROJECT,MF_GRAYED);
                    EnableMenuItem(GetMenu(GetParent(hwndZC)),
                                   IDM_SAVE,MF_GRAYED);
                    EnableMenuItem(GetMenu(GetParent(hwndZC)),
                                   IDM_SAVEAS,MF_GRAYED);
                    EnableMenuItem(GetMenu(GetParent(hwndZC)),
                                   IDM_SET_DEF_OUTPUT,MF_GRAYED);

                    //delete on treeview
                    for(int i=0;i<g_mtv[0].nbProjects;i++)
                    {
                        if(!strcmp(g_mtv[i].desc.szFilePath,projet.descProjet.szFilePath))
                        {
                            MyDelTreeItem(i);
                            pszPathTV=NULL;
                            if(g_mtv[0].nbProjects>0)
                            {
                                if(!MyActivateProjet(g_mtv[i%g_mtv[0].nbProjects].hItem))
                                {
                                    MyPerror("mopdigoif");
                                    return 0;
                                }
                            }
                            break;
                        }
                    }

                    if(g_mtv[0].nbProjects==0)
                    {
                        SetWindowText(g_hwndTitleProj,"No project");
                        EnableWindow(g_hwndCloseProj,FALSE);
                        MyEnableButtonToolBar(g_hwndToolBar,ID_STOP_TOOL,TRUE,TRUE);
                    }

                    if(!isG_parentDeletable)
                    {
                        //Return 0, for not activating suppress cause a parent isn't deletable
                        return 0;
                    }
                    break;//Odrer to delete g_hwndParentObj
                }
                else if(result==IDNO)
                {

                    if(isG_parentDeletable && !g_fisTestRunAble)
                    {
                        SendMessage(GetParent(hwndZC),WM_COMMAND,
                            ID_STOP_TOOL,0);
                    }

                    MyDeleteObj(-1);
                    MyEnableButtonToolBar(g_hwndToolBar,IDM_SAVE,TRUE,TRUE);
                    EnableWindow(hwndZC,FALSE);
                    EnableMenuItem(GetMenu(GetParent(hwndZC)),
                                   IDM_CLOSE_PROJECT,MF_GRAYED);
                    EnableMenuItem(GetMenu(GetParent(hwndZC)),
                                   IDM_SAVE,MF_GRAYED);
                    EnableMenuItem(GetMenu(GetParent(hwndZC)),
                                   IDM_SAVEAS,MF_GRAYED);
                    EnableMenuItem(GetMenu(GetParent(hwndZC)),
                                   IDM_SET_DEF_OUTPUT,MF_GRAYED);


                    for(int i=0;i<g_mtv[0].nbProjects;i++)
                    {
                        if(!strcmp(g_mtv[i].desc.szFilePath,projet.descProjet.szFilePath))
                        {
                            MyDelTreeItem(i);
                            if(g_mtv[0].nbProjects>0)
                            {
                                if(!MyActivateProjet(g_mtv[i%g_mtv[0].nbProjects].hItem))
                                {
                                    MyPerror("mopdigoif");
                                    return 0;
                                }
                            }
                            break;
                        }
                    }

                    if(g_mtv[0].nbProjects==0)
                    {
                        SetWindowText(g_hwndTitleProj,"No project");
                        EnableWindow(g_hwndCloseProj,FALSE);
                        MyEnableButtonToolBar(g_hwndToolBar,ID_STOP_TOOL,TRUE,TRUE);
                    }

                    if(!isG_parentDeletable)
                    {
                        return 0;
                    }
                    break;
                }
                else return 0;

            }
            break;//uNREACHEABLE

        case WM_DESTROY:
            if(isTypeModDlg )
            {
                //Si la fenetre qu'on veut fermer
                break;
            }
            return 0;
        }
    }//fin !g_hwndParentObj || hwnd==g_hwndParentObj)
    else
    {
        //Here we are working with hwnd=hwndZC
        int scrollPos;
        switch(msg)
        {
        case WM_ENABLE:
            if(wParam)
            {//Window is enabled
                //Change background
                DeleteObject((HBRUSH)SetClassLongPtr(hwnd,GCLP_HBRBACKGROUND,
                                         (LONG_PTR)GetStockObject(WHITE_BRUSH)));
                InvalidateRect(hwnd,NULL,TRUE);
            }
            else
            {//Window is disabled
                DeleteObject((HBRUSH)SetClassLongPtr(hwnd,GCLP_HBRBACKGROUND,
                                         (LONG_PTR)GetStockObject(GRAY_BRUSH)));
                InvalidateRect(hwnd,NULL,TRUE);
            }
            break;

        case WM_VSCROLL:
        case WM_HSCROLL:
        {

            switch(LOWORD(wParam))
            {
            case SB_LINEUP:
                if(msg==WM_VSCROLL)
                    scrollPos =max(0, GetScrollPos(hwnd, SB_VERT) - 10);
                else
                    scrollPos = max(0,GetScrollPos(hwnd, SB_HORZ) - 10);

                break;
            case SB_LINEDOWN:
                if(msg==WM_VSCROLL)
                    scrollPos = min(MAX_VSCROLL_RANGE-1,GetScrollPos(hwnd, SB_VERT) +10);
                else
                    scrollPos = min(MAX_HSCROLL_RANGE-1,GetScrollPos(hwnd, SB_HORZ) + 10);

                break;
            case SB_PAGEUP:
                if(msg==WM_VSCROLL)
                    scrollPos = max(0,GetScrollPos(hwnd, SB_VERT) - 100);
                else
                    scrollPos = max(0,GetScrollPos(hwnd, SB_HORZ) - 100);

                break;
            case SB_PAGEDOWN:
                if(msg==WM_VSCROLL)
                    scrollPos = min(MAX_VSCROLL_RANGE-1,GetScrollPos(hwnd, SB_VERT) + 100);
                else
                    scrollPos = min(MAX_HSCROLL_RANGE-1, GetScrollPos(hwnd, SB_HORZ) + 100);
                break;

            case SB_THUMBPOSITION:
            case SB_THUMBTRACK:
                scrollPos = HIWORD(wParam);
                break;
            default:
                if(msg==WM_VSCROLL)
                    scrollPos = GetScrollPos(hwnd, SB_VERT);
                else
                    scrollPos = GetScrollPos(hwnd, SB_HORZ);

                break;
            }
            if(msg==WM_VSCROLL)
            {
                ScrollWindow(hwnd,0,(GetScrollPos(hwnd, SB_VERT)-scrollPos),NULL,NULL);
                SetScrollPos(hwnd, SB_VERT, scrollPos, TRUE);
                InvalidateRect(hwnd,NULL,TRUE);
                UpdateWindow(hwnd);
            }
            else
            {

                ScrollWindow(hwnd,(GetScrollPos(hwnd, SB_HORZ)-scrollPos),0,NULL,NULL);
                SetScrollPos(hwnd, SB_HORZ, scrollPos, TRUE);
                InvalidateRect(hwnd,NULL,TRUE);
                UpdateWindow(hwnd);
            }
            break;
        }

        case WM_MOUSEWHEEL:
            {

                //Recu soit quand le static a recu ou quand hwnd la recu
                int  pas=GET_WHEEL_DELTA_WPARAM(wParam)/WHEEL_DELTA;
                WPARAM direction;

                if(pas<0)direction=SB_PAGEDOWN;
                else direction=SB_PAGEUP;

                SendMessage (hwnd, WM_VSCROLL, direction, 0) ;
            }
            return 0;//fin case WM_MOUSEWHEEL

        }//fin switch

    }//fin else
    return DefWindowProc(hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK ManageProc(HWND hwnd, UINT msg,WPARAM wParam, LPARAM lParam  )
{
    //Window procedure o  manage zone
    return DefWindowProc(hwnd,msg,wParam,lParam);
}

BOOL MySetAppIconToWnd(HWND hwnd)
{
    HICON hIcon=LoadIcon((HINSTANCE)GetModuleHandle(NULL),"GCO");
    if(!hIcon)
    {
        perror("kdslg;");
        exit(EXIT_FAILURE);
    }
    SetClassLongPtr(hwnd,GCLP_HICON,(LONG_PTR)hIcon);
    return TRUE;
}

LRESULT CALLBACK DlgNotifProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{

    //This procedure manage many differents dialogBoxes
    //DEPENDING on their type,a behavior isnt the same
    static int iUserChoice;
    static int iTypeDlg;
    static BOOL hasUserAlreadyClickedRadio=FALSE;

    if(message==WM_INITDIALOG)
    {
        iTypeDlg=((DLGPARAMINFO*)lParam)->idDlg;
        MySetAppIconToWnd(hDlg);
    }

    switch(iTypeDlg)
    {
    case IDD_MYDIALOG:
        switch (message)
        {
        case WM_INITDIALOG:
            {
                //centrer sur hwndZc
                 RECT rcParent, rcDlg;
                 HWND hwndZC=((DLGPARAMINFO*)lParam)->hwndParentDlg;
                 ShowWindow(hDlg,SW_HIDE);
                GetWindowRect(hwndZC, &rcParent);
                GetWindowRect(hDlg, &rcDlg);

                int dlgWidth = rcDlg.right - rcDlg.left;
                int dlgHeight = rcDlg.bottom - rcDlg.top;

                int x = rcParent.left + (rcParent.right - rcParent.left) / 2 - dlgWidth / 2;
                int y = rcParent.top + (rcParent.bottom - rcParent.top) / 2 - dlgHeight / 2;
                 SetWindowPos(hDlg, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
                ShowWindow(hDlg,SW_NORMAL);
            }
            return TRUE;

        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
            case ID_SUIV:
            case IDCANCEL:
                EndDialog(hDlg,LOWORD(wParam));
                return TRUE;
            }
            break;
        }
        break;//fin case IDD_MYDIALOG


    case IDD_HOW_TO_RESIZE:
        switch (message)
        {
        case WM_INITDIALOG:
            {
                //centrer sur hwndZc
                 RECT rcParent, rcDlg;
                 HWND hwndZC=((DLGPARAMINFO*)lParam)->hwndParentDlg;
                 ShowWindow(hDlg,SW_HIDE);
                GetWindowRect(hwndZC, &rcParent);
                GetWindowRect(hDlg, &rcDlg);

                int dlgWidth = rcDlg.right - rcDlg.left;
                int dlgHeight = rcDlg.bottom - rcDlg.top;

                int x = rcParent.left + (rcParent.right - rcParent.left) / 2 - dlgWidth / 2;
                int y = rcParent.top + (rcParent.bottom - rcParent.top) / 2 - dlgHeight / 2;
                 SetWindowPos(hDlg, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
                ShowWindow(hDlg,SW_NORMAL);
            }

             return TRUE;

        case WM_COMMAND:
            switch(LOWORD(wParam))
            {

            case IDCANCEL:
                return TRUE;
            case IDOK:
                if(IsDlgButtonChecked(hDlg,IDB_DONT_ANNOY))
                    g_fShowHowSize=FALSE;
                else
                    g_fShowHowSize=TRUE;

                EndDialog(hDlg,LOWORD(wParam));
                return TRUE;
            }
            break;
        }
        break;//fin case IDD_HOW_TO_SIZE

    case IDD_MYDIALOG2:
        switch (message)
        {
        case WM_INITDIALOG:
            {
             hasUserAlreadyClickedRadio=FALSE;
                //centrer sur hwndZc

                 RECT rcParent, rcDlg;
                 HWND hwndZC=((DLGPARAMINFO*)lParam)->hwndParentDlg;
                 ShowWindow(hDlg,SW_HIDE);
                GetWindowRect(hwndZC, &rcParent);
                GetWindowRect(hDlg, &rcDlg);

                int dlgWidth = rcDlg.right - rcDlg.left;
                int dlgHeight = rcDlg.bottom - rcDlg.top;

                int x = rcParent.left + (rcParent.right - rcParent.left) / 2 - dlgWidth / 2;
                int y = rcParent.top + (rcParent.bottom - rcParent.top) / 2 - dlgHeight / 2;
                 SetWindowPos(hDlg, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
                ShowWindow(hDlg,SW_NORMAL);
                EnableWindow(GetDlgItem(hDlg,ID_SUIV),FALSE);
            }
            return TRUE;

        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
            case ID_SIMPWND:
            case ID_DLGWND:

                iUserChoice=LOWORD(wParam);
                if(HIWORD(wParam)==BN_CLICKED)
                {
                    if(!hasUserAlreadyClickedRadio)
                        hasUserAlreadyClickedRadio=TRUE;
                    else
                        EnableWindow(GetDlgItem(hDlg,ID_SUIV),TRUE);
                }

                return TRUE;

            case ID_SUIV:
                EndDialog(hDlg,iUserChoice);
                return TRUE;
            case IDCANCEL:
                EndDialog(hDlg,LOWORD(wParam));
                return TRUE;
            }
            break;
        }
        break;//fin case IDD_MYDIALOG2

    case IDD_SETDEFOUTPUT:
    {
        //DlgBox where user specify a folder where
        //create A GCODEF.h
        static TCHAR *szPathDef=NULL;
        switch (message)
        {
        case WM_INITDIALOG:
            {
                //centrer sur hwndZc

                 RECT rcParent, rcDlg;
                 HWND hwndZC=((DLGPARAMINFO*)lParam)->hwndParentDlg;
                 ShowWindow(hDlg,SW_HIDE);
                GetWindowRect(hwndZC, &rcParent);
                GetWindowRect(hDlg, &rcDlg);

                int dlgWidth = rcDlg.right - rcDlg.left;
                int dlgHeight = rcDlg.bottom - rcDlg.top;

                int x = rcParent.left + (rcParent.right - rcParent.left) / 2 - dlgWidth / 2;
                int y = rcParent.top + (rcParent.bottom - rcParent.top) / 2 - dlgHeight / 2;
                 SetWindowPos(hDlg, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
                ShowWindow(hDlg,SW_NORMAL);
            }
            szPathDef=(TCHAR*)(((DLGPARAMINFO*)lParam)->moreData);
            SetWindowText(GetDlgItem(hDlg,IDE_DEFPATH),szPathDef);
            return TRUE;

        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
            case IDB_SELPATH:
                //User wanna browse and select a folder
                BrowseForFolder(hDlg,GetDlgItem(hDlg,IDE_DEFPATH));
                return TRUE;
            case ID_FINISH:
            {//Update projet.szDefPath=szPathDef
                char buf[1024];
                GetWindowText(GetDlgItem(hDlg,IDE_DEFPATH),buf,1023);
                if(!PathFileExists(buf))
                {
                    MessageBox(hDlg,"Invalid path!","Error",MB_ICONERROR|
                               MB_TASKMODAL|MB_OK);
                    return TRUE;
                }
                strcpy(szPathDef,buf);
            }
            case IDCANCEL:
                EndDialog(hDlg,LOWORD(wParam));
                return TRUE;
            }
            break;
        }
    }
        break;//fin case IDD_DEFOUTPUT
    }//fin switch GetDlgCtrlID(hDlg)

    return FALSE;
}

BOOL MyLoadG_Paths(void)
{
    //Open last save.filesm and apply last paths modifcaitons
    //done by user on this new instance which is running

    //A file is stocked in appdata\GCO

    /*
    char szPathExe[1024];
    char *c;

    //GetPath where  ou graphiccreationObject.exe is running
    GetModuleFileName(NULL,szPathExe,1024);

    //Get A Folder's path
    c=strrchr(szPathExe,'\\');
    if(c) *c='\0';
    if(PathFileExists(szPathExe))
    {
        //Set this folder's path as default's path
        SetCurrentDirectory(szPathExe);
    }


    GetCurrentDirectory(MAX_PATH+1,szCurrentPath);
    */

    //1.Get save.bin 's path
    char szPathSave[1024]="";

    if(!GetAppDataPath(szPathSave))
    {
        MyPerror("vklsdk;kfg");
        MyMsgError();
        exit(EXIT_FAILURE);
    }
    if(!(CreateDirectory(strcat(szPathSave,"GCO\\"), NULL) ||
                GetLastError() == ERROR_ALREADY_EXISTS))
    {
        MyPerror("kvlds;fkds;");
        return FALSE;
    }

    strcat(szPathSave,"save.bin");



    FILE *File=fopen(szPathSave,"rb");
    if(!File)
    {
        MyPerror("vklsdlflllkjal");
        return FALSE;
    }
    int nbBlock=0;

    fread(&nbBlock,sizeof(int),1,File);
    if(nbBlock)
        fread(g_szPathFolder,sizeof(char),nbBlock,File);
    else
    {
        fclose(File);
        MyPerror("mpdooidgf");
        return FALSE;
    }

    //g_fShowHowSize
    fread(&g_fShowHowSize,sizeof(BOOL),1,File);
    fclose(File);

    return TRUE;
}

BOOL MySaveG_Paths(void)
{
    //Just save few of paths that user has used on this
    //instance


    //1.Get save.bin 's path
    char szPathSave[1024]="";

    if(!GetAppDataPath(szPathSave))
    {
        MyPerror("gd54646s");
        MyMsgError();
        exit(EXIT_FAILURE);
    }
    if(!(CreateDirectory(strcat(szPathSave,"GCO\\"), NULL) ||
                GetLastError() == ERROR_ALREADY_EXISTS))
    {
        MyPerror("gfds8g798");
        return FALSE;
    }

    strcat(szPathSave,"save.bin");


    FILE *File=fopen(szPathSave,"wb");
    if(!File)
    {
        MyPerror("vklks;lg;sa");
        return FALSE;
    }
    int nbBlock=strlen(g_szPathFolder)+1;
    fwrite(&nbBlock,sizeof(int),1,File);
    fwrite(g_szPathFolder,sizeof(char),nbBlock,File);
    //g_fShowHowSize
    fwrite(&g_fShowHowSize,sizeof(BOOL),1,File);

    fclose(File);
    return TRUE;
}

LONG MyGetCharDim(HWND hwnd,HFONT hFontCurrent)
{
    //Return a cxChar and a cyChar of hwnd, knowing is font
    if(!hFontCurrent)
    {
        return GetDialogBaseUnits();
    }

    TEXTMETRIC tm;
    HFONT hOldFont;


    HDC hdc=GetDC(hwnd);

    hOldFont=(HFONT)SelectObject(hdc,hFontCurrent);
    if(!hOldFont)
    {
        return GetDialogBaseUnits();
    }

    GetTextMetrics(hdc,&tm);
    int cx=tm.tmAveCharWidth;
    int cy=tm.tmHeight+tm.tmExternalLeading;
    int cxCaps=(tm.tmPitchAndFamily&1? 3:2)* cx/2;

    SelectObject(hdc,hOldFont);
    ReleaseDC(hwnd,hdc);

    return MAKELONG(cxCaps,cy);
}

void MyMsgError(void)
{//The most used :)
    MessageBox(GetForegroundWindow(),
        TEXT("An unknown error occured.."),TEXT("ERROR"),
        MB_OK|MB_ICONERROR|MB_TASKMODAL);
}

void InitStyleDesc(void)
{
    //Initialize a constant structure which constains
    //a general descriptions of all the styles that user
    //can set to an objects when he sets it.
    STYLE_DESCRIPTION styleDesc[MAX_STYLES_DESC]=
    {
        {WS_OVERLAPPED,"WS_OVERLAPPED","The window is an overlapped window. An overlapped window has a title bar and a border. Same as the WS_TILED style."},
        {WS_VISIBLE,"WS_VISIBLE","The window is initially visible.\
This style can be turned on and off by using the ShowWindow or SetWindowPos function."},
        {WS_POPUP,"WS_POPUP","The window is a pop-up window. This style cannot be used with the WS_CHILD style."},
        {WS_CHILD,"WS_CHILD","The window is a child window. A window with this style cannot have a menu bar. This style cannot be used with the WS_POPUP style."},
        {WS_MINIMIZE,"WS_MINIMIZE","The window is initially minimized. Same as the WS_ICONIC style."},
        {WS_DISABLED,"WS_DISABLED","The window is initially disabled. A disabled window cannot receive input from the user. To change this after a window has been created, use the EnableWindow function"},
        {WS_CLIPSIBLINGS,"WS_CLIPSIBLINGS","Clips child windows relative to each other; that is, when a particular child window receives a WM_PAINT message, the WS_CLIPSIBLINGS style clips all other overlapping child windows out of the region of the child window to be updated. If WS_CLIPSIBLINGS is not specified and child windows overlap, it is possible, when drawing within the client area of a child window, to draw within the client area of a neighboring child window."},
        {WS_CLIPCHILDREN,"WS_CLIPCHILDREN","Excludes the area occupied by child windows when drawing occurs within the parent window. This style is used when creating the parent window."},
        {WS_MAXIMIZE,"WS_MAXIMIZE","The window is initially maximized."},
        {WS_CAPTION,"WS_CAPTION","The window has a title bar (includes the WS_BORDER style)."},
        {WS_BORDER,"WS_BORDER","The window has a thin-line border"},
        {WS_DLGFRAME,"WS_DLGFRAME","The window has a border of a style typically used with dialog boxes. A window with this style cannot have a title bar."},
        {WS_VSCROLL,"WS_VSCROLL","The window has a vertical scroll bar."},
        {WS_HSCROLL,"WS_HSCROLL","The window has a horizontal scroll bar."},
        {WS_SYSMENU,"WS_SYSMENU","The window has a window menu on its title bar. The WS_CAPTION style must also be specified."},
        {WS_THICKFRAME,"WS_THICKFRAME","The window has a sizing border. Same as the WS_SIZEBOX style."},
        {WS_GROUP,"WS_GROUP","The window is the first control of a group of controls. The group consists of this first control and all controls defined after it, up to the next control with the WS_GROUP style. The first control in each group usually has the WS_TABSTOP style so that the user can move from group to group. The user can subsequently change the keyboard focus from one control in the group to the next control in the group by using the direction keys.\
        You can turn this style on and off to change dialog box navigation. To change this style after a window has been created, use the SetWindowLong function."},
        {WS_TABSTOP,"WS_TABSTOP","The window is a control that can receive the keyboard focus when the user presses the TAB key. Pressing the TAB key changes the keyboard focus to the next control with the WS_TABSTOP style."},
        {WS_MINIMIZEBOX,"WS_MINIMIZEBOX","The window has a minimize button. Cannot be combined with the WS_EX_CONTEXTHELP style. The WS_SYSMENU style must also be specified."},
        {WS_MAXIMIZEBOX,"WS_MAXIMIZEBOX","The window has a maximize button. Cannot be combined with the WS_EX_CONTEXTHELP style. The WS_SYSMENU style must also be specified."},
        {WS_TILED,"WS_TILED","The window is an overlapped window. An overlapped window has a title bar and a border. Same as the WS_OVERLAPPED style."},
        {WS_ICONIC,"WS_ICONIC","The window is initially minimized. Same as the WS_MINIMIZE style."},
        {WS_SIZEBOX,"WS_SIZEBOX","The window has a sizing border. Same as the WS_THICKFRAME style."},
        {WS_TILEDWINDOW,"WS_TILEDWINDOW","(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX)	The window is an overlapped window. Same as the WS_OVERLAPPEDWINDOW style."},
        {WS_OVERLAPPEDWINDOW,"WS_OVERLAPPEDWINDOW","(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX)	The window is an overlapped window. Same as the WS_TILEDWINDOW style."},
        {WS_POPUPWINDOW,"WS_POPUPWINDOW","The window is a pop-up window. The WS_CAPTION and WS_POPUPWINDOW styles must be combined to make the window menu visible."},
        {WS_CHILDWINDOW,"WS_CHILDWINDOW","Same as the WS_CHILD style."},
        {WS_ACTIVECAPTION,"WS_ACTIVECAPTION","define the visual style of the title bar of the active window. This can include different attributes (like color) to make it distinguishable from inactive windows, which helps users identify the active application or window more easily."},
        {CBS_SIMPLE,"CBS_SIMPLE","Displays the list box at all times. The current selection in the list box is displayed in the edit control."},
        {CBS_DROPDOWN,"CBS_DROPDOWN","Similar to CBS_SIMPLE, except that the list box is not displayed unless the user selects an icon next to the edit control."},
        {CBS_DROPDOWNLIST,"CBS_DROPDOWNLIST","Similar to CBS_DROPDOWN, except that the edit control is replaced by a static text item that displays the current selection in the list box."},
        {CBS_OWNERDRAWFIXED,"CBS_OWNERDRAWFIXED","Specifies that the owner of the list box is responsible for drawing its contents and that the items in the list box are all the same height. The owner window receives a WM_MEASUREITEM message when the combo box is created and a WM_DRAWITEM message when a visual aspect of the combo box has changed."},
        {CBS_OWNERDRAWVARIABLE,"CBS_OWNERDRAWVARIABLE","Specifies that the owner of the list box is responsible for drawing its contents and that the items in the list box are variable in height. The owner window receives a WM_MEASUREITEM message for each item in the combo box when you create the combo box and a WM_DRAWITEM message when a visual aspect of the combo box has changed."},
        {CBS_AUTOHSCROLL,"CBS_AUTOHSCROLL","Automatically scrolls the text in an edit control to the right when the user types a character at the end of the line. If this style is not set, only text that fits within the rectangular boundary is allowed."},
        {CBS_OEMCONVERT,"CBS_OEMCONVERT","Converts text entered in the combo box edit control from the Windows character set to the OEM character set and then back to the Windows character set. This ensures proper character conversion when the application calls the CharToOem function to convert a Windows string in the combo box to OEM characters. This style is most useful for combo boxes that contain file names and applies only to combo boxes created with the CBS_SIMPLE or CBS_DROPDOWN style."},
        {CBS_SORT,"CBS_SORT","Automatically sorts strings added to the list box."},
        {CBS_HASSTRINGS,"CBS_HASSTRINGS","Specifies that an owner-drawn combo box contains items consisting of strings. The combo box maintains the memory and address for the strings so the application can use the CB_GETLBTEXT message to retrieve the text for a particular item."},
        {CBS_NOINTEGRALHEIGHT,"CBS_NOINTEGRALHEIGHT","Specifies that the size of the combo box is exactly the size specified by the application when it created the combo box. Normally, the system sizes a combo box so that it does not display partial items."},
        {CBS_DISABLENOSCROLL,"CBS_DISABLENOSCROLL","Shows a disabled vertical scroll bar in the list box when the box does not contain enough items to scroll. Without this style, the scroll bar is hidden when the list box does not contain enough items."},
        {CBS_UPPERCASE,"CBS_UPPERCASE","Converts to uppercase all text in both the selection field and the list."},
        {CBS_LOWERCASE,"CBS_LOWERCASE","Converts to lowercase all text in both the selection field and the list."},
        {ES_LEFT,"ES_LEFT","Aligns text with the left margin."},
        {ES_CENTER,"ES_CENTER","Centers text in a single-line or multiline edit control."},
        {ES_RIGHT,"ES_RIGHT","Right-aligns text in a single-line or multiline edit control."},
        {ES_MULTILINE,"ES_MULTILINE","Designates a multiline edit control. The default is single-line edit control."},
        {ES_UPPERCASE,"ES_UPPERCASE","Converts all characters to uppercase as they are typed into the edit control."},
        {ES_LOWERCASE,"ES_LOWERCASE","Converts all characters to lowercase as they are typed into the edit control.\
To change this style after the control has been created, use SetWindowLong."},
        {ES_PASSWORD,"ES_PASSWORD","Displays an asterisk (*) for each character typed into the edit control. This style is valid only for single-line edit controls."},
        {ES_AUTOVSCROLL,"ES_AUTOVSCROLL","Automatically scrolls text up one page when the user presses the ENTER key on the last line."},
        {ES_AUTOHSCROLL,"ES_AUTOHSCROLL","Automatically scrolls text to the right by 10 characters when the user types a character at the end of the line. When the user presses the ENTER key, the control scrolls all text back to position zero."},
        {ES_NOHIDESEL,"ES_NOHIDESEL","Negates the default behavior for an edit control. The default behavior hides the selection when the control loses the input focus and inverts the selection when the control receives the input focus. If you specify ES_NOHIDESEL, the selected text is inverted, even if the control does not have the focus."},
        {ES_OEMCONVERT,"ES_OEMCONVERT","Converts text entered in the edit control. The text is converted from the Windows character set to the OEM character set and then back to the Windows character set"},
        {ES_READONLY,"ES_READONLY","Prevents the user from typing or editing text in the edit control."},
        {ES_WANTRETURN,"ES_WANTRETURN","Specifies that a carriage return be inserted when the user presses the ENTER key while entering text into a multiline edit control in a dialog box. If you do not specify this style, pressing the ENTER key has the same effect as pressing the dialog box's default push button. This style has no effect on a single-line edit control."},
        {ES_NUMBER,"ES_NUMBER","Allows only digits to be entered into the edit control. Note that, even with this set, it is still possible to paste non-digits into the edit control."},
        {SS_LEFT,"SS_LEFT","A simple rectangle and left-aligns the text in the rectangle. The text is formatted before it is displayed. Words that extend past the end of a line are automatically wrapped to the beginning of the next left-aligned line. Words that are longer than the width of the control are truncated."},
        {SS_CENTER,"SS_CENTER","A simple rectangle and centers the text in the rectangle. The text is formatted before it is displayed. Words that extend past the end of a line are automatically wrapped to the beginning of the next centered line. Words that are longer than the width of the control are truncated."},
        {SS_RIGHT,"SS_RIGHT","A simple rectangle and right-aligns the text in the rectangle. The text is formatted before it is displayed. Words that extend past the end of a line are automatically wrapped to the beginning of the next right-aligned line. Words that are longer than the width of the control are truncated."},
        {SS_ICON,"SS_ICON","An icon to be displayed in the dialog box.(We have tell more about it :)) "},
        {SS_BLACKRECT,"SS_BLACKRECT","A rectangle filled with the current window frame color. This color is black in the default color scheme."},
        {SS_GRAYRECT,"SS_GRAYRECT","A rectangle filled with the current screen background color. This color is gray in the default color scheme."},
        {SS_WHITERECT,"SS_WHITERECT","A rectangle filled with the current window background color. This color is white in the default color scheme."},
        {SS_BLACKFRAME,"SS_BLACKFRAME","A box with a frame drawn in the same color as the window frames. This color is black in the default color scheme."},
        {SS_GRAYFRAME,"SS_GRAYFRAME","A box with a frame drawn with the same color as the screen background (desktop). This color is gray in the default color scheme."},
        {SS_WHITEFRAME,"SS_WHITEFRAME","A box with a frame drawn with the same color as the window background. This color is white in the default color scheme."},
        {SS_USERITEM,"SS_USERITEM","used for static controls where you need custom drawing or behavior."},
        {SS_SIMPLE,"SS_SIMPLE","A simple rectangle and displays a single line of left-aligned text in the rectangle. The text line cannot be shortened or altered in any way. Also, if the control is disabled, the control does not gray its text."},
        {SS_LEFTNOWORDWRAP,"SS_LEFTNOWORDWRAP","A simple rectangle and left-aligns the text in the rectangle. Tabs are expanded, but words are not wrapped. Text that extends past the end of a line is clipped."},
        {SS_OWNERDRAW,"SS_OWNERDRAW","The owner of the static control is responsible for drawing the control. The owner window receives a WM_DRAWITEM message whenever the control needs to be drawn."},
        {SS_BITMAP,"SS_BITMAP","A bitmap is to be displayed in the static control. The text is the name of a bitmap (not a filename) defined elsewhere in the resource file. The style ignores the nWidth and nHeight parameters; the control automatically sizes itself to accommodate the bitmap."},
        {SS_ENHMETAFILE,"SS_ENHMETAFILE","An enhanced metafile is to be displayed in the static control. The text is the name of a metafile. An enhanced metafile static control has a fixed size; the metafile is scaled to fit the static control's client area."},
        {SS_ETCHEDHORZ,"SS_ETCHEDHORZ","Draws the top and bottom edges of the static control using the EDGE_ETCHED edge style."},
        {SS_ETCHEDVERT,"SS_ETCHEDVERT","Draws the left and right edges of the static control using the EDGE_ETCHED edge style"},
        {SS_ETCHEDFRAME,"SS_ETCHEDFRAME","Draws the frame of the static control using the EDGE_ETCHED edge style."},
        {SS_TYPEMASK,"SS_TYPEMASK","A composite style bit that results from using the OR operator on SS_* style bits. Can be used to mask out valid SS_* bits from a given bitmask. Note that this is out of date and does not correctly include all valid styles. Thus, you should not use this style."},
        {SS_REALSIZECONTROL,"SS_REALSIZECONTROL","Adjusts the bitmap to fit the size of the static control. For example, changing the locale can change the system font, and thus controls might be resized. If a static control had a bitmap, the bitmap would no longer fit the control. This style bit dictates automatic redimensioning of bitmaps to fit their controls.\
If SS_CENTERIMAGE is specified, the bitmap or icon is centered (and clipped if needed). If SS_CENTERIMAGE is not specified, the bitmap or icon is stretched or shrunk.\
Note that the redimensioning in the two axes are independent, and the result may have a changed aspect ratio.\
Compare with SS_REALSIZEIMAGE."},
        {SS_NOPREFIX,"SS_NOPREFIX","Prevents interpretation of any ampersand (&) characters in the control's text as accelerator prefix characters. These are displayed with the ampersand removed and the next character in the string underlined. This static control style may be included with any of the defined static controls. You can combine SS_NOPREFIX with other styles. This can be useful when filenames or other strings that may contain an ampersand (&) must be displayed in a static control in a dialog box."},
        {SS_NOTIFY,"SS_NOTIFY","Sends the parent window STN_CLICKED, STN_DBLCLK, STN_DISABLE, and STN_ENABLE notification codes when the user clicks or double-clicks the control."},
        {SS_CENTERIMAGE,"SS_CENTERIMAGE","A bitmap is centered in the static control that contains it. The control is not resized, so that a bitmap too large for the control will be clipped. If the static control contains a single line of text, the text is centered vertically in the client area of the control."},
        {SS_RIGHTJUST,"SS_RIGHTJUST","The lower right corner of a static control with the SS_BITMAP or SS_ICON style is to remain fixed when the control is resized. Only the top and left sides are adjusted to accommodate a new bitmap or icon."},
        {SS_REALSIZEIMAGE,"SS_REALSIZEIMAGE","Specifies that the actual resource width is used and the icon is loaded using LoadImage. SS_REALSIZEIMAGE is always used in conjunction with SS_ICON"},
        {SS_SUNKEN,"SS_SUNKEN","Draws a half-sunken border around a static control."},
        {SS_EDITCONTROL,"SS_EDITCONTROL","The static control duplicates the text-displaying characteristics of a multiline edit control. Specifically, the average character width is calculated in the same manner as with an edit control, and the function does not display a partially visible last line."},
        {SS_ENDELLIPSIS,"SS_ENDELLIPSIS","If the end of a string does not fit in the rectangle, it is truncated and ellipses are added. If a word that is not at the end of the string goes beyond the limits of the rectangle, it is truncated without ellipses. Using this style will force the control's text to be on one line with no word wrap. Compare with SS_PATHELLIPSIS and SS_WORDELLIPSIS."},
        {SS_PATHELLIPSIS,"SS_PATHELLIPSIS","Replaces characters in the middle of the string with ellipses so that the result fits in the specified rectangle. If the string contains backslash (\\) characters, SS_PATHELLIPSIS preserves as much as possible of the text after the last backslash. Using this style will force the control's text to be on one line with no word wrap. Compare with SS_ENDELLIPSIS and SS_WORDELLIPSIS."},
        {SS_WORDELLIPSIS,"SS_WORDELLIPSIS","Truncates any word that does not fit in the rectangle and adds ellipses. Using this style will force the control's text to be on one line with no word wrap.\
Compare with SS_ENDELLIPSIS and SS_PATHELLIPSIS."},
        {SS_ELLIPSISMASK,"SS_ELLIPSISMASK","used with STATIC controls to automatically truncate text with an ellipsis (...) when the text overflows the bounds of the control."},
        {BS_PUSHBUTTON,"BS_PUSHBUTTON","Creates a push button that posts a WM_COMMAND message to the owner window when the user selects the button."},
        {BS_DEFPUSHBUTTON,"BS_DEFPUSHBUTTON","Creates a push button that behaves like a BS_PUSHBUTTON style button, but has a distinct appearance. If the button is in a dialog box, the user can select the button by pressing the ENTER key, even when the button does not have the input focus. This style is useful for enabling the user to quickly select the most likely (default) option."},
        {BS_CHECKBOX,"BS_CHECKBOX","Creates a small, empty check box with text. By default, the text is displayed to the right of the check box. To display the text to the left of the check box, combine this flag with the BS_LEFTTEXT style (or with the equivalent BS_RIGHTBUTTON style)."},
        {BS_AUTOCHECKBOX,"BS_AUTOCHECKBOX","Creates a button that is the same as a check box, except that the check state automatically toggles between checked and cleared each time the user selects the check box."},
        {BS_RADIOBUTTON,"BS_RADIOBUTTON","Creates a small circle with text. By default, the text is displayed to the right of the circle. To display the text to the left of the circle, combine this flag with the BS_LEFTTEXT style (or with the equivalent BS_RIGHTBUTTON style). Use radio buttons for groups of related, but mutually exclusive choices."},
        {BS_3STATE,"BS_3STATE","Creates a button that is the same as a check box, except that the box can be grayed as well as checked or cleared. Use the grayed state to show that the state of the check box is not determined."},
        {BS_AUTO3STATE,"BS_AUTO3STATE","Creates a button that is the same as a three-state check box, except that the box changes its state when the user selects it. The state cycles through checked, indeterminate, and cleared."},
        {BS_GROUPBOX,"BS_GROUPBOX","Creates a rectangle in which other controls can be grouped. Any text associated with this style is displayed in the rectangle's upper left corner."},
        {BS_USERBUTTON,"BS_USERBUTTON","Obsolete, but provided for compatibility with 16-bit versions of Windows. Applications should use BS_OWNERDRAW instead."},
        {BS_AUTORADIOBUTTON,"BS_AUTORADIOBUTTON","Creates a button that is the same as a radio button, except that when the user selects it, the system automatically sets the button's check state to checked and automatically sets the check state for all other buttons in the same group to cleared."},
        {BS_PUSHBOX,"BS_PUSHBOX"," style used with BUTTON controls that gives the button a sunken or pressed appearance by default."},
        {BS_OWNERDRAW,"BS_OWNERDRAW","Creates an owner-drawn button. The owner window receives a WM_DRAWITEM message when a visual aspect of the button has changed. Do not combine the BS_OWNERDRAW style with any other button styles."},
        {BS_TYPEMASK,"BS_TYPEMASK","Do not use this style. A composite style bit that results from using the OR operator on BS_* style bits. It can be used to mask out valid BS_* bits from a given bitmask. Note that this is out of date and does not correctly include all valid styles. Thus, you should not use this style."},
        {BS_LEFTTEXT,"BS_LEFTTEXT","Places text on the left side of the radio button or check box when combined with a radio button or check box style. Same as the BS_RIGHTBUTTON style."},
        {BS_TEXT,"BS_TEXT","Specifies that the button displays text."},
        {BS_ICON,"BS_ICON","Specifies that the button displays an icon. See the Remarks section for its interaction with BS_BITMAP."},
        {BS_BITMAP,"BS_BITMAP","Specifies that the button displays a bitmap. See the Remarks section for its interaction with BS_ICON."},
        {BS_LEFT,"BS_LEFT","Left-justifies the text in the button rectangle. However, if the button is a check box or radio button that does not have the BS_RIGHTBUTTON style, the text is left justified on the right side of the check box or radio button."},
        {BS_RIGHT,"BS_RIGHT","Right-justifies text in the button rectangle. However, if the button is a check box or radio button that does not have the BS_RIGHTBUTTON style, the text is right justified on the right side of the check box or radio button."},
        {BS_CENTER,"BS_CENTER","Centers text horizontally in the button rectangle."},
        {BS_TOP,"BS_TOP","Places text at the top of the button rectangle."},
        {BS_BOTTOM,"BS_BOTTOM","Places text at the bottom of the button rectangle."},
        {BS_VCENTER,"BS_VCENTER","Places text in the middle (vertically) of the button rectangle."},
        {BS_PUSHLIKE,"BS_PUSHLIKE","Makes a button (such as a check box, three-state check box, or radio button) look and act like a push button. The button looks raised when it isn't pushed or checked, and sunken when it is pushed or checked."},
        {BS_MULTILINE,"BS_MULTILINE","Wraps the button text to multiple lines if the text string is too long to fit on a single line in the button rectangle."},
        {BS_NOTIFY,"BS_NOTIFY","Enables a button to send BN_KILLFOCUS and BN_SETFOCUS notification codes to its parent window.\
Note that buttons send the BN_CLICKED notification code regardless of whether it has this style. To get BN_DBLCLK notification codes, the button must have the BS_RADIOBUTTON or BS_OWNERDRAW style."},
        {BS_FLAT,"BS_FLAT","Specifies that the button is two-dimensional; it does not use the default shading to create a 3-D image."},
        {BS_RIGHTBUTTON,"BS_RIGHTBUTTON","Positions a radio button's circle or a check box's square on the right side of the button rectangle. Same as the BS_LEFTTEXT style."},
        {LBS_NOTIFY,"LBS_NOTIFY","Causes the list box to send a notification code to the parent window whenever the user clicks a list box item (LBN_SELCHANGE), double-clicks an item (LBN_DBLCLK), or cancels the selection (LBN_SELCANCEL)."},
        {LBS_SORT,"LBS_SORT","Sorts strings in the list box alphabetically."},
        {LBS_NOREDRAW,"LBS_NOREDRAW","Specifies that the list box's appearance is not updated when changes are made.\
To change the redraw state of the control, use the WM_SETREDRAW message."},
        {LBS_MULTIPLESEL,"LBS_MULTIPLESEL","Turns string selection on or off each time the user clicks or double-clicks a string in the list box. The user can select any number of strings."},
        {LBS_OWNERDRAWFIXED,"LBS_OWNERDRAWFIXED","Specifies that the owner of the list box is responsible for drawing its contents and that the items in the list box are the same height. The owner window receives a WM_MEASUREITEM message when the list box is created and a WM_DRAWITEM message when a visual aspect of the list box has changed."},
        {LBS_OWNERDRAWVARIABLE,"LBS_OWNERDRAWVARIABLE","Specifies that the owner of the list box is responsible for drawing its contents and that the items in the list box are variable in height. The owner window receives a WM_MEASUREITEM message for each item in the box when the list box is created and a WM_DRAWITEM message when a visual aspect of the list box has changed.\
This style causes the LBS_NOINTEGRALHEIGHT style to be enabled.\
This style is ignored if the LBS_MULTICOLUMN style is specified."},
        {LBS_HASSTRINGS,"LBS_HASSTRINGS","Specifies that a list box contains items consisting of strings. The list box maintains the memory and addresses for the strings so that the application can use the LB_GETTEXT message to retrieve the text for a particular item. By default, all list boxes except owner-drawn list boxes have this style. You can create an owner-drawn list box either with or without this style.\
For owner-drawn list boxes without this style, the LB_GETTEXT message retrieves the value associated with an item (the item data)."},
        {LBS_USETABSTOPS,"LBS_USETABSTOPS","Enables a list box to recognize and expand tab characters when drawing its strings. You can use the LB_SETTABSTOPS message to specify tab stop positions. The default tab positions are 32 dialog template units apart. Dialog template units are the device-independent units used in dialog box templates. To convert measurements from dialog template units to screen units (pixels), use the MapDialogRect function."},
        {LBS_NOINTEGRALHEIGHT,"LBS_NOINTEGRALHEIGHT","Specifies that the size of the list box is exactly the size specified by the application when it created the list box. Normally, the system sizes a list box so that the list box does not display partial items.\
For list boxes with the LBS_OWNERDRAWVARIABLE style, the LBS_NOINTEGRALHEIGHT style is always enforced."},
        {LBS_MULTICOLUMN,"LBS_MULTICOLUMN","Specifies a multi-column list box that is scrolled horizontally. The list box automatically calculates the width of the columns, or an application can set the width by using the LB_SETCOLUMNWIDTH message. If a list box has the LBS_OWNERDRAWFIXED style, an application can set the width when the list box sends the WM_MEASUREITEM message.\
A list box with the LBS_MULTICOLUMN style cannot scroll vertically it ignores any WM_VSCROLL messages it receives.\
The LBS_MULTICOLUMN and LBS_OWNERDRAWVARIABLE styles cannot be combined. If both are specified, LBS_OWNERDRAWVARIABLE is ignored."},
        {LBS_WANTKEYBOARDINPUT,"LBS_WANTKEYBOARDINPUT","Specifies that the owner of the list box receives WM_VKEYTOITEM messages whenever the user presses a key and the list box has the input focus. This enables an application to perform special processing on the keyboard input."},
        {LBS_EXTENDEDSEL,"LBS_EXTENDEDSEL","Allows multiple items to be selected by using the SHIFT key and the mouse or special key combinations."},
        {LBS_DISABLENOSCROLL,"LBS_DISABLENOSCROLL","Shows a disabled horizontal or vertical scroll bar when the list box does not contain enough items to scroll. If you do not specify this style, the scroll bar is hidden when the list box does not contain enough items. This style must be used with the WS_VSCROLL or WS_HSCROLL style."},
        {LBS_NODATA,"LBS_NODATA","Turns string selection on or off each time the user clicks or double-clicks a string in the list box. The user can select any number of strings."},
        {LBS_NOSEL,"LBS_NOSEL","Specifies that the list box contains items that can be viewed but not selected."},
        {LBS_COMBOBOX,"LBS_COMBOBOX","Notifies a list box that it is part of a combo box. This allows coordination between the two controls so that they present a unified UI. The combo box itself must set this style. If the style is set by anything but the combo box, the list box will regard itself incorrectly as a child of a combo box and a failure will result."},
        {LBS_STANDARD,"LBS_STANDARD","Sorts strings in the list box alphabetically. The parent window receives a notification code whenever the user clicks a list box item, double-clicks an item, or cancels the selection. The list box has a vertical scroll bar, and it has borders on all sides. This style combines the LBS_NOTIFY, LBS_SORT, WS_VSCROLL, and WS_BORDER styles."},
        {WS_EX_DLGMODALFRAME,"WS_EX_DLGMODALFRAME","The window has a double border; the window can, optionally, be created with a title bar by specifying the WS_CAPTION style in the dwStyle parameter."},
        {WS_EX_NOPARENTNOTIFY,"WS_EX_NOPARENTNOTIFY","The child window created with this style does not send the WM_PARENTNOTIFY message to its parent window when it is created or destroyed."},
        {WS_EX_TOPMOST,"WS_EX_TOPMOST","The window should be placed above all non-topmost windows and should stay above them, even when the window is deactivated. To add or remove this style, use the SetWindowPos function."},
        {WS_EX_ACCEPTFILES,"WS_EX_ACCEPTFILES","The window accepts drag-drop files."},
        {WS_EX_TRANSPARENT,"WS_EX_TRANSPARENT","The window should not be painted until siblings beneath the window (that were created by the same thread) have been painted. The window appears transparent because the bits of underlying sibling windows have already been painted.\
To achieve transparency without these restrictions, use the SetWindowRgn function."},
        {WS_EX_MDICHILD,"WS_EX_MDICHILD","he window is a MDI child window."},
        {WS_EX_TOOLWINDOW,"WS_EX_TOOLWINDOW","The window is intended to be used as a floating toolbar. A tool window has a title bar that is shorter than a normal title bar, and the window title is drawn using a smaller font. A tool window does not appear in the taskbar or in the dialog that appears when the user presses ALT+TAB. If a tool window has a system menu, its icon is not displayed on the title bar. However, you can display the system menu by right-clicking or by typing ALT+SPACE."},
        {WS_EX_WINDOWEDGE,"WS_EX_WINDOWEDGE","The window has a border with a raised edge."},
        {WS_EX_CLIENTEDGE,"WS_EX_CLIENTEDGE","The window has a border with a sunken edge."},
        {WS_EX_CONTEXTHELP,"WS_EX_CONTEXTHELP","The title bar of the window includes a question mark. When the user clicks the question mark, the cursor changes to a question mark with a pointer. If the user then clicks a child window, the child receives a WM_HELP message. The child window should pass the message to the parent window procedure, which should call the WinHelp function using the HELP_WM_HELP command. The Help application displays a pop-up window that typically contains help for the child window.\
WS_EX_CONTEXTHELP cannot be used with the WS_MAXIMIZEBOX or WS_MINIMIZEBOX styles."},
        {WS_EX_RIGHT,"WS_EX_RIGHT","(WS_EX_WINDOWEDGE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST)\
The window is palette window, which is a modeless dialog box that presents an array of commands."},
        {WS_EX_LEFT,"WS_EX_LEFT","The window has generic left-aligned properties. This is the default."},
        {WS_EX_RTLREADING,"WS_EX_RTLREADING","If the shell language is Hebrew, Arabic, or another language that supports reading-order alignment, the window text is displayed using right-to-left reading-order properties. For other languages, the style is ignored."},
        {WS_EX_LTRREADING,"WS_EX_LTRREADING","The window text is displayed using left-to-right reading-order properties. This is the default."},
        {WS_EX_LEFTSCROLLBAR,"WS_EX_LEFTSCROLLBAR","If the shell language is Hebrew, Arabic, or another language that supports reading order alignment, the vertical scroll bar (if present) is to the left of the client area. For other languages, the style is ignored."},
        {WS_EX_RIGHTSCROLLBAR,"WS_EX_RIGHTSCROLLBAR","The vertical scroll bar (if present) is to the right of the client area. This is the default."},
        {WS_EX_CONTROLPARENT,"WS_EX_CONTROLPARENT","The window itself contains child windows that should take part in dialog box navigation. If this style is specified, the dialog manager recurses into children of this window when performing navigation operations such as handling the TAB key, an arrow key, or a keyboard mnemonic."},
        {WS_EX_STATICEDGE,"WS_EX_STATICEDGE","The window has a three-dimensional border style intended to be used for items that do not accept user input."},
        {WS_EX_APPWINDOW,"WS_EX_APPWINDOW","Forces a top-level window onto the taskbar when the window is visible."},
        {WS_EX_OVERLAPPEDWINDOW,"WS_EX_OVERLAPPEDWINDOW","The window is an overlapped window."},
        {WS_EX_PALETTEWINDOW,"WS_EX_PALETTEWINDOW","(WS_EX_WINDOWEDGE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST)\
The window is palette window, which is a modeless dialog box that presents an array of commands."},
        {WS_EX_LAYERED,"WS_EX_LAYERED","The window is a layered window. This style cannot be used if the window has a class style of either CS_OWNDC or CS_CLASSDC.\
Windows 8: The WS_EX_LAYERED style is supported for top-level windows and child windows. Previous Windows versions support WS_EX_LAYERED only for top-level windows."},
        {WS_EX_NOINHERITLAYOUT,"WS_EX_NOINHERITLAYOUT","The window does not pass its window layout to its child windows."},
        {WS_EX_NOREDIRECTIONBITMAP,"WS_EX_NOREDIRECTIONBITMAP","The window does not render to a redirection surface. This is for windows that do not have visible content or that use mechanisms other than surfaces to provide their visual."},
        {WS_EX_LAYOUTRTL,"WS_EX_LAYOUTRTL","If the shell language is Hebrew, Arabic, or another language that supports reading order alignment, the horizontal origin of the window is on the right edge. Increasing horizontal values advance to the left."},
        {WS_EX_COMPOSITED,"WS_EX_COMPOSITED","Paints all descendants of a window in bottom-to-top painting order using double-buffering. Bottom-to-top painting order allows a descendent window to have translucency (alpha) and transparency (color-key) effects, but only if the descendent window also has the WS_EX_TRANSPARENT bit set. Double-buffering allows the window and its descendents to be painted without flicker. This cannot be used if the window has a class style of either CS_OWNDC or CS_CLASSDC.\
Windows 2000: This style is not supported."},
        {WS_EX_NOACTIVATE,"WS_EX_NOACTIVATE","A top-level window created with this style does not become the foreground window when the user clicks it. The system does not bring this window to the foreground when the user minimizes or closes the foreground window.\
The window should not be activated through programmatic access or via keyboard navigation by accessible technology, such as Narrator.\
To activate the window, use the SetActiveWindow or SetForegroundWindow function.\
The window does not appear on the taskbar by default. To force the window to appear on the taskbar, use the WS_EX_APPWINDOW style.."}
    };

    //Affectation to a global variable
    for(int i=0;i<MAX_STYLES_DESC;i++)
        g_styleDescription[i]=styleDesc[i];
}
BOOL CALLBACK AboutDlgProc (HWND hDlg, UINT message,
 WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG :
        return TRUE ;//SetFocus au premier child qui a TABSTOP


    case WM_COMMAND :
        switch (LOWORD (wParam))
        {
        case IDOK :
            EndDialog (hDlg, 0) ;
            return TRUE ;
        }
        break ;

    }
    return FALSE ;
}

void MyCreateTempTxtFile(HRSRC hResInfo,TCHAR szPathFile[])
{

    //Create a temporary file :temp.txt, and fills it with a content of
    //Gco's resource in hResInfo

    if(!hResInfo)
    {
        MyPerror("kgskgospg");
        MyMsgError();
        exit(EXIT_FAILURE);
    }

    HINSTANCE hinst=(HINSTANCE)GetWindowLongPtr(hwndZC,GWLP_HINSTANCE);
    if(!hinst)
    {
        MyPerror("vkodkogkd");
        MyMsgError();
        exit(EXIT_FAILURE);
    }

    HGLOBAL hResource=LoadResource(hinst,hResInfo);
    if(!hResource)
    {
        MyPerror("dgh5d4645");
        MyMsgError();
        exit(EXIT_FAILURE);
    }


   LPVOID pData=LockResource(hResource);
    if(!pData)
    {
        MyPerror("kssd4n8ter68y");
        MyMsgError();
        exit(EXIT_FAILURE);
    }

    TCHAR szNameFile[]="temp.txt";

    //Get path trough aPPDATA
    //Create GCO's folder if not already erxists
    TCHAR szPathDestFile[1024]="";

    if(GetAppDataPath(szPathDestFile))
    {
        if(!(CreateDirectory(strcat(szPathDestFile,"GCO\\"), NULL) ||
                GetLastError() == ERROR_ALREADY_EXISTS))
        {
            MyPerror("kvlds;fkds;");
            return;
        }

        strcat(szPathDestFile,szNameFile);

    }
    else
    {
        MyPerror("vjdkjlsj");
        return;
    }

    DWORD nbBlock=SizeofResource(hinst,hResInfo);
    FILE* file=fopen(szPathDestFile,"wb");
    if(!file)
    {
        MyPerror("sfjkjlsfs");
        exit(EXIT_FAILURE);
    }

    fwrite(pData,1,nbBlock,file);
    fclose(file);

    strcpy(szPathFile,szPathDestFile);
    FreeResource(hResource);

}

void MyPerror(TCHAR *szKeyError)
{
    //Puts keyError in addData log and in debug screen
    perror(szKeyError);//puts in debug 's screen

    TCHAR szPathFile[1024]="";

    if(!GetAppDataPath(szPathFile))
    {
        perror("kslklkd");
        exit(EXIT_FAILURE);
    }

    if(!(CreateDirectory(strcat(szPathFile,"GCO\\"),NULL) ||
            GetLastError()==ERROR_ALREADY_EXISTS))
    {
        perror("kdslfklv");
        exit(EXIT_FAILURE);
    }
    strcat(szPathFile,"GCO_Err.txt");

    FILE *file=fopen(szPathFile,"a+");
    if(!file)
    {
        perror("vkldskf;dlsf");
        exit(EXIT_FAILURE);
    }

    fseek(file,SEEK_END,0);
    fprintf(file,"%s\n",szKeyError);
    fclose(file);
}

BOOL MyIsThisExtFile(TCHAR *szFileName,TCHAR *szNameExtension)
{
    //Check is szFileName which also hsa an extension
    //is a .gco's File

    //1:Get extension
    if(!_tcslen(szNameExtension))
    {
        MyPerror("kleke;kr");
        return FALSE;
    }
    if(!szFileName ||!_tcslen(szFileName))
    {
        MyPerror("kvslkf;");
        return FALSE;
    }
    TCHAR szCopie[_tcslen(szFileName)+1];

    _tcscpy(szCopie,szFileName);

    RemoveQuotes(szCopie);

    PTCH pszExt1=szNameExtension;
    PTCH pszExt2=_tcsrchr(szCopie,'.');
    if(!pszExt2 || _tcslen(pszExt1)!=_tcslen(pszExt2))
        return FALSE;

    while(*pszExt1 && *pszExt2 )
    {
        if(tolower(*pszExt1)!=tolower(*pszExt2))
        {
            return FALSE;
        }
        pszExt1++;
        pszExt2++;
    }
    return TRUE;
}

