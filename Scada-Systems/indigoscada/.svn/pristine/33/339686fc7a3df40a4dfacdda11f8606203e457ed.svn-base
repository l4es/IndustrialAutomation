/**********************************************************************
--- Qt Architect generated file ---
File: ReportDir.cpp
Last generated: Thu May 18 10:44:23 2000
*********************************************************************/
#include "ReportDir.h"
#define Inherited ReportDirData
#include <qt.h>
#include "common.h"

ReportDir::ReportDir
(
QWidget* parent,
const char* name
)
:
Inherited( parent, name )
{
	setCaption(tr("Select Report"));
	//
	// list names of existing reports - this is just listing the directories in the report results directory
	QDir dir;
	dir.setFilter(QDir::Dirs);
	dir.setPath(QSREPORT_DIR);
	//
	for(unsigned i = 0; i < dir.count(); i++)
	{
		QString s = dir[i];
		if(s != QString::null)
		{
			if(s.left(1) != ".")
			{
				List->insertItem(s);
			};
		}
	};
	//
}
ReportDir::~ReportDir()
{
}

