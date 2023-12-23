# Microsoft Developer Studio Project File - Name="DX63DS" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=DX63DS - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "R.DirectX.6.0.3DS.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "R.DirectX.6.0.3DS.mak" CFG="DX63DS - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DX63DS - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DX63DS - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DX63DS - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DX63DS_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\..\Source\AZX" /I "..\..\..\SDK\DX60\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DX63DS_EXPORTS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ddraw.lib dxguid.lib /nologo /dll /machine:I386 /libpath:"..\..\..\SDK\DX60\Lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=..\Watcom\Release\Watcom.exe Release\DX63DS.dll
# End Special Build Tool

!ELSEIF  "$(CFG)" == "DX63DS - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DX63DS_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\..\Source\AZX" /I "..\..\..\SDK\DX60\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DX63DS_EXPORTS" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ddraw.lib dxguid.lib /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:"..\..\..\SDK\DX60\Lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=..\Watcom\Debug\Watcom.exe Debug\DX63DS.dll
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "DX63DS - Win32 Release"
# Name "DX63DS - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\Source\R.DirectX.6.0.3DS\Main.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\R.DirectX.6.0.3DS\Module.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\R.DirectX.6.0.3DS\Renderer.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\R.DirectX.6.0.3DS\Renderer.Module.Watcom.def
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\R.DirectX.6.0.3DS\RendererValues.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\R.DirectX.6.0.3DS\Settings.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\Source\R.DirectX.6.0.3DS\DirectDraw.hxx
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\R.DirectX.6.0.3DS\Module.hxx
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\R.DirectX.6.0.3DS\Renderer.hxx
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\R.DirectX.6.0.3DS\RendererValues.hxx
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\R.DirectX.6.0.3DS\Settings.hxx
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
