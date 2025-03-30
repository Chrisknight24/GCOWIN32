/*Le but est de creer une fonction qui permet de mieux utiliser
create windwos, pour le programmeurs, juste en creant une structure
pour regrouper tous les informations passes en parametres
de la fonction createWindows classique

Par CHRISTIAN EDOUARDO LE 13/01/2024 */

#include "myCreateWindow.h"

HWND myCreateWindow(INFWND _)
{
    HWND hWnd;

    hWnd=CreateWindow(\
                      _._1_szClassName,
                      _._2_szWndName,
                      _._3_dwWndStyle,
                      _._4_x,
                      _._5_y,
                      _._6_nWidth,
                      _._7_nHeight,
                      _._8_hWndParent,
                      _._9_hMenu,
                      _._10_hInstance,
                      _._11_lpParam);
    return hWnd;
}//fin HWND myCreateWindows
