/*Nom du File:projetManagement.c
Role:Gere toutes les taches de management du projet(sauvegarde,
Activation,Suppression de projet
BY CHRISTIAN EDOUARDO LE 24/11/2024 a 17h19*/
#include <windows.h>
#include "manageProjet.h"
#include "toolBar.h"
#include <commdlg.h>
PROJETSTRUCT projet;
static OPENFILENAME ofn ;
HWND hwndLastZC;
BOOL MyCreateNewProjet(void)
{//1fonction pour entammer la procedure de creation d'un nouveau projet
    //Si elle aboutit, la structure projet.desc.contient les infos sur le projet qui sera cree

    INT_PTR iResult;
    iResult=DialogBoxParamW((HINSTANCE)GetWindowLongPtr(hwndZC,GWLP_HINSTANCE)
        ,MAKEINTRESOURCEW(IDD_NEW_PROJET),
        GetParent(hwndZC),(DLGPROC)ProjectManageProc,
        0);
    if(iResult==IDCANCEL) return FALSE;
    return TRUE;
}

LRESULT CALLBACK ProjectManageProc(HWND hDlg,UINT msg,WPARAM wParam,LPARAM lParam)
{
    static HWND hEditPathProj,hEditNameFileProj,
    hEditNameProj;

    switch(msg)
    {
    case WM_INITDIALOG:
        MySetAppIconToWnd(hDlg);
        {
        //centrer sur hDlgMenu
        RECT rcParent, rcDlg;
        HWND hDlgParent=GetParent(hDlg);
        ShowWindow(hDlg,SW_HIDE);
        GetWindowRect(hDlgParent, &rcParent);
        GetWindowRect(hDlg, &rcDlg);

        int dlgWidth = rcDlg.right - rcDlg.left;
        int dlgHeight = rcDlg.bottom - rcDlg.top;

        int x = rcParent.left + (rcParent.right - rcParent.left) / 2 - dlgWidth / 2;
        int y = rcParent.top + (rcParent.bottom - rcParent.top) / 2 - dlgHeight / 2;
        SetWindowPos(hDlg, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        ShowWindow(hDlg,SW_NORMAL);
        }
        //Get hwnd Edits
        hEditNameFileProj=GetDlgItem(hDlg,IDE_PROJET_FILE_NAME);
        hEditNameProj=GetDlgItem(hDlg,IDE_PROJET_NAME);
        hEditPathProj=GetDlgItem(hDlg,IDE_PROJET_FOLDER_NAME);
        if(!hEditNameFileProj ||!hEditNameProj ||
           !hEditPathProj)
        {
            MyPerror("M SDFKKD");

        }

        //Charger le recent chemin ou creer le dossier du projet
        if(!PathFileExists(g_szPathFolder))
            if(!GetDocumentsPath(g_szPathFolder))
                MyPerror("m apotiot");
        SetWindowText(hEditPathProj,g_szPathFolder);
        return FALSE;

    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case IDB_SELPATH://Bouton pour parcourir
            BrowseForFolder(hDlg,hEditPathProj);
            return TRUE;

        case IDE_PROJET_NAME:
            if(HIWORD(wParam)==EN_CHANGE)
            {
                char szName[1024];
                GetWindowText((HWND)lParam,szName,1024);
                strcat(szName,".gco");
                SetWindowText(hEditNameFileProj,szName);
            }
            return TRUE;

        case ID_FINISH:
            //Verifier que tous les champs sont remplis
            //Si oui, alors save les infos sur le nouveau projet a creer
            {
            char szName[1024];
            //Nom du Projet
            GetWindowText(hEditNameProj,szName,1024);
            if(!*szName)
            {
                MessageBox(hDlg,TEXT("Please Fill <Projet Title> 's Option"),
                           TEXT("ERROR"),MB_ICONERROR|MB_TASKMODAL|MB_OK);
                return TRUE;
            }
            //Nom du dossier du projet
            strcpy(projet.descProjet.szNameFolder,szName);


            //Nom du CHEMIN  du dossier ou creer le projet
            GetWindowText(hEditPathProj,szName,1024);
            if(!*szName)
            {
                MessageBox(hDlg,TEXT("Please Fill <Create In> 's Option"),
                           TEXT("ERROR"),MB_ICONERROR|MB_TASKMODAL|MB_OK);
                return TRUE;
            }
               strcpy(g_szPathFolder,szName);
               //Nom du chemin vers le dossier du projet
               strcpy(projet.descProjet.szFolderPath,g_szPathFolder);

           //Nom du File du Porjet
            GetWindowText(hEditNameFileProj,szName,1024);
            if(!*szName)
            {
                MessageBox(hDlg,TEXT("Please Fill <Projet FileName> 's Option"),
                           TEXT("ERROR"),MB_ICONERROR|MB_TASKMODAL|MB_OK);
                return TRUE;
            }

            //Nom du File projet.gco
            strcpy(projet.descProjet.szNameFile,szName);

            //Creer le Dossier du projet, ainsi que le File du projet dans le Dossier
            if(!CreateDirectoryIfValid(g_szPathFolder,szName))
                return TRUE;
            }
        case IDCANCEL:
            EndDialog(hDlg,LOWORD(wParam));
            return TRUE;
        }
        break;
    }
    return FALSE;
}

// Fonction pour obtenir le chemin absolu vers "Mes Documents"
BOOL GetDocumentsPath(char* path)
{
    if (SHGetSpecialFolderPath(NULL, path, CSIDL_MYDOCUMENTS, FALSE))
    {
        // Si la récupération réussit, nous avons le chemin des "Mes Documents"
        // Ajouter un séparateur de dossier à la fin pour permettre à l'utilisateur de saisir un sous-dossier
        strcat(path, "\\");
    }
    else
        return FALSE;

    return TRUE;
}

// Fonction pour obtenir le chemin absolu vers "AppData"
BOOL GetAppDataPath(char* path)
{
    if (SHGetSpecialFolderPath(NULL, path, CSIDL_APPDATA, FALSE))
    {
        // Si la récupération réussit, nous avons le chemin de "AppData"
        // Ajouter un séparateur de dossier à la fin pour permettre à l'utilisateur de saisir un sous-dossier
        strcat(path, "\\");
    }
    else
        return FALSE;

    return TRUE;
}

// Fonction pour Create le dossier si le chemin est valide
//ainsi que le File du projet
BOOL CreateDirectoryIfValid(const char* szFolderPath,const char* szNameFile)
{
    //szFolder path est le chemin du dossier chhoisi par User
    //On va creer par la suite un nouveau dossier pour le projet
    //et un File dans ce nouveau projet cree

    char *szCopiePath=malloc(sizeof(char)*(strlen(szFolderPath)+1) +strlen(szNameFile)+3);
    if(!szCopiePath)
    {
        MyPerror("m apodt");
        return FALSE;
    }
    strcpy(szCopiePath,szFolderPath);
    strcat(szCopiePath,projet.descProjet.szNameFolder);

    if (CreateDirectory(szCopiePath, NULL) || GetLastError() == ERROR_ALREADY_EXISTS)
    {

        //Creer le File
        int taille=strlen(szNameFile)+strlen(szCopiePath)+3;
        char *szchaine=malloc(sizeof(char)*taille);
        if(!szchaine)
        {
            MyPerror("m vdokg");
            free(szCopiePath);
            return FALSE;
        }
        strcpy(szchaine,szCopiePath);
        strcat(szchaine,"\\");
        strcat(szchaine,szNameFile);

        if(PathFileExists(szchaine))
        {
            int iResult;
            char szBuf[strlen(szchaine)+1024];

            wsprintf(szBuf,"This File: (%s) already exist,do you want to overwrite it? ",szchaine);
            iResult=MessageBox(GetParent(GetFocus()),szBuf,"INFORMATION",MB_YESNO|
                       MB_TASKMODAL|MB_ICONINFORMATION);

            if(iResult==IDNO)
            {
                return FALSE;
            }
        }
        FILE *File=fopen(szchaine,"w+");
        if(!File)
        {
            MyPerror("m dokgoig");
            free(szCopiePath);
            return FALSE;
        }
        //Save le nom du chemin du File
        strcpy(projet.descProjet.szFilePath,szchaine);
        fclose(File);
        free(szCopiePath);
        free(szchaine);

        return TRUE;  // Dossier créé ou déjà existant
    }
    else
    {
        free(szCopiePath);

        MyPerror("vjkdjld");
        return FALSE;
    }
}

