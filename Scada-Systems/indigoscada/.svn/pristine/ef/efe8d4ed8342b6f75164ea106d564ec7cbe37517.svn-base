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

#include <qwidget.h>
#include "plcdnumber.h"

/*!
  \brief Constructor
  \param parent Parent Widget
  \param name Name
*/
PLCDNumber::PLCDNumber(QWidget *parent, const char *name)
        : QLCDNumber(parent,name)
{

}

/*!
  \brief Mouse Release Event management
*/
void PLCDNumber::mouseReleaseEvent(QMouseEvent * e)
{
	if(e->button() == Qt::RightButton)
	{
		//emit RightClicked();
		QString name = this->name();

		emit RightClicked(QString("PLCDNumber"), name);
	}
}

