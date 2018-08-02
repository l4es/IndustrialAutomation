/**********************************************************************
--- Qt Architect generated file ---
File: Iec104driverInput.cpp
Last generated: Mon May 22 17:08:25 2000
*********************************************************************/
#include <qt.h>
#include "iec104driverInput.h"
#include "iec104driver.h"
#include "iec104driver_instance.h"

#define Inherited Iec104driverInputData

Iec104driverInput::Iec104driverInput
(
QWidget* parent,
const char* name
)
:
Inherited( parent, name ),Name(name)
{
	// add FP validators to Low and High
	//Low->setValidator(new QDoubleValidator(-100000.0,100000.0,3,Low));
	//High->setValidator(new QDoubleValidator(-100000.0,100000.0,3,High));
	IOA->setValidator(new QIntValidator(0,16777215,NULL,NULL));
}

Iec104driverInput::~Iec104driverInput()
{
}

void Iec104driverInput::Load(const QString &s)// load the configuration
{
	IT_IT("Iec104driverInput::Load");

	/*
	if(GetConfigureDb()->GetNumberResults())
	{ 
		QString s = UndoEscapeSQLText(GetConfigureDb()->GetString("DVAL"));
		QTextIStream is (&s);
		//
		QString a;
		//is >> a;
		//Time->setText(a.stripWhiteSpace());
		//
		//is >> a;
		//Mean->setText(a.stripWhiteSpace());
		//
		is >> a;
		IOA->setText(a.stripWhiteSpace());
	}
	else
	{
		//Time->setText("00:01:00");
		//Mean->setText("100");
		IOA->setText("10");
	}
	*/

	if(GetConfigureDb()->GetNumberResults())
	{ 
		QString s = UndoEscapeSQLText(GetConfigureDb()->GetString("IOA"));
		QTextIStream is (&s);
		//
		QString a;
		is >> a;
		IOA->setText(a.stripWhiteSpace());
	}
	else
	{
		IOA->setText("0");
	}
};

void Iec104driverInput::Save(const QString &s)// save the configuration
{
	//
	// save from fields to properties
	// 
	QString cmd = "delete from PROPS where SKEY ='SAMPLEPROPS' and IKEY='"+s+"';";
	GetConfigureDb()->DoExec(0,cmd,0);
	//
	QString t;
	t =  IOA->text();
	//
	cmd = "insert into PROPS values('SAMPLEPROPS','"+s+"','"+t.stripWhiteSpace()+"');";
	GetConfigureDb()->DoExec(0,cmd,0);
};