// Fonction pour afficher la boîte de dialogue de sélection de dossier
void BrowseForFolder(HWND hwnd, HWND hwndEdit)
{
    // Initialiser la structure de la boîte de dialogue
    BROWSEINFO bi = { 0 };
    bi.lpszTitle = "Select a folder";
    bi.hwndOwner = hwnd;
    bi.ulFlags = BIF_RETURNONLYFSDIRS;  // Sélectionner seulement des dossiers

    // Afficher la boîte de dialogue
    PIDLIST_ABSOLUTE pidl = SHBrowseForFolder(&bi);
    if (pidl != NULL)
    {
        // Récupérer le chemin du dossier sélectionné
        char path[MAX_PATH];
        if (SHGetPathFromIDList(pidl, path))
        {
            strcat(path,"\\");
            SetWindowText(hwndEdit, path);  // Mettre à jour le champ de texte avec le chemin choisi
        }
        CoTaskMemFree(pidl);  // Libérer la mémoire allouée pour le PIDL
    }
}

BOOL MySaveProjet(void)
{
    if(!projet.puObjet)
    {
        MyPerror("meffot");
        return FALSE;
    }

    //Met a jour GCODEF.h, en le remplacant de define
    MyUpdateGCODEF();

    char *psznameFile=projet.descProjet.szFilePath;
    int nbBlock;
    VOID *pDest;
    FILE *File=fopen(psznameFile,"wb");//wb erase aussi
    if(!File)
    {
        MyPerror(" mvoiro");
        char szBuf[1024];
        wsprintf(szBuf,"Unable to Access to <<%s>>. Do you want to save as?",psznameFile);
        int iResult;
        iResult=MessageBox(g_hwndParentObj,szBuf,"Error:",
                   MB_ICONERROR|MB_YESNO|MB_TASKMODAL);

        if(iResult==IDYES)
        {
            SendMessage(GetParent(hwndZC),WM_COMMAND,IDM_SAVEAS,0);
            return TRUE;

        }
        else
        {
            MessageBox(g_hwndParentObj,"This project won't be saved","INFORMATION",
                       MB_OK|MB_ICONINFORMATION|MB_TASKMODAL);
            return TRUE;
        }
        return FALSE;
    }

    //Rempli szPathDefoutput
    nbBlock=strlen(projet.szPathDefFile)+1;
    pDest=&nbBlock;
    fwrite(pDest,sizeof(int),1,File);
    pDest=projet.szPathDefFile;
    fwrite(pDest,sizeof(char),nbBlock,File);

    //Rempli lactuel hwndZC
    pDest=&hwndZC;
    fwrite(pDest,sizeof(HWND),1,File);

    //REMPLISSAGE DU CHAMP .puObjet
    //pUobjet
    nbBlock=projet.puObjet[0].nbObjCreated;

    fwrite(&nbBlock,sizeof(int),1,File);


    //Rempli lactuel iActualId
    pDest=&iActualIdForObjet;
    fwrite(pDest,sizeof(int),1,File);


    for(int i=0;i<projet.puObjet[0].nbObjCreated;i++)
    {
        //Dw style
        pDest=&projet.puObjet[i].dwStyle;
        fwrite(pDest,sizeof(DWORD),1,File);

        //h
        pDest=&projet.puObjet[i].h;
        fwrite(pDest,sizeof(int),1,File);

        //w
        pDest=&projet.puObjet[i].w;
        fwrite(pDest,sizeof(int),1,File);

        //x
        pDest=&projet.puObjet[i].x;
        fwrite(pDest,sizeof(int),1,File);

        //y
        pDest=&projet.puObjet[i].y;
        fwrite(pDest,sizeof(int),1,File);

        //cxChar
        pDest=&projet.puObjet[i].cxChar;
        fwrite(pDest,sizeof(int),1,File);

        //cyChar
        pDest=&projet.puObjet[i].cyChar;
        fwrite(pDest,sizeof(int),1,File);


        //cxRest
        pDest=&projet.puObjet[i].cxRest;
        fwrite(pDest,sizeof(int),1,File);


        //cyRest
        pDest=&projet.puObjet[i].cyRest;
        fwrite(pDest,sizeof(int),1,File);

        //hinst
        pDest=&projet.puObjet[i].hinst;
        fwrite(pDest,sizeof(HINSTANCE),1,File);

        //hwnd
        pDest=&projet.puObjet[i].hwnd;
        fwrite(pDest,sizeof(HWND),1,File);

        //hwndChildGB
        pDest=&projet.puObjet[i].hwndChildGB;
        fwrite(pDest,sizeof(HWND),1,File);

        //hwndParent
        pDest=&projet.puObjet[i].hwndParent;
        fwrite(pDest,sizeof(HWND),1,File);

        //id
        pDest=&projet.puObjet[i].id;
        fwrite(pDest,sizeof(int),1,File);

        //isMainParent
        pDest=&projet.puObjet[i].isMainParent;
        fwrite(pDest,sizeof(BOOL),1,File);

        //isVisible
        pDest=&projet.puObjet[i].isVisible;
        fwrite(pDest,sizeof(BOOL),1,File);

        //ItYPEoBJ
        pDest=&projet.puObjet[i].iTypeObj;
        fwrite(pDest,sizeof(int),1,File);

        //NBoBJcREATED
        pDest=&projet.puObjet[i].nbObjCreated;
        fwrite(pDest,sizeof(int),1,File);

        //NewObjWindproc
        pDest=&projet.puObjet[i].newObjWndProc;
        fwrite(pDest,sizeof(WNDPROC),1,File);

        //OldWndProc
        pDest=&projet.puObjet[i].oldObjWndProc;
        fwrite(pDest,sizeof(WNDPROC),1,File);

        //szDescription
        nbBlock=strlen(projet.puObjet[i].szDescription);
        nbBlock+=1;
        pDest=&nbBlock;
        fwrite(pDest,sizeof(int),1,File);

        pDest=projet.puObjet[i].szDescription;
        fwrite(pDest,sizeof(char),nbBlock,File);

        //szLastStyleChoosen
        nbBlock=strlen(projet.puObjet[i].szLastStylesChosen);
        nbBlock+=1;
        pDest=&nbBlock;
        fwrite(pDest,sizeof(int),1,File);

        pDest=projet.puObjet[i].szLastStylesChosen;
        fwrite(pDest,sizeof(char),nbBlock,File);

        //szTitle
        nbBlock=strlen(projet.puObjet[i].szTitle);
        nbBlock+=1;
        pDest=&nbBlock;
        fwrite(pDest,sizeof(int),1,File);

        pDest=projet.puObjet[i].szTitle;
        fwrite(pDest,sizeof(char),nbBlock,File);

        //szNameId
        nbBlock=strlen(projet.puObjet[i].szNameId);
        nbBlock+=1;
        pDest=&nbBlock;
        fwrite(pDest,sizeof(int),1,File);
        pDest=projet.puObjet[i].szNameId;
        fwrite(pDest,sizeof(char),nbBlock,File);

    //CAS DE .linkopt
        //isLinked
        pDest=&projet.puObjet[i].linkopt.isLinked;
        fwrite(pDest,sizeof(BOOL),1,File);

        //nbLink
        //NB dans la fonction lire, il vaut aussi nbBlock pour les champs id et hwndLinkWith
        pDest=&projet.puObjet[i].linkopt.nbLink;
        fwrite(pDest,sizeof(int),1,File);

        //iIndex
        //NB dans la fonction lire, il vaut aussi nbBlock pour les champs id et hwndLinkWith
        pDest=&projet.puObjet[i].linkopt.iLastIndex;
        fwrite(pDest,sizeof(int),1,File);

        //*id
        nbBlock=projet.puObjet[i].linkopt.nbLink;
        pDest=projet.puObjet[i].linkopt.id;
        fwrite(pDest,sizeof(int),nbBlock,File);

        //*hwndLinkWith
        nbBlock=projet.puObjet[i].linkopt.nbLink;
        pDest=projet.puObjet[i].linkopt.hwndLinkWith;
        fwrite(pDest,sizeof(HWND),nbBlock,File);

    //CAS DU CHAMP .mycolorObj
        nbBlock=2;
        pDest=projet.puObjet[i].mycolorobj;
        fwrite(pDest,sizeof(MYCOLOROBJ),nbBlock,File);

    //CAS DU FONT
        pDest=&projet.puObjet[i].font;
        fwrite(pDest,sizeof(MYFONT),1,File);

    //CAS DU CHAMP .menu
       if(!projet.puObjet[i].isMainParent)
            continue;

        //sEUL LE main parent dlgBox a des menus a save
        nbBlock=projet.puObjet[i].menu[0].iLastIndexMenu;
        pDest=&nbBlock;
        fwrite(pDest,sizeof(int),1,File);

        for(int j=0;j<projet.puObjet[i].menu[0].iLastIndexMenu;j++ )
        {
            MYMENULIST *mnl=&projet.puObjet[i].menu[j];

            //idMenu
            pDest=&mnl->idMenu;
            fwrite(pDest,sizeof(int),1,File);

            //iIndexCombo
            pDest=&mnl->iIndexComboCreate;
            fwrite(pDest,sizeof(int),1,File);

            //iIndexParent
            pDest=&mnl->iIndexParent;
            fwrite(pDest,sizeof(int),1,File);

            //iLastIiNDEXmENU
            pDest=&mnl->iLastIndexMenu;
            fwrite(pDest,sizeof(int),1,File);

            //iIndexElt
            pDest=&mnl->iLastIinexElt;
            fwrite(pDest,sizeof(int),1,File);

            //isSubMenu
            pDest=&mnl->isSubMenu;
            fwrite(pDest,sizeof(int),1,File);

            //cas de *tabElt
            nbBlock=mnl->iLastIinexElt;
            pDest=mnl->tabElt;
            fwrite(pDest,sizeof(MENUELT),nbBlock,File);

            //Cas de *tabOrder
            nbBlock=mnl->tabOrder[0].nbOrder;
            pDest=&nbBlock;
            fwrite(pDest,sizeof(int),1,File);

            pDest=mnl->tabOrder;
            fwrite(pDest,sizeof(MYMENUORDER),nbBlock,File);
        }
    }

    //Save mycache
    nbBlock=NBOBJECT;
    pDest=&nbBlock;
    fwrite(pDest,sizeof(int),1,File);

    for(int i=0;i<NBOBJECT;i++)
    {
        //iLastX
        pDest=&mycache[i].iLastX;
        fwrite(pDest,sizeof(int),1,File);

        //iLastY
        pDest=&mycache[i].iLastY;
        fwrite(pDest,sizeof(int),1,File);

        //iLastW
        pDest=&mycache[i].iLastW;
        fwrite(pDest,sizeof(int),1,File);

        //iLasth
        pDest=&mycache[i].iLastH;
        fwrite(pDest,sizeof(int),1,File);

        //isPixelUnit
        pDest=&mycache[i].isPixelUnit;
        fwrite(pDest,sizeof(BOOL),1,File);

        //dwLastStyleChosen
        pDest=&mycache[i].dwLastStylesChosen;
        fwrite(pDest,sizeof(DWORD),1,File);

        //FONT
        pDest=&mycache[i].font;
        fwrite(pDest,sizeof(MYFONT),1,File);

        //idObj
        pDest=&mycache[i].idObj;
        fwrite(pDest,sizeof(int),1,File);

        //mycolorobj
        pDest=&mycache[i].mycolorobj;
        fwrite(pDest,sizeof(MYCOLOROBJ),2,File);

        //szLastStylechoosen
        nbBlock=strlen(mycache[i].szLastStylesChosen)+1;
        pDest=&nbBlock;
        fwrite(pDest,sizeof(int),1,File);

        pDest=&mycache[i].szLastStylesChosen;
        fwrite(pDest,sizeof(char),nbBlock,File);

        //szTitle
        nbBlock=strlen(mycache[i].szTitle)+1;
        pDest=&nbBlock;
        fwrite(pDest,sizeof(int),1,File);

        pDest=&mycache[i].szTitle;
        fwrite(pDest,sizeof(char),nbBlock,File);

    }

    //Save isFirsttime
    pDest=&isFirstTime;
    fwrite(pDest,sizeof(BOOL),NBOBJECT,File);

    //write isFirstLoad
    pDest=&isFirstLoad;
    fwrite(pDest,sizeof(BOOL),NBOBJECT,File);

    //Save po
    pDest=po;
    nbBlock=projet.puObjet[0].nbObjCreated;
    fwrite(pDest,sizeof(POSOBJ),nbBlock,File);

    //Note this zone isn't read in dll
    //Save g_idItemActu
    pDest=&g_idItemActu;
    fwrite(pDest,sizeof(int),1,File);

    //Save g_idIMenuActu
    pDest=&g_idMenuActu;
    fwrite(pDest,sizeof(int),1,File);

    fclose(File);
    g_fShouldIAskSave=FALSE;
    return TRUE;
}

