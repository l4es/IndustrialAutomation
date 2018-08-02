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
 $Log: wxemail.h,v $
 Revision 1.4  2003/11/14 15:43:09  tavasti
 Sending email with alternatives works

 Revision 1.3  2003/11/13 17:12:15  tavasti
 - Muutettu tiedostojen nimet wx-alkuisiksi

 Revision 1.2  2003/11/07 09:17:40  tavasti
 - K‰‰ntyv‰ versio, fileheaderit lis‰tty.


****************************************************************************/

/*
 * Purpose: wxWindows email implementation
 * Author:  Frank Buﬂ
 * Created: 2002
 */

#ifndef EMAIL_H
#define EMAIL_H

#include <wx/wx.h>
#include <wx/filename.h>
#include <wx/wfstream.h>

#include "wxmime.h"


/**
 * An email without transport mechanism specific declarations
 * or implementations.
 *
 * When the message is sent, the transport mechanism manipulates the
 * recipient disposition lists via OnRecipientStatus and OnMessageStatus
 * so that the necessary retry state information is maintained in this
 * object.
 */

class wxEmailMessage
{
public:
	enum disposition {
	    dispFail = 0,	// hard fail
	    dispRetry,		// temp fail: try again later
	    dispAccept,		// ok'd by RCPT command
	    dispSucceed,	// on their way
	    dispMax
	};

	/**
	 * Constructs a new email.
	 * \param subject Subject.
	 * \param text The body text.
	 * \param from From.
	 */
	wxEmailMessage(
	    const wxString& subject,
	    const wxString& text,
	    const wxString& from);

	/**
	 * Gets the from string, which was specified in the constructor.
	 * \return The from string.
	 */
	const wxString& GetFrom();

	/**
	 * Adds an additional recipient. You can add as many recipients
	 * as you want and don't have to specify it in the email message,
	 * for example if you want to send a mailing list. The RFC 821
	 * (SMTP) garantuees 100 recipients at once, but most 
	 * \param address Email address of the additional recipient.
	 */
	void AddRecipient(const wxString& address);

	/**
	 * Adds an additional recipient in the to-field.
	 * \param address The email address of the recipient.
	 */
	void AddTo(const wxString& address);

	/**
	 * Adds an additional recipient in the cc-field.
	 * \param address The email address of the recipient.
	 */
	void AddCc(const wxString& address);

	/**
	 * Adds an additional recipient in the bcc-field.
	 * \param address The email address of the recipient.
	 */
	void AddBcc(const wxString& address);

	// loads the file in memory and adds it to the message.

	/**
	 * Adds a file attachment. If no mime-type is specified,
	 * the type will be determined by the file extension.
	 * \param fileName Filename of the new file attachment.
	 * \param mimeMainType MIME main type, like 'image'.
	 * \param mimeSubType MIME sub type, like 'jpeg'.
	 */
	void AddFile(const wxString fileName,
                     const wxString mimeMainType =wxEmptyString,
                     const wxString mimeSubType =wxEmptyString);

        void AddAlternative(const wxString data,
                            const wxString mimeMainType,
                            const wxString mimeSubType);

	/**
	 * Writes the message as one blob, MIME encoded, if necessary,
	 * e.g. for sending it with SMTP.
	 * \param out The output stream to which the email has to be written.
	 */
	// 
	void Encode(wxOutputStream& out);

	//
	// Recipient/message disposition manipulation, for use in
	// error handling to set up retries etc.
	//

	// relies on calls to OnRecipientStatus to consume
	// m_rcptAttempt items
	bool GetNextRecipient(wxString **rcpt)
	{
	    if (m_rcptAttempt.IsEmpty())
		return FALSE;
	    *rcpt = &(m_rcptAttempt.Item(0));
	    return TRUE;
	}

	void OnRecipientStatus(const wxString &recipient, disposition d);
	void OnMessageStatus(disposition d);

	void GetRcptStats(wxArrayInt &stats)
	{
	    stats.Alloc(dispMax);
	    stats[dispFail]	= m_rcptDisp[dispFail].GetCount();
	    stats[dispRetry]	= m_rcptDisp[dispRetry].GetCount();
	    stats[dispAccept]	= m_rcptDisp[dispAccept].GetCount();
	    stats[dispSucceed]	= m_rcptDisp[dispSucceed].GetCount();
	}

private:
	wxString m_subject;
	wxString m_text;
	wxString m_from;
	wxArrayString m_rcptArray;	// envelope to; (complete)
	wxArrayString m_toArray;	// to: header
	wxArrayString m_ccArray;	// cc: header
	wxArrayString m_bccArray;	// bcc: header?

	wxArrayString m_rcptAttempt;	// envelope to: for this send attempt

	//
	// dispositions
	//
	// We have to keep them with the message and not in wxSMTP
	// because the message persists after wxSMTP is deleted.
	//
	// This is somewhat inefficient. Consider locking the
	// various recipient arrays above upon initial send and then
	// just storing arrays of integer index values below.
	//
	wxArrayString m_rcptDisp[dispMax];

	wxArrayMimePart m_mimeParts;
        wxArrayMimePart m_mimeAlternatives;

};


#endif
