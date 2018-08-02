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

/*
*Header For: database interface
*/

#ifndef include_database_hpp 
#define include_database_hpp
#include "common.h"

struct QSTransaction // transaction structure
{
	QObject *Client; // the callee
	QString Cmd; // the command string
	int Id; // the transaction id
	//
	QString Data1; // extra data 
	QString Data2;
	QString Data3;
	QString Data4;
	//
	QSTransaction(QObject *p = 0,const QString &c = "", int id = 0, const QString &d1 = "", const QString &d2 = "", const QString &d3 = "", const QString &d4 = "") : 
	Client(p),Cmd(c),Id(id),Data1(d1),Data2(d2),Data3(d3),Data4(d4)
	{
	};
	//
	// we do deep copies of strings
	QSTransaction(const QSTransaction &t) : Client(t.Client),Cmd(t.Cmd),Id(t.Id),Data1(t.Data1),Data2(t.Data2),Data3(t.Data3),Data4(t.Data4)
	{
	};
	//
	virtual ~QSTransaction()
	{
	};
	static QSTransaction Null; // the empty transaction
};
//
// transaction failure code
// calls when the database fails
#define DB_FAILED_AND_RECOVERED (-1000)
//
//
// Helper Functions
QSEXPORT QString EscapeSQLText(const QString &in);
QSEXPORT QString UndoEscapeSQLText(const QString &in);  //APA added
QSEXPORT QString ReplaceNewLineWithSpace(const QString &in);
//
// for a property insert
// Use this where a single quote might slip into the data string
// 
inline QString QSInsertIntoProps(const QString &skey, const QString &ikey, const QString &data)
{
	QString cmd =
	"insert into PROPS values('" + skey + "','" + ikey +"','" +  EscapeSQLText(data) +	"');";
	return cmd;
};
//
class QSEXPORT QSDatabase : public QObject  // base class all things are virtual
{
	Q_OBJECT
	protected:
	typedef std::map <QString,QString, std::less<QString> > RecordDict;
	RecordDict Record;  // the dictionary of fields and values for building transactions
	protected:
	QString Name; // name of database
	public:
	QSDatabase();
	virtual ~QSDatabase();
	const QString & GetName() const { return Name;}; // get the name of the database to which we are connecting
	void SetName(const QString &s) { Name = s;}; // set the name 
	// connect to the database
	virtual bool Connect(const QString &host,const QString &dbname, const QString &user, const QString &password = "", bool is_low_freq = true) { return false;};
	virtual bool Disconnect() { return true;}; //  disconnect from the database
	virtual bool Exec(const QString &) { return 0;}; // execute an SQL statement
	virtual bool Ok() { return false;}; // error status
	virtual bool AnyUnRecovableErrors() { return false;}; // Does exist any unrecovable error in the client part of the database
	virtual bool IsConnected(){ return false;}; // error status
	virtual QString GetErrorMessage() {return QString::null;};
	virtual QString GetDbMessage() {return QString::null;};
	virtual QString GetDbQueryString() {return QString::null;};
	virtual void AcnoledgeError(){};
	//    
	void ClearRecord() { Record.clear();}; // clear the record diticonary
	//
	void AddToRecord(const QString &key, const QString &data, bool isstr = true) // add the field name and data
	{
		if(isstr) // is this a string field - hence must quote it
		{
			QString d = QString("'")+EscapeSQLText(data)+QString("'");
			RecordDict::value_type pr(key,d);
			Record.insert(pr);
		}
		else
		{
			// not a string field - so we do not need to quote it
			RecordDict::value_type pr(key,data);
			Record.insert(pr);
		};
	};
	void AddInt(const QString &key, int data) // add the field name and data
	{
		QString s;
		s.setNum(data);
		AddToRecord(key,s,false);
	};
	void AddDouble(const QString &key, double data) // add the field name and data
	{
		QString s;
		s.setNum(data);
		AddToRecord(key,s,false);
	};
	void AddBool(const QString &key, bool data) // add the field name and data
	{
		QString s;
		s.setNum(data?1:0);
		AddToRecord(key,s,false);
	};
	//
	virtual bool DoExec(QObject *Client,const QString &Cmd, int Id, const QString &d1 = "", const QString &d2 = "", const QString &d3 = "", const QString &d4 = ""); // action the transaction or set of transactions
	//
	virtual bool ExecDirect(const QString &Cmd) { return false;}; // exec direct non-async mode - we need this sometimes
	//
	virtual void DoneExec(QObject *) {}; // completed the exec - tidy up
	//
	//  Basic Database Operations - build strings
	//  
	QString Insert(const QString &table); // using the record dictionary generate an insert command
	QString Delete(const QString &table, const QString &field, const QString &value,bool isstr = true); // delete a record based on a field
	QString Update(const QString &table, const QString &field, const QString &value,bool isstr = true); // using the record dict build an insert transaction
	QString Get(const QString &table, const QString &field, const QString &value,bool isstr = true); // fetch a record
	virtual QString QueryForTableFields(const QString &table); // get the list of table fields
	//
	//
	// Field access functions
	// When a tuple is read from the database these functions get the data and convert it to the target type
	// Use the toInt etc functions to convert from string to numbers
	// 
	virtual QString GetString(const QString &name) { return QString("");}; // get a string version of the data at the named column/tuple
	virtual QString GetIsoDateString(const QString &) { return QString("");};// get the Iso Date string value at the given field / tuple
	virtual int GetInt(const QString &name,bool *ok=0) { return GetString(name).toInt(ok);}; // get the data as Int or zero
	virtual bool GetBool(const QString &name, bool *ok = 0) { return (GetString(name).toInt(ok) != 0);}; // get the data as a bool
	virtual double GetDouble(const QString &name, bool *ok = 0) { return GetString(name).toDouble(ok);}; // get the data as a double
	
