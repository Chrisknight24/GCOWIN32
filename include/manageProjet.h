#ifndef MANAGEPROJET_H_INCLUDED
#define MANAGEPROJET_H_INCLUDED
//Il faut inclure le header qui stocke les fonctions qui gerent
//L'univers, pour linstant on prendra tout OpenDialog.h
//Mais par la suite,il faudra remplacer OpendDialog.h
//par un simple hearder "Univers.h" par exemple
//Ca rend la compilation plus rapide
#include "OpenDialogBox.h"
#include <shlobj.h>
#include <ole2.h>
#include <Objbase.h>
#include <commdlg.h>
#include <shlwapi.h>
//Les defines

#define NB_MIN_PROJ_TV 3
//DLG:IDD_NEW_PROJET
#define IDD_NEW_PROJET  5000

#define IDE_PROJET_NAME 5001
#define IDS_PROJET_NAME 5002
#define IDS_PROJET_FOLDER_NAME 5003
#define IDE_PROJET_FOLDER_NAME 5004
#define IDS_PROJET_FILE_NAME 5005
#define IDE_PROJET_FILE_NAME 5006
#define ID_FINISH 5007
#define IDB_CANCEL 5008
#define IDB_SELPATH 5009

//DLG:SETDEFOUTPUT
#define IDD_SETDEFOUTPUT 2500

#define IDE_DEFPATH 2501



#define IDM_NEW 1025
#define IDM_OPEN  1026
#define IDM_QUIT 1027
#define IDM_SAVE 1028
#define IDM_SAVEAS 1029
#define IDM_CLOSE_PROJECT 1030
#define IDM_ABOUT 1031
#define IDM_TUTORIAL 1032
//Menu TREEVIEEW
#define IDM_ACTIVATE_PROJ 1050
#define IDM_SAVE_TV 1051
#define IDM_SAVEAS_TV 1052
#define IDM_CLOSE_PROJECT_TV 1053
#define IDM_SET_DEF_OUTPUT 1054
#define IDM_SET_DEF_OUTPUT_TV IDM_SET_DEF_OUTPUT

//LOrs des communications inter-processus
#define PATH_OF_GCO_FILES 1

typedef struct DESCRIPTION
{
    TCHAR szNameFolder[MAXDLGCHAR];
    TCHAR szNameFile[MAXDLGCHAR];
    TCHAR szFolderPath[1024];
    TCHAR szFilePath[1024];
}DESCRIPTION;

typedef struct PROJETSTRUCT
{
    UNIVERS *puObjet;
    DESCRIPTION descProjet;
    TCHAR szPathDefFile[1024];
}PROJETSTRUCT;

//Les typedef

typedef struct MYCACHE_TREEVIEW
{
    DESCRIPTION desc;
    HTREEITEM hItem;
    int nbProjects;
}MYCACHE_TREEVIEW;

//Les extern
extern PROJETSTRUCT projet;
extern MYCACHE_TREEVIEW *g_mtv;
extern COUPLEHWND *g_coupHwnd;//Utiise dans MyTestProje.c
extern HWND g_hwndParentTest;//Parent window sur F9 ou Run
extern BOOL g_IsDoubleClickedGcoFile;
extern HTREEITEM g_htrProjects;

//Prototypes
BOOL  GetDocumentsPath(char* path);
BOOL GetAppDataPath(char* path);
BOOL CreateDirectoryIfValid(const char* path,const char* szNameFile);
void BrowseForFolder(HWND hwnd, HWND hwndEdit);
BOOL MySaveProjet(void);
BOOL MySaveAsProjet(HWND hwndOwner);
void PopFileInitialize (HWND hwnd);
BOOL PopFileSaveDlg (HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName);
BOOL PopFileOpenDlg (HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName);
BOOL MyOpenProjet(HWND hwndOwner);
BOOL MyLoadProjet(void);
BOOL MyLoadUnivers(void);
BOOL MyLoadChilds(HWND hwndZC);
BOOL MyLoadObj(int iIndexObj);
HTREEITEM AddTreeItem(HWND hwndTreeView, HTREEITEM hParent, LPCSTR lpszText);
BOOL MyAddTreeItem(HTREEITEM hItem,DESCRIPTION desc);
BOOL MyActivateProjet(HTREEITEM hItem);
BOOL MyDelTreeItem(int i);
BOOL  MyGetNameFolderProjet(char *szPathFile,char *szDest);
BOOL MyManageOpenGCOfiles(HWND hwnd);
BOOL MyGetProjetDescWithPathFile(TCHAR *szPath,DESCRIPTION *desc);
void RemoveQuotes(char* str) ;
void MyInformRunningInstanceOF(char *szPathFile);
BOOL MyManageDefOutPut(TCHAR *szPathDefFile);
void MyUpdateGCODEF(void);
void MyDispLayTutorial(void);
//Procedure de fenetre
LRESULT CALLBACK ProjectManageProc(HWND hDlg,UINT msg,WPARAM wParam,LPARAM lParam);
#endif // MANAGEPROJET_H_INCLUDED
