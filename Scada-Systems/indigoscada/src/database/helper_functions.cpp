/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2009 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */

#include "helper_functions.h"

// User configuration - global
static UserDetails userDetails;

UserDetails & GetUserDetails ()
{
	return userDetails;
};

/*
*Function:QSAuditTrail
*generates an audit log message - prompts for comment from user
*Inputs:parent widget, caption or event source, message text
*Outputs:none
*Returns:none
*/
void  QSAuditTrail(QWidget *parent,const QString &src, const QString &msg) // audit trail routine
{
	IT_IT("QSAuditTrail");
	
	UserDetails &u = GetUserDetails();

	//if(u.AuditLevel > UserDetails::AuditLevelNone)
	{
		QString user = QObject::tr("User");
		
		if(u.AuditLevel > UserDetails::AuditChanges)
		{
			// we must ask for a user comment with each change
			bool ok = 0;
			QString s = QInputDialog::getText(QObject::tr("Enter Reason For Change"),
				QObject::tr("Reason"),QLineEdit::Normal,QObject::tr("No Reason Given"),&ok,parent);
			if(ok && !s.isEmpty())
			{
				s = msg + "[" + s + "]";
				QString a = src + "/" + user + ":" + u.Name;
				QSAudit(a,s);
			}
			else
			{
				s = msg + "[" + QObject::tr("No Reason Given") + "]";
				QString a = src + "/" + user + ":" + u.Name;
				QSAudit(a,s);
			};
		}
		else
		{
			QString a = src + "/" + user + ":" + u.Name;
			QSAudit(a,msg);
		};
	};
};

/*
*Function: Various Database dependent time formats
*Inputs:none
*Outputs:none
*Returns:none
*/
const char * GetDateTimeFormat()
{
	#ifdef USING_GARRET
	return "'%04d-%02d-%02d %02d:%02d:%02d.%03d'";
	#endif
};
QString GetDateTimeColType()
{
	#ifdef USING_GARRET
	return QString("int8");
	#endif
};
QString GetDateTimeEpoch()
{
	#ifdef USING_GARRET
	return  QString("0");
	#endif
//	#ifdef USING_ODBC
//	return QString("'1970-01-01'");
//	#endif
};


QString GetDateTimeNowInSeconds()
{
	return QDateTimeToIsoDateInSeconds(QDateTime::currentDateTime());
};


QString GetDateTimeNowInMilliSeconds()
{
	return QDATE_TIME_ISO_DATE(QDateTime::currentDateTime());
};
