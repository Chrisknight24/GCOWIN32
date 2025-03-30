#ifndef OPENDIALOGBOX_H_INCLUDED
#define OPENDIALOGBOX_H_INCLUDED
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <commctrl.h>
#include <tchar.h>
#include <windowsx.h>
#include <shlwapi.h>

#define CREATE 0
#define MANAGE 1
#define ALL -1

#define ID_DLG_GB 1 //id du dilaog child de chaque groupBox cree
#define ID_TIMER_CLIGNOTE 1
#define ID_TIMER_ALIGNOBJ 2
#define COLOR_TXT_PARENT 100
#define COLOR_BK_PARENT 101

#define ID_PREV 100
#define ID_CREATE 101
#define ID_SUIV 102
#define ID_COLOR 103
#define ID_FONT 104
#define ID_APPLY 105
#define ID_DEFCOLOR_BK 106
#define ID_RGB_BK 107
#define ID_DEFCOLOR_TXT 108
#define ID_RGB_TXT 109
#define ID_COMBO_BK 110
#define ID_R_BK 111
#define ID_G_BK 112
#define ID_B_BK 113
#define ID_R_TXT 114
#define ID_G_TXT 115
#define ID_B_TXT 116
#define ID_COMBO_TXT 117
#define ID_SET_DEFAULT 118

#define ID_ADD_MENU 119//bouton add Menu
#define ID_CHOOSE_CLR_BK 120
#define ID_CHOOSE_CLR_TXT 121
//Main Menu Dlg|| Mod_menu dlg
#define IDR_MOD_EX__MENU 120//
#define IDR_CREATE_MENU 121
#define IDC_MOD_MENU 122
#define IDR_MOD_DEL_MENU 123
#define IDR_MOD_ADD_ITEM 124
#define IDR_MOD_MOD_ITEM 125
#define IDR_MOD_MOD_MENU 126
#define IDB_MOD_GO_MENU 127
#define IDE_CREATE_MENU 128
#define IDB_CREATE_GO_MENU 129
#define  IDB_MANAGE_GROUP 130
#define  IDB_CREATE_GROUP 131
#define  IDB_OPTION_GROUP 132
#define IDS_MOD_MENU 133
#define IDS_CREATE_MENU 134
#define IDR_MOD_DEL_ITEM 135
#define IDB_CREATE_DEF_GROUP 136
#define IDR_CREATE_MAIN 137
#define IDR_CREATE_SUBMAIN 138
#define IDS_CREATE_PARENT 139
#define IDC_CREATE_MENUPARENT 140

//Add_item menu Dlg
#define IDS_MOD_NAMEITEM 141
#define IDE_MOD_NAMEITEM 142
#define IDS_MOD_IDNAMEITEM 143
#define IDE_MOD_IDNAMEITEM 144
#define IDS_MOD_FLAGITEM 145
#define IDC_MOD_FLAGITEM 146
#define ID_ADD 147
#define IDB_MOD_FLAGITEMGROUP 148
#define ID_FLAGITEM_LIST 149
#define IDS_MOD_PARENTMENU 150
#define IDE_MOD_PARENTMENU 151

//mod_Item menu dlg
#define IDC_MOD_NAMEITEM 120
#define  IDR_BEFORE 121
#define IDR_AFTER 122
#define IDB_GB_SETPOS_ITEM 123
#define IDC_SET_AFTER 124
#define IDC_SET_BEFORE 125
#define IDR_NOCHANGE 126

#define ID_BUTTON 151
#define ID_LISTBOX 152
#define ID_EDITBOX 153
#define ID_DIALOGBOX 154
#define ID_COMBOBOX 155
#define ID_GROUPBOX 156
#define ID_STATIC 157

#define ID_EDIT_TITLE 207
#define ID_X_DEC 208
#define ID_X_INC 209
#define ID_Y_DEC 210
#define ID_Y_INC 211
#define ID_W_DEC 212
#define ID_W_INC 213
#define ID_H_DEC 214
#define ID_H_INC 215


