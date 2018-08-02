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
/**********************************************************************
--- Qt Architect generated file ---
File: SimulatorDigital.cpp
Last generated: Sat Apr 22 11:04:57 2000
*********************************************************************/
#include <qt.h>
#include "SimulatorDigital.h"
#include "IndentedTrace.h"

#define Inherited SimulatorDigitalData

SimulatorDigital::SimulatorDigital
(
QWidget* parent,
const char* name
)
:
Inherited( parent, name )
{
	IT_IT("SimulatorDigital::SimulatorDigital");
}

SimulatorDigital::~SimulatorDigital()
{
	IT_IT("SimulatorDigital::~SimulatorDigital");
}
void SimulatorDigital::Load(const QString &s)// load the configuration
{
	IT_IT("SimulatorDigital::Load");

	/*
	if(GetConfigureDb()->GetNumberResults())
	{ 
		QString s = UndoEscapeSQLText(GetConfigureDb()->GetString("DVAL"));
		QTextIStream is (&s);
		//
		QString a;
		is >> a;
		Time->setText(a.stripWhiteSpace());
		//
		is >> a;
		Mean->setText(a.stripWhiteSpace());
		//
		is >> a;
		Variation->setText(a.stripWhiteSpace());
	}
	else
	{
		Time->setText("00:01:00");
		Mean->setText("100");
		Variation->setText("10");
	}
	*/
};
void SimulatorDigital::Save(const QString &s)// save the configuration
{
	IT_IT("SimulatorDigital::Save");
	
	//
	// save from fields to properties
	// 
	QString cmd = "delete from PROPS where SKEY ='SAMPLEPROPS' and IKEY='"+s+"';";
	GetConfigureDb()->DoExec(0,cmd,0);
	//
	QString t;
	t =  Time->text() + " " + Mean->text() + " " + Variation->text();
	//
	cmd = "insert into PROPS values('SAMPLEPROPS','"+s+"','"+t.stripWhiteSpace()+"');";
	GetConfigureDb()->DoExec(0,cmd,0);
	//
};

