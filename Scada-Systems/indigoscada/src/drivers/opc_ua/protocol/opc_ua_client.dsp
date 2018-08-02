# Microsoft Developer Studio Project File - Name="opc_ua_client" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=opc_ua_client - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "opc_ua_client.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "opc_ua_client.mak" CFG="opc_ua_client - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "opc_ua_client - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "opc_ua_client - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "opc_ua_client - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\Stack\core" /I ".\Stack\platforms\win32" /I ".\Stack\stackcore" /I ".\Stack\securechannel" /I ".\Stack\transport\tcp" /I ".\Stack\proxystub\clientproxy" /I ".\Stack\proxystub\serverstub" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "RIPC_DLL" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib ws2_32.lib /nologo /subsystem:console /machine:I386 /out:"c:\scada\bin/opc_ua_client.exe" /libpath:"$(QTDIR)\lib" /libpath:"c:\scada\lib"

!ELSEIF  "$(CFG)" == "opc_ua_client - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "opc_ua_client___Win32_Debug"
# PROP BASE Intermediate_Dir "opc_ua_client___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\Stack\core" /I ".\Stack\platforms\win32" /I ".\Stack\stackcore" /I ".\Stack\securechannel" /I ".\Stack\transport\tcp" /I ".\Stack\proxystub\clientproxy" /I ".\Stack\proxystub\serverstub" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "RIPC_DLL" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib ws2_32.lib /nologo /subsystem:console /incremental:no /debug /machine:I386 /out:"c:\scada\bin/opc_ua_client.exe" /pdbtype:sept /libpath:"$(QTDIR)\lib" /libpath:"c:\scada\lib"

!ENDIF 

# Begin Target

# Name "opc_ua_client - Win32 Release"
# Name "opc_ua_client - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\client\main.c
# End Source File
# Begin Source File

SOURCE=.\client\main.h
# End Source File
# Begin Source File

SOURCE=.\client\main_uatcp.c
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua.h
# End Source File
# Begin Source File

SOURCE=.\Stack\proxystub\clientproxy\opcua_asynccallstate.c
# End Source File
# Begin Source File

SOURCE=.\Stack\proxystub\clientproxy\opcua_asynccallstate.h
# End Source File
# Begin Source File

SOURCE=.\Stack\stackcore\opcua_attributes.h
# End Source File
# Begin Source File

SOURCE=.\Stack\stackcore\opcua_binarydecoder.c
# End Source File
# Begin Source File

SOURCE=.\Stack\stackcore\opcua_binaryencoder.c
# End Source File
# Begin Source File

SOURCE=.\Stack\stackcore\opcua_binaryencoder.h
# End Source File
# Begin Source File

SOURCE=.\Stack\stackcore\opcua_binaryencoderinternal.h
# End Source File
# Begin Source File

SOURCE=.\Stack\stackcore\opcua_browsenames.h
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua_buffer.c
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua_buffer.h
# End Source File
# Begin Source File

SOURCE=.\Stack\stackcore\opcua_builtintypes.c
# End Source File
# Begin Source File

SOURCE=.\Stack\stackcore\opcua_builtintypes.h
# End Source File
# Begin Source File

SOURCE=.\Stack\proxystub\clientproxy\opcua_channel.c
# End Source File
# Begin Source File

SOURCE=.\Stack\proxystub\clientproxy\opcua_channel.h
# End Source File
# Begin Source File

SOURCE=.\Stack\proxystub\clientproxy\opcua_channel_internal.h
# End Source File
# Begin Source File

SOURCE=.\Stack\proxystub\clientproxy\opcua_clientapi.c
# End Source File
# Begin Source File

SOURCE=.\Stack\proxystub\clientproxy\opcua_clientapi.h
# End Source File
# Begin Source File

SOURCE=.\Stack\proxystub\clientproxy\opcua_clientproxy.h
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua_config.h
# End Source File
# Begin Source File

SOURCE=.\Stack\stackcore\opcua_connection.c
# End Source File
# Begin Source File