#define ID_STATIC_X 250
#define ID_STATIC_Y 251
#define ID_STATIC_W 252
#define ID_STATIC_H 253
#define ID_STYLE_LIST 254 //child quui stocke le nom des styles
#define ID_ASPECT 255
#define ID_DESCRIPTION 256
#define ID_EDIT_STYLES_CHOOSEN 257
#define IDE_STYLE_DESC 258
#define IDS_STYLE_TO_DESC 259
#define IDE_NAMEID 260
#define  IDC_STATIC_GROUP -5

#define MAXDIM_X 1000
#define MAXDIM_Y 1000
#define MAXDIM_W 1000
#define MAXDIM_H 1000
#define MAXSTYLES 250
#define MAXDLGCHAR 30 //taille maximale d'un texte de boite de dialogue
#define MAXLENTGHT_OF_SZSTYLECHOSEN 5000
#define MAXLENTGHT_OF_DESCRIPTION 2500
#define MIN_OBJ_CREABLE 3
#define BOR_H 5
#define MIN_NBELT_PER_MENU 15
#define MIN_NBMENU 15
#define MAX_HSCROLL_RANGE 1024
#define MAX_VSCROLL_RANGE 1024

#define ID_MANAGE 1
#define ID_ZC  2
#define ID_FRAME 3
#define IDS_TITLE_PROJ 4
#define IDB_CLOSE_PROJ 5
#define IDS_MANAGE 6

#define MAX_STYLES_DESC 159

/*Les enums*/
enum{UP,DOWN,RIGHT,LEFT,COR_LU,COR_LD,COR_RU,COR_RD};
enum {OBJ_BUTTON,OBJ_LISTBOX,OBJ_COMBOBOX,
OBJ_EDITBOX,OBJ_STATIC,OBJ_IMGSTATIC,OBJ_DIALOGBOX};
enum{X,Y,W,H,MODAL,MODELESS,};
enum{BK,TXT};
enum {DLG_BUTTON,DLG_LISTBOX,DLG_EDITBOX,DLG_DIALOGBOX,
           DLG_COMBOBOX,DLG_IMGSTATIC,DLG_STATIC,DLG_CHOOSETYPEOBJET,DLG_COLOR};
#define NBOBJECT 7 //de DLG_BUTTON a DLG_STATIC ,
/*NB NBOBJECT pourra varier, mais pour ce faire, il faudra en consequence modifier tout le haut
 pour avoir le meme nombre delement, la meme chose a la meme position etc on se comprend*/



/*Les typesdef*/

typedef struct FOUR_BOOL
{
    BOOL top;
    BOOL bottom;
    BOOL right;
    BOOL left;
}FOUR_BOOL;
typedef struct STYLE_DESCRIPTION
{
    DWORD dwValue;
    char *szNameValue;
    char *szDescription;
}STYLE_DESCRIPTION;

typedef struct MYFONT
{
    HFONT hFont;
    LOGFONT logfont;
    int isFontParent;
}MYFONT;

typedef struct STYLE_VALUE STYLE_VALUE;
struct STYLE_VALUE
{
    DWORD dwValuesStyles;
    char  szValuesStyles[MAXDLGCHAR];
};

typedef struct MYCACHE_ITEM
{
    STYLE_VALUE tabStyleItemMenu[250];
    UINT uLastStylesChosen;
    char szLastStylesChosen[MAXLENTGHT_OF_SZSTYLECHOSEN];

}MYCACHE_ITEM;

typedef struct MYDESCPOPUPMENU
{
    char szNameMenu[1024];
    UINT uFlag;
    char szFlagName[1024];
    HMENU hMenu;

}MYDESCPOPUPMENU;

typedef struct MYDESCITEMMENU
{
    char szNameItem[1024];
    UINT uFlag;
    char szFlagName[1024];
    UINT  idItem;
    char szNameIdItem[1024];
    MYCACHE_ITEM mycacheItem;
}MYDESCITEMMENU;

typedef struct MENUELT
{
    MYDESCITEMMENU dataItem;
    MYDESCPOPUPMENU dataPop;
    BOOL isPopPupMenu;
}MENUELT;

