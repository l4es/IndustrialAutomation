# Copyright (c) 1996-2017, OPC Foundation. All rights reserved.
#
#   The source code in this file is covered under a dual-license scenario:
#     - RCL: for OPC Foundation members in good-standing
#     - GPL V2: everybody else
#
#   RCL license terms accompanied with this source code. See http://opcfoundation.org/License/RCL/1.00/
#
#   GNU General Public License as published by the Free Software Foundation;
#   version 2 of the License are accompanied with this source code. See http://opcfoundation.org/License/GPLv2
#
#   This source code is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
ROOT = ..
ODIR = win32
OPENSSLDIR = $(ROOT)\openssl
TARGET_DIR = $(ROOT)\lib\$(ODIR)

TARGET = $(TARGET_DIR)\uastack.lib

OPENSSLINC = $(OPENSSLDIR)\include

CFLAGS = /MT /Ox /W3 /Gs0 /GF /Gy /nologo /Zl /Zi /Fd$(TARGET).pdb \
         /Icore /Istackcore /Isecurechannel /Itransport\tcp /Itransport\https \
         /Iproxystub\clientproxy /Iproxystub\serverstub /Iplatforms\win32 /I$(OPENSSLINC)

OBJECTS = \
	$(ODIR)\opcua_buffer.obj \
	$(ODIR)\opcua_core.obj \
	$(ODIR)\opcua_datetime.obj \
	$(ODIR)\opcua_guid.obj \
	$(ODIR)\opcua_list.obj \
	$(ODIR)\opcua_memory.obj \
	$(ODIR)\opcua_memorystream.obj \
	$(ODIR)\opcua_proxystub.obj \
	$(ODIR)\opcua_string.obj \
	$(ODIR)\opcua_thread.obj \
	$(ODIR)\opcua_threadpool.obj \
	$(ODIR)\opcua_timer.obj \
	$(ODIR)\opcua_trace.obj \
	$(ODIR)\opcua_utilities.obj \
	$(ODIR)\opcua_binarydecoder.obj \
	$(ODIR)\opcua_binaryencoder.obj \
	$(ODIR)\opcua_builtintypes.obj \
	$(ODIR)\opcua_connection.obj \
	$(ODIR)\opcua_crypto.obj \
	$(ODIR)\opcua_decoder.obj \
	$(ODIR)\opcua_encodeableobject.obj \
	$(ODIR)\opcua_encoder.obj \
	$(ODIR)\opcua_enumeratedtype.obj \
	$(ODIR)\opcua_extensionobject.obj \
	$(ODIR)\opcua_listener.obj \
	$(ODIR)\opcua_messagecontext.obj \
	$(ODIR)\opcua_pki.obj \
	$(ODIR)\opcua_securechannel.obj \
	$(ODIR)\opcua_stream.obj \
	$(ODIR)\opcua_stringtable.obj \
	$(ODIR)\opcua_types.obj \
	$(ODIR)\opcua_secureconnection.obj \
	$(ODIR)\opcua_securelistener.obj \
	$(ODIR)\opcua_securelistener_channelmanager.obj \
	$(ODIR)\opcua_securelistener_policymanager.obj \
	$(ODIR)\opcua_securestream.obj \
	$(ODIR)\opcua_soapsecurechannel.obj \
	$(ODIR)\opcua_tcpsecurechannel.obj \
	$(ODIR)\opcua_asynccallstate.obj \
	$(ODIR)\opcua_channel.obj \
	$(ODIR)\opcua_clientapi.obj \
	$(ODIR)\opcua_endpoint.obj \
	$(ODIR)\opcua_serverapi.obj \
	$(ODIR)\opcua_servicetable.obj \
	$(ODIR)\opcua_tcpconnection.obj \
	$(ODIR)\opcua_tcplistener.obj \
	$(ODIR)\opcua_tcplistener_connectionmanager.obj \
	$(ODIR)\opcua_tcpstream.obj \
	$(ODIR)\opcua_httpsconnection.obj \
	$(ODIR)\opcua_https_internal.obj \
	$(ODIR)\opcua_httpslistener.obj \
	$(ODIR)\opcua_httpslistener_connectionmanager.obj \
	$(ODIR)\opcua_httpsstream.obj \
	$(ODIR)\opcua_p_binary.obj \
	$(ODIR)\opcua_p_cryptofactory.obj \
	$(ODIR)\opcua_p_datetime.obj \
	$(ODIR)\opcua_p_guid.obj \
	$(ODIR)\opcua_p_interface.obj \
	$(ODIR)\opcua_p_internal.obj \
	$(ODIR)\opcua_p_memory.obj \
	$(ODIR)\opcua_p_mutex.obj \
	$(ODIR)\opcua_p_openssl.obj \
	$(ODIR)\opcua_p_openssl_aes.obj \
	$(ODIR)\opcua_p_openssl_hmac_sha.obj \
	$(ODIR)\opcua_p_openssl_nosecurity.obj \
	$(ODIR)\opcua_p_openssl_pki.obj \
	$(ODIR)\opcua_p_openssl_pki_nosecurity.obj \
	$(ODIR)\opcua_p_openssl_random.obj \
	$(ODIR)\opcua_p_openssl_rsa.obj \
	$(ODIR)\opcua_p_openssl_sha.obj \
	$(ODIR)\opcua_p_openssl_x509.obj \
	$(ODIR)\opcua_p_pkifactory.obj \
	$(ODIR)\opcua_p_semaphore.obj \
	$(ODIR)\opcua_p_socket.obj \
	$(ODIR)\opcua_p_socket_interface.obj \
	$(ODIR)\opcua_p_socket_internal.obj \
	$(ODIR)\opcua_p_socket_ssl.obj \
	$(ODIR)\opcua_p_string.obj \
	$(ODIR)\opcua_p_thread.obj \
	$(ODIR)\opcua_p_timer.obj \
	$(ODIR)\opcua_p_trace.obj \
	$(ODIR)\opcua_p_utilities.obj \
	$(ODIR)\opcua_p_win32_pki.obj \

all: $(TARGET)

clean:
	-del $(TARGET) $(TARGET).pdb 2>nul
	-del $(OBJECTS) 2>nul
	-rmdir $(ODIR) 2>nul

$(TARGET_DIR):
	-mkdir $(ROOT)\lib
	mkdir $(ROOT)\lib\$(ODIR)

$(ODIR):
	mkdir $(ODIR)

$(TARGET): $(TARGET_DIR) $(ODIR) $(OBJECTS)
	lib /nologo /out:$(TARGET) $(OBJECTS)

{core}.c{$(ODIR)}.obj:
	$(CC) $(CFLAGS) /Fo$@ /c $<

{stackcore}.c{$(ODIR)}.obj:
	$(CC) $(CFLAGS) /Fo$@ /c $<

{securechannel}.c{$(ODIR)}.obj:
	$(CC) $(CFLAGS) /Fo$@ /c $<

{proxystub\clientproxy}.c{$(ODIR)}.obj:
	$(CC) $(CFLAGS) /Fo$@ /c $<

{proxystub\serverstub}.c{$(ODIR)}.obj:
	$(CC) $(CFLAGS) /Fo$@ /c $<

{transport\tcp}.c{$(ODIR)}.obj:
	$(CC) $(CFLAGS) /Fo$@ /c $<

{transport\https}.c{$(ODIR)}.obj:
	$(CC) $(CFLAGS) /Fo$@ /c $<

{platforms\win32}.c{$(ODIR)}.obj:
	$(CC) $(CFLAGS) /Fo$@ /c $<
