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
#ifndef BUTTONS_GROUPS_H
#define BUTTONS_GROUPS_H

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif
#include <qt.h>

class QCheckBox;
class QRadioButton;

class ButtonsGroups : public QWidget
{
    Q_OBJECT

public:
    ButtonsGroups( QWidget *parent = 0, const char *name = 0 );
	void EndProcess(int nIndex);
	BOOL StartProcess(int nIndex);
	void WriteLog(char* pMsg);
	void ProcessRestarter();

protected:
    QCheckBox *state;
    QRadioButton *rb21, *rb22, *rb23;

	const int nBufferSize;
	char pExeFile[501];
	char pInitFile[501];
	char pLogFile[501];
	const int nMaxProcCount;
	PROCESS_INFORMATION pProcInfo[127];
	SERVICE_STATUS          serviceStatus; 
	SERVICE_STATUS_HANDLE   hServiceStatusHandle;
	bool started;

protected slots:    
    void slotChangeGrp3State();
	void slotStartProcesses();
	void slotStopProcesses();

};

#endif
