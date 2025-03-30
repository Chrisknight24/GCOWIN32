cmake_minimum_required(VERSION 3.10)
project(GCOWIN32)

set(SOURCES
    src/CacheManagment.c
    src/DialogProc.c
    src/DisplayTuto.c
    src/main.c
    src/MyChildProcDlgButton.c
    src/MyChildProcDlgComboBox.c
    src/MyChildProcDlgDialogBox.c
    src/MyChildProcDlgEditBox.c
    src/MyChildProcDlgGroupBox.c
    src/MyChildProcDlgListBox.c
    src/MyChildProcDlgStatic.c
    src/myCreateWindow.c
    src/MyLoadBorderOpt.c
    src/MyManageCheckStyle.c
    src/MyManageColor.c
    src/MyManageFont.c
    src/MyManageMenu.c
    src/MyParentProcObj.c
    src/MyRegisterOtherClass.c
    src/mystrtok.c
    src/MyTestProje.c
    src/NewObjWndProc.c
    src/OpenDialogBox.c
    src/OpenDlg_button.c
    src/OpenDlg_choosetypeobj.c
    src/OpenDlg_combobox.c
    src/OpenDlg_dlgbox.c
    src/OpenDlg_editbox.c
    src/OpenDlg_listbox.c
    src/OpenDlg_static.c
    src/OpenMenuDialogBox.c
    src/toolBar.c
    src/projetManagement.c
	)

set(HEADERS
    include/GCO.h
    include/GCODEF.h
    include/managefont.h
    include/manageProjet.h
    include/myCreateWindow.h
    include/mystrtok.h
    include/OpenDialogBox.h
    include/resource.h
    include/toolBar.h
)

set(RESOURCES
    resources/1.gco
    resources/GCO.rc
    resources/GCO2.ico
    resources/projetManagement.rc
    resources/resource.rc
    resources/run.bmp
    resources/stopbmp.bmp
    resources/toolbar.rc
	resources/defcol.txt
	resources/mf.txt
	resources/style.txt
	
)

include_directories(include)

add_executable(GCOWIN32 ${SOURCES} ${HEADERS} ${RESOURCES})

target_link_libraries(GCOWIN32
    gdi32
    user32
    comctl32
	comdlg32
	kernel32
	ole32
	shlwapi
    #lib/libGCO_LIB.a  is a x64 version, change it to libGCOx86.a if you wanna to compile in x86
	"${CMAKE_SOURCE_DIR}/lib/libGCO_LIB.a" # Chemin absolu
    #lib/GCO_LIB.dll is the x64 version, change it to libGCOx86 if you want compile x86 version
	"${CMAKE_SOURCE_DIR}/lib/GCO_LIB.dll" # Chemin absolu
)