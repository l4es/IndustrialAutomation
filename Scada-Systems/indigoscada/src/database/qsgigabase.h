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

#ifndef include_qsgigabase_h 
#define include_qsgigabase_h 
#include "common.h"
#include "dbase.h"
#include "IndentedTrace.h" //APA added

#define DB_NOT_CONNECTED "Database_not_connected"

class Giga_Thread;
class QSGigabase  : public QSDatabase
{
	Q_OBJECT
	//
	Giga_Thread *pConnect; 
	unsigned int currentRecord; // current record index
	bool event(QEvent *e);
	//
	QQueue <QSTransaction> InQueue;  // transaction queue, e' usata solo dal thread principale

	int pending_queris;

	//
	public:
	QSGigabase() : pConnect(NULL),currentRecord(0)
	{
		IT_IT("QSGigabase::QSGigabase()");
		
		InQueue.setAutoDelete(true);
		InQueue.clear();
		pending_queris = 0;
	};
	virtual ~QSGigabase()
	{
		Disconnect();
	};
	virtual bool Connect(const QString &host, const QString &dbname, const QString &user, const QString &password = "", bool is_low_freq = true);
	virtual bool Disconnect();              // disconnect from database
	virtual QString GetString(const QString &); // get the string value at the given field / tuple
	virtual QString GetIsoDateString(const QString &); // get the Iso Date string value at the given field / tuple
	bool Ok(); // transaction completed OK ?
	bool AnyUnRecovableErrors(); // Does exist any unrecovable error in the client part of the database
	QString GetErrorMessage();
	QString GetDbMessage();
	QString GetDbQueryString();
	void DoneExec(QObject *p);
	void AcnoledgeError();
	virtual int GetFieldNames(QStringList &l); // returns the list of field names and the number of fields
	virtual bool DoExec(QObject *Client,const QString &Cmd, int Id, const QString &d1 = "", const QString &d2 = "", const QString &d3 = "", const QString &d4 = "");
	virtual QString  DumpTuple(int t = 0); // dump out the selected tuple to stderr
	virtual unsigned int GetNumberResults() const ; // return the number of results
	virtual QSTransaction & CurrentTransaction();
	virtual unsigned int GetTransQueueSize();
	virtual bool FetchNext(); // fetch next tuple
	virtual bool FetchPrev(); // fetch prev tuple
	virtual void GotoBegin() { currentRecord = 0;}; // go to the start
	virtual void GotoEnd() { currentRecord = GetNumberResults() - 1;}; // go to the end
};
#endif //include_qsgigabase_h 