typedef struct MYMENUORDER
{
    BOOL isMenuChild;
    int iIndexChild;
    int nbOrder;

}MYMENUORDER;

typedef struct MYMENULIST
{
    int iLastIndexMenu;
    int iLastIinexElt;
    int iIndexParent;
    int iIndexComboCreate;
    BOOL isSubMenu;
    MENUELT *tabElt;
    MYMENUORDER *tabOrder;
    int idMenu;

}MYMENULIST;

//Strucutre qui geres les proprietes de liaison des obj
typedef struct MYLINKOBJ
{
    BOOL isLinked;
    int *id;
    HWND *hwndLinkWith;
    int nbLink;
    int iLastIndex;
}MYLINKOBJ;

//Structure qui gere la boite de dialogue couleur
typedef struct MYCOLOROBJ MYCOLOROBJ;
struct MYCOLOROBJ
{
    int iIndexDefColor;
    BOOL isDefChoosen;
    int defColor;
    COLORREF rgbColor;
    int qr;
    int qg;
    int qb;
};

typedef struct UNIVERS UNIVERS;
struct UNIVERS//Structure qui regroupera tous les objetcs crees par user
{
    char szDescription[MAXLENTGHT_OF_DESCRIPTION];
    char szLastStylesChosen[MAXLENTGHT_OF_SZSTYLECHOSEN];
    DWORD dwStyle;
    TCHAR  szTitle[MAXDLGCHAR];
    TCHAR szNameId[MAXDLGCHAR];
    int nbObjCreated;
    HWND hwnd;
    HWND hwndParent;
    int id;
    int x;
    int y;
    int w;
    int h;
    int cxChar;// A save
    int cyChar;//A save
     int cxRest;// A save
    int cyRest;// Asave
    HINSTANCE hinst;
    WNDPROC oldObjWndProc;
    WNDPROC newObjWndProc;
    MYCOLOROBJ mycolorobj[2];
    int iTypeObj;
    MYLINKOBJ linkopt;
    BOOL isVisible;
    HWND hwndChildGB;
    MYMENULIST *menu;//tableau pour chaque colonne
    BOOL isMainParent;
    MYFONT font;
    FOUR_BOOL fShouldAutosize;
};

typedef struct HWNDMODELESS HWNDMODELESS;
struct HWNDMODELESS
{
    int nbHwndOpen;
    HWND newHwnd;
} ;

typedef struct MY_DLGDATA MY_DLGDATA;
struct MY_DLGDATA
{
    int idDlg;
    int iTypeDlg;
    HWND hWndOwner;
    HINSTANCE hinst;
};

//Structrue qui stocke les donnees sauvegardees dans la Dlg lors
//du parametrage
typedef struct MYCACHE_DLG MYCACHE_DLG;
struct MYCACHE_DLG
{
    DWORD dwLastStylesChosen;
    char szLastStylesChosen[MAXLENTGHT_OF_SZSTYLECHOSEN];
    int iLastX;
    int iLastY;
    int iLastW;
    int iLastH;
    int idObj;
    int cxChar;//A save
    int cyChar;//A save
    BOOL isPixelUnit;//A save
    int cxRest;// A save
    int cyRest;// Asave
    TCHAR szTitle[MAXDLGCHAR];
    TCHAR szNameId[MAXDLGCHAR];
    STYLE_VALUE valueStyle[MAXSTYLES];//tableau qui debute a lindex 1 et qui associe l'index valant l'id a la valeur du style correspondant a l'id
    MYCOLOROBJ mycolorobj[2];//deux pouor BK et TXT
    MYLINKOBJ linkopt;
    MYMENULIST *menu;//tableau pour chaque colonne
    MYFONT font;
    FOUR_BOOL fShouldAutosize;
};

//Structure qui stokce les infos d'un static
typedef struct MY_STATIC MY_STATIC;
struct MY_STATIC
{
    BOOL focusOn;
    HWND hwndStatic;
    int idStatic;
};

