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
*Header For: database.cpp
*Purpose: these are the base databse functions
*/

#include "dbase.h"
#include "IndentedTrace.h"

QSTransaction QSTransaction::Null; // empty transaction
QSDatabase::QSDatabase ()
{
	IT_IT("QSDatabase::QSDatabase");
};
QSDatabase::~QSDatabase ()
{
	IT_IT("QSDatabase::~QSDatabase");
	Disconnect ();
};
/*
*Function: QueryForTableFields
*Inputs:table name
*Outputs:none
*Returns:none
*/
QString QSDatabase::QueryForTableFields(const QString &table) // get the list of table fields
{
	IT_IT("QSDatabase::QueryForTableFields");
	return QString(";");
};
/*
*Function:DoExec
*Inputs:Client, command, transaction id, extra data
*Outputs:none
*Returns:true on success
*/
bool QSDatabase::DoExec(QObject *Client,const QString &Cmd, int Id, const QString &d1, const QString &d2, const QString &d3, const QString &d4) // action the transaction or set of transactions
{
	IT_IT("QSDatabase::DoExec");
	return false;
};
/*-Function:FillListBox
*Inputs:list box pointer, name of key field, name of table, selection criteria
*Outputs:none
*Returns:true on success
*/
bool QSDatabase::FillListBox (QListBox * pList, const QString & fieldname)
{
	IT_IT("QSDatabase::FillListBox");
	bool res = false;
	unsigned n = GetNumberResults ();
	//
	pList->clear ();
	//
	if (n)
	{
		do
		{
			pList->insertItem (GetString (fieldname));
		}while (FetchNext());
	};
	//
	return res;
};
/*-Function: FillCombox
*Fill a combo box from a table
*Inputs:combo box, file name , table name, selection criteria
*Outputs:none
*Returns:true on success
*/
// fill a combo box
bool QSDatabase::FillComboBox (QComboBox * pList, const QString & fieldname)
{
	IT_IT("QSDatabase::FillComboBox");
	bool res = false;
	unsigned n = GetNumberResults ();
	//
	pList->clear ();
	//
	if (n)
	{
		do
		{
			pList->insertItem (GetString (fieldname));
		} while(FetchNext());
	};
	//
	return res;
};
/*-Function: CopyToText
*copies from a field to edit control
*Inputs:edit control, field name
*Outputs:none
*Returns:true on success
*/
//
// read /write to a edit box
bool QSDatabase::CopyToText (QLineEdit * pT, const QString & fieldname)
{
	IT_IT("QSDatabase::CopyToText");
	pT->setText (GetString (fieldname));
	return true;
};
/*-Function: CopyFromText
*copies from an edit control to a field
*Inputs:edit control , field name
*Outputs:none
*Returns:true on success
*/
bool QSDatabase::CopyFromText (QLineEdit * pT, const QString & fieldname)
{
	IT_IT("QSDatabase::CopyFromText");
	AddToRecord (fieldname, pT->text ());
	return true;
};
/*-Function: CopyToCheckbox
*copies from field to checkbox
*Inputs:control, fieldname
*Outputs:none
*Returns:true on success
*/
// read / write to a checkbox
bool QSDatabase::CopyToCheckbox (QCheckBox * pT, const QString & fieldname)
{
	IT_IT("QSDatabase::CopyToCheckbox");
	pT->setChecked (GetString (fieldname).toInt ());
	return true;
};
/*-Function:CopyFromCheckbox
*Inputs:control, field name
*Outputs:none
*Returns:true on success
*/
bool QSDatabase::CopyFromCheckbox (QCheckBox * pT, const QString & fieldname)
{
	IT_IT("QSDatabase::CopyFromCheckbox");
	AddToRecord (fieldname, (pT->isChecked ()? "1" : "0"));
	return true;
};
/*-Function: CopyToCombo
*sets the combo selection to the field value 
*Inputs:control, field
*Outputs:none
*Returns:true on success
*/
//
// read / write to a combo box
bool QSDatabase::CopyToCombo (QComboBox * pT, const QString & fieldname)
{
	IT_IT("QSDatabase::CopyToCombo");
	SetComboItem (pT, GetString (fieldname));
	return true;
};
/*-Function: CopyFromCombo
*Copies from combo to field (currently selected value)
*Inputs:combo, field
*Outputs:none
*Returns:true on success
*/
bool QSDatabase::CopyFromCombo (QComboBox * pT, const QString & fieldname)
{
	IT_IT("QSDatabase::CopyFromCombo");
	AddToRecord (fieldname, pT->currentText ());
	return false;
};
/*-Function: Insert
* walk the record dict and generate the insert
* Inputs:table name
* Outputs:the command
* Returns:true on success
*/
QString QSDatabase::Insert (const QString & table)	// insert a new record
{
	IT_IT("QSDatabase::Insert");
	RecordDict::iterator i = Record.begin ();
	QString cmd = QString ("insert into ") + table + QString (" ( ");
	QString val = QString ("values (");
	for (; !(i == Record.end ()); i++)
	{
		if (!(i == Record.begin ()))
		{
			cmd += QString (",");
			val += QString (",");
		};
		cmd +=   (*i).first;
		val += (*i).second;
	};
	cmd += " ) ";		// build the record
	val += " ) ; ";
	cmd += val;
	return cmd;
};
/*-Function: Delete
*Inputs:table name, key field, key value
*Outputs:none
*Returns:true on success
*/
QString QSDatabase::Delete (const QString & table, const QString & field,
const QString & value, bool isstr)
{
	IT_IT("QSDatabase::Delete");
	QString cmd = QString ("delete from ") + table + " where " + field + "=";
	if (isstr)
	{
		cmd += "'" +  EscapeSQLText(value) + "';";	// maybe necessary to quote it all 
	}
	else
	{
		cmd += value + ";";
	}
	return cmd;
};
/*-Function: Update
*Inputs:table name , key field, key value
*Outputs:none
*Returns:true on success
*/
QString QSDatabase::Update (const QString & table, const QString & field,
const QString & value, bool isstr)
{
	IT_IT("QSDatabase::Update");
	RecordDict::iterator i = Record.begin ();
	//
	QString cmd = QString ("update ") + table + " set ";
	for (; !(i == Record.end ()); i++)
	{
		if (!(i == Record.begin ()))
		{
			cmd += QString (",");
		};
		cmd +=   (*i).first + "=" + (*i).second;
	};
	cmd += QString (" where ") + field + "=";
	if (isstr)
	{
		cmd += "'" +  EscapeSQLText(value) + "'; ";
	}
	else
	{
		cmd += value + ";";
	};
	return cmd;
};
/*-Function: Get
*Inputs:table name, key field, key value
*Outputs:none
*Returns:true on success
*/
QString QSDatabase::Get (const QString & table, const QString & field, const QString & value, bool isstr)
{
	IT_IT("QSDatabase::Get");

	QString cmd =
	QString ("select * from ") + table + " where " + field + " =";
	//
	if (isstr)
	{
		cmd += "'" + value + "';";	// maybe necessary to quote it all 
	}
	else
	{
		cmd += value + ";";
	};
	return cmd;
};
/*
*Function: DumpRecord
*dumps the record map to stderr
*Inputs:none
*Outputs:none
*Returns:none
*/
void QSDatabase::DumpRecord () // trace dump
{
	IT_IT("QSDatabase::DumpRecord");

	#ifdef UNIX
	RecordDict::iterator i = Record.begin ();
	cerr << "No Entries " << Record.size () << endl;
	for (; i != Record.end (); i++)
	{
		cerr << (const char *) (*i).first << " " << (const char *) (*i).
		second << endl;
	};
	cerr << " ****************************** " << endl;
	#endif
};
/*
*Function: EscapeSQLText
*Inputs:string to check
*Outputs:none
*Returns:processed string
*/

