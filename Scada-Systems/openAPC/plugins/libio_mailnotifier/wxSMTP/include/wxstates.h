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
 $Log: wxstates.h,v $
 Revision 1.4  2004/06/29 11:06:20  tavasti
 - Added OnResponse handler also for initial state (sometimes OnConnect
   arrives after first data)
 - Minor changes in indentation & comments

 Revision 1.3  2003/11/13 17:12:15  tavasti
 - Muutettu tiedostojen nimet wx-alkuisiksi

 Revision 1.2  2003/11/07 09:17:40  tavasti
 - K��ntyv� versio, fileheaderit lis�tty.


****************************************************************************/

/*
 * Purpose: private wxWindows helper classes for SMTP
 * Author:  Frank Bu�
 * Created: 2002
 */

#ifndef STATES_H
#define STATES_H

#include <wx/wx.h>
#include <wx/socket.h>

#include "wxSMTP.h"

/**
 * Base class for all mail states. Every mail state is only instantiated once
 * and has no attributes (flyweight pattern? I don't know the name). The only
 * reason for the state classes is not to have a big switch-case block.
 */
class MailState
{
public:
    virtual void onConnect(wxSMTP& context, wxSocketEvent& event) const {}

    virtual void onResponse(
	wxSMTP& context,
	int smtpCode,
	const wxString& line) const {}

    virtual void onTimeout(wxSMTP& context) const;
};

/**
 * This state is the initial state, when no server connection is made.
 */
class InitialState : public MailState
{
public:
   InitialState();
    /**
     * Switches to ConnectedState, if successfully connected.
     * \sa ConnectedState.
     */
    void onConnect(wxSMTP& context, wxSocketEvent& event) const ;

    void onResponse(
	wxSMTP& context,
	int smtpCode,
	const wxString &smtpRsp) const ;

};


/**
 * This state is active, if the server connection is established.
 */
class ConnectedState : public MailState
{
public:
   ConnectedState();
    /**
     * Switches to HeloState, if welcome message is received.
     * \sa HeloState.
     */

    void onResponse(
	wxSMTP& context,
	int smtpCode,
	const wxString &smtpRsp) const ;

};


/**
 * This state is active, when the helo message was sent.
 */
class HeloState : public MailState
{
public:
   HeloState();
    /**
     * Switches to SendMailFromState, if the helo message was acknowledged.
     * \sa SendMailFromState.
     */
    void onResponse(
	wxSMTP& context,
	int smtpCode,
	const wxString &smtpRsp) const ;

};


/**
 * This state is active, when the from message was sent.
 */
class SendMailFromState : public MailState
{
public:
   SendMailFromState();
    /**
     * Switches to RcptListState, if the from message was acknowledged.
     * \sa RcptListState.
     */

    void onResponse(
	wxSMTP& context,
	int smtpCode,
	const wxString &smtpRsp) const ;

};


/**
 * This state is active as long as there are additional recipients
 * and no error occured (TODO: error-handling not implemented).
 */
class RcptListState : public MailState
{
public:
   RcptListState();
    /**
     * Switches to StartDataState, if there are no more recipients.
     * \sa StartDataState.
     */

    void onResponse(
	wxSMTP& context,
	int smtpCode,
	const wxString &smtpRsp) const ;

};


/**
 * This state is active, when the DATA command was sent.
 */
class StartDataState : public MailState
{
public:
   StartDataState();
    /**
     * Switches to DataState, if the DATA command was acknowledged.
     * \sa DataState.
     */
    void onResponse(
	wxSMTP& context,
	int smtpCode,
	const wxString &smtpRsp) const ;

};


/**
 * This state is active, when the data was sent.
 */
class DataState : public MailState
{
public:
   DataState();
	/**
	 * Acknowledges by calling MailContext::OnDataSuccess,
	 * if the data sent was acknowledged.
	 * \sa SendmailImpl::OnDataSuccess().
	 */

    void onResponse(
	wxSMTP& context,
	int smtpCode,
	const wxString &smtpRsp) const ;

};

class QuitState : public MailState
{
public:
   QuitState();
    void onResponse(
	wxSMTP& context,
	int smtpCode,
	const wxString &smtpRsp) const ;

    void onTimeout(wxSMTP& context) const ;
};

class ClosedState : public MailState
{
public:
   ClosedState();
   // No escape from this state
};

class RetryState : public MailState
{
public:
   RetryState();
   void onTimeout(wxSMTP& context) const ;
};

extern const InitialState g_initialState;
extern const ConnectedState g_connectedState;
extern const HeloState g_heloState;
extern const SendMailFromState g_sendMailFromState;
extern const RcptListState g_rcptListState;
extern const StartDataState g_startDataState;
extern const DataState g_dataState;
extern const QuitState g_quitState;
extern const ClosedState g_closedState;
extern const RetryState g_retryState;

#endif
