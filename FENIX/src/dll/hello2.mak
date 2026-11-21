# Microsoft Developer Studio Generated NMAKE File, Based on hello2.dsp
!IF "$(CFG)" == ""
CFG=hello2 - Win32 Debug
!MESSAGE No configuration specified. Defaulting to hello2 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "hello2 - Win32 Release" && "$(CFG)" != "hello2 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "hello2.mak" CFG="hello2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "hello2 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "hello2 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "hello2 - Win32 Release"

OUTDIR=.\bin
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\bin
# End Custom Macros

ALL : "$(OUTDIR)\hello2.dll"


CLEAN :
	-@erase "$(INTDIR)\hello2.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\hello2.dll"
	-@erase "$(OUTDIR)\hello2.exp"
	-@erase "$(OUTDIR)\hello2.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "include\fenix" /I "include\sdl" /I "include\zlib" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "hello2_EXPORTS" /Fp"$(INTDIR)\hello2.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\hello2.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\hello2.pdb" /machine:I386 /def:".\fenixdll.def" /out:"$(OUTDIR)\hello2.dll" /implib:"$(OUTDIR)\hello2.lib" 
DEF_FILE= \
	".\fenixdll.def"
LINK32_OBJS= \
	"$(INTDIR)\hello2.obj"

"$(OUTDIR)\hello2.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "hello2 - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\hello2.dll"


CLEAN :
	-@erase "$(INTDIR)\hello2.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\hello2.dll"
	-@erase "$(OUTDIR)\hello2.exp"
	-@erase "$(OUTDIR)\hello2.ilk"
	-@erase "$(OUTDIR)\hello2.lib"
	-@erase "$(OUTDIR)\hello2.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\sdl\include" /I "..\libpng" /I "..\mikmod" /I "..\zlib" /I "..\fxi\inc\\" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "hello2_EXPORTS" /Fp"$(INTDIR)\hello2.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\hello2.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\hello2.pdb" /debug /machine:I386 /def:".\fenixdll.def" /out:"$(OUTDIR)\hello2.dll" /implib:"$(OUTDIR)\hello2.lib" /pdbtype:sept 
DEF_FILE= \
	".\fenixdll.def"
LINK32_OBJS= \
	"$(INTDIR)\hello2.obj"

"$(OUTDIR)\hello2.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("hello2.dep")
!INCLUDE "hello2.dep"
!ELSE 
!MESSAGE Warning: cannot find "hello2.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "hello2 - Win32 Release" || "$(CFG)" == "hello2 - Win32 Debug"
SOURCE=.\hello2.c

"$(INTDIR)\hello2.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

