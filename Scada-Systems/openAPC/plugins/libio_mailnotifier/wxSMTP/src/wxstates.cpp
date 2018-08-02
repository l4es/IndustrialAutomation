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
 $Log: wxstates.cpp,v $
 Revision 1.5  2004/06/29 11:06:20  tavasti
 - Added OnResponse handler also for initial state (sometimes OnConnect
   arrives after first data)
 - Minor changes in indentation & comments

 Revision 1.4  2003/11/21 12:36:46  tavasti
 - Makefilet -Wall optioilla
 - Korjattu 'j�rkev�t' varoitukset pois (J��nyt muutama joita ei saa
   kohtuudella poistettua)

 Revision 1.3  2003/11/13 17:12:15  tavasti
 - Muutettu tiedostojen nimet wx-alkuisiksi

 Revision 1.2  2003/11/07 09:17:40  tavasti
 - K��ntyv� versio, fileheaderit lis�tty.


****************************************************************************/

//static char cvs_id[] = "$Header: /v/CVS/olive/notifier/wxSMTP/src/wxstates.cpp,v 1.3 2004/09/09 20:38:51 paul Exp $";

/*
 * Purpose: private wxWindows helper classes for SMTP
 * Author:  Frank Bu�
 * Created: 2002
 */

#include "wxstates.h"


const InitialState g_initialState;
const ConnectedState g_connectedState;
const HeloState g_heloState;
const SendMailFromState g_sendMailFromState;
const RcptListState g_rcptListState;
const StartDataState g_startDataState;
const DataState g_dataState;
const QuitState g_quitState;
const ClosedState g_closedState;
const RetryState g_retryState;



ClosedState::ClosedState()
{

}



void MailState::onTimeout(wxSMTP& context) const
{
    context.OnMessageStatus(wxEmailMessage::dispRetry);
    context.Final();
}



InitialState::InitialState()
{

}



void InitialState::onConnect(wxSMTP& context, wxSocketEvent& event) const
{
    if (event.GetSocketEvent() == wxSOCKET_CONNECTION) {
	context.ChangeState(g_connectedState);
	context.TimerStart(wxSMTP_TIMEOUT_READY);
    } else {
	// error
	context.OnMessageStatus(wxEmailMessage::dispRetry);
	context.Final();
    }
}

void InitialState::onResponse(wxSMTP& context, int smtpCode, const wxString &smtpRsp) const
{
    // Make sure this is same as ConnectedState::onResponse
    g_connectedState.onResponse(context, smtpCode, smtpRsp);
}



ConnectedState::ConnectedState()
{

}



void ConnectedState::onResponse(wxSMTP& context, int smtpCode, const wxString &smtpRsp) const
{
    double d;
    wxString rspLetter;
   
    context.TimerStop();

    // Make sure this is same as InitialState::onResponse
    if (smtpCode == 220) {
	    context.ChangeState(g_heloState);
	    // TODO: using some wxWindows function for getting the hostname
	    context.Write(_T("HELO localhost\x00d\x00a"));
	    context.TimerStart(wxSMTP_TIMEOUT_HELO);
	    return;
    }

    rspLetter=smtpRsp.Mid(0,1);
    rspLetter.ToDouble(&d);
    switch ((int)d) {
	case 4:
	    context.OnMessageStatus(wxEmailMessage::dispRetry);
	    context.ChangeState(g_quitState);
	    context.SendQuit();
	    context.TimerStart(wxSMTP_TIMEOUT_QUIT);
	    break;

	case 5:
	default:
	    context.OnMessageStatus(wxEmailMessage::dispFail);
	    context.ChangeState(g_quitState);
	    context.SendQuit();
	    context.TimerStart(wxSMTP_TIMEOUT_QUIT);
	    break;
    }
}



HeloState::HeloState()
{

}



void HeloState::onResponse(wxSMTP& context, int smtpCode, const wxString &smtpRsp) const
{
    double d;
    wxString rspLetter;
   
    context.TimerStop();

    if (smtpCode == 250) {
	    context.ChangeState(g_sendMailFromState);
	    context.SendFrom();
	    context.TimerStart(wxSMTP_TIMEOUT_MAIL);
	    return;
    }

    rspLetter=smtpRsp.Mid(0,1);
    rspLetter.ToDouble(&d);
    switch ((int)d) {
	case 4:
	    context.OnMessageStatus(wxEmailMessage::dispRetry);
	    context.ChangeState(g_quitState);
	    context.SendQuit();
	    context.TimerStart(wxSMTP_TIMEOUT_QUIT);
	    break;

	case 5:
	default:
	    context.OnMessageStatus(wxEmailMessage::dispFail);
	    context.ChangeState(g_quitState);
	    context.SendQuit();
	    context.TimerStart(wxSMTP_TIMEOUT_QUIT);
	    break;
    }
}



