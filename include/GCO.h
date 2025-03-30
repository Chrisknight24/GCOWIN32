#ifndef __MAIN_H__
#define __MAIN_H__

#include <windows.h>
#include <stdio.h>

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif


#define TO_GCO 255

//Declaration des protoypes de fonctions avec gestion du Mangling
#ifdef __cplusplus
extern "C"
{
#endif

//tHIS GOES into GCO.h without DLL_EXPORT declaration
#define WM_GCOINIT WM_USER+1
int DLL_EXPORT GcoGetId(HWND hwndObjet);
HWND DLL_EXPORT GcoGetHwnd(int idGcoObjet);
HWND DLL_EXPORT GcoDisPlay(HRSRC hResInfo,HINSTANCE hinst,PWNDCLASSEX pWc);
HWND DLL_EXPORT GcoGetMainWindow(void);
BOOL DLL_EXPORT GcoCenterWindow(HWND hwndToCenter, HWND hwndParent);
#ifdef __cplusplus
}
#endif

#endif // __MAIN_H__
