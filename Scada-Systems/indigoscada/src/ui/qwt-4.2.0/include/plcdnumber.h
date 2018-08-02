/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2012 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */

#ifndef PLCDNUMBER_H
#define PLCDNUMBER_H

#include <qlcdnumber.h>
#include <qwidgetplugin.h>
#include "qwt_global.h"
/*!
  \brief The LCD Number Widget.
*/

class QWT_EXPORT PLCDNumber: public QLCDNumber
{
    Q_OBJECT

public:
    PLCDNumber(QWidget *parent=0,const char *name=0);

signals:
	void RightClicked(QString &class_name, QString &name);

protected:
    void mouseReleaseEvent(QMouseEvent *);
};

#endif //PLCDNUMBER_H
