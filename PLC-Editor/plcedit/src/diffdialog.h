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


#ifndef DIFFDIALOG_H
#define DIFFDIALOG_H

#include <QDialog>

class PageCtrl; 		//get access to PageCtrl class
class QBtWorkspace; 	//get access to QBtWorkspace class
class QLabel;


class DiffDialog : public QDialog
{
    Q_OBJECT


public:
    DiffDialog(QWidget *parent = 0, PageCtrl *pageCtrl = 0);
    virtual ~DiffDialog();


public slots:
    void		selectLeftFile();
    void		readLeftFile(QString path);
    void		readRightFile(QString path);
    void 		retranslateUi();
    void		selectRightFile();


private slots:
    void 		stat_total(int value);
    void 		stat_chg(int value);
    void 		stat_add(int value);
    void 		stat_del(int value);
    void		configureBrowserA();
    void		configureBrowserB();
    

private:
//STATUSBAR items
    QLabel		*st_total;
    QLabel		*st_chg;
    QLabel		*st_add;
    QLabel		*st_del;

//MESSAGES
    QString 	msg_st_total;		//!< message statusbar item total 
    QString 	msg_st_chg;			//!< message statusbar item changes
    QString 	msg_st_add;			//!< message statusbar item added 
    QString 	msg_st_del;			//!< message statusbar item deleted 


//INSTANCES  
    PageCtrl 	 *m_PageCtrl; 		//!< instance of PageCtrl class
    QBtWorkspace *m_workspace;		//!< instance of QBtWorkspace class
    
};
#endif
