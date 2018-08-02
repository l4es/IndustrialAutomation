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

#ifndef RINTRO_H
#define RINTRO_H

#include <qt.h>

class RIntro : public QWidget
{
  Q_OBJECT
public:
  RIntro(const QString& _version,
         QWidget* _parent=0, 
         const char* _name=0);
  ~RIntro();

private:
  QString    sVersion;
  QPixmap*   pIntro;

protected:
  virtual void paintEvent(QPaintEvent* _ev);
  
};

#endif
