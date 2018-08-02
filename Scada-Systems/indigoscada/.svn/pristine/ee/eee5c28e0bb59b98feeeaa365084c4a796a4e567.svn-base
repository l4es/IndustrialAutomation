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
/*h
*Header For: Message Display Window
*Purpose: this displays current messages and trace
*/
#ifndef include_messages_hpp 
#define include_messages_hpp 
#include <qt.h>
#include <common.h>

// max number of messages in list box   
#define MESSAGES_LIMIT 256
class QSEXPORT MessageDisplay : public QSplitter
{
	Q_OBJECT
	//
	//QTableHeader *topHeader;
	QListBox *pMessages; // pane with event messages
	QListBox *pTrace; // pane with trace and diagnostics
	//
	public:
	static MessageDisplay *pMessageWindow;
	MessageDisplay(QWidget *parent): QSplitter(QSplitter::Vertical,parent)
	{
		pMessages = new QListBox(this);
		pTrace = new QListBox(this);
		pMessageWindow = this;

	    // Create headers
	/*	topHeader = new QTableHeader( 1, this, "header messaggi" );
		topHeader->setOrientation( Horizontal );
		topHeader->setTracking( TRUE );
		topHeader->setMovingEnabled( TRUE );
		if ( QApplication::reverseLayout() )
		setMargins( 0, fontMetrics().height() + 4, 30, 0 );
		else
		setMargins( 30, fontMetrics().height() + 4, 0, 0 );

		topHeader->setUpdatesEnabled( FALSE );
		
		// Initialize headers
		int i = 0;
		//for ( i = 0; i < numCols(); ++i )
		topHeader->resizeSection( i, QMAX( 100, QApplication::globalStrut().width() ) );
		//topHeader->setUpdatesEnabled( TRUE );
	*/	

	}
	~MessageDisplay() { pMessageWindow = 0;};
	void Message(const QString &s)
	{
		pMessages->insertItem(s);
		if(pMessages->count())
		{
			pMessages->setSelected (pMessages->count() - 1, TRUE );
			pMessages->ensureCurrentVisible();
		}
		if(pMessages->count() > MESSAGES_LIMIT)
		{
			QListBoxItem* p = pMessages->firstItem();
			pMessages->takeItem(p);
			delete p;
		};
	};
	void Trace(const QString &s)
	{
		pTrace->insertItem(s);
		if(pTrace->count())
		{
			pTrace->setSelected (pTrace->count() - 1, TRUE );
			pTrace->ensureCurrentVisible();
		}
		if(pTrace->count() > MESSAGES_LIMIT)
		{
			QListBoxItem* p = pTrace->firstItem();
			pTrace->takeItem(p);
			delete p;
		};
	};
};
//
//
//
//

#endif

