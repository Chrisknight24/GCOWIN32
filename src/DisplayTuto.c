/*Nom du fichier:DisplayTuto.c
Role:Gere l'affichage de la fenetre qui affiche le tuto pour utiliser lapp GCO
*/
#include "manageProjet.h"
#include "GCO.h"
#include "GCODEF.h"
LRESULT CALLBACK GcoProc(HWND hwnd,UINT msg,WPARAM wParam, LPARAM lParam);
void MyDispLayTutorial(void)
{
    //Deux memes instances de cette fonction ne doivent
    //pas etre en cour

    static HWND hwnd=NULL;
    if(IsWindow(hwnd))
    {
        SetForegroundWindow(hwnd);
        return;
    }
    HINSTANCE hInstance=(HINSTANCE)GetWindowLongPtr(hwndZC,GWLP_HINSTANCE);

    char szPathExe[1024];
    char *c;

    //GetPath where  ou graphiccreationObject.exe is running
    GetModuleFileName(NULL,szPathExe,1024);

    //Get A Folder's path
    c=strrchr(szPathExe,'\\');
    if(c) *c='\0';
    if(PathFileExists(szPathExe))
    {
        HRSRC hResInfo=FindResource(hInstance,"GCO_TUTO","GCOTYPE");
        WNDCLASSEX wc={0};
        wc.hIcon=LoadIcon(hInstance,"GCO");

        hwnd=GcoDisPlay(hResInfo,hInstance,&wc);
    }

    if(hwnd)
    {

        TCHAR szTuto[10000]="Note:This Window has been done using GCO :)                                                                                                        	                 1)DESCRIPTION                                                                           GCO(Graphic Creation Object) is a GUI's Library which helps win32 C developper for fastly create easy simple windows with controls, colors and fonts. And permit a developper to Load his creation into a c win32 source's code.                                                                                                                              2) YOUR FIRST GCO's PROJECT			    			   a)Before create                                                                             Any GCO's project, is created on a GRAY's zone that you see when you lunch a program.We are calling this zone ZC(Zone's Creation).                                                                                       As many ohers apps in windows, GoTo File->New project. After having created a .gco'sFile,    a new dialogBox will appear with at the bottom, the buttons <Previous>(Disabled) and <Create>.Any GCO's project begins with this dialogBox, it is for create a main window where all the others objects will be created.                                                                                              If you do not create this windows, your GCO's project will be automatically closed and your .GCO 's file will be empty.                                                                                                       Else, if you want to continue, you need to click a <Create>'s button. Before clicking , you can do some modifications as fonts, background's color, text's color. The X and Y value specify the coordinates where your main window will be create on ZC.  W and H are the width and Height of a object you want to create. They are expressed in units of a caracter (width and Height) which will be set on this object(your main window here).                                               Example: 2W=2*(width of a character which depends on font you've set for your object)             pixels.3H=3*(Height of a character which depends on font you've set for your object)pixels.      By default, an object's font is a system font.                                                                                                                                                                                                                                                                                          b)After create                                                                                   Here ,you can see your mainWindow appear in ZC at the coordinates you've specified.                 >SIZE OBJECT: Use a touch <CapsLOck> or <Maj> of your keyboard for enable/disable         frame which permit you to resize the object you have created. you can also use directionnal        keyboard keys(LEFT,RIGHT,UP,DOWN) for improve an align perfectly an object. Use        the touches <+> before using directionnal keys for increase, same for <-> but if you want to       decrease a size of an object. You can also ,size with a mouse by placing a mouse on a frame created on <Caps lock>'s keys, just make a click and move.                                                             >ADD OBJECT: Make a right click on a main Window an just select <add object>, a                window will appear and will guide you for creating your object's choice.                                   >DUPLICATE OBJECT: Make a right click on an object an select duplicate. GCO will           recreate a same object will the same characteristics.                                                                       >MODIFY OBJECT: Make a right click on a object(already added or duplicated),a dialog      as the same used when creating will helps you to modify some characteristics.                              >LINK/UNLINK OBJECT: when two objects are linked, when you use a mouse for move          one, the other will also be moved, but a distance which separes both of them, won't                     change.UNLINK, just removes this effect.                                                                                                                                                                                                                                                                 3)USE my creation On my c win32 CODE                                                                           When you've created your project, just save it. If you haven't done any change, a GCODEF.h's File which includes the definitions of all objects and items of menu (theirs IDs) will be created.For using your creation in a c win32 APP, you will need those files :                                                                                                                                                                                 >your .GCO 's file: Created when you saved                                                                                  >A GCODEF.h associated with your .GCO 's file                                                                          >A GCO.h , libGCO_LIB.a, GCO_LIB.dll  files present where you have installed GCO on your pc.Put all thoses files on your IDE s project, link your project with .a s file, add the .h header files. When you have done it, you will use GcoDisplay() func before your main loop(while(GetMessage(.....))) for load your creation done in GCO's app.GcoDisplay() has three parameters, a first is a HRSRC hResInfo associated to a .gco file created. For this case, you will need to add your .gco's file as a resource of your project,then use FindResource() for get the hResInfo.The Third parameter is a PWNDCLASSEX , like with all default win32 's code. So you create a WNDCLASS wc, and fills it with your preference. Remember that the most important field to fill is your wc.lpwndproc. GCO need it for sending messages that your creation will receive,And you will be able to manage those messages through your code.If you set the thrid parameter of GcoDisplay as NULL, your creation will only be loaded but you will be unable to manage messages cause you won't have linked your window's procedure.When you have finished, if you have specified your window's procedure, remember to use a personalized WM_GCOINIT as your WM_CREATE for initializing stuff, and to return TO_GCO; when you are managing msg which could affect appearence of windows managed internally by GCO.This is not a great documentation, it's somehow a begining :).\0";

        HWND hwndEdit=GcoGetHwnd(ID_EDIT1);
        if(!hwndEdit)
        {
            MyPerror("kvdkfgf");
            MyMsgError();
            return;
        }
        SetWindowText(hwndEdit,szTuto);
    }
    return ;
}

LRESULT CALLBACK GcoProc(HWND hwnd,UINT msg,WPARAM wParam, LPARAM lParam)
{
    return TO_GCO;
}
