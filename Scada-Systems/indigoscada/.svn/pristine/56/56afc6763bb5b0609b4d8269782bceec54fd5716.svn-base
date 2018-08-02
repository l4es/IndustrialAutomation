############ UniLog Makefile for Visual C #########
# Select the *SINGLE* path
###################################################

MSDIR="C:\Program Files\Microsoft Visual Studio\VC98"

###################################################

#INCLUDES=-I $(MSDIR)\include
#LIBDIR=/LIBPATH:$(MSDIR)\lib
CFLAGS= /MD /W3 /O2
LDFLAGS= /OPT:NOREF /MAP /DLL
LIBS =advapi32.lib wsock32.lib
CC=cl -nologo
LD=link
AR=lib /OUT:

DLLNAME = unilog
DLL  = $(DLLNAME).dll
STLIB = stunilog.lib
ALLTARG = $(DLL) $(IMPLIB) $(STLIB) rwlock.obj condsb.obj

############################################
# Common targets & rules
O=obj
RES=res
!include objects.mk


$(DLL) $(IMPLIB): dllmain.obj $(OBJ) $(DLLNAME).def
        $(LD) $(LDFLAGS) /OUT:$*.dll /DEF:$*.def $(OBJ) dllmain.obj $(LIBDIR) $(LIBS)

$(STLIB): $(OBJ)
	$(AR)$@ $(OBJ)

#############################################