typedef struct POSOBJ
{
    POINT _beg;
    POINT _end;
}POSOBJ;
typedef struct COUPLEHWND
{
    HWND hwndLast;
    HWND hwndNew;
    int iIndexValid;
    int tailleTab;
}COUPLEHWND;

typedef struct FRAMELINK
{
    HBRUSH hBrushLink;
    HWND hwndObjToLink;
    BOOL isFrameLinkOn;
    RECT rcFrame;
} FRAMELINK;

typedef struct FRAMEUNLINK
{
    HBRUSH hBrushUnLink;
    HWND hwndObjToUnLink;
    BOOL isMaskUnLinkOn;
    RECT rcMask;
} FRAMEUNLINK;

typedef struct LPARAM_ENUMANCES
{
        BOOL response;
        HWND hwndChild;
}LPARAM_ENUMANCES;

typedef struct DLGPARAMINFO
{
    HWND hwndParentDlg;
    int idDlg;
    void *moreData;
    size_t cbSizeMoreData;
}DLGPARAMINFO;

/*Les externs*/
extern POSOBJ *po;
extern MYCACHE_DLG mycache[NBOBJECT];
extern TCHAR *szMyClassChildName[];
extern wchar_t *szMyUtfClassChildName[];//version Unicode
extern WNDCLASS myWc;
extern HWNDMODELESS *hDlgModeless;//Tableau des handles des boites de dialogues modeless crees
extern int cxChar,cyChar;
extern BOOL isFirstLoad[NBOBJECT];
extern char szStylesChosen[MAXLENTGHT_OF_SZSTYLECHOSEN];
extern  MYCACHE_DLG myValues;
extern int iActualIdForObjet;
extern UNIVERS *uObjet;
extern int maxObjectCreable;//nombre maximal d'objet present dans l'univers
extern HBRUSH hBrushGlobal;
extern BOOL isTypeModDlg;
extern BOOL isParentWnd;
extern int idObjToDelete;
extern HWND g_hToolTip;//Handle des infobulles
extern TCHAR   *szClassDlgObj;
extern HWND g_hwndParentObj;
extern HWND g_hwndPrevParentObj;
extern HWND g_hwndParentObjToSet;
extern TCHAR* szClassZC;
extern TCHAR* szClassManage;
extern TCHAR* szClassFrame;
extern TCHAR *szClassLinkMask;
extern TCHAR *szClassGroupBoxChild;
extern TCHAR *szClassTestProjet;
extern POINT ptWhereCreate;
extern COUPLEHWND *g_tabHwnd;
extern HWND hwndZC;
extern HWND hwndManage;
extern FRAMELINK g_link;
extern FRAMEUNLINK g_unlink;
extern HWND hwndMask;
extern HWND g_hwndLastChildGB;
extern int g_idMenuActu;
extern MYMENULIST *menuTemp;
extern MYMENULIST *copieMenuTemp;
extern UINT g_idItemActu;
extern MYCACHE_ITEM g_mycacheItem;
extern BOOL fShouldInotify;
extern TCHAR szCurrentPath[MAX_PATH+1];
extern TCHAR g_szPathFolder[MAX_PATH];
extern BOOL isProjetLoadedWithOpenItem;
extern MYFONT g_fontSys;
extern BOOL isFirstTime[NBOBJECT];
extern BOOL g_fShouldIAskSave;
extern BOOL g_fIsProcessNewProject;
extern HWND g_hwndTitleProj;
extern HWND g_hwndCloseProj;
extern HWND g_hwndStaticManage;
extern BOOL g_fShouldApplyFontOnChilds;
extern int g_nbStyle;
extern HWND g_hwndToolBar;
extern STYLE_DESCRIPTION g_styleDescription[MAX_STYLES_DESC];
extern WNDPROC g_oldEditStyleDescProc;
extern BOOL g_fShowHowSize;
extern TCHAR *szProjectAppExt;
/*Les prototypes*/
    //Procedures de fenetres
