# Microsoft Developer Studio Generated NMAKE File, Based on hello.dsp
!IF "$(CFG)" == ""
CFG=hello - Win32 Debug
!MESSAGE No configuration specified. Defaulting to hello - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "hello - Win32 Release" && "$(CFG)" != "hello - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "hello.mak" CFG="hello - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "hello - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "hello - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "hello - Win32 Release"

OUTDIR=.\bin
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\bin
# End Custom Macros

ALL : "$(OUTDIR)\hello.dll"


CLEAN :
	-@erase "$(INTDIR)\hello.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\hello.dll"
	-@erase "$(OUTDIR)\hello.exp"
	-@erase "$(OUTDIR)\hello.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "include\fenix" /I "include\sdl" /I "include\zlib" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "HELLO_EXPORTS" /Fp"$(INTDIR)\hello.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\hello.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\hello.pdb" /machine:I386 /def:".\fenixdll.def" /out:"$(OUTDIR)\hello.dll" /implib:"$(OUTDIR)\hello.lib" 
DEF_FILE= \
	".\fenixdll.def"
LINK32_OBJS= \
	"$(INTDIR)\hello.obj"

"$(OUTDIR)\hello.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "hello - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\hello.dll"


CLEAN :
	-@erase "$(INTDIR)\hello.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\hello.dll"
	-@erase "$(OUTDIR)\hello.exp"
	-@erase "$(OUTDIR)\hello.ilk"
	-@erase "$(OUTDIR)\hello.lib"
	-@erase "$(OUTDIR)\hello.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\sdl\include" /I "..\libpng" /I "..\mikmod" /I "..\zlib" /I "..\fxi\inc\\" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "HELLO_EXPORTS" /Fp"$(INTDIR)\hello.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\hello.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\hello.pdb" /debug /machine:I386 /def:".\fenixdll.def" /out:"$(OUTDIR)\hello.dll" /implib:"$(OUTDIR)\hello.lib" /pdbtype:sept 
DEF_FILE= \
	".\fenixdll.def"
LINK32_OBJS= \
	"$(INTDIR)\hello.obj"

"$(OUTDIR)\hello.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("hello.dep")
!INCLUDE "hello.dep"
!ELSE 
!MESSAGE Warning: cannot find "hello.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "hello - Win32 Release" || "$(CFG)" == "hello - Win32 Debug"
SOURCE=.\hello.c

"$(INTDIR)\hello.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

