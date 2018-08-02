/**********************************************************************
--- Qt Architect generated file ---
File: AckAlarmDlg.cpp
Last generated: Mon Jun 12 08:55:01 2000
*********************************************************************/
#include "AckAlarmDlg.h"
#include <general_defines.h>
#define Inherited AckAlarmDlgData
AckAlarmDlg::AckAlarmDlg
(
QWidget* parent,
const char* name
)
:
Inherited( parent, name )
{
	setCaption(tr("Acknowledge Alarms"));
	
	Comment->setMaxLength(MAX_LENGHT_OF_STRING);
	
}
AckAlarmDlg::~AckAlarmDlg()
{
}