LRESULT CALLBACK DialogProc(HWND hDlg,UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MyChildProcDlgButton(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK MyChildProcDlgListBox(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK MyChildProcDlgEditBox(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK MyChildProcDlgComboBox(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK MyChildProcDlgDialogBox(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK MyChildProcDlgGroupBox(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK MyChildProcDlgStatic(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK MyClrDlgObjProc(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK ZcProc(HWND hwnd, UINT MESSAGE,WPARAM wParam, LPARAM lParam  );
LRESULT CALLBACK ManageProc(HWND hwnd, UINT MESSAGE,WPARAM wParam, LPARAM lParam  );
LRESULT CALLBACK DlgNotifProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK FrameProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MyLinkMaskProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK EnumChildIsAncetorProc(HWND hwndChild,LPARAM lParam);
LRESULT CALLBACK ChildGroupBoxProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK SimpleMenuOptProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK TestProjetProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK NewEditStyleDescProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
//Porcedure sous classee des objets de lunivers nouvellement crees
LRESULT CALLBACK NewObjWndProc(HWND,UINT,WPARAM,LPARAM);

    //Dans Main.c
BOOL MyLoadG_Paths(void);
BOOL MySaveG_Paths(void);
LONG MyGetCharDim(HWND hwnd,HFONT hFontCurrent);
void MyMsgError(void);
void InitStyleDesc(void);
void MyCreateTempTxtFile(HRSRC hResInfo,PTCHAR pszPathFile);
void MyPerror(TCHAR *szKeyError);
BOOL MySetAppIconToWnd(HWND hwnd);
BOOL MyIsThisExtFile(TCHAR *szFileName,TCHAR *szNameExtension);

    //Dans OpenDialogBox.c
LPDLGTEMPLATE AlignLpdt(LPVOID ptr);
LPWORD lpwAlign(LPWORD lpIn);
LPDLGITEMTEMPLATE AlignLpdit(LPVOID ptr);
size_t GetPow2(size_t maxdlgtemp);
BOOL OpenDialogBox(int iType,int id,HWND hwndOwner,HINSTANCE hinst,HWND hwndToClose,int iTypeDlgToClose);

    //Dans DialogProc.c
BOOL DeleteWindow(HWND hDlg,HWNDMODELESS *hDlgModeless);
BOOL MySubClass(HWND hwndChild,WNDPROC oldWndProc, WNDPROC DimWindowProc);
HWND CreateObjet(PSTR pClassName,int idDlg,HWND hwndOwner);
BOOL MyAddToolTipToObj(HWND hwndObj,HWND hwndParentObj);
BOOL MyGetDescOfObj(UNIVERS* uTemp);
BOOL MyUpdateDimObjUniv(void);
HWND MyGetLastParentObj(HWND hwndNewParentObj);
BOOL MySetLastParentObj(HWND hwndLastParentObj,HWND hwndNewParentObj);
BOOL MyChangeHwndNewParent(HWND A,HWND B);
BOOL MyShowAllObj(void);
BOOL MyApplyMenuCache(HWND hwndObj,MYMENULIST *menu);
BOOL MyAppendPopupElt(int iIndexMenu,HMENU hMenuMain,MYMENULIST *menu);
BOOL MyAppendItemElt(int iIndexMenu,int iIndexElt,HMENU hMenuMain,MYMENULIST *menu);
void MySetNameId(TCHAR *szNameId,int idDlg);
int MyGetNbObjSameType(int objType);
BOOL isThisNameIdAlreadyUsed(TCHAR *szNameId,int *iIndexUsing);
BOOL isValidNameId(TCHAR *szNameId,int idObjExcluded);
BOOL MyInitFshouldAutosize(FOUR_BOOL *tab,BOOL defValue);
    //Dans CacheMangement.c
BOOL MyLoadCache(int idDlg,MYCACHE_DLG *myValues,HWND hDlg);
BOOL MySaveCache(int idDlg,MYCACHE_DLG myValues,HWND hDlg);
BOOL Decrement(int idDlg,int idChildDlg,MYCACHE_DLG *myValues,HWND hDlg);
BOOL Increment(int idDlg,int idChildDlg,MYCACHE_DLG *myValues,HWND hDlg);
BOOL MyApplyCache(int idDlg,MYCACHE_DLG *myValues,HWND hDlg);
BOOL MyRefreshAspect(HWND hDlg,int idDlg);
BOOL MyInitCacheDlg(void);

    //Dans MyChildProcDlgButton
char* strrep(char *source, char *szOld,char *szNew);
BOOL myGiveIDofString(PDWORD pdw ,PTCH szNameMsg,char *szNameFile);
BOOL StringToiD(PSTR pBuffer,PDWORD pdwStyle,char *szNameFile);
void MyDisplayStyleDesc(STYLE_VALUE *tabStyleValue, WPARAM wParam,
                        LPARAM lParam,HWND hDlg);

        //Dans MyManageCheckStyle.c
BOOL MyManageCheckStyle(int idDlg,WPARAM wParam,LPARAM lParam);
BOOL MyUpdateDwStyle(int idDlg);

    //Dans NewObjWndProc.c
int GetObjIndex(int idObj);
BOOL MyDeleteObj(int idObj);
BOOL MyDisPlayInfoObj(int idObj);
BOOL MyDuplicateObj(int idObj);
BOOL MyModifyObj(int idObj);
BOOL MyCheckAlignObj(int dir,HWND hwndObj,POSOBJ *poTemp,int pas,BOOL *hasPosChanged);
BOOL isPassedTrough(int target,int before,int after);
BOOL DrawLine(HWND hwnd,POINT ptBeg,POINT ptEnd);
BOOL MyLinkObj(int idObj);
BOOL MyUnLinkObj(int idObj);
BOOL MyFrameLink(int iIndexObj,BOOL isLink);
BOOL MyApproveLink(HWND hwndToLink,HWND hwndLinkTo);
BOOL MyApproveUnlink(HWND hwndToUnlink,HWND hwndUnlinkTo);
BOOL MyAddLink(HWND hwnd1,HWND hwnd2);
BOOL MyRemLink(HWND hwnd1,HWND hwnd2);
BOOL isAncestor(HWND hwndAncestor,HWND hwndChild);
BOOL MyUpdateLinksUniv(void);
HWND MyGetHwndObj(int idObj);
int MyGetIindexObjWithHwnd(HWND hwndObj);
WINBOOL MyMoveWindow(int iIndexObj,int left,int top,int right,int bottom,BOOL fRedraw);
BOOL isLinkedWith(int iIndex1,int iIndex2);
BOOL isGroupBoxObj(int iIndex);
BOOL isChildGB(HWND hwnd);
BOOL MyFreeuObjet(UNIVERS **puObjet);
BOOL MyFreeLinkOpt(MYLINKOBJ *link);
BOOL fHasChilds(int iIndex);
BOOL MyCheckAutosizeOpt(HMENU hMenu,int iIndexObj);
BOOL MyApplyAutoSizeOnChild(HWND hwndParent,int cxLastClient,int cyLastClient);
BOOL MyAdjustDimChild(int side,int iIndexChild,int iIndexParent,int cxLastClient,int cyLastClient);

    //Dans MyLoadBorderOpt.c
BOOL MyLoadBorderOpt(HWND hwnd);
BOOL Myborder(HWND hwndTarget);
LRESULT CALLBACK BorderProc(HWND hwnd,UINT msg,WPARAM wParam, LPARAM lParam);
BOOL MyMaskBorder(BOOL choice);
BOOL MyChangeParentBorder(HWND hwndNewParentBorder);
    //Dans MyManageColor.c
BOOL MyManageColor(int idDlg,HWND hwndOwner);
LRESULT CALLBACK ColorProc(HWND hDlg,UINT msg, WPARAM wParam, LPARAM lParam);
BOOL MyLoadColorObj(HWND hDlg,MY_DLGDATA *infoDlg);
BOOL MyFillComboColor(HWND hwndCombo,int iIndex);
BOOL MyScreenRcToClientRc(HWND hwnd,LPRECT rcCombo);
COLORREF MyGetSysColor(MYCOLOROBJ mycolorobj, HWND hwndParent);
BOOL MySetColor(MYCOLOROBJ* colorobj,COLORREF couleur);
//Dans MyManageMenu
BOOL MyManageMenu(HWND hwndOwner);
void MyEnableMenuObj(HWND hDlg,BOOL fEnable,int manner);
BOOL isEmptyCombo(HWND hwndCombo);
BOOL MyAddMenu(HWND hDlgMenu,char *szNameMenu,int idDefMenuSelected);
int MyGetIndexCombo(HWND hDlg,int id);
BOOL OpenMenuDialogBox(int idDlg,HWND hwndOwner);
BOOL MyDelMenu(MYMENULIST *menuTab,int iIndexMenu,int idMenu,HWND hDlgMenu);
BOOL MyModMenu(int iIndexMenu,HWND hDlgMenu);
BOOL MyApplyMenuTempOnCombos(HWND hDlg,int idCombo);
BOOL MyAddItemToMenu(int iIndexMenu,HWND hDlgMenu);
BOOL MyModifyItem(int iIndexMenu,HWND hDlgMenu);
BOOL MyAllocMenuTab(MYMENULIST **pg_menuTab);
BOOL MyFreeMenuTab(MYMENULIST **pg_menuTab);
BOOL MyCopieMenuTab(const MYMENULIST *source,MYMENULIST **pdest);
BOOL isIdenticalTabMenu(const MYMENULIST *menuTab1,const MYMENULIST *menuTab2);
BOOL isIdenticalTabElt(const MENUELT *melt1,const MENUELT *melt2,int tailleTab);
BOOL MyGetIindexMenuWithIdMenu(MYMENULIST *menuTab,int idMenu);
BOOL MyDelItemToMenu(int iIndexMenu,HWND hDlgMenu);
BOOL MyDelChildToOrder(MYMENULIST *g_menuTab,int iIndexChildToDel,int iIndexMenu,BOOL isMenuChild,BOOL fShouldDecrease);
BOOL MyInitMenuChildOrder(HWND hDlg,MYMENULIST *g_menuTab,int iIndexItem,int iIndexMenu,BOOL isMenuChild,int *tabPosCombo);
BOOL MySetPosChildMenu(MYMENULIST *g_menuTab,int iIndexMenu,int iIndexChildMenu,int iNewAft,int iNewBef,BOOL isChildMenu,int *tabPosCombo);
BOOL MyAddChildToOrder(MYMENULIST *g_menuTab,int iIndexMenu,int iIndexChild,BOOL isMenuChild);
    //Dans MyTestProjet.c
BOOL MyTestProjet(void);
BOOL MyCreateObjTest(int iIndex,COUPLEHWND *tabHwndNewParent);
int  MyGetIIndexWithhwndNew(HWND hwnd);

    //Dans ProjetManagement.c
BOOL MyCreateNewProjet(void);

    //Dans OpenDlg_choosetypeobj.c
BOOL OpenDlg_choosetypeobj(int iType,HWND hwndOwner,HINSTANCE hinst);
    //Dans OpenDlg_static.c
BOOL OpenDlg_static(int iType,HWND hwndOwner,HINSTANCE hinst);
    //Dans OpenDlg_button.c
BOOL OpenDlg_button(int iType,HWND hwndOwner,HINSTANCE hinst);
    //Dans OpenDlg_combobox.c
BOOL OpenDlg_combobox(int iType,HWND hwndOwner,HINSTANCE hinst);
    //Dans OpenDlg_editbox.c
BOOL OpenDlg_editbox(int iType,HWND hwndOwner,HINSTANCE hinst);
    //Dans OpenDLG_IMGSTATIC.c
BOOL OpenDLG_IMGSTATIC(int iType,HWND hwndOwner,HINSTANCE hinst);
    //Dans OpenDlg_dialogbox.c
BOOL OpenDlg_dialogbox(int iType,HWND hwndOwner,HINSTANCE hinst);
    //Dans OpenDlg_listbox.c
BOOL OpenDlg_listbox(int iType,HWND hwndOwner,HINSTANCE hinst);
#endif // OPENDIALOGBOX_H_INCLUDED