#define EQUOT "&quot"
#define ENEWL "&newl"
#define EAMPE "&ampe"
#define ETABE "&tabe"
#define STRLEN_EQUOT 5
#define STRLEN_ENEWL 5
#define STRLEN_EAMPE 5
#define STRLEN_ETABE 5

QString EscapeSQLText(const QString &in)  
{
	QString out;
	for(unsigned i = 0; i < in.length();i++)
	{
		QChar c  = in.constref(i);

		switch(c)
		{
			case '\'': //single quote
			out.append(EQUOT);
			break;

			case '\n': //new line
			out.append(ENEWL);
			break;

			case '&':  //ampersend
			out.append(EAMPE);
			break;

			case '\t':  //TAB
			//out.append(ETABE);
			out.append("");
			break;

			default:
			out.append(c);
			break;
		}
	}
	return out;
}


/*
*Function: EscapeSQLText
*Inputs:string to check
*Outputs:none
*Returns:processed string
*/
QString UndoEscapeSQLText(const QString &in)  //APA added
{
	    int pos;
		QString internal = in;
		QString out = "";

		while((pos = internal.find(EQUOT)) != -1)
		{
			out = internal.left(pos);
			QString a = internal.right(internal.length() - pos - STRLEN_EQUOT);
			out += "\'" + a;
			internal = out;
		}

		while((pos = internal.find(ENEWL)) != -1)
		{
			out = internal.left(pos);
			out += "\n" + internal.right(internal.length() - pos - STRLEN_ENEWL);
			internal = out;
		}

		while((pos = internal.find(EAMPE)) != -1)
		{
			out = internal.left(pos);
			out += "&" + internal.right(internal.length() - pos - STRLEN_EAMPE);
			internal = out;
		}

		while((pos = internal.find(ETABE)) != -1)
		{
			out = internal.left(pos);
			out += "\t" + internal.right(internal.length() - pos - STRLEN_ETABE);
			internal = out;
		}
		
		return ((out == "") ? in : out);
}

QString ReplaceNewLineWithSpace(const QString &in)  
{
	QString out;
	for(unsigned i = 0; i < in.length();i++)
	{
		QChar c  = in.constref(i);

		switch(c)
		{
			case '\n': //new line
			out.append(" ");
			break;
			default:
			out.append(c);
			break;
		}
	}
	return out;
}