SOURCE=.\Stack\stackcore\opcua_connection.h
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua_core.c
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua_core.h
# End Source File
# Begin Source File

SOURCE=.\Stack\stackcore\opcua_credentials.h
# End Source File
# Begin Source File

SOURCE=.\Stack\stackcore\opcua_crypto.c
# End Source File
# Begin Source File

SOURCE=.\Stack\stackcore\opcua_crypto.h
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua_cryptofactory.h
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua_datetime.c
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua_datetime.h
# End Source File
# Begin Source File

SOURCE=.\Stack\stackcore\opcua_decoder.c
# End Source File
# Begin Source File

SOURCE=.\Stack\stackcore\opcua_decoder.h
# End Source File
# Begin Source File

SOURCE=.\Stack\stackcore\opcua_encodeableobject.c
# End Source File
# Begin Source File

SOURCE=.\Stack\stackcore\opcua_encodeableobject.h
# End Source File
# Begin Source File

SOURCE=.\Stack\stackcore\opcua_encoder.c
# End Source File
# Begin Source File

SOURCE=.\Stack\stackcore\opcua_encoder.h
# End Source File
# Begin Source File

SOURCE=.\Stack\stackcore\opcua_enumeratedtype.c
# End Source File
# Begin Source File

SOURCE=.\Stack\stackcore\opcua_enumeratedtype.h
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua_errorhandling.h
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua_exclusions.h
# End Source File
# Begin Source File

SOURCE=.\Stack\stackcore\opcua_extensionobject.c
# End Source File
# Begin Source File

SOURCE=.\Stack\stackcore\opcua_extensionobject.h
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua_guid.c
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua_guid.h
# End Source File
# Begin Source File

SOURCE=.\Stack\stackcore\opcua_identifiers.h
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua_list.c
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua_list.h
# End Source File
# Begin Source File

SOURCE=.\Stack\stackcore\opcua_listener.c
# End Source File
# Begin Source File

SOURCE=.\Stack\stackcore\opcua_listener.h
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua_memory.c
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua_memory.h
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua_memorystream.c
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua_memorystream.h
# End Source File
# Begin Source File

SOURCE=.\Stack\stackcore\opcua_messagecontext.c
# End Source File
# Begin Source File

SOURCE=.\Stack\stackcore\opcua_messagecontext.h
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua_mutex.h
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_binary.c
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_binary.h
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_crypto.h
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_cryptofactory.c
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_cryptofactory.h
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_datetime.c
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_datetime.h
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_guid.c
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_guid.h
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_interface.c
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_interface.h
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_internal.c
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_internal.h
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_memory.c
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_memory.h
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_mutex.c
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_mutex.h
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_openssl.c
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_openssl.h
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_openssl_aes.c
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_openssl_hmac_sha.c
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_openssl_nosecurity.c
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_openssl_pki.c
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_openssl_pki.h
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_openssl_pki_nosecurity.c
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_openssl_random.c
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_openssl_rsa.c
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_openssl_sha.c
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_openssl_x509.c
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_pki.h
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_pkifactory.c
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_pkifactory.h
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_semaphore.c
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_semaphore.h
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_socket.c
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_socket.h
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_socket_interface.c
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_socket_interface.h
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_socket_internal.c
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_socket_internal.h
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_socket_ssl.c
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_socket_ssl.h
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_string.c
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_string.h
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_thread.c
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_thread.h
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_timer.c
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_timer.h
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_trace.c
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_trace.h
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_types.h
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_utilities.c
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_utilities.h
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_win32_pki.c
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_p_win32_pki.h
# End Source File
# Begin Source File

SOURCE=.\Stack\stackcore\opcua_pki.c
# End Source File
# Begin Source File

SOURCE=.\Stack\stackcore\opcua_pki.h
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua_pkifactory.h
# End Source File
# Begin Source File

SOURCE=.\Stack\platforms\win32\opcua_platformdefs.h
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua_proxystub.c
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua_proxystub.h
# End Source File
# Begin Source File