BOOL MySaveAsProjet(HWND hwndOwner)
{
    //Pour l'instant ,le saveAs ne s'effectue que sur le porject actuellement actif
    TCHAR szFilePath[10224]="";
    TCHAR szNameFile[1024]="";
    char szLast[1024];

    strcpy(szLast,projet.descProjet.szFilePath);

    if(PopFileSaveDlg(hwndOwner,szFilePath,szNameFile))
    {
        //1:Save lactuelle projet active sous son ancien path
        MySaveProjet();
        //Charge le nouveau path specifie par User
        strcpy(projet.descProjet.szFilePath,szFilePath);
        MyGetProjetDescWithPathFile(szFilePath,&projet.descProjet);
        MySaveProjet();//sauve le saveAs gco's file modifie

        //2:Supprime cet ancien path de larbre
        strcpy(projet.descProjet.szFilePath,szLast);
        MyGetProjetDescWithPathFile(szLast,&projet.descProjet);
        SendMessage(GetParent(hwndZC),WM_COMMAND,(WPARAM)IDM_CLOSE_PROJECT,0);
        //ReCharge le nouveau path specifie par User
        strcpy(projet.descProjet.szFilePath,szFilePath);
        MyGetProjetDescWithPathFile(szFilePath,&projet.descProjet);

        //Simule un double clique exterieur d'un fichier .gco poour ne pas
        //demander a user de selectionner un fichier a ouvrir,car nous lavons deja dans project.desc
        g_IsDoubleClickedGcoFile=TRUE;
        MyManageOpenGCOfiles(GetParent(hwndZC));
        g_IsDoubleClickedGcoFile=FALSE;//renitialisation

        return TRUE;
    }

    return FALSE;
}

