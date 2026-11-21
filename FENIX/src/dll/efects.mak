# Microsoft Developer Studio Generated NMAKE File, Based on efects.dsp
!IF "$(CFG)" == ""
CFG=efects - Win32 Debug
!MESSAGE No configuration specified. Defaulting to efects - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "efects - Win32 Release" && "$(CFG)" != "efects - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "efects.mak" CFG="efects - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "efects - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "efects - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "efects - Win32 Release"

OUTDIR=.\bin
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\bin
# End Custom Macros

ALL : "$(OUTDIR)\efects.dll"


CLEAN :
	-@erase "$(INTDIR)\efects.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\efects.dll"
	-@erase "$(OUTDIR)\efects.exp"
	-@erase "$(OUTDIR)\efects.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "include\fenix" /I "include\sdl" /I "include\zlib" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "HELLO_EXPORTS" /Fp"$(INTDIR)\efects.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\efects.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\efects.pdb" /machine:I386 /def:".\fenixdll.def" /out:"$(OUTDIR)\efects.dll" /implib:"$(OUTDIR)\efects.lib" 
DEF_FILE= \
	".\fenixdll.def"
LINK32_OBJS= \
	"$(INTDIR)\efects.obj"

"$(OUTDIR)\efects.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "efects - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\efects.dll"


CLEAN :
	-@erase "$(INTDIR)\efects.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\efects.dll"
	-@erase "$(OUTDIR)\efects.exp"
	-@erase "$(OUTDIR)\efects.ilk"
	-@erase "$(OUTDIR)\efects.lib"
	-@erase "$(OUTDIR)\efects.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "HELLO_EXPORTS" /Fp"$(INTDIR)\efects.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\efects.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\efects.pdb" /debug /machine:I386 /def:".\fenixdll.def" /out:"$(OUTDIR)\efects.dll" /implib:"$(OUTDIR)\efects.lib" /pdbtype:sept 
DEF_FILE= \
	".\fenixdll.def"
LINK32_OBJS= \
	"$(INTDIR)\efects.obj"

"$(OUTDIR)\efects.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("efects.dep")
!INCLUDE "efects.dep"
!ELSE 
!MESSAGE Warning: cannot find "efects.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "efects - Win32 Release" || "$(CFG)" == "efects - Win32 Debug"
SOURCE=.\efects.c

"$(INTDIR)\efects.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