SOURCE=.\Stack\stackcore\opcua_securechannel.c
# End Source File
# Begin Source File

SOURCE=.\Stack\stackcore\opcua_securechannel.h
# End Source File
# Begin Source File

SOURCE=.\Stack\securechannel\opcua_securechannel_types.h
# End Source File
# Begin Source File

SOURCE=.\Stack\securechannel\opcua_secureconnection.c
# End Source File
# Begin Source File

SOURCE=.\Stack\securechannel\opcua_secureconnection.h
# End Source File
# Begin Source File

SOURCE=.\Stack\securechannel\opcua_securelistener.c
# End Source File
# Begin Source File

SOURCE=.\Stack\securechannel\opcua_securelistener.h
# End Source File
# Begin Source File

SOURCE=.\Stack\securechannel\opcua_securelistener_channelmanager.c
# End Source File
# Begin Source File

SOURCE=.\Stack\securechannel\opcua_securelistener_channelmanager.h
# End Source File
# Begin Source File

SOURCE=.\Stack\securechannel\opcua_securelistener_policymanager.c
# End Source File
# Begin Source File

SOURCE=.\Stack\securechannel\opcua_securelistener_policymanager.h
# End Source File
# Begin Source File

SOURCE=.\Stack\securechannel\opcua_securestream.c
# End Source File
# Begin Source File

SOURCE=.\Stack\securechannel\opcua_securestream.h
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua_semaphore.h
# End Source File
# Begin Source File

SOURCE=.\Stack\securechannel\opcua_soapsecurechannel.c
# End Source File
# Begin Source File

SOURCE=.\Stack\securechannel\opcua_soapsecurechannel.h
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua_socket.h
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua_stackstatuscodes.h
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua_statuscodes.h
# End Source File
# Begin Source File

SOURCE=.\Stack\stackcore\opcua_stream.c
# End Source File
# Begin Source File

SOURCE=.\Stack\stackcore\opcua_stream.h
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua_string.c
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua_string.h
# End Source File
# Begin Source File

SOURCE=.\Stack\stackcore\opcua_stringtable.c
# End Source File
# Begin Source File

SOURCE=.\Stack\stackcore\opcua_stringtable.h
# End Source File
# Begin Source File

SOURCE=.\Stack\transport\tcp\opcua_tcpconnection.c
# End Source File
# Begin Source File

SOURCE=.\Stack\transport\tcp\opcua_tcpconnection.h
# End Source File
# Begin Source File

SOURCE=.\Stack\transport\tcp\opcua_tcplistener.c
# End Source File
# Begin Source File

SOURCE=.\Stack\transport\tcp\opcua_tcplistener.h
# End Source File
# Begin Source File

SOURCE=.\Stack\transport\tcp\opcua_tcplistener_connectionmanager.c
# End Source File
# Begin Source File

SOURCE=.\Stack\transport\tcp\opcua_tcplistener_connectionmanager.h
# End Source File
# Begin Source File

SOURCE=.\Stack\securechannel\opcua_tcpsecurechannel.c
# End Source File
# Begin Source File

SOURCE=.\Stack\securechannel\opcua_tcpsecurechannel.h
# End Source File
# Begin Source File

SOURCE=.\Stack\transport\tcp\opcua_tcpstream.c
# End Source File
# Begin Source File

SOURCE=.\Stack\transport\tcp\opcua_tcpstream.h
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua_thread.c
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua_thread.h
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua_threadpool.c
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua_threadpool.h
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua_timer.c
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua_timer.h
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua_trace.c
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua_trace.h
# End Source File
# Begin Source File

SOURCE=.\Stack\stackcore\opcua_types.c
# End Source File
# Begin Source File

SOURCE=.\Stack\stackcore\opcua_types.h
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua_utilities.c
# End Source File
# Begin Source File

SOURCE=.\Stack\core\opcua_utilities.h
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
