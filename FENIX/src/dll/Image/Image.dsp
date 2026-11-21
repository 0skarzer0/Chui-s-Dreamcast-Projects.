# Microsoft Developer Studio Project File - Name="Image" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Image - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Image.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Image.mak" CFG="Image - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Image - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Image - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Image - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IMAGE_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "SDL_image-1.2.2\include" /I "..\..\include" /I "..\..\sdl\include" /I "..\..\libpng" /I "..\..\mikmod" /I "..\..\zlib" /I "..\..\fxi\inc\\" /I "..\..\fxi\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IMAGE_EXPORTS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc0a /d "NDEBUG"
# ADD RSC /l 0xc0a /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "Image - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IMAGE_EXPORTS" /YX /FD /GZ  /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "SDL_image-1.2.2\include" /I "..\..\include" /I "..\..\sdl\include" /I "..\..\libpng" /I "..\..\mikmod" /I "..\..\zlib" /I "..\..\fxi\inc\\" /I "..\..\fxi\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IMAGE_EXPORTS" /FR /FD /GZ  /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc0a /d "_DEBUG"
# ADD RSC /l 0xc0a /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Image - Win32 Release"
# Name "Image - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\fenixdll.def
# End Source File
# Begin Source File

SOURCE=.\image.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Libs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\DXSDK\include\basetsd.h
# End Source File
# Begin Source File

SOURCE=..\..\SDL\include\begin_code.h
# End Source File
# Begin Source File

SOURCE=..\..\SDL\include\close_code.h
# End Source File
# Begin Source File

SOURCE=..\..\SDL\include\SDL.h
# End Source File
# Begin Source File

SOURCE=..\..\SDL\include\SDL_active.h
# End Source File
# Begin Source File

SOURCE=..\..\SDL\include\SDL_audio.h
# End Source File
# Begin Source File

SOURCE=..\..\SDL\include\SDL_byteorder.h
# End Source File
# Begin Source File

SOURCE=..\..\SDL\include\SDL_cdrom.h
# End Source File
# Begin Source File

SOURCE=..\..\SDL\include\SDL_error.h
# End Source File
# Begin Source File

SOURCE=..\..\SDL\include\SDL_events.h
# End Source File
# Begin Source File

SOURCE=..\..\SDL\include\SDL_getenv.h
# End Source File
# Begin Source File

SOURCE=".\SDL_image-1.2.2\include\SDL_image.h"
# End Source File
# Begin Source File

SOURCE=..\..\SDL\include\SDL_joystick.h
# End Source File
# Begin Source File

SOURCE=..\..\SDL\include\SDL_keyboard.h
# End Source File
# Begin Source File

SOURCE=..\..\SDL\include\SDL_keysym.h
# End Source File
# Begin Source File

SOURCE=..\..\SDL\include\SDL_main.h
# End Source File
# Begin Source File

SOURCE=..\..\SDL\include\SDL_mouse.h
# End Source File
# Begin Source File

SOURCE=..\..\SDL\include\SDL_mutex.h
# End Source File
# Begin Source File

SOURCE=..\..\SDL\include\SDL_quit.h
# End Source File
# Begin Source File

SOURCE=..\..\SDL\include\SDL_rwops.h
# End Source File
# Begin Source File

SOURCE=..\..\SDL\include\SDL_timer.h
# End Source File
# Begin Source File

SOURCE=..\..\SDL\include\SDL_types.h
# End Source File
# Begin Source File

SOURCE=..\..\SDL\include\SDL_version.h
# End Source File
# Begin Source File

SOURCE=..\..\SDL\include\SDL_video.h
# End Source File
# Begin Source File

SOURCE=..\..\ZLIB\zconf.h
# End Source File
# Begin Source File

SOURCE=..\..\ZLIB\zlib.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\include\files_st.h
# End Source File
# Begin Source File

SOURCE=..\..\fxi\inc\flic_st.h
# End Source File
# Begin Source File

SOURCE=..\..\include\fxdll.h
# End Source File
# Begin Source File

SOURCE=..\..\fxi\inc\grlib_st.h
# End Source File
# Begin Source File

SOURCE=..\..\fxi\inc\i_procdef_st.h
# End Source File
# Begin Source File

SOURCE=..\..\fxi\inc\instance_st.h
# End Source File
# Begin Source File

SOURCE=..\..\include\offsets.h
# End Source File
# Begin Source File

SOURCE=..\..\include\pslang.h
# End Source File
# Begin Source File

SOURCE=..\..\include\typedef_st.h
# End Source File
# Begin Source File

SOURCE=..\..\include\xctype_st.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=..\..\LIB\SDL.lib
# End Source File
# Begin Source File

SOURCE=".\SDL_image-1.2.2\lib\SDL_image.lib"
# End Source File
# End Target
# End Project
