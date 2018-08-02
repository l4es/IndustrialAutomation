/****************************************************************************

 Project     : 
 Author      : 
 Description : 

 VERSION INFORMATION:
 File    : $Source$
 Version : $Revision$
 Date    : $Date$
 Author  : $Author$

 History: 
 $Log: wxcmdprot.h,v $
 Revision 1.3  2003/11/13 17:12:15  tavasti
 - Muutettu tiedostojen nimet wx-alkuisiksi

 Revision 1.2  2003/11/07 09:17:40  tavasti
 - K‰‰ntyv‰ versio, fileheaderit lis‰tty.


****************************************************************************/

/*
 * Purpose: base class for all command line oriented internet protocols
 * Author:  Frank Buﬂ
 * Created: 2002
 */

#ifndef CMDPROT_H
#define CMDPROT_H

#include <wx/wx.h>

#include "wxemail.h"


class wxCmdlineProtocolSocketEventHandler;

/**
 * Base class for all command line oriented internet protocols.
 */
class wxCmdlineProtocol : public wxSocketClient
{
public:
    wxCmdlineProtocol();

	~wxCmdlineProtocol();

	/**
	 * Sets the server and port information. Call one or the
	 * other version; the first is for a numeric port number,
	 * the second is for a corresponding service name.
	 *
	 * No server connection will be established until 'Send' is called.
	 *
	 * \param host server.
	 * \param port TCP port number
	 * \param service alias for TCP port number
	 */
	virtual void SetHost(
	    const wxString& host,
	    const int port)
	{
	    m_host = host;
	    m_port = port;
	    m_service =_T("");
	}

	virtual void SetHost(
	    const wxString& host,
	    const wxString& service)
	{
	    m_host = host;
	    m_service = service;
	    m_port = 0;
	}

	/**
	 * Sets the server and the optional login information.
	 * Not currently used.
	 * \param user User used for login, if specified.
	 * \param password Password used for login, if specified.
	 */
	void SetUser(
	    const wxString& user =wxEmptyString,
	    const wxString& password =wxEmptyString)

	{
	    m_user = user;
	    m_password = password;
	}

	void Connect();

	/**
	 * Writes the specified string to the socket.
	 * \param msg The string to be written.
	 */
	void Write(const wxString& msg);

	// documented as void, but actually bool in wx 2.4.2
	virtual bool Close();

protected:

	friend class wxCmdlineProtocolSocketEventHandler;

	// handling for wxSOCKET_INPUT

	/**
	 * Is called if successfully connected to the server.
	 * \param event The wxSOCKET_CONNECTION event.
	 */
	virtual void OnConnect(wxSocketEvent& event) = 0;

	virtual void OnConnectionLost(wxSocketEvent& event) = 0;

	/**
	 * This method is called if new data is available on the socket.
	 * It extracts one line, terminated by CRLF, and calls EvaluateLine.
	 * \param event The wxSOCKET_INPUT event.
	 */
	void OnInput(wxSocketEvent& event);

	/**
	 * Is called for every line input.
	 * \param line The server response.
	 */
	virtual void EvaluateLine(const wxString& line) = 0;

	wxString	m_host;
	wxString	m_service;
	int		m_port;
	wxString	m_user;
	wxString	m_password;

private:
	/**
	 * Message handler for wxWindows socket events.
	 */
	void OnSocketEvent(wxSocketEvent& event);

	wxCmdlineProtocolSocketEventHandler* m_pCmdlineProtocolSocketEventHandler;
	wxString	m_inputLine;
};

#endif
