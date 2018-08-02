// SimCmdLine.h: interface for the CSimCmdLine class.
//
// Mod_RSSim (c) Embedded Intelligence Ltd. 1993,2009
// AUTHOR: Conrad Braam.  http://www.plcsimulator.org
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Affero General Public License as
//    published by the Free Software Foundation, either version 3 of the
//    License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Affero General Public License for more details.
//
//    You should have received a copy of the GNU Affero General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMCMDLINE_H__4A30F9B7_455E_41F3_810A_6072FF55AD2D__INCLUDED_)
#define AFX_SIMCMDLINE_H__4A30F9B7_455E_41F3_810A_6072FF55AD2D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSimCmdLine : public CCommandLineInfo  
{
public:
	CSimCmdLine();
	virtual ~CSimCmdLine();

   /*virtual */ void ParseParam( LPCTSTR lpszParam, BOOL bFlag, BOOL bLast );

   void SetPortParams (  const DWORD &aBaudRate, 
                         const DWORD &aByteSize, 
                         const DWORD &aParity, 
                         const DWORD & aStop, 
                         const DWORD &aRTS);

   bool GetPortSettings(char aPortName[MAX_COMPORT_NAME] , 
                         DWORD &aBaudRate, 
                         DWORD &aByteSize, 
                         DWORD &aParity, 
                         DWORD & aStop, 
                         DWORD &aRTS);
   bool GetProtocol(DWORD &aProtocol);
   bool GetIPSettings(DWORD &aPort);

   bool ParamsValid() { return( m_SerialControls || m_protocol!= -1);};
   // data
private:
   CString m_portName;
   DWORD m_baud ;
   DWORD m_byte;
   DWORD m_parity ;
   DWORD m_stop ;
   DWORD m_rts ;
   DWORD m_protocol ;
   DWORD m_ipPort ;
   
   DCB   m_dcbBlock;
   bool  m_SerialControls;
   CString m_DCBParams;
};

#endif // !defined(AFX_SIMCMDLINE_H__4A30F9B7_455E_41F3_810A_6072FF55AD2D__INCLUDED_)
