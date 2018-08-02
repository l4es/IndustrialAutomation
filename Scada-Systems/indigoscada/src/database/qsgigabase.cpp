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

#include "qsgigabase.h"
#include "gigaclient.h"
#include "realtimedb.h"

/*
*Function:event
*event handler
*Inputs:none
*Outputs:none
*Returns:none
*/
bool QSGigabase::event(QEvent *e)
{
	IT_IT("QSGigabase::event");

	if(e->type() == QEvent::User)
	{
		if(InQueue.count() > 0 && pConnect)
		{
			currentRecord = 0;
			QSTransaction *p = InQueue.head(); // get the current transaction
			emit TransactionDone(p->Client,p->Cmd,p->Id, this); // the current result is the only result set
			InQueue.remove(); // delete the transaction
			pending_queris--;

			IT_COMMENT2("PENDING QUERIES %d, obj = %p", pending_queris, this);

			if(InQueue.count() > 0)
			{
				pConnect->SetQuery(InQueue.head()->Cmd); // send the next transaction
			}
		}		
		return true;
	}
	return QSDatabase::event(e);
};
/*
*Function:Connect
*Inputs:hostname, database name , user name, password
*Outputs:none
*Returns:success status
*/
bool QSGigabase::Connect(const QString &host, const QString &dbname, 
				 const QString &user, const QString &password, bool is_low_freq)
{
	IT_IT("QSGigabase::Connect");

	SetName(dbname);

	if(host.isNull() || dbname.isNull()) //27-11-09
	{
		return false;
	}
	
	if(pConnect) delete pConnect;
	
	pConnect = new Giga_Thread(this,host, dbname, user, password, is_low_freq); // create the connection

	return pConnect->Ok();
};
/*
*Function:Disconnect
*Inputs:none
*Outputs:none
*Returns:none
*/
bool  QSGigabase::Disconnect()              // disconnect from database
{
	IT_IT("QSGigabase::Disconnect");
	
	bool res = true;
	if(pConnect) delete pConnect;
	pConnect = NULL;
	InQueue.clear();
	return res;
};

/*
*Function: GetIsoDateString
*Inputs:field name
*Outputs:none
*Returns: Iso Date string in field in current tuple
*/
QString  QSGigabase::GetIsoDateString(const QString &ns) // get the string value at the given field / tuple
{
	struct tm *ts;

	QString time_stamp = GetString(ns);
			
	__int64 timedate = _atoi64((const char*)time_stamp);

	int ms = timedate%1000;

	time_t seconds = timedate/1000;

	//Time stamp in database is UTC

	if((ts = localtime((time_t*)(&seconds))) == NULL)
	{
		IT_IT("QSFastdb::GetIsoDateString");
		IT_COMMENT("Error in localtime function");
		return QString::null;
	}

	//if(_isindst(ts))
	//{
	//	ts->tm_hour + 1; // Se siamo in ora legale aggiungo un'ora
	//}

	//
	// format the tm_struct into ISO time with milliseconds
	// 
	char b[50];
	sprintf(b,"%04d-%02d-%02d %02d:%02d:%02d.%03d",ts->tm_year + 1900,ts->tm_mon + 1,ts->tm_mday,
	ts->tm_hour,ts->tm_min,ts->tm_sec,ms);

	return QString(b);
};


/*
*Function: Getstring
*Inputs:field name
*Outputs:none
*Returns:string in field in current tuple
*/
QString  QSGigabase::GetString(const QString &ns) // get the string value at the given field / tuple
{
//	IT_IT("QSGigabase::GetString");
	
	QString s = ns;	
	QString res = "";
	if(pConnect)
	{
		Giga_Result * pR = pConnect->GetLastResult();
		if(pR && (pR->Tuples.count() > 0))
		{
			QStringList &l = *(pR->Tuples[currentRecord]);
			QStringList &f = pR->fieldNames;
			for(unsigned i = 0; i < f.count();i++)
			{
				if(f[i] == s)
				{
					return l[i];	
				}
			}
		}
	}

	return res;
};
/*
*Function: FetchNext
*fetchs the next tuple - or updates index to list
*Inputs:none
*Outputs:none
*Returns:none
*/
bool QSGigabase::FetchNext() // fetch next tuple
{
//	IT_IT("QSGigabase::FetchNext");
	
	bool res = false;
	if(pConnect)
	{
		Giga_Result * pR = pConnect->GetLastResult();
		if(pR)
		{
			if(currentRecord < (pR->Tuples.count() - 1)) // c'e' - 1 poiche' currentRecord parte da 0
			{
				res = true;
				currentRecord++;
			};
		};
	};	
	return res;
};

