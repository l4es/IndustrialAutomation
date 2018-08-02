/**********************************************************************
--- Qt Architect generated file ---
File: Dnp3driverInput.cpp
Last generated: Mon May 22 17:08:25 2000
*********************************************************************/
#include <qt.h>
#include "dnp3driverInput.h"
#include "dnp3driver.h"
#include "dnp3driver_instance.h"

#define Inherited Dnp3driverInputData

Dnp3driverInput::Dnp3driverInput
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

Dnp3driverInput::~Dnp3driverInput()
{
}

void Dnp3driverInput::Load(const QString &s)// load the configuration
{
	IT_IT("Dnp3driverInput::Load");

	if(GetConfigureDb()->GetNumberResults())
	{ 
		QString s = UndoEscapeSQLText(GetConfigureDb()->GetString("IOA"));
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
		IOA->setText("0");
	};
};

void Dnp3driverInput::Save(const QString &s)// save the configuration
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
