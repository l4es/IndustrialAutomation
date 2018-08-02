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
*Header For:System
*
*Purpose: this is the header for the System device - this monitors disk space and memory. 
*
*/
#ifndef include_System 
#define include_System
#include "driver.h"
#include "sptypes.h"

#ifdef SYSTEM_EXPORTS
#define SYSTEMDRV __declspec(dllexport)
#else
#define SYSTEMDRV //__declspec(dllimport)
#endif


class SystemInstance;
class SYSTEMDRV System : public Driver // all this is private to this one file
{
	Q_OBJECT
	SystemInstance *pInstance; // one and only one instance of interface
	enum
	{
		tListUnits = 1,tProps, tcreateNewUnit
	};

	public:
	System(QObject *parent,const QString &name);
	~System();
	//
	// user interface stuff
	// 
	void UnitConfigure(QWidget *parent, const QString &name, const QString &receipe); // configure a unit
	void SetTypeList(QComboBox *pCombo, const QString &unitname); // set the type list for unit type
	void GetInputList(const QString &type, QStringList &,const QString &,const QString &); // set the permitted input IDs
	QWidget * GetSpecificConfig(QWidget *, const QString &spname, const QString &sptype); //specific config for sample point of type
	void GetTagList(const QString &type, QStringList &,const QString &, const QString &); // returns the permitted tags for a given type for this unit
	void CreateNewUnit(QWidget *parent, const QString &); // create a new unit - quick configure
	bool AmUnique() {return true;}; // there is only one System monitor interface
	//
	// 
	static System *pDriver; // only one instance shoudl be created
	//
	// Actual driver stuff
	//
	// these are slots - the declarations are virtual in Driver
	//  
	public slots:
	void Start(); // start everything under this driver's control
	void Stop(); // stop everything under this driver's control
	void Command(const QString &,const QString &); // process a command for a named unit 
	void QueryResponse (QObject *, const QString &, int, QObject*);
};
//
//
// *****************************************************************
// 
// 
class SYSTEMDRV SystemInstance : public DriverInstance // the unit interface
{
	Q_OBJECT
	enum
	{
		tList = 1, tTags, tProperties, tUnitProps,tProps,tUnitProperties,tListUnits
	};

	//
	QString Name;
	QStringList taglist;
	IECValueList ValueList;
	
	int MinDiskSpace; // min disk space in mega bytes
	QString Drive; // the drive (partition) with the database on it
	bool fFailed; // failure status
	//
	QTimer *pTimer;      // timer object

	double InitialMemory;
	double ui_mem_data;
	//
	public:
	//
	SystemInstance(Driver *parent, const QString &name) : 
	DriverInstance(parent,name),MinDiskSpace(100),Drive(""),fFailed(0),pTimer(new QTimer(this)),
	InitialMemory(0.0),ui_mem_data(0.0) 
	{
		connect (GetConfigureDb (),
		SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
		SLOT (QueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the database
		pTimer->connect(pTimer,SIGNAL(timeout()),this,SLOT(Tick()));
		connect (GetDispatcher (),
		SIGNAL (ReceivedNotify(int, const char *)), this,
		SLOT (ReceivedNotify(int, const char *)));
		
		((System*)parent)->GetTagList(TYPE_M_ME_TF_1,taglist,"",""); 
	};
	~SystemInstance()
	{    
	};
	public slots:
	virtual void Start(); // start everything under this driver's control
	virtual void Stop(); // stop everything under this driver's control
	virtual void Command(const QString &,const QString &); // process a command for a named unit 
	virtual void QueryResponse (QObject *, const QString &, int, QObject*); // handles database responses
	void Tick(); // one second timer tick
	void ReceivedNotify(int, const char *);
};

#ifdef WIN32

DWORD get_directory_usage_percentage( const QString& directory_path );
double get_used_disk_space_percentage( const QString& directory_path );
bool FillInSystemInfo(char *buff);
bool FillInMemoryStatus(char *buff);
bool FillInProcessInfo(char *buff);

#endif //WIN32

#endif