BOOL MyOpenProjet(HWND hwndOwner)
{
    //Cette fonction recupere juste le nom du File
    //selectionner par user
    //le chargement du projet se fera explicitement via activateProjet
    char  szPathFile[1024]="", szFileName[1024]="";

    if(PopFileOpenDlg(hwndOwner,szPathFile,
                      szFileName))
    {
        //Si User a choisi unFile
        //Ses informations sontdonc deja dans projet
        //Loader  le File projet dans uObjet

        if(!PathFileExists(szPathFile))
        {
            char szBuf[1024];
            wsprintf(szBuf,"We can't see This file : <<%s>>",szPathFile);
            MessageBox(NULL,szBuf,"Error",MB_ICONERROR|MB_TASKMODAL|MB_OK);
            return
            FALSE;
        }

        //Si les chemin sont valid, alor copier
        strcpy(projet.descProjet.szFilePath,szPathFile);
        strcpy(projet.descProjet.szNameFile,szFileName);

        MyGetNameFolderProjet(projet.descProjet.szFilePath,projet.descProjet.szNameFolder);

    }
    else
    {
        return FALSE;
    }
    return TRUE;
}

BOOL MyLoadProjet(void)
{
    //Recuperer les informations du File projet pointe par projet.desc
    //charger uObjet avec les infos de ce File
    FILE *File;
    char *pszNameFile=projet.descProjet.szFilePath;


    File=fopen(pszNameFile,"rb");
    if(!File)
    {
        MyPerror("m ofodi");
        char szRep[1024];
        wsprintf(szRep,"Unable to Open %s",pszNameFile);
        MessageBox(NULL,szRep,"Error",MB_ICONERROR|MB_OK|MB_TASKMODAL);
        return FALSE;
    }
    int nbBlock=0;
    VOID *pDest;

    //lire szPathDefoutput
    pDest=&nbBlock;
    fread(pDest,sizeof(int),1,File);
    pDest=projet.szPathDefFile;
    fread(pDest,sizeof(char),nbBlock,File);

    //lire  lancien hwndZC
    pDest=&hwndLastZC;
    fread(pDest,sizeof(HWND),1,File);
    //REMPLISSAGE DU CHAMP .puObjet
    //pUobjet
    nbBlock=0;
    fread(&nbBlock,sizeof(int),1,File);
    int nbObj=nbBlock;

    if(nbObj<0)
    {
        fclose(File);
        MyPerror("\nprojet vide");
        return FALSE;
    }
    else if(nbObj==0)
    {
        nbObj=MIN_OBJ_CREABLE;
        MyPerror("projet vide, nbObj=0");
    }

    MyDeleteObj(-1);
    free(po);
    if(!MyFreeuObjet(&uObjet))
    {
        MyPerror(" modsioid");
        return FALSE;
    }

    //Allouer po et uObjet et pointer sur projet

    uObjet=malloc(sizeof(UNIVERS)*nbObj);
    po=malloc(sizeof(POSOBJ)*nbObj);
    if(!uObjet || !po)
    {
        fclose(File);
        MyPerror("vldifiodi");
        return FALSE;
    }
    memset(uObjet,0,sizeof(UNIVERS)*nbObj);
    memset(po,0,sizeof(POSOBJ)*nbObj);
    maxObjectCreable=nbObj;//important pour la creation

    //Affectation
    projet.puObjet=uObjet;

    //Get lancien iActualId
    pDest=&iActualIdForObjet;
    fread(pDest,sizeof(int),1,File);

    for(int i=0;i<nbObj;i++)
    {
        //Dw style
        pDest=&projet.puObjet[i].dwStyle;
        fread(pDest,sizeof(DWORD),1,File);

        //h
        pDest=&projet.puObjet[i].h;
        fread(pDest,sizeof(int),1,File);

        //w
        pDest=&projet.puObjet[i].w;
        fread(pDest,sizeof(int),1,File);

        //x
        pDest=&projet.puObjet[i].x;
        fread(pDest,sizeof(int),1,File);

        //y
        pDest=&projet.puObjet[i].y;
        fread(pDest,sizeof(int),1,File);

        //cxChar
        pDest=&projet.puObjet[i].cxChar;
        fread(pDest,sizeof(int),1,File);

        //cyChar
        pDest=&projet.puObjet[i].cyChar;
        fread(pDest,sizeof(int),1,File);

        //cxRest
        pDest=&projet.puObjet[i].cxRest;
        fread(pDest,sizeof(int),1,File);

        //cyRest
        pDest=&projet.puObjet[i].cyRest;
        fread(pDest,sizeof(int),1,File);

        //hinst
        pDest=&projet.puObjet[i].hinst;
        fread(pDest,sizeof(HINSTANCE),1,File);

        //hwnd
        pDest=&projet.puObjet[i].hwnd;
        fread(pDest,sizeof(HWND),1,File);

        //hwndChildGB
        pDest=&projet.puObjet[i].hwndChildGB;
        fread(pDest,sizeof(HWND),1,File);

        //hwndParent
        pDest=&projet.puObjet[i].hwndParent;
        fread(pDest,sizeof(HWND),1,File);

        //id
        pDest=&projet.puObjet[i].id;
        fread(pDest,sizeof(int),1,File);

        //isMainParent
        pDest=&projet.puObjet[i].isMainParent;
        fread(pDest,sizeof(BOOL),1,File);

        //isVisible
        pDest=&projet.puObjet[i].isVisible;
        fread(pDest,sizeof(BOOL),1,File);

        //ItYPEoBJ
        pDest=&projet.puObjet[i].iTypeObj;
        fread(pDest,sizeof(int),1,File);

        //NBoBJcREATED
        pDest=&projet.puObjet[i].nbObjCreated;
        fread(pDest,sizeof(int),1,File);

        //NewObjWindproc
        pDest=&projet.puObjet[i].newObjWndProc;
        fread(pDest,sizeof(WNDPROC),1,File);

        //OldWndProc
        pDest=&projet.puObjet[i].oldObjWndProc;
        fread(pDest,sizeof(WNDPROC),1,File);

        //szDescription
        pDest=&nbBlock;
        fread(pDest,sizeof(int),1,File);

        pDest=projet.puObjet[i].szDescription;
        fread(pDest,sizeof(char),nbBlock,File);

        //szLastStyleChoosen
        pDest=&nbBlock;
        fread(pDest,sizeof(int),1,File);

        pDest=projet.puObjet[i].szLastStylesChosen;
        fread(pDest,sizeof(char),nbBlock,File);

        //szTitle
        pDest=&nbBlock;
        fread(pDest,sizeof(int),1,File);

        pDest=projet.puObjet[i].szTitle;
        fread(pDest,sizeof(char),nbBlock,File);

        //szNameId
        pDest=&nbBlock;
        fread(pDest,sizeof(int),1,File);

        pDest=projet.puObjet[i].szNameId;
        fread(pDest,sizeof(char),nbBlock,File);

    //CAS DE .linkopt
        //isLinked
        pDest=&projet.puObjet[i].linkopt.isLinked;
        fread(pDest,sizeof(BOOL),1,File);

        //nbLink
        //NB dans la fonction lire, il vaut aussi nbBlock pour les champs id et hwndLinkWith
        pDest=&projet.puObjet[i].linkopt.nbLink;
        fread(pDest,sizeof(int),1,File);

        //iIndex
        //NB dans la fonction lire, il vaut aussi nbBlock pour les champs id et hwndLinkWith
        pDest=&projet.puObjet[i].linkopt.iLastIndex;
        fread(pDest,sizeof(int),1,File);

        //*id
        nbBlock=projet.puObjet[i].linkopt.nbLink;//projet,nbLink vient detre lu
        //Allocation
        uObjet[i].linkopt.hwndLinkWith=malloc(sizeof(HWND)*nbBlock);
        uObjet[i].linkopt.id=malloc(sizeof(int)*nbBlock);
        if(!uObjet[i].linkopt.hwndLinkWith ||
           !uObjet[i].linkopt.id)
        {
            fclose(File);
            MyPerror("m poifoigg");
            return FALSE;
        }
        memset(uObjet[i].linkopt.hwndLinkWith,0,sizeof(HWND)*nbBlock);
        memset(uObjet[i].linkopt.id,0,sizeof(int)*nbBlock);

        pDest=projet.puObjet[i].linkopt.id;
        fread(pDest,sizeof(int),nbBlock,File);

        //*hwndLinkWith
        nbBlock=projet.puObjet[i].linkopt.nbLink;

        pDest=projet.puObjet[i].linkopt.hwndLinkWith;
        fread(pDest,sizeof(HWND),nbBlock,File);

    //CAS DU CHAMP .mycolorObj
        nbBlock=2;
        pDest=projet.puObjet[i].mycolorobj;
        fread(pDest,sizeof(MYCOLOROBJ),nbBlock,File);

        //CAS DU FONT
        pDest=&projet.puObjet[i].font;
        fread(pDest,sizeof(MYFONT),1,File);

    //CAS DU CHAMP .menu
        if(!projet.puObjet[i].isMainParent)
            continue;

        //Seul le mainParent a une zoneMenu
        //Allouer le champs menu de uObjet
        uObjet[i].menu=NULL;
        if(!MyAllocMenuTab(&uObjet[i].menu))
        {
            fclose(File);
            MyPerror("m[odsfpdof");
            return FALSE;
        }

        pDest=&nbBlock;
        fread(pDest,sizeof(int),1,File);
        int nbMenus=nbBlock;
        for(int j=0;j<nbMenus;j++ )
        {

            MYMENULIST *mnl=&projet.puObjet[i].menu[j];

            //idMenu
            pDest=&mnl->idMenu;//JAI PEUR
            fread(pDest,sizeof(int),1,File);
            //iIndexCombo
            pDest=&mnl->iIndexComboCreate;
            fread(pDest,sizeof(int),1,File);

            //iIndexParent
            pDest=&mnl->iIndexParent;
            fread(pDest,sizeof(int),1,File);

            //iLastIiNDEXmENU
            pDest=&mnl->iLastIndexMenu;
            fread(pDest,sizeof(int),1,File);

            //iIndexElt
            pDest=&mnl->iLastIinexElt;
            fread(pDest,sizeof(int),1,File);

            //isSubMenu
            pDest=&mnl->isSubMenu;
            fread(pDest,sizeof(int),1,File);

            //cas de *tabElt
            nbBlock=mnl->iLastIinexElt;//deja lu plus haut
            pDest=mnl->tabElt;
            fread(pDest,sizeof(MENUELT),nbBlock,File);

            //Cas de *tabOrder
            pDest=&nbBlock;
            fread(pDest,sizeof(int),1,File);

            pDest=mnl->tabOrder;
            fread(pDest,sizeof(MYMENUORDER),nbBlock,File);
        }
    }


    //lire mycache
    pDest=&nbBlock;
    fread(pDest,sizeof(int),1,File);

    int nbTypeObj=nbBlock;
    for(int i=0;i<nbTypeObj;i++)
    {
        //iLastX
        pDest=&mycache[i].iLastX;
        fread(pDest,sizeof(int),1,File);

        //iLastY
        pDest=&mycache[i].iLastY;
        fread(pDest,sizeof(int),1,File);

        //iLastW
        pDest=&mycache[i].iLastW;
        fread(pDest,sizeof(int),1,File);

        //iLasth
        pDest=&mycache[i].iLastH;
        fread(pDest,sizeof(int),1,File);

        //isPixelUnit
        pDest=&mycache[i].isPixelUnit;
        fread(pDest,sizeof(BOOL),1,File);

        //dwLastStyleChosen
        pDest=&mycache[i].dwLastStylesChosen;
        fread(pDest,sizeof(DWORD),1,File);

        //FONT
        pDest=&mycache[i].font;
        fread(pDest,sizeof(MYFONT),1,File);

        //idObj
        pDest=&mycache[i].idObj;
        fread(pDest,sizeof(int),1,File);

        //mycolorobj
        pDest=&mycache[i].mycolorobj;
        fread(pDest,sizeof(MYCOLOROBJ),2,File);

        //szLastStylechoosen
        pDest=&nbBlock;
        fread(pDest,sizeof(int),1,File);

        pDest=&mycache[i].szLastStylesChosen;
        fread(pDest,sizeof(char),nbBlock,File);

        //szTitle
        pDest=&nbBlock;
        fread(pDest,sizeof(int),1,File);

        pDest=&mycache[i].szTitle;
        fread(pDest,sizeof(char),nbBlock,File);

    }

    //Read isFirsttime
    pDest=&isFirstTime;
    fread(pDest,sizeof(BOOL),NBOBJECT,File);

    //Read isFirstLoad
    pDest=&isFirstLoad;
    fread(pDest,sizeof(BOOL),NBOBJECT,File);

    //lire po
    pDest=po;
    nbBlock=projet.puObjet[0].nbObjCreated;
    if(nbBlock>0)
        fread(pDest,sizeof(POSOBJ),nbBlock,File);

    //Note this zone isn't read in dll
    //read g_idItemActu
    pDest=&g_idItemActu;
    fread(pDest,sizeof(int),1,File);

    //read g_idIMenuActu
    pDest=&g_idMenuActu;
    fread(pDest,sizeof(int),1,File);

    fclose(File);

    //Charger uObjet
    MyLoadUnivers();

    return TRUE;
}


