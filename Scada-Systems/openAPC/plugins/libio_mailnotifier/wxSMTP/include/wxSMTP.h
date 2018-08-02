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
 $Log: wxsmtp.h,v $
 Revision 1.4  2004/06/29 11:06:20  tavasti
 - Added OnResponse handler also for initial state (sometimes OnConnect
   arrives after first data)
 - Minor changes in indentation & comments

 Revision 1.3  2003/11/13 17:12:15  tavasti
 - Muutettu tiedostojen nimet wx-alkuisiksi

 Revision 1.2  2003/11/07 09:17:40  tavasti
 - K‰‰ntyv‰ versio, fileheaderit lis‰tty.


****************************************************************************/

/*
 * Purpose: private wxWindows mail transport implementation
 * Author:  Frank Buﬂ
 * Created: 2002
 */

#ifndef SMPT_H
#define SMPT_H

#include <wx/wx.h>
#include <wx/protocol/protocol.h>

#include "wxemail.h"
#include "wxcmdprot.h"


/**
 * Public available interface for classes who wants to be notified
 * asynchronly about SMTP events. Such classes can use this class
 * as a base class and implement the methods of this class for
 * receiving and evaluating some events.
 * \sa For an example see WinoDialog.
 */
class wxSMTPListener
{
  public:

    //
    // Called when server responds to RCPT command for the recipient.
    // Note that in the current implementation, if a message-scope (as
    // opposed to a recipient-scope) error occurs, this method will NOT
    // be called for all recipients.
    //
    // \param address The recipient who was added.
    // \param disposition:
    //
    //  wxEmailMessage::dispAccept	address OK
    //  wxEmailMessage::dispFail	address rejected
    //  wxEmailMessage::dispRetry	address temporarily failed, try later
    //
    virtual void OnRecipientStatus(
	const wxString& address,
	wxEmailMessage::disposition) {};

    //
    // Called after we are done with this attempt.
    // \param disposition:
    //
    //  wxEmailMessage::dispSucceed	done: sent to all recipients
    //  wxEmailMessage::dispFail	done: failed
    //  wxEmailMessage::dispRetry	not done: retrying
    //
    // When dispSucceed or dispFail is signaled, caller of wxSMTP
    // should call wxSMTP::Destroy (DO NOT CALL delete!!)
    //
    virtual void OnMessageStatus(
	wxEmailMessage::disposition,
	bool &halt) {};
};


// forward declarations
class wxSMTP;
class MailState;
class RetryState;

class wxSMTPTimer : public wxTimer
{
public:
    wxSMTPTimer(wxSMTP *c) : m_pSmtp(c) {}
    void Notify(void);

private:
    wxSMTP	*m_pSmtp;
};


/**
 * Implementation of the sendmail class for SMTP email sending.
 */
class wxSMTP : public wxCmdlineProtocol //, public wxEvtHandler
{
public:
    wxSMTP(wxSMTPListener* pListener = NULL);
    
    ~wxSMTP();

    void SetHost(
	const wxString& host,
	const int port = 25)
    {
	m_host = host;
	m_port = port;
	m_service = _T("");
    }

    void SetHost(
	const wxString& host,
	const wxString& service)
    {
	m_host = host;
	m_service = service;
	m_port = 0;
    }

    void SetRetry(
	double start_seconds,
	double multiplier,
	double max_time)
    {
	m_retry_start_interval = start_seconds;
	m_retry_multiplier = multiplier;
	m_retry_max_time = max_time;
    }

    /**
     * Called by Sendmail::Send().
     * \sa Sendmail::Send()
     */
    void Send(wxEmailMessage* pMessage);

    void Discontinue();

    bool Destroy();

    ////////////////////////////////////////////////////////////////////
    // MailStateContext interface
    //
    // The following methods are called only from the state machine.
    // Users of wxSMTP should not call them.
    ////////////////////////////////////////////////////////////////////
    
    /**
     * Changes the internal state.
     * \param pMailState The new state.
     * \sa MailState.
     */
    void ChangeState(const MailState& mailState);

    void TimerStart(int seconds)
    {
	m_pTimer.Start(seconds * 1000, wxTIMER_ONE_SHOT);
    }

    void TimerStop(void)
    {
	m_pTimer.Stop();
    }

    /**
     * Is called by the SendMailFromState, if the SMTP server has acknowledged.
     * This method sends then the next recipient, until all recipients have
     * been sent. Then the StartDataState will be set.
     */
    bool SendNextRecipient();

    void OnRecipientStatus(wxEmailMessage::disposition d) {
	m_pMessage->OnRecipientStatus(m_currentRecipient, d);
	m_pListener->OnRecipientStatus(m_currentRecipient, d);
    }

    void OnMessageStatus(wxEmailMessage::disposition d) {
	m_msgdisposition = d;
    }

    void SetFastRetryMode()
    {
	m_fastretry = TRUE;
    }

    void Final();

    /**
     * Is called by HeloState to indicate that the from command can be sent.
     */
    void SendFrom()
    {
	Write(_T("MAIL FROM:<") + m_pMessage->GetFrom() + _T(">\x00d\x00a"));
    }

    /**
     * Is called by StartDataState to indicate that the DATA command can
     be sent.
     */
    void SendData();

    void SendQuit()
    {
	Write(_T("QUIT\x00d\x00a"));
    }

protected:

private:
    friend class	RetryState;
    friend class	wxSMTPTimer;

    void SendInner();
    void EvaluateLine(const wxString& line);
    void OnTimeout();
    void OnConnect(wxSocketEvent& event);
    void OnConnectionLost(wxSocketEvent& event);

    void UpdateElapsed()
    {
	m_retry_total_elapsed +=
	    (m_fastretry? m_retry_fast_interval: m_retry_current_interval);
    }

    //
    // This gets called when it is safe to call Destroy, namely,
    // immediately after the state machine calls Close().
    //
    // Caller of wxSMTP should call Destroy when it gets a "final"
    // status, i.e., either "succeed" or "fail".
    //
    void NotifyMessageStatus(bool &halt)
    {
	 m_pMessage->OnMessageStatus(m_msgdisposition);
	 m_pListener->OnMessageStatus(m_msgdisposition, halt);
    }

    wxSMTPListener	*m_pListener;
    wxEmailMessage	*m_pMessage;
    double		m_retry_start_interval;		// secs
    double		m_retry_multiplier;
    double		m_retry_max_time;		// secs
    int			m_retry_fast_interval;

    const MailState	*m_pMailState;
    wxEmailMessage::disposition m_msgdisposition;
    bool		m_fastretry;
    wxSMTPTimer		m_pTimer;
    wxString		m_currentRecipient;
    double		m_retry_current_interval;	// secs
    double		m_retry_total_elapsed;		// secs
};

//
// SMTP response timeouts, in seconds
//
#define wxSMTP_TIMEOUT_READY	30	// awaiting initial 220
#define wxSMTP_TIMEOUT_HELO	30	// rsp to HELO
#define wxSMTP_TIMEOUT_MAIL	30	// rsp to MAIL
#define wxSMTP_TIMEOUT_RCPT	30	// rsp to RCPT
#define wxSMTP_TIMEOUT_DATA	30	// rsp to DATA
#define wxSMTP_TIMEOUT_DATAEND	30	// rsp to data's final "."
#define wxSMTP_TIMEOUT_QUIT	30	// rsp to QUIT
#endif
