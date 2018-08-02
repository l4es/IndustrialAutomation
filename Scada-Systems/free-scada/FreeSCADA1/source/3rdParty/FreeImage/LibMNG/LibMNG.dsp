# Microsoft Developer Studio Project File - Name="LibMNG" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=LibMNG - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LibMNG.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LibMNG.mak" CFG="LibMNG - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LibMNG - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "LibMNG - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LibMNG - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O1 /I "..\Zlib" /I "..\libjpeg" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x413 /d "NDEBUG"
# ADD RSC /l 0x413 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "LibMNG - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\ZLib" /I "..\LibJPEG" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "MNG_BUILD_SO" /YX /FD /GZ /c
# ADD BASE RSC /l 0x413 /d "_DEBUG"
# ADD RSC /l 0x413 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "LibMNG - Win32 Release"
# Name "LibMNG - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\libmng_callback_xs.c
# End Source File
# Begin Source File

SOURCE=.\libmng_chunk_io.c
# End Source File
# Begin Source File

SOURCE=.\libmng_chunk_prc.c
# End Source File
# Begin Source File

SOURCE=.\libmng_chunk_xs.c
# End Source File
# Begin Source File

SOURCE=.\libmng_cms.c
# End Source File
# Begin Source File

SOURCE=.\libmng_display.c
# End Source File
# Begin Source File

SOURCE=.\libmng_dither.c
# End Source File
# Begin Source File

SOURCE=.\libmng_error.c
# End Source File
# Begin Source File

SOURCE=.\libmng_filter.c
# End Source File
# Begin Source File

SOURCE=.\libmng_hlapi.c
# End Source File
# Begin Source File

SOURCE=.\libmng_jpeg.c
# End Source File
# Begin Source File

SOURCE=.\libmng_object_prc.c
# End Source File
# Begin Source File

SOURCE=.\libmng_pixels.c
# End Source File
# Begin Source File

SOURCE=.\libmng_prop_xs.c
# End Source File
# Begin Source File

SOURCE=.\libmng_read.c
# End Source File
# Begin Source File

SOURCE=.\libmng_trace.c
# End Source File
# Begin Source File

SOURCE=.\libmng_write.c
# End Source File
# Begin Source File

SOURCE=.\libmng_zlib.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\libmng.h
# End Source File
# Begin Source File

SOURCE=.\libmng_chunk_io.h
# End Source File
# Begin Source File

SOURCE=.\libmng_chunk_prc.h
# End Source File
# Begin Source File

SOURCE=.\libmng_chunks.h
# End Source File
# Begin Source File

SOURCE=.\libmng_cms.h
# End Source File
# Begin Source File

SOURCE=.\libmng_conf.h
# End Source File
# Begin Source File

SOURCE=.\libmng_data.h
# End Source File
# Begin Source File

SOURCE=.\libmng_display.h
# End Source File
# Begin Source File

SOURCE=.\libmng_dither.h
# End Source File
# Begin Source File

SOURCE=.\libmng_error.h
# End Source File
# Begin Source File

SOURCE=.\libmng_filter.h
# End Source File
# Begin Source File

SOURCE=.\libmng_jpeg.h
# End Source File
# Begin Source File

SOURCE=.\libmng_memory.h
# End Source File
# Begin Source File

SOURCE=.\libmng_object_prc.h
# End Source File
# Begin Source File

SOURCE=.\libmng_objects.h
# End Source File
# Begin Source File

SOURCE=.\libmng_pixels.h
# End Source File
# Begin Source File

SOURCE=.\libmng_read.h
# End Source File
# Begin Source File

SOURCE=.\libmng_trace.h
# End Source File
# Begin Source File

SOURCE=.\libmng_types.h
# End Source File
# Begin Source File

SOURCE=.\libmng_write.h
# End Source File
# Begin Source File

SOURCE=.\libmng_zlib.h
# End Source File
# End Group
# End Target
# End Project