BOOL MyLoadUnivers(void)
{
    //Le role de cette fonction est de charger uObjet sur hwndZc;
    if(!hwndZC || !uObjet || (!hwndLastZC && uObjet[0].nbObjCreated>0))
    {
        MyPerror("ODOSDKF");
        return FALSE;
    }
    PSTR    pszClassName=NULL;
    EnableWindow(hwndZC,TRUE);

    isTypeModDlg=TRUE;
    int iIndexParent=-1,idDlg=0;
    int nbObj=uObjet[0].nbObjCreated;
    g_fShouldApplyFontOnChilds=FALSE;

    g_hwndParentObj=NULL;//IMPORTANT
    for(int i=0;i<nbObj;i++)
    {
        if( uObjet[i].hwndParent==hwndLastZC )
        {
            //Creation du mainParent
            iIndexParent=i;
            if(uObjet[iIndexParent].isMainParent)
            {
                pszClassName=szClassZC;
                isParentWnd=TRUE;
                idDlg=DLG_DIALOGBOX;
            }
            else
            {
                isParentWnd=FALSE;
                switch(uObjet[i].iTypeObj)
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
                    //g_hwndParentObjToSet=hwnd;
                    MySetLastParentObj(uObjet[iIndexParent].hwnd,NULL);
                    break;

                case OBJ_IMGSTATIC:
                    pszClassName=NULL;
                    idDlg=DLG_IMGSTATIC;
                    break;
                }
            }

            strcpy(mycache[idDlg].szLastStylesChosen,uObjet[iIndexParent].szLastStylesChosen);
            mycache[idDlg].dwLastStylesChosen=uObjet[iIndexParent].dwStyle;
            mycache[idDlg].iLastX=max(0,uObjet[iIndexParent].x);
            mycache[idDlg].iLastY=max(0,uObjet[iIndexParent].y);
            mycache[idDlg].iLastW=uObjet[iIndexParent].w;
            mycache[idDlg].iLastH=uObjet[iIndexParent].h;
            mycache[idDlg].cxChar=uObjet[iIndexParent].cxChar;
            mycache[idDlg].cyChar=uObjet[iIndexParent].cyChar;
            mycache[idDlg].cxRest=uObjet[iIndexParent].cxRest;
            mycache[idDlg].cyRest=uObjet[iIndexParent].cyRest;

            mycache[idDlg].mycolorobj[BK]=uObjet[iIndexParent].mycolorobj[BK];
            mycache[idDlg].mycolorobj[TXT]=uObjet[iIndexParent].mycolorobj[TXT];
            mycache[idDlg].font=uObjet[iIndexParent].font;
            _tcscpy(mycache[idDlg].szTitle,uObjet[iIndexParent].szTitle);
            _tcscpy(mycache[idDlg].szNameId,uObjet[iIndexParent].szNameId);
            mycache[idDlg].fShouldAutosize=uObjet[iIndexParent].fShouldAutosize;
            mycache[idDlg].linkopt=uObjet[iIndexParent].linkopt;

            if(uObjet[iIndexParent].isMainParent)
            {
                if(!MyCopieMenuTab(uObjet[iIndexParent].menu,&mycache[idDlg].menu))
                {
                    MyPerror("vfsdfhkhj");
                    return FALSE;
                }

            }
            g_hwndPrevParentObj=uObjet[iIndexParent].hwnd;//A revoir
            g_hwndParentObj=NULL;
            idObjToDelete=uObjet[iIndexParent].id;
            isTypeModDlg=TRUE;
            MyDeleteObj(idObjToDelete);

           HWND hwndObjet=CreateObjet(pszClassName,idDlg,hwndZC);
            if(!hwndObjet)
            {
                MessageBox(NULL,"Unable to create this object",NULL,
                MB_ICONERROR);
            }
            i--;//important
            nbObj--;//important

        }
    }

    MyUpdateLinksUniv();
    isParentWnd=FALSE;
    isTypeModDlg=FALSE;
    idObjToDelete=0;
    return TRUE;
}

