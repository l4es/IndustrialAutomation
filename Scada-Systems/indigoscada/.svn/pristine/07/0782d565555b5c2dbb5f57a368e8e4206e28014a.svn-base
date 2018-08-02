/*
*Header For:Calculated
*
*Purpose: 
*
*/
#ifndef include_Calculated 
#define include_Calculated
#include "driver.h"
#include "sptypes.h"
#include "fifo.h"
#include "fifoc.h"
#include "clear_crc_eight.h"

#ifdef CALCULATED_EXPORTS
#define CALCULATED_API __declspec(dllexport)
#else
#define CALCULATED_API //__declspec(dllimport)
#endif

class CalculatedInstance;
// data that must be cached for rapid response
struct CalculatedCfg // friend to anyone
{
	unsigned MinDiskSpace; // minimum allowed disk space on any drive
	unsigned PurgeTime; // how often to purge the databases
};
class CALCULATED_API Calculated : public Driver // all this is private to this one file
{
	Q_OBJECT
	typedef std::map<QString,CalculatedInstance *, std::less<QString> > IDict;
	IDict Instances;
	enum
	{
		tListUnits = 0,tProps
	};
	public:
	Calculated(QObject *parent,const QString &name);
	~Calculated();
	//
	// user interface stuff
	// 
	void UnitConfigure(QWidget *parent, const QString &name, const QString &receipe); // configure a unit
	void SetTypeList(QComboBox *pCombo, const QString &unitname); // set the type list for unit type
	void GetInputList(const QString &type, QStringList &,const QString &,const QString &); // set the permitted input IDs
	QWidget * GetSpecificConfig(QWidget *, const QString &spname, const QString &sptype); //specific config for sample point of type
	void GetTagList(const QString &type, QStringList &,const QString &, const QString &); // returns the permitted tags for a given type for this unit
	void CreateNewUnit(QWidget *parent, const QString &); // create a new unit - quick configure
	//
	// 
	static Calculated *pDriver; // only one instance shoudl be created
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
class CALCULATED_API CalculatedInstance : public DriverInstance // the unit interface
{
	Q_OBJECT
	//
	//
	int Interval; // the re calculation interval
	int Countdown; // count down track
	int StartWait; // we wait two sample intervals
	//
	struct ExpItem
	{
		QString Sample; // the sample point
		QString Expr; // the expression
		ExpItem() {};
		ExpItem(const ExpItem &e) : Sample(e.Sample),Expr(e.Expr) {};
	};
	//
	std::vector<ExpItem>  ExprTable; // the table
	//
	enum
	{
		tList, tTags, tProperties, tUnitProps,tProps,tUnitProperties,tTagsCurrent,tUnit
	};
	//
	QTimer *pTimer;      // timer object   
	int nSamplePoints;
	//
	public:
	fifo_h p_fifo_script;
	//struct scada_point* p_item;
	//
	CalculatedInstance(Driver *parent, const QString &name) : 
	DriverInstance(parent,name),pTimer(new QTimer(this)),nSamplePoints(0),p_fifo_script(NULL)
	{
		connect (GetConfigureDb (),
		SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
		SLOT (QueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the database

		connect (GetCurrentDb (),
		SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
		SLOT (QueryResponse (QObject *, const QString &, int, QObject*)));	

		pTimer->connect(pTimer,SIGNAL(timeout()),this,SLOT(Tick()));
	};
	~CalculatedInstance()
	{    
	};
	void get_utc_host_time(struct cp56time2a* time);
	public slots:
	virtual void Start(); // start everything under this driver's control
	virtual void Stop(); // stop everything under this driver's control
	virtual void Command(const QString &,const QString &); // process a command for a named unit 
	virtual void QueryResponse (QObject *, const QString &, int, QObject*); // handles database responses
	void Tick(); // one second timer tick
};
#endif

