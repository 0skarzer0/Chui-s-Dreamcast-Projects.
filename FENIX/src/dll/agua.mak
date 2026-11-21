# Microsoft Developer Studio Generated NMAKE File, Based on agua.dsp
!IF "$(CFG)" == ""
CFG=agua - Win32 Debug
!MESSAGE No configuration specified. Defaulting to agua - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "agua - Win32 Release" && "$(CFG)" != "agua - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "agua.mak" CFG="agua - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "agua - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "agua - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "agua - Win32 Release"

OUTDIR=.\bin
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\bin
# End Custom Macros

ALL : "$(OUTDIR)\agua.dll"


CLEAN :
	-@erase "$(INTDIR)\agua.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\agua.dll"
	-@erase "$(OUTDIR)\agua.exp"
	-@erase "$(OUTDIR)\agua.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "include\fenix" /I "include\sdl" /I "include\zlib" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "HELLO_EXPORTS" /Fp"$(INTDIR)\agua.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\agua.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\agua.pdb" /machine:I386 /def:".\fenixdll.def" /out:"$(OUTDIR)\agua.dll" /implib:"$(OUTDIR)\agua.lib" 
DEF_FILE= \
	".\fenixdll.def"
LINK32_OBJS= \
	"$(INTDIR)\agua.obj"

"$(OUTDIR)\agua.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "agua - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\agua.dll"


CLEAN :
	-@erase "$(INTDIR)\agua.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\agua.dll"
	-@erase "$(OUTDIR)\agua.exp"
	-@erase "$(OUTDIR)\agua.ilk"
	-@erase "$(OUTDIR)\agua.lib"
	-@erase "$(OUTDIR)\agua.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "include\fenix" /I "include\sdl" /I "include\zlib" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "HELLO_EXPORTS" /Fp"$(INTDIR)\agua.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\agua.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\agua.pdb" /debug /machine:I386 /def:".\fenixdll.def" /out:"$(OUTDIR)\agua.dll" /implib:"$(OUTDIR)\agua.lib" /pdbtype:sept 
DEF_FILE= \
	".\fenixdll.def"
LINK32_OBJS= \
	"$(INTDIR)\agua.obj"

"$(OUTDIR)\agua.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("agua.dep")
!INCLUDE "agua.dep"
!ELSE 
!MESSAGE Warning: cannot find "agua.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "agua - Win32 Release" || "$(CFG)" == "agua - Win32 Debug"
SOURCE=.\agua.c

"$(INTDIR)\agua.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