void PopFileInitialize (HWND hwnd)
{
    static TCHAR szFilter[] = TEXT ("GCO Files (*.GCO)\0*.gco*\0")\
        TEXT ("All Files (*.*)\0*.*\0\0") ;
    char *ppppppp=NULL;
    GetDocumentsPath(ppppppp);

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof (OPENFILENAME) ;
    ofn.hwndOwner = hwnd ;
    ofn.hInstance = NULL ;
    ofn.lpstrFilter = szFilter ;
    ofn.lpstrCustomFilter = NULL ;
    ofn.nMaxCustFilter = 0 ;
    ofn.nFilterIndex = 0 ;
    ofn.lpstrFile = NULL ; // Set in Open and Close functions
    ofn.nMaxFile = MAX_PATH ;
    ofn.lpstrFileTitle = NULL ; // Set in Open and Close functions
    ofn.nMaxFileTitle = MAX_PATH ;
    ofn.lpstrInitialDir = ppppppp ;
    ofn.lpstrTitle = NULL ;
    ofn.Flags = 0 ; // Set in Open and Close functions
    ofn.nFileOffset = 0 ;
    ofn.nFileExtension = 0 ;
    ofn.lpstrDefExt = TEXT ("gco") ;
    ofn.lCustData = 0L ;
    ofn.lpfnHook = NULL ;
    ofn.lpTemplateName = NULL ;
}

BOOL PopFileSaveDlg (HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName)
{
    /*
    Cette fonction affiche une boîte de dialogue
    "Enregistrer sous" standard permettant à l'utilisateur de
    spécifier un nom et un emplacement pour enregistrer un
     File,
     Cette fonction retourne OUI si User a choisi un nom de File
     et FALSE sinon*/
    ofn.hwndOwner = hwnd ;
    ofn.lpstrFile = pstrFileName ;
    ofn.lpstrFileTitle = pstrTitleName ;
    ofn.Flags = OFN_OVERWRITEPROMPT ;
    //OFN_OVERWRITEPROMPT affiche une boite de dialogue
    //si le File existe deja
    //Open La boite de dialogue
    return GetSaveFileName (&ofn) ;
}

BOOL PopFileOpenDlg (HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName)
{//Cette fonction affiche une boite de dialogue Open
    //standard
    //Retroune True si user a choisi un File, et FALSE sinon
    //Les noms de Title et chemin seront stockes dans pstrFileName
    //et pstrTitleName, grace aux membres de la structure ofn, qui pointeront
    //au meme endroit que ces pstr
    ofn.hwndOwner = hwnd ;
    ofn.lpstrFile = pstrFileName ;
    ofn.lpstrFileTitle = pstrTitleName ;
    //HIDEREADONLY cache latt:lecture seule
    //CREATEPROMPT affiche une boite de dialogue si le
    //File nexiste pas, LUI PERMETTANT ainsi de le creer
    //si il le souhaite

    ofn.Flags = OFN_HIDEREADONLY;
    //Ce qui suit realise l'ouverture proprement dite
    return GetOpenFileName (&ofn) ;
}

HTREEITEM AddTreeItem(HWND hwndTreeView, HTREEITEM hParent, LPCSTR lpszText)
{
    TVINSERTSTRUCT tvis;
    ZeroMemory(&tvis, sizeof(tvis));
    tvis.hParent = hParent;
    tvis.hInsertAfter = TVI_FIRST;
    tvis.item.mask = TVIF_TEXT;
    tvis.item.pszText = (LPSTR)lpszText;
    return TreeView_InsertItem(hwndTreeView, &tvis);
}

BOOL MyAddTreeItem(HTREEITEM hItem,DESCRIPTION desc)
{
    //Le role de cette fonction est dajoute un item de tree view(projet)
    //cree ou ouvert par user
    //g_mtv[0].nbProjets contient deja le nombre de projets du tab
    if(!hItem || !PathFileExists(desc.szFilePath)|| !g_mtv)
    {
        if(!hItem) printf("a");
        if(!g_mtv) printf("b");
        else printf("c");
        MyPerror("m sodfdfdsf");
        return FALSE;
    }

    int nbProj=g_mtv[0].nbProjects;

    //reaAllocation
    if(nbProj && !(nbProj%NB_MIN_PROJ_TV))
    {
        nbProj+=NB_MIN_PROJ_TV;
        g_mtv=realloc(g_mtv,sizeof(MYCACHE_TREEVIEW)*nbProj);
        if(!g_mtv)
        {
            MyPerror("msofisd");
            return FALSE;
        }
        memset(g_mtv+g_mtv[0].nbProjects,0,sizeof(MYCACHE_TREEVIEW)*NB_MIN_PROJ_TV);
    }

    nbProj=g_mtv[0].nbProjects;
    g_mtv[nbProj].hItem=hItem;
    g_mtv[nbProj].desc=desc;
    g_mtv[0].nbProjects+=1;

    return TRUE;
}

