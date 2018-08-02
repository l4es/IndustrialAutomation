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
*Header For: Sample Status Display
*Purpose: this window is a vertically split window. On the left are the sample points (colour coded status) 
*on the right is the status of the currently selected sample point
*/

#ifndef include_statusdisplay_hpp 
#define include_statusdisplay_hpp 
#include <qt.h>
#include "display.h"
#include "statuspane.h"
#include "table.h"
class StatusDisplay : public QSplitter
{
	Q_OBJECT
	enum { tList = 1,tTags,tSamplePoint,tReceipeRecord,tItem,tTagLimits};
	Table *pStatus; // pane with sample point status
	QString CurrentSP; // sample point in status spane
	StatusPane Status;  // the status pane for tracks
	int nSpace;
	typedef std::map<QString,QString, std::less<QString> > SampleMap;
	SampleMap umap; // map of sample point to units    
	public:
	StatusDisplay(QWidget *parent);
	public slots:
	void SelChanged(); // handle the selection
	void PointMenu(TableItem *, const QPoint &); // an item has been right clicked - open the point menu
	void QueryResponse (QObject *, const QString &, int, QObject*);
	void UpdateSamplePoint(); // handle updated sample points
	void Restart(); // restart everything
	void UpdateTags(); // update tag values
};
#endif

