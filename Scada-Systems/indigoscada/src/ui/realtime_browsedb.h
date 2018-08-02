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
*Purpose: Browsing the real time online databases
*/
#ifndef include_realtime_browsedb_hpp 
#define include_realtime_browsedb_hpp

#include <qt.h> 
#include "dbase.h" 
#include "driver.h"
#include "graphpane.h"
#include "display.h"
#include "realtimedb.h"
#include "statuspane.h"
#include "dbtable.h"
//
// the realtime browsedb window
//
class RealTimeBrowsedb : public QMainWindow
{
	Q_OBJECT
	enum {
		tData=1, tResults, tPurge, tPrepareToDeleteBlob, tDeleteBlob,tDeleteBlobEnd, tBackupDb,
		tList,tUpdateDb,tValueChanged,tBrowse
	};
	//
	RealTimeDbDict db_dictionary;
	///////browser data///////
	long maxItems;
	//////////////////////////

	QToolBar       *pToolBarBrowse; // the current tool bar
	QToolBar       *pToolBarAdmin; // the current tool bar
	QSplitter *pSm;
	QDateTime LastTime; // last time
	QListBox *pMessages; // pane with event messages
	QListView *List; // the setup bits
	DbTable *db_table;
	bool DateTime;
	unsigned int records_fetched;
	unsigned int delete_blob_counter;
	__int64 MoreOldRecord;
	__int64 MoreRecentRecord;
	__int64 timestamp;

	//
	public:

	#ifdef USE_STD_MAP
	RealTimeBrowsedb(RealTimeDbDict& dct); // top level window - so no parent
	#else
	RealTimeBrowsedb(RealTimeDbDict dct); // top level window - so no parent
	#endif

	~RealTimeBrowsedb() 
	{
		if(List)
		{
			delete List;
			List = NULL;
		}
		if(pMessages)
		{
			delete pMessages;
			pMessages = NULL;
		}
		if(db_table)
		{
		  delete (DbTable*)db_table;
		  db_table = NULL;
		}
		if(pSm)
		{
			delete pSm;
			pSm = NULL;
		}
	};
	void ShowDbMessages(QSDatabase* pdb);
	public slots:
	//
	void Close();   // close window
	//
	void QueryResponse (QObject *,const QString &, int, QObject*);  // handles configuration response
	void mouseButtonPressed(int, QListViewItem*, const QPoint &, int);
	void DataTimeOn(bool);
	void returnPressed ( QListViewItem * ); 
	void valueChanged(int,int);
	void getMaxItems();

	//Toolbar slots
	void first();
	void before();
	void next();
	void last();
	void Help(); // help
};
#endif