BOOL MyActivateProjet(HTREEITEM hItem)
{
    //Le role de cette fonction est de parcourir
    //g_mtv, de rechercher hItem, puis
    //De lactiver en ouvrant le File connaissant son path depuis
    //g_mtv
     if(g_hwndParentObj && !g_fisTestRunAble && uObjet[0].nbObjCreated>0)
    {
        //si une instance de test en en cours d'execution
        return TRUE;
    }

    if(!hItem)
    {
        MyPerror(" ofioipoier");
        return FALSE;
    }

    int i;
    for(i=0;i<g_mtv[0].nbProjects;i++)
    {
        if(g_mtv[i].hItem==hItem)
        {
            MyDeleteObj(-1);
            free(po);
            if(!MyFreeuObjet(&uObjet))
            {
                MyPerror("fsdpofpd");
                return FALSE;
            }

            //Allouer po et uObjet et pointer sur projet
            int nbObj=MIN_OBJ_CREABLE;
            uObjet=malloc(sizeof(UNIVERS)*nbObj);
            po=malloc(sizeof(POSOBJ)*nbObj);
            if(!uObjet || !po)
            {
                MyPerror("dspops");
                return FALSE;
            }
            memset(uObjet,0,sizeof(UNIVERS)*nbObj);
            memset(po,0,sizeof(POSOBJ)*nbObj);
            maxObjectCreable=nbObj;//important pour la creation

            if(!MyFreeMenuTab(&mycache[DLG_DIALOGBOX].menu))
            {
                MyPerror("sfsfpof");
                return FALSE;
            }
            if(!MyAllocMenuTab(&mycache[DLG_DIALOGBOX].menu))
            {
                MyPerror("gporpot");
                return FALSE;
            }
            projet.descProjet=g_mtv[i].desc;
            projet.puObjet=uObjet;
            MyEnableButtonToolBar(g_hwndToolBar,ID_STOP_TOOL,TRUE,TRUE);

            g_hwndParentObj=NULL;//renitialise car on souahaite Loader un nouveau projet
            if(!MyLoadProjet())
            {
                printf("\n load error");
                MyPerror(" ooiroewpr");
                return FALSE;
            }
            //Si MyLoad a marche, alors on a deja un projet.desc,pathFile Valid
            //Obtenir les reste de project.desc
            if(!MyGetProjetDescWithPathFile(projet.descProjet.szFilePath,
                        &projet.descProjet))
            {
                MyPerror("jdkflkls");
            }
            //Mettra a jour szPathDefFile
            if(!PathFileExists(projet.szPathDefFile))
            {
                //sI LE dossier ou creer le GcoDef.h existe pas
                if(!PathFileExists(projet.descProjet.szFolderPath))
                {
                    MyPerror("gkokgofkd");
                    MyMsgError();
                    return FALSE;

                }
                strcpy(projet.szPathDefFile,projet.descProjet.szFolderPath);
                //strcat(projet.szPathDefFile,projet.descProjet.szNameFolder);
                 //Ajouter un \ ala fin
                strcat(projet.szPathDefFile,"\\");

            }
            SetWindowText(g_hwndTitleProj,projet.descProjet.szFilePath);

            return TRUE;
        }
    }

    return FALSE;
}

BOOL MyDelTreeItem(int iIndexToDel)
{
    //Cette fonction parcours g_mtv, et retire iIndex=i du tableau
    //Aussi, cette fonction Active le prochain projet ouvert si il
    //en existe
    int nbProjets=g_mtv[0].nbProjects;

    if(iIndexToDel>=nbProjets)
    {
        MyPerror("ksldklg");
        return FALSE;
    }


    //Delete  dans la hierachie TVN
    if(!TreeView_DeleteItem(hwndManage,TreeView_GetParent(hwndManage,g_mtv[iIndexToDel].hItem)))
    {
        MyPerror("asodifidf");
        MyMsgError();
        exit(EXIT_FAILURE);
    }

    for(int i=iIndexToDel;i<nbProjets-1;i++)
    {
        g_mtv[i]=g_mtv[i+1];
    }
    g_mtv[0].nbProjects=nbProjets-1;

    return TRUE;
}

BOOL MyGetNameFolderProjet(char *szPathFile,char *szDest)
{
    //cette fonction obtient le nom du dossier parent d'un File
    char *szLast=NULL,*szAct=NULL;
    char szCopie[strlen(szPathFile)+1];

    strcpy(szCopie,szPathFile);
    for(szAct=strtok(szCopie,"\\"); strstr(strstr(szPathFile,szAct),"\\");szAct=strtok(NULL,"\\"))
    {
        szLast=szAct;
    }

    if(szLast)
    {
        strcpy(szDest,szLast);
        return TRUE;
    }
     return FALSE;
}

BOOL MyManageOpenGCOfiles(HWND hwnd)
{
    //Cette fonction gere l'ouverture et le chargement des projets gco
    //via la lecture du contenus des Files .gco selectionnes par user,
    //Soient par la boite de dialogue open, soit en doublCliquant

    HTREEITEM htr;

    if(!MyFreeMenuTab(&mycache[DLG_DIALOGBOX].menu))
    {
        MyPerror("msoidf");
        return FALSE;
    }
    if(!MyAllocMenuTab(&mycache[DLG_DIALOGBOX].menu))
    {
        MyPerror("m sodifdsf");
        return FALSE;
    }

    projet.puObjet=uObjet;

    //Si user a doubleCLique un Filegco, alors, la description
    //est deja connue dans projet.desc, sinon, il faut afficher la boite Open
    //afin que user choisisse lui meme le File qu;il souhaite Open

    if(!g_IsDoubleClickedGcoFile)
    {
        //Si le chargement n'est pas du a un double clique de gco file
        //Save
        //NB; en cas de g_doubleclicked, le save est fait dans WM_COPYDATA
        if(uObjet[0].nbObjCreated>0)
        {
            MyUpdateDimObjUniv();
            projet.puObjet=uObjet;
            MySaveProjet();
        }

        if( !MyOpenProjet(hwnd))//Obtiens juste le nom valid de chemin a Open
        {
            MyPerror("sofdfdo");
            return 0;
        }

    }

    g_IsDoubleClickedGcoFile=FALSE;//renitialisation importante
    isProjetLoadedWithOpenItem=TRUE;
    //Verifie si le projet qu'on souahite loader existe deja
    //si oui, alors, le supprimer de l'arbre
    for(int i=0;i<g_mtv[0].nbProjects;i++)
    {
        if(!strcmp(g_mtv[i].desc.szFilePath,projet.descProjet.szFilePath))
        {
            //projet deja ouvert
            if(!MyActivateProjet(g_mtv[i].hItem))
            {
                MyPerror("skosifgop");
                MyMsgError();
                MessageBox(hwnd,"Unable to Laod Project","Error",
                MB_ICONERROR|MB_TASKMODAL|MB_OK);

            }
            return 0;//important, pour ne pas le recreer a nouveau
        }
    }

    //Sil nexiste pas encore, alorsl l'ajouter a larbre et lactiver
    htr = AddTreeItem(hwndManage,g_htrProjects,projet.descProjet.szNameFolder);
    if(htr)
    {
        HTREEITEM hChild=AddTreeItem(hwndManage,htr,projet.descProjet.szNameFile);
        if(hChild)
        {
            TreeView_EnsureVisible(hwndManage,hChild);
            MyAddTreeItem(hChild,projet.descProjet);

            if(!MyActivateProjet(hChild))
            {
                MyDelTreeItem(g_mtv[0].nbProjects-1);
                MyPerror("wousssss");
                MessageBox(hwnd,"Unable to Laod Project","Error",
                            MB_ICONERROR|MB_TASKMODAL|MB_OK);
                return 0;
            }

        }
    }
    g_fShouldIAskSave=FALSE;
    SetWindowText(g_hwndTitleProj,projet.descProjet.szFilePath);
    EnableWindow(g_hwndCloseProj,TRUE);

    return TRUE;
}

