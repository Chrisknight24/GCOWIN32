/*Nom du File:MyRegisterOtherClass
Rôle : permet de d'enregistrer d'autres classes de fenetres personnalisees
,donc les noms sont incrits dans un tableau de noms!Ces classes sont utilisees
par les boites de dialogues crees! a chaque classe, on associe une nouvelle procedure
de fenetre
BY CHRSITIAN EDOUARDO le 05/09/2024 à 13h08 */
#include <windows.h>
#include "resource.h"
#include "OpenDialogBox.h"

//nom des classes a utiliser pour enregister des nouvelles classes personnalisees
TCHAR *szMyClassChildName[]={"DLG_BUTTON_CLASS",
"DLG_LISTBOX_CLASS","DLG_EDITBOX_CLASS","DLG_DIALOGBOX_CLASS",
"DLG_COMBOBOX_CLASS","DLG_IMGSTATIC_CLASS","DLG_STATIC_CLASS","DLG_CHOOSETYPEOBJECT_CLASS"};
    /*NB:Toujours se rassurer que ce qui suit est la copie unicode de ce qui est en haut*/
//Noms de classe pour les fenetres presonalisees des boites de dialogue
wchar_t *szMyUtfClassChildName[]={L"DLG_BUTTON_CLASS",
L"DLG_LISTBOX_CLASS",L"DLG_EDITBOX_CLASS",L"DLG_DIALOGBOX_CLASS",
L"DLG_COMBOBOX_CLASS",L"DLG_IMGSTATIC_CLASS",L"DLG_STATIC_CLASS",L"DLG_CHOOSETYPEOBJECT_CLASS"};

//Classe qui qui gere la fenetre principale qui portera tous les childs
TCHAR   *szClassDlgObj=TEXT("PARENT_OBJ_CLASS");
TCHAR *szClassLinkMask=TEXT("LINKMASK_CLASS");
TCHAR *szClassGroupBoxChild=TEXT("CHILDGROUPBOX_CLASS");
TCHAR *szClassTestProjet=TEXT("TESTPROJET_CLASS");
BOOL MyRegisterOtherClass(WNDCLASS myWc)
{
    int i,nbClass=sizeof(szMyClassChildName)/sizeof(szMyClassChildName[0]);

    for(i=0;i<nbClass;i++)
    {
        switch(i)
        {
        case DLG_BUTTON:
            myWc.lpszClassName=szMyClassChildName[i];
            myWc.lpfnWndProc=MyChildProcDlgButton;
            if(!RegisterClass(&myWc))
            {
                MessageBox(NULL,TEXT("Unable to Register Class perso for DLG_BUTTON"),NULL,0);
                exit(EXIT_FAILURE);
            }

            myWc.lpszClassName=szClassDlgObj;
            myWc.lpfnWndProc=MyClrDlgObjProc;
            if(!RegisterClass(&myWc))
            {
                MessageBox(NULL,TEXT("Unable to Register Class parent of obj"),NULL,0);
                exit(EXIT_FAILURE);
            }

            myWc.lpszClassName=szClassLinkMask;
            myWc.lpfnWndProc=MyLinkMaskProc;
            if(!RegisterClass(&myWc))
            {
                MessageBox(NULL,TEXT("Unable to Register Class parent of obj"),NULL,0);
                exit(EXIT_FAILURE);
            }

            myWc.lpszClassName=szClassGroupBoxChild;
            myWc.lpfnWndProc=ChildGroupBoxProc;
            if(!RegisterClass(&myWc))
            {
                MessageBox(NULL,TEXT("Unable to Register Class parent of obj"),NULL,0);
                exit(EXIT_FAILURE);
            }

            myWc.lpszClassName=szClassTestProjet;
            myWc.lpfnWndProc=TestProjetProc;
            if(!RegisterClass(&myWc))
            {
                MessageBox(NULL,TEXT("Unable to Register Class parent of obj"),NULL,0);
                exit(EXIT_FAILURE);
            }

            break;

        case DLG_LISTBOX:
            myWc.lpszClassName=szMyClassChildName[i];
            myWc.lpfnWndProc=MyChildProcDlgListBox;
            if(!RegisterClass(&myWc))
            {
                MessageBox(NULL,TEXT("Unable to Register Class perso for DLG_LISTBOX"),NULL,0);
                exit(EXIT_FAILURE);
            }
            break;

        case DLG_EDITBOX:
            myWc.lpszClassName=szMyClassChildName[i];
            myWc.lpfnWndProc=MyChildProcDlgEditBox;
            if(!RegisterClass(&myWc))
            {
                MessageBox(NULL,TEXT("Unable to Register Class perso for DLG_EDITBOX"),NULL,0);
                exit(EXIT_FAILURE);
            }
            break;

        case DLG_COMBOBOX:
            myWc.lpszClassName=szMyClassChildName[i];
            myWc.lpfnWndProc=MyChildProcDlgComboBox;
            if(!RegisterClass(&myWc))
            {
                MessageBox(NULL,TEXT("Unable to Register Class perso for DLG_DIALOGBOX"),NULL,0);
                exit(EXIT_FAILURE);
            }
            break;

        case DLG_DIALOGBOX:
            myWc.lpszClassName=szMyClassChildName[i];
            myWc.lpfnWndProc=MyChildProcDlgDialogBox;
            if(!RegisterClass(&myWc))
            {
                MessageBox(NULL,TEXT("Unable to Register Class perso for DLG_DIALOGBOX"),NULL,0);
                exit(EXIT_FAILURE);
            }
            break;


        case DLG_IMGSTATIC:
            myWc.lpszClassName=szMyClassChildName[i];
            myWc.lpfnWndProc=MyChildProcDlgGroupBox;
            if(!RegisterClass(&myWc))
            {
                MessageBox(NULL,TEXT("Unable to Register Class perso for DLG_LISTBOX"),NULL,0);
                exit(EXIT_FAILURE);
            }
            break;

        case DLG_STATIC:

            myWc.lpszClassName=szMyClassChildName[i];
            myWc.lpfnWndProc=MyChildProcDlgStatic;
            if(!RegisterClass(&myWc))
            {
                MessageBox(NULL,TEXT("Unable to Register Class perso for DLG_LISTBOX"),NULL,0);
                exit(EXIT_FAILURE);
            }
            break;

        }//fin switch(i)

    }//fin for i
    return TRUE;
}