SendMailFromState::SendMailFromState()
{

}



void SendMailFromState::onResponse(wxSMTP& context, int smtpCode, const wxString &smtpRsp) const
{
    double d;
    wxString rspLetter;
   
    context.TimerStop();

    if (smtpCode == 250) {
	    context.ChangeState(g_rcptListState);
	    context.SendNextRecipient();
	    context.TimerStart(wxSMTP_TIMEOUT_RCPT);
	    return;
    }

    rspLetter=smtpRsp.Mid(0,1);
    rspLetter.ToDouble(&d);
    switch ((int)d) {
	case 4:
	    context.OnMessageStatus(wxEmailMessage::dispRetry);
	    context.ChangeState(g_quitState);
	    context.SendQuit();
	    context.TimerStart(wxSMTP_TIMEOUT_QUIT);
	    break;

	case 5:
	default:
	    context.OnMessageStatus(wxEmailMessage::dispFail);
	    context.ChangeState(g_quitState);
	    context.SendQuit();
	    context.TimerStart(wxSMTP_TIMEOUT_QUIT);
	    break;
    }
}



RcptListState::RcptListState()
{

}



void RcptListState::onResponse(wxSMTP& context, int smtpCode, const wxString &smtpRsp) const
{
    double d;
    wxString rspLetter;
   
    context.TimerStop();

    if (smtpCode == 421) {
	// 421 has per-message scope, not per-recipient scope
	context.OnMessageStatus(wxEmailMessage::dispRetry);
	context.ChangeState(g_quitState);
	context.TimerStart(wxSMTP_TIMEOUT_QUIT);
	context.SendQuit();
    }

    rspLetter=smtpRsp.Mid(0,1);
    rspLetter.ToDouble(&d);
    switch ((int)d) {
	case 2:
	    context.OnRecipientStatus(wxEmailMessage::dispAccept);
	    break;

	case 4:
	    context.OnRecipientStatus(wxEmailMessage::dispRetry);

	    // trigger early retry
	    context.SetFastRetryMode();

	    //
	    // don't attempt further addrs since we probably
	    // hit limit for this message
	    //
	    goto startdata;

	case 5:
	    if (smtpCode == 552) {

		// see RFC 2821 p.56 - treat as temporary
		context.OnRecipientStatus(wxEmailMessage::dispRetry);

		// trigger early retry
		context.SetFastRetryMode();

		//
		// don't attempt further addrs since we probably
		// hit limit for this message
		//
		goto startdata;
	    }
	    // fall through

	default:
	    context.OnRecipientStatus(wxEmailMessage::dispFail);
	    break;
    }

    if (context.SendNextRecipient()) {

	context.TimerStart(wxSMTP_TIMEOUT_RCPT);
	return;
    }

startdata:

    context.Write(_T("DATA\x00d\x00a"));
    context.ChangeState(g_startDataState);
    context.TimerStart(wxSMTP_TIMEOUT_DATA);
}



StartDataState::StartDataState()
{

}



void StartDataState::onResponse(wxSMTP& context, int smtpCode, const wxString &smtpRsp) const
{
    context.TimerStop();

    if (smtpCode == 354) {
	context.ChangeState(g_dataState);
	context.SendData();
	context.TimerStart(wxSMTP_TIMEOUT_DATAEND);
	return;
    }

    if (smtpRsp[0] == '4') {
	context.OnMessageStatus(wxEmailMessage::dispRetry);
    } else {
	context.OnMessageStatus(wxEmailMessage::dispFail);
    }

    context.ChangeState(g_quitState);
    context.SendQuit();
    context.TimerStart(wxSMTP_TIMEOUT_QUIT);
}



DataState::DataState()
{

}



void DataState::onResponse(wxSMTP& context, int smtpCode, const wxString &smtpRsp) const
{
    context.TimerStop();

    if (smtpCode == 250) {
	context.OnMessageStatus(wxEmailMessage::dispSucceed);
    } else if (smtpRsp[0] == '4') {
	context.OnMessageStatus(wxEmailMessage::dispRetry);
    } else {
	context.OnMessageStatus(wxEmailMessage::dispFail);
    }

    context.ChangeState(g_quitState);
    context.SendQuit();
    context.TimerStart(wxSMTP_TIMEOUT_QUIT);
}



QuitState::QuitState()
{

}



void QuitState::onResponse(wxSMTP& context, int smtpCode, const wxString &smtpRsp) const
{
    context.TimerStop();

    context.Final();
}

void QuitState::onTimeout(wxSMTP& context) const
{
    // message has already been accepted for delivery, so no error here
    context.Final();
}



RetryState::RetryState()
{

}



void RetryState::onTimeout(wxSMTP& context) const
{
    context.UpdateElapsed();
    context.SendInner();
}