BOOL MyGetProjetDescWithPathFile(TCHAR * szPathFile,DESCRIPTION *desc)
{
    //Le role de cette fonction est de remplir les champs desc grace
    //au chemin szPath de File connu

    if(!szPathFile)
    {
        MyPerror("mvrohoe");
        MyMsgError();
        exit(EXIT_FAILURE);
    }
    TCHAR *bef, *act,*befbef;
    TCHAR szCopie[1024];

    _tcscpy(szCopie,szPathFile);
    for(act=_tcstok(szCopie,"\\"),bef=NULL,befbef=NULL;act!=NULL;
        befbef=bef,bef=act,act=_tcstok(NULL,"\\"));

    if(!bef)// nom du File
    {
        MyPerror("kvkgo");
        return FALSE;
    }
    if(!befbef)//Nom du dossier
    {
        MyPerror("vmekoke");
        return FALSE;
    }
    _tcscpy(desc->szNameFile,bef);
    _tcscpy(desc->szNameFolder,befbef);
    _tcscpy(desc->szFilePath,szPathFile);
    //CAS DU champ fOLDER PATH
    int l=(int)(befbef-szCopie)+strlen(befbef);
    _tcscpy(szCopie,szPathFile);
    szCopie[l]='\0';
    if(strlen(szCopie)<1)
    {
        MyPerror("vokokfd");
        exit(EXIT_FAILURE);
    }
    _tcscpy(desc->szFolderPath,szCopie);
    return TRUE;
}

void RemoveQuotes(char* str) {
    // Vérifier que la chaîne n'est pas vide et qu'elle commence et finit par des guillemets
    int len = strlen(str);
    if (len > 1 && str[0] == '\"' && str[len - 1] == '\"')
    {
        // Déplacer la chaîne pour supprimer les guillemets
        memmove(str, str + 1, len - 2);  // Décaler la chaîne d'un caractère et retirer le dernier guillemet
        str[len - 2] = '\0';  // Ajouter le caractère nul à la fin de la chaîne
    }
}

void MyInformRunningInstanceOF(char *szPathFile)
{
    //cette fonction est appellee lorsque user a double clique un File .gco
    //et qu'une autre instance de notre projet existait deja
    //Alors, pour ne pas reOpen une nouvelle instance, on va localiser
    //l'ancienne instance puis demander a ce programme de prendre en charge
    //le doubleClique.

    //1:Trouver l'instance deja existante
    HWND hwnd = FindWindow(NULL, "GCO");
    if (hwnd != NULL)
    {
        // Préparer la structure COPYDATASTRUCT pour transmettre le chemin du File
        COPYDATASTRUCT cds;
        cds.dwData = PATH_OF_GCO_FILES;  // Identifiant pour le type de données (1 pour un chemin de File)
        cds.cbData = (DWORD)(strlen(szPathFile) + 1);  // Taille de la chaîne, y compris '\0'
        cds.lpData = (PVOID)szPathFile;  // Pointeur vers les données à envoyer (le chemin du File)

        // Envoyer le message avec les données
        ShowWindow(hwnd,SW_MAXIMIZE);
        SetForegroundWindow(hwnd);
        SetFocus(hwnd);
        SendMessage(hwnd, WM_COPYDATA, (WPARAM)NULL, (LPARAM)&cds);

    }
    else
    {
        MyPerror("e,oerkee");
        return ;
    }
}
BOOL MyManageDefOutPut(TCHAR *szPathDefFile)
{
    //Affiche la boite de  dialogue qui permet a user
    //de definir le nom du ficher .h qu va contenir
    //Les define de tous les id des obj crees depuis obj.exe

    DLGPARAMINFO dlgparaminfo;

    dlgparaminfo.hwndParentDlg=GetParent(hwndZC);
    dlgparaminfo.cbSizeMoreData=sizeof(szPathDefFile);
    dlgparaminfo.moreData=(VOID *)szPathDefFile;

    dlgparaminfo.idDlg=IDD_SETDEFOUTPUT;
    DialogBoxParamW((HINSTANCE)GetWindowLongPtr(hwndZC,GWLP_HINSTANCE)
        , MAKEINTRESOURCEW(IDD_SETDEFOUTPUT),
        GetParent(hwndZC),(DLGPROC)DlgNotifProc,
                (LPARAM)&dlgparaminfo);

    return TRUE;
}

void MyUpdateGCODEF(void)
{
    //cette fonction ouvre GCODEF.h puis le rempli de directives
    //#define etc
    TCHAR szFileName[1024]="";
    strcpy(szFileName,projet.szPathDefFile);
    strcat(szFileName,"GCODEF.h");
/*
    if(!strstr(projet.szPathDefFile,projet.descProjet.szFolderPath))
    {
        char buf[2048];
        sprintf(buf,"GCODEF.h will be save on this path <%s>! It is on another folder different to <%s>'sFolder!\nDo you still want to save here?",
                projet.szPathDefFile,projet.descProjet.szNameFile);

        int iResult=MessageBox(GetForegroundWindow(),buf,"Question?",
                   MB_ICONERROR|MB_YESNO|MB_TASKMODAL);

        if(iResult==IDNO)
        {
            MyManageDefOutPut(projet.szPathDefFile);
        }
    }
  */
    FILE *File=fopen(szFileName,"w+");
    if(!File)
    {
        char buf[2048];
        sprintf(buf,"Unable to open %s",szFileName);
        MessageBox(GetForegroundWindow(),buf,"Error:",
                   MB_ICONERROR|MB_OK|MB_TASKMODAL);
        return;
    }


    fprintf(File,"#ifndef GCODEF_H_INCLUDED\n");
    fprintf(File,"#define GCODEF_H_INCLUDED\n");
    //Definir les idDes objets
    fprintf(File,"\n //IDs in GCO:");
    for(int i=0;i<projet.puObjet[0].nbObjCreated;i++)
    {
        int id=projet.puObjet[i].id;
        PTCHAR pszNameid=projet.puObjet[i].szNameId;
        fprintf(File,"\n#define %s %d",pszNameid,id);
    }
    //definir les id des menus
    fprintf(File,"\n\n //IDs of MENU ITEMS in GCO:");
    {
        int iIndexParent=MyGetIindexObjWithHwnd(g_hwndParentObj);
        if(iIndexParent!=-1)
        {
            for(int i=0;i<uObjet[iIndexParent].menu[0].iLastIndexMenu;i++)
            {
                for(int j=0;j<uObjet[iIndexParent].menu[i].iLastIinexElt;j++)
                {
                    if(!uObjet[iIndexParent].menu[i].tabElt[j].isPopPupMenu)
                    {
                        //S'il sagit d'un index d'item
                        int id=uObjet[iIndexParent].menu[i].tabElt[j].dataItem.idItem;
                        PTCHAR pszNameid=uObjet[iIndexParent].menu[i].tabElt[j].dataItem.szNameIdItem;
                        fprintf(File,"\n#define %s %d",pszNameid,id);
                    }
                }
            }
        }

    }
    fprintf(File,"\n\n#endif // GCODEF_H_INCLUDED");

    fclose(File);

    return ;
}
