# Microsoft Visual C++ generated build script - Do not modify

PROJ = PROCHOOK
DEBUG = 0
PROGTYPE = 1
CALLER = \sources\system\prochook\hooktool\hooktool.exe
ARGS = 
DLLS = 
D_RCDEFINES = /d_DEBUG 
R_RCDEFINES = /dNDEBUG 
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = C:\SOURCES\SYSTEM\PROCHOOK\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = PROCHOOK.C  
FIRSTCPP =             
RC = rc
CFLAGS_D_WDLL = /nologo /Gs /G2 /Zp1 /W3 /Gf /Zi /ASw /Od /D "_DEBUG" /GD /Fd"PROCHOOK.PDB"
CFLAGS_R_WDLL = /nologo /Gs /G2 /Zp1 /W3 /Gf /ASw /O1 /D "WINVER"="0x300" /D "NDEBUG" /GD 
LFLAGS_D_WDLL = /NOLOGO /NOD /NOE /PACKC:61440 /ALIGN:16 /ONERROR:NOEXE /CO /MAP:FULL    
LFLAGS_R_WDLL = /NOLOGO /NOD /NOE /PACKC:61440 /NOPACKF /ALIGN:16 /ONERROR:NOEXE 
LIBS_D_WDLL = oldnames libw sdllcew commdlg.lib olecli.lib olesvr.lib shell.lib 
LIBS_R_WDLL = oldnames libw sdllcew 
RCFLAGS = /nologo 
RESFLAGS = /nologo /30 
RUNFLAGS = 
DEFFILE = PROCHOOK.DEF
OBJS_EXT = 
LIBS_EXT = 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_WDLL)
LFLAGS = $(LFLAGS_D_WDLL)
LIBS = $(LIBS_D_WDLL)
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_WDLL)
LFLAGS = $(LFLAGS_R_WDLL)
LIBS = $(LIBS_R_WDLL)
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist MSVC.BND del MSVC.BND]
!endif
SBRS = PROCHOOK.SBR \
		PROCUTIL.SBR \
		PROCINFO.SBR


PROCHOOK_DEP = c:\sources\system\prochook\prochook.h \
	c:\sources\system\prochook\phintern.h


PROCUTIL_DEP = c:\sources\system\prochook\prochook.h \
	c:\sources\system\prochook\phintern.h


PROCINFO_DEP = c:\sources\system\prochook\prochook.h \
	c:\sources\system\prochook\phintern.h


all:	$(PROJ).DLL

PROCHOOK.OBJ:	PROCHOOK.C $(PROCHOOK_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c PROCHOOK.C

PROCUTIL.OBJ:	PROCUTIL.C $(PROCUTIL_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c PROCUTIL.C

PROCINFO.OBJ:	PROCINFO.C $(PROCINFO_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c PROCINFO.C


$(PROJ).DLL::	PROCHOOK.OBJ PROCUTIL.OBJ PROCINFO.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
PROCHOOK.OBJ +
PROCUTIL.OBJ +
PROCINFO.OBJ +
$(OBJS_EXT)
$(PROJ).DLL
$(MAPFILE)
c:\msvc\lib\+
c:\msvc\mfc\lib\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) $@
	implib /nowep $(PROJ).LIB $(PROJ).DLL


run: $(PROJ).DLL
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