	#ifdef USING_GARRET
	virtual QDateTime GetDateTime(const QString &name) 
	{ 
		QString isodate = GetIsoDateString(name);
		return IsoDateMsToQDateTime(isodate);
	};// get the data as a date/time
	#else
	virtual QDateTime GetDateTime(const QString &name) { return IsoDateQDateTime(GetString(name));};// get the data as a date/time
	#endif

	virtual unsigned int GetNumberResults() const { return 0;}; // return the number of results
	virtual int GetFieldNames(QStringList &l) { return 0;}; // returns the list of field names and the number of fields
	//
	// some helper functions
	// fill a list box from the text in a given field (usually the name field). Does a query select <field> from <table>;
	virtual bool FillListBox(QListBox *pList, const QString &fieldname); // fill a list box from a column
	//
	// fill a combo box
	virtual bool FillComboBox(QComboBox *pList, const QString &fieldname); // fill a combo box from a column
	//
	// read /write to a edit box
	virtual bool CopyToText(QLineEdit *pT, const QString &fieldname) ; // fill and edit box from a col / tuple
	virtual bool CopyFromText(QLineEdit *pT, const QString &fieldname); // move from edit field to record
	//
	// read / write to a checkbox
	virtual bool CopyToCheckbox(QCheckBox *pT, const QString &fieldname); // set a check box from a tuple /col
	virtual bool CopyFromCheckbox(QCheckBox *pT, const QString &fieldname); // set a field from a checkbox
	//
	// read / write to a combo box
	virtual bool CopyToCombo(QComboBox *pT, const QString &fieldname); // set a combo selection from a col/tuple
	virtual bool CopyFromCombo(QComboBox *pT, const QString &fieldname); // save the selection from a combo to a field
	//
	virtual QString  DumpTuple(int t = 0) {return QString(tr(NONE_STR));}; // dump out the selected tuple to stderr
	void DumpRecord(); // debug trace the current record
	//
	// We need the current receipe name for filtering configurations
	static QString ReceipeName; // the name of the current receipe - used for various selections default = "(default)"
	//
	// 
	virtual QSTransaction & CurrentTransaction(){ return QSTransaction::Null;}; // returns the transaction at the head of the queue
	virtual unsigned int GetTransQueueSize() { return 0;}; // returns the size of the transation queue
	//
	virtual bool FetchNext() { return false;}; // fetch the next tuple
	virtual bool FetchPrev() { return false;}; // fetch the prev tuple
	virtual void GotoBegin() {}; // goto the first row 
	virtual void GotoEnd() {}; // goto the last row 

	signals:
	void TransactionDone(QObject *,const QString &, int, QObject*); // send a transaction complete message - unpacked
	void Ready(void *);                   // notify that the database is free for object with given address  
	
};
//
#endif