/*
*Function: FetchPrev
*fetchs the prev tuple - or updates index to list
*Inputs:none
*Outputs:none
*Returns:none
*/
bool QSGigabase::FetchPrev() // fetch prev tuple
{
//	IT_IT("QSGigabase::FetchPrev");
	
	bool res = false;
	if(pConnect)
	{
		Giga_Result * pR = pConnect->GetLastResult();
		if(pR)
		{
			if(currentRecord > 0)
			{
				res = true;
				currentRecord--;
			};
		};
	};	
	return res;
};

/*
*Function:Ok
*Inputs:none
*Outputs:none
*Returns:true if no errors
*/
bool  QSGigabase::Ok() // transaction completed OK ?
{
	//IT_IT("QSGigabase::Ok");
	
	bool res = false;
	if(pConnect)
	{
		res = pConnect->Ok();
	};
	return res;
};

/*
*Function:AnyUnRecovableErrors
*Inputs:none
*Outputs:none
*Returns:true if exist any unrecovable errors
*/
bool  QSGigabase::AnyUnRecovableErrors()
{
	//IT_IT("QSGigabase::AnyUnRecovableErrors");
	
	bool res = false;

	if(pConnect)
	{
		res = pConnect->AnyUnRecovableErrors();
	};
	return res;
};


QString QSGigabase::GetErrorMessage()
{ 
	return pConnect->GetErrorMessage();
}

void QSGigabase::AcnoledgeError(){ pConnect->AcnoledgeError();}

QString QSGigabase::GetDbMessage(){ return pConnect->GetDbMessage(); }
QString QSGigabase::GetDbQueryString(){ return pConnect->GetDbQueryString(); }

/*
*Function:GetFieldNames
*Inputs:none
*Outputs:list of fields
*Returns:none
*/
int  QSGigabase::GetFieldNames(QStringList &l) // returns the list of field names and the number of fields
{
	IT_IT("QSGigabase::GetFieldNames");
	
	int res = 0;
	if(pConnect)
	{
		l = pConnect->GetLastResult()->fieldNames;
		res = l.count();
	};
	return res;
};
/*
*Function:DoExec
*Inputs:clien tobject, command string,transaction id,data1 and data 2
*Outputs:none
*Returns:none
*/
bool  QSGigabase::DoExec(QObject *Client,const QString &Cmd, int Id, const QString &d1, const QString &d2, const QString &d3, const QString &d4 )
{
	IT_IT("QSGigabase::DoExec");
		
	if(pConnect)
	{
		IT_COMMENT((const char*)Cmd);
		
		QSTransaction *t = new QSTransaction(Client,Cmd,Id,d1,d2,d3,d4); // form the transaction object
		if(!InQueue.count()) 
		{
			// we have a zero count so must trigger the send receive loop
			pConnect->SetQuery(Cmd);
		}
		
		pending_queris++;

		IT_COMMENT2("PENDING QUERIES %d, obj = %p", pending_queris, this);

		InQueue.enqueue(t); 

		return true;
	}
	return false;
};
/*
*Function:DumpTuple
*Inputs:tuple index
*Outputs:none
*Returns:none
*/
QString   QSGigabase::DumpTuple(int t) // dump out the selected tuple to stderr
{
	IT_IT("QSGigabase::DumpTuple");
	
	QString res = "";
	if(pConnect)
	{
		Giga_Result *pR = pConnect->GetLastResult();
		if(pR)
		{
			if((pR->Tuples.size() > 0) && ((unsigned)t < pR->Tuples.size()))
			{
				// find the tuple
				QStringList &l = *(pR->Tuples[t]);
				for(unsigned i = 0; i < pR->fieldNames.count();i++)
				{
					res += pR->fieldNames[i] + "|" + l[i] + " | ";
				};
			};
		};
	};
	return res;
};
/*
*Function:GetNumberResults()
*Inputs:none
*Outputs:none
*Returns:none
*/
unsigned int QSGigabase::GetNumberResults() const // return the number of results
{
//	IT_IT("QSGigabase::GetNumberResults");
	
	unsigned int res = 0;
	if(pConnect && pConnect->GetLastResult())
	{
		res = (pConnect->GetLastResult())->Tuples.count();
	}
		
//	IT_COMMENT1("fetched records %d", res);

	return res;
};
/*
*Function:CurrentTransaction
*Inputs:none
*Outputs:none
*Returns:none
*/
QSTransaction &  QSGigabase::CurrentTransaction()
{
	//IT_IT("QSGigabase::CurrentTransaction");

	return *(InQueue.head());
};
/*
*Function:GetTransQueueSize
*Inputs:none
*Outputs:none
*Returns:none
*/
unsigned int QSGigabase::GetTransQueueSize()
{
	IT_IT("QSGigabase::GetTransQueueSize");

	return  InQueue.count();
};

/*-Function: DoneExec - 
*Inputs: link object - call this to clear any pending results or the final null result
*Outputs:none
*Returns:none
*/
void QSGigabase::DoneExec(QObject *p)
{

}; 
