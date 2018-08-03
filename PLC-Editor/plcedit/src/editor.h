/****************************************************************************
**
** This file is part of PLCEdit, an open-source cross-platform editor 
** for PLC source files (e.g. from Moeller, CoDeSys and Siemens).
** Copyright (C) 2005-2010  M. Rehfeldt
**
** This software uses classes of Trolltech Qt toolkit and is freeware. 
** This file may be used under the terms of the GNU General Public License 
** version 2.0 or (at your option) any later version as published by the 
** Free Software Foundation and appearing in the file LICENSE.GPL included 
** in the packaging of this file. 
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** Contact e-mail: M. Rehfeldt <info@plcedit.org>
** Program URL   : http://www.plcedit.org
**
****************************************************************************/


#ifndef EDITOR_H
#define EDITOR_H

#include <QtGui>


class Editor : public QTextEdit
{
Q_OBJECT
public:
  Editor(QWidget *parent=0);
  ~Editor();
  bool 		enableHighlightLines;			//!< enable highlighting current line
  QColor  	hiLineColor;					//!< highlighting color for current line
  int		printArea;						//!< print area indicator postion for declarations and instructions


public:
    bool contentIsModified();
    void retranslateUi();
    void setContentModified(bool modified);


signals:
    void contentModified(bool modified);	//!< signal status of editor content modification
    void compareWith();						//!< signal file comparision with
    void compareWithNext();					//!< signal file comparision with next
    void compareWithPrev();					//!< signal file comparision with previous
    

private slots:
    void cursorPositionChangedOperations();


protected:
  //void contextMenuEvent(QContextMenuEvent *event);
  bool event(QEvent *event);

	
private:
  int prefWidth; 							//!< preferred width of line numbers column
  QString str_contextmenu_comparewith;		//!< string context menu item compare with
  QString str_contextmenu_comparewithnext;	//!< string context menu item compare with next
  QString str_contextmenu_comparewithprev;	//!< string context menu item compare with previous
  //QAction *compareWithAction; 				//!> action context menu compare with
  //QAction *compareWithNextAction; 			//!> action context menu compare with next
  //QAction *compareWithPrevAction; 			//!> action context menu compare with previous
  
  void paintEvent(QPaintEvent *event);
};

#endif
