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
File: SimulatorAnalog.cpp
Last generated: Sat Apr 22 11:04:57 2000
*********************************************************************/
#include <qt.h>
#include "SimulatorAnalog.h"
#include "IndentedTrace.h"

#define Inherited SimulatorAnalogData

SimulatorAnalog::SimulatorAnalog
(
QWidget* parent,
const char* name
)
:
Inherited( parent, name )
{
	IT_IT("SimulatorAnalog::SimulatorAnalog");
}

SimulatorAnalog::~SimulatorAnalog()
{
	IT_IT("SimulatorAnalog::~SimulatorAnalog");
}
void SimulatorAnalog::Load(const QString &s)// load the configuration
{
	IT_IT("SimulatorAnalog::Load");

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
	};   
};
void SimulatorAnalog::Save(const QString &s)// save the configuration
{
	IT_IT("SimulatorAnalog::Save");
	
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

