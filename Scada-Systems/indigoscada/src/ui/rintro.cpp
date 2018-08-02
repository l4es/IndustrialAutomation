#include <qapplication.h>
#include <qfont.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qpixmap.h>

#include "rintro.h"
#include "logo_enscada_30.xpm"
#include "logo_blank_30.xpm"
#include "general_defines.h"

// Constructor:
//
RIntro::RIntro(const QString& _version, QWidget* _parent, const char* _name)
  :QWidget(_parent, _name, WStyle_NoBorder)
{
  setFixedSize(222, 149);  // 256 158

  if(parentWidget()) {
    move((parentWidget()->width()-width())/2,
         (parentWidget()->height()-height())/2);
  }

  sVersion =_version;
}

// Destructor:
//
RIntro::~RIntro()
{

}

// Paint:
//
void 
RIntro::paintEvent(QPaintEvent* _ev)
{
  QWidget::paintEvent(_ev);

  QPainter paint;        // painter
  
  paint.begin(this);
  paint.drawPixmap(0, 0, LOGO_SUPPLIER);
  
  paint.setFont(QFont("helvetica", 10));
  paint.setPen(black);
  paint.drawText(0, 0, width(), height()-2,
                 AlignLeft|AlignBottom,
                 "you name");
                 
  paint.drawText(0, 0, width(), height()-2,
                 AlignRight|AlignBottom,
                 sVersion);
  paint.end();

}


