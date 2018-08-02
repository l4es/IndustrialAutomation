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
 $Log: wxsmtp.cpp,v $
 Revision 1.5  2004/06/29 11:06:20  tavasti
 - Added OnResponse handler also for initial state (sometimes OnConnect
   arrives after first data)
 - Minor changes in indentation & comments

 Revision 1.4  2003/11/21 12:36:46  tavasti
 - Makefilet -Wall optioilla
 - Korjattu 'j‰rkev‰t' varoitukset pois (J‰‰nyt muutama joita ei saa
   kohtuudella poistettua)

 Revision 1.3  2003/11/13 17:12:15  tavasti
 - Muutettu tiedostojen nimet wx-alkuisiksi

 Revision 1.2  2003/11/07 09:17:40  tavasti
 - K‰‰ntyv‰ versio, fileheaderit lis‰tty.


****************************************************************************/

//static char cvs_id[] = "$Header: /v/CVS/olive/notifier/wxSMTP/src/wxsmtp.cpp,v 1.3 2004/09/09 20:38:51 paul Exp $";

/*
 * Purpose: private wxWindows mail transport implementation
 * Author:  Frank Buﬂ
 * Created: 2002
 */

#include <wx/sckstrm.h>

#include "wxSMTP.h"
#include "wxstates.h"

/**
 * Default listener, if the user doesn't specify one. Does nothing.
 */
wxSMTPListener g_nullListener;

void
wxSMTPTimer::Notify(void)
{
    m_pSmtp->OnTimeout();
}

wxSMTP::wxSMTP(wxSMTPListener* pListener) :
	m_retry_start_interval(30*60),
	m_retry_multiplier(1.5),
	m_retry_max_time(6*60*60),
	m_retry_fast_interval(20),
	m_pMailState(&g_initialState),
	m_msgdisposition(wxEmailMessage::dispMax),
	m_fastretry(FALSE),
	m_pTimer(this),
	m_retry_current_interval(30*60),
	m_retry_total_elapsed(0)
{
	if (pListener) {
		m_pListener = pListener;
	} else {
		m_pListener = &g_nullListener;
	}
	m_pMessage = NULL;
}

wxSMTP::~wxSMTP()
{
}

bool wxSMTP::Destroy()
{
    TimerStop();
    return wxCmdlineProtocol::Destroy();
}

void wxSMTP::EvaluateLine(const wxString& line)
{

    // TODO: implementing response timeout somewhere
    // TODO: implementing multiline response
    
    // get command
    unsigned long cmd = 0;
    line.ToULong(&cmd);
    m_pMailState->onResponse(*this, cmd, line);
}

void wxSMTP::OnTimeout()
{
    m_pMailState->onTimeout(*this);
}


void wxSMTP::Send(wxEmailMessage* pMessage)
{

    // no message means nothing to do
    if (!pMessage) return;
    m_pMessage = pMessage;

    m_retry_current_interval = 0;
    m_retry_total_elapsed = 0;

    SendInner();
}

void wxSMTP::SendInner()
{
    m_msgdisposition = wxEmailMessage::dispMax;	// invalid value
    m_fastretry = FALSE;

    // init new socket connection
    m_pMailState = &g_initialState;
    Connect();
}

void wxSMTP::OnConnect(wxSocketEvent& event)
{
    m_pMailState->onConnect(*this, event);
}

void wxSMTP::OnConnectionLost(wxSocketEvent& event)
{
    OnMessageStatus(wxEmailMessage::dispRetry);
    Final();
}

void wxSMTP::ChangeState(const MailState& mailState)
{
    m_pMailState = &mailState;
}

bool wxSMTP::SendNextRecipient()
{
    wxString	*rcpt;

    if (m_pMessage->GetNextRecipient(&rcpt)) {
	    m_currentRecipient = *rcpt;
	    Write(_T("RCPT TO:<") + m_currentRecipient + _T(">\x00d\x00a"));
	    return true;
    }

    return false;
}

void wxSMTP::SendData()
{
	wxSocketOutputStream out(*this);
	m_pMessage->Encode(out);
}

//
// if one or more recipients must be retried, do another smtp transaction
//
void wxSMTP::Final()
{
    int waittime;
    bool halt = FALSE;

    ChangeState(g_closedState);

    Close();

    if (m_msgdisposition != wxEmailMessage::dispRetry) {
	NotifyMessageStatus(halt);
	return;
    }

    if (m_fastretry) {
	waittime = m_retry_fast_interval;
    } else {
	if (!m_retry_current_interval)
	    m_retry_current_interval = m_retry_start_interval;
	else
	    m_retry_current_interval *= m_retry_multiplier;

	if (m_retry_current_interval + m_retry_total_elapsed >
	    m_retry_max_time) {

	    OnMessageStatus(wxEmailMessage::dispFail);
	    NotifyMessageStatus(halt);
	    return;
	}

	waittime = (int)m_retry_current_interval;
    }

    //
    // Wait, then retry
    //

    // give opportunity to stop the retry here
    NotifyMessageStatus(halt);

    if (halt) {
	return;
    }

    ChangeState(g_retryState);

    TimerStart(waittime);
}

