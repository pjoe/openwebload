# Microsoft Developer Studio Generated NMAKE File, Based on openload.dsp
!IF "$(CFG)" == ""
CFG=openload - Win32 Debug
!MESSAGE No configuration specified. Defaulting to openload - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "openload - Win32 Release" && "$(CFG)" != "openload - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "openload.mak" CFG="openload - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "openload - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "openload - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "openload - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\openload.exe"


CLEAN :
	-@erase "$(INTDIR)\event_loop.obj"
	-@erase "$(INTDIR)\http_client.obj"
	-@erase "$(INTDIR)\http_headers.obj"
	-@erase "$(INTDIR)\http_test.obj"
	-@erase "$(INTDIR)\sock.obj"
	-@erase "$(INTDIR)\url.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\openload.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\openload.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\openload.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\openload.pdb" /machine:I386 /out:"$(OUTDIR)\openload.exe" 
LINK32_OBJS= \
	"$(INTDIR)\http_client.obj" \
	"$(INTDIR)\http_test.obj" \
	"$(INTDIR)\sock.obj" \
	"$(INTDIR)\event_loop.obj" \
	"$(INTDIR)\url.obj" \
	"$(INTDIR)\http_headers.obj"

"$(OUTDIR)\openload.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "openload - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\openload.exe"


CLEAN :
	-@erase "$(INTDIR)\event_loop.obj"
	-@erase "$(INTDIR)\http_client.obj"
	-@erase "$(INTDIR)\http_headers.obj"
	-@erase "$(INTDIR)\http_test.obj"
	-@erase "$(INTDIR)\sock.obj"
	-@erase "$(INTDIR)\url.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\openload.exe"
	-@erase "$(OUTDIR)\openload.ilk"
	-@erase "$(OUTDIR)\openload.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\openload.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ  /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\openload.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\openload.pdb" /debug /machine:I386 /out:"$(OUTDIR)\openload.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\http_client.obj" \
	"$(INTDIR)\http_test.obj" \
	"$(INTDIR)\sock.obj" \
	"$(INTDIR)\event_loop.obj" \
	"$(INTDIR)\url.obj" \
	"$(INTDIR)\http_headers.obj"

"$(OUTDIR)\openload.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("openload.dep")
!INCLUDE "openload.dep"
!ELSE 
!MESSAGE Warning: cannot find "openload.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "openload - Win32 Release" || "$(CFG)" == "openload - Win32 Debug"
SOURCE=.\event_loop.cpp

"$(INTDIR)\event_loop.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\http_client.cpp

"$(INTDIR)\http_client.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\http_headers.cpp

"$(INTDIR)\http_headers.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\http_test.cpp

"$(INTDIR)\http_test.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\sock.cpp

"$(INTDIR)\sock.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\url.cpp

"$(INTDIR)\url.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

