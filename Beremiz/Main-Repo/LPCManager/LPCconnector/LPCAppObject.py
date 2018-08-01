#!/usr/bin/env python
# -*- coding: utf-8 -*-

#This file is part of Beremiz, a Integrated Development Environment for
#programming IEC 61131-3 automates supporting plcopen standard and CanFestival. 
#
#Copyright (C) 2007: Edouard TISSERANT and Laurent BESSARD
#
#See COPYING file for copyrights details.
#
#This library is free software; you can redistribute it and/or
#modify it under the terms of the GNU General Public
#License as published by the Free Software Foundation; either
#version 2.1 of the License, or (at your option) any later version.
#
#This library is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#General Public License for more details.
#
#You should have received a copy of the GNU General Public
#License along with this library; if not, write to the Free Software
#Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

import ctypes
from LPCAppProto import *
from LPCObject import *
from targets.typemapping import  LogLevelsCount, TypeTranslator, UnpackDebugBuffer

class LPCAppObject(LPCObject):
    def connect(self,comport):
        self.SerialConnection = LPCAppProto(comport,#number
                                         115200, #speed
                                         5)      #timeout

    def StartPLC(self, debug=False):
        self.HandleSerialTransaction(STARTTransaction())
            
    def StopPLC(self):
        self.HandleSerialTransaction(STOPTransaction())
        return True

    def ResetPLC(self):
        self.HandleSerialTransaction(RESETTransaction())
        return self.PLCStatus

    def GetPLCstatus(self):
        strcounts = self.HandleSerialTransaction(GET_LOGCOUNTSTransaction())
        if strcounts is not None and len(strcounts) == LogLevelsCount * 4:
            cstrcounts = ctypes.create_string_buffer(strcounts)
            ccounts = ctypes.cast(cstrcounts, ctypes.POINTER(ctypes.c_uint32))
            counts = [int(ccounts[idx]) for idx in xrange(LogLevelsCount)]
        else :
            counts = [0]*LogLevelsCount
        return self.PLCStatus, counts 

    def MatchMD5(self, MD5):
        data = self.HandleSerialTransaction(GET_PLCIDTransaction())
        if data is not None:
            return data[:32] == MD5[:32]
        return False

    def SetTraceVariablesList(self, idxs):
        """
        Call ctype imported function to append 
        these indexes to registred variables in PLC debugger
        """
        if idxs:
            buff = ""
            # keep a copy of requested idx
            self._Idxs = idxs[:]
            for idx,iectype,force in idxs:
                idxstr = ctypes.string_at(
                          ctypes.pointer(
                           ctypes.c_uint32(idx)),4)
                if force !=None:
                    c_type,unpack_func, pack_func = TypeTranslator.get(iectype, (None,None,None))
                    forced_type_size = ctypes.sizeof(c_type) \
                        if iectype != "STRING" else len(force)+1
                    forced_type_size_str = chr(forced_type_size)
                    forcestr = ctypes.string_at(
                                ctypes.pointer(
                                 pack_func(c_type,force)),
                                 forced_type_size)
                    buff += idxstr + forced_type_size_str + forcestr
                else:
                    buff += idxstr + chr(0)
        else:
            buff = ""
            self._Idxs =  []

        self.HandleSerialTransaction(SET_TRACE_VARIABLETransaction(buff))

    def GetTraceVariables(self):
        """
        Return a list of variables, corresponding to the list of required idx
        """
        strbuf = self.HandleSerialTransaction(
                                     GET_TRACE_VARIABLETransaction())
        if strbuf is not None and len(strbuf) > 4 and self.PLCStatus == "Started":
            size = len(strbuf) - 4
            ctick = ctypes.create_string_buffer(strbuf[:4])
            tick = ctypes.cast(ctick, ctypes.POINTER(ctypes.c_int)).contents
            cbuff = ctypes.create_string_buffer(strbuf[4:])
            buff = ctypes.cast(cbuff, ctypes.c_void_p)
            TraceVariables = UnpackDebugBuffer(buff, size,  self._Idxs)
            if TraceVariables is not None:
                return self.PLCStatus, tick.value, TraceVariables
        return self.PLCStatus, None, [] 

    def ResetLogCount(self):
        self.HandleSerialTransaction(RESET_LOGCOUNTSTransaction())

    def GetLogMessage(self, level, msgid):
        strbuf = self.HandleSerialTransaction(GET_LOGMSGTransaction(level, msgid))
        if strbuf is not None and len(strbuf) > 12:
            cbuf = ctypes.cast(
                          ctypes.c_char_p(strbuf[:12]),
                          ctypes.POINTER(ctypes.c_uint32))
            return (strbuf[12:],)+tuple(int(cbuf[idx]) for idx in range(3))
        return None
