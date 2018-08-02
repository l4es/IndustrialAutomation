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
File: SimulatorUnit.cpp
Last generated: Sat Apr 22 10:59:12 2000
*********************************************************************/
#include "SimulatorUnit.h"
#include "IndentedTrace.h"

#define Inherited SimulatorUnitData
SimulatorUnit::SimulatorUnit
(
QWidget* parent,
const char* name
)
: Inherited( parent, name ),Name(name)
{
	IT_IT("SimulatorUnit::SimulatorUnit");
	
	//
	setCaption(tr("Simulator Unit Configuration"));
	//
	connect (GetConfigureDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (QueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the database
	//
	QString cmd = "select IKEY,DVAL from PROPS where SKEY='UNITSIMULATOR' and IKEY='"+QString(name)+"';";
	GetConfigureDb()->DoExec(this,cmd,tItem);  
	//
}
SimulatorUnit::~SimulatorUnit()
{
	IT_IT("SimulatorUnit::~SimulatorUnit");
}
void SimulatorUnit::QueryResponse (QObject *p, const QString &c, int id, QObject*caller) // handles database responses
{
	if(p != this) return;

	IT_IT("SimulatorUnit::QueryResponse");

	switch(id)
	{
		case tItem:
		{
			if(GetConfigureDb()->GetNumberResults() > 0)
			{
				//
				// we have the properties response
				// do nothing
			}
			else
			{
				// create a properties 
				QString cmd = "insert into PROPS values('UNITSIMULATOR','"+Name+"','0');";
				GetConfigureDb()->DoExec(0,cmd,0);
				Simulator::pDriver->CreateNewUnit(this,Name, 16);
			};
		}; 
		break;
		default:
		break;
	};
};

