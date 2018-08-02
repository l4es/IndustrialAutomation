/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#ifndef QWT_SLIDER_H
#define QWT_SLIDER_H

#include "qwt_global.h"
#include "qwt_sclif.h"
#include "qwt_sldbase.h"

/*!
  \brief The Slider Widget

  QwtSlider is a slider widget which operates on an interval
  of type double. QwtSlider supports different layouts as
  well as a scale.
  \sa QwtSliderBase and QwtScaleIf for the descriptions
      of the inherited members.
*/

class QWT_EXPORT QwtSlider : public QwtSliderBase, public QwtScaleIf
{
    Q_OBJECT
    Q_ENUMS( ScalePos )
    Q_ENUMS( BGSTYLE )
    Q_PROPERTY( ScalePos scalePosition READ scalePosition
    WRITE setScalePosition )
    Q_PROPERTY( BGSTYLE bgStyle READ bgStyle WRITE setBgStyle )
    Q_PROPERTY( int thumbLength READ thumbLength WRITE setThumbLength )
    Q_PROPERTY( int thumbWidth READ thumbWidth WRITE setThumbWidth )
    Q_PROPERTY( int borderWidth READ borderWidth WRITE setBorderWidth )
 
public:

    /*! 
      Scale position. QwtSlider tries to enforce valid combinations of its
      orientation and scale position:
      - Qt::Horizonal combines with None, Top and Bottom
      - Qt::Vertical combines with None, Left and Right

      \sa QwtSlider::QwtSlider
     */
    enum ScalePos { None, Left, Right, Top, Bottom };

    /*! 
      Background style.
      \sa QwtSlider::QwtSlider
     */
    enum BGSTYLE { BgTrough = 0x1, BgSlot = 0x2, BgBoth = BgTrough | BgSlot};

    QwtSlider(QWidget *parent, const char *name = 0,
          Qt::Orientation = Qt::Horizontal,
          ScalePos = None, BGSTYLE bgStyle = BgTrough);
    
    virtual void setOrientation(Qt::Orientation); 

    /*!
      Set the background style.
    */
    void setBgStyle(BGSTYLE st) {d_bgStyle = st; layoutSlider();}
    /*!
      \return the background style.
    */
    BGSTYLE bgStyle() const { return d_bgStyle; }
    
    /*!
      Deprecated. Use QwtSlider::setScalePosition.
    */
    void setScalePos(ScalePos s) { setScalePosition(s); }
    /*!
      Deprecated. Use QwtSlider::scalePosition.
    */
    ScalePos scalePos() const { return d_scalePos; }

    void setScalePosition(ScalePos s);
    ScalePos scalePosition() const;

    /*!
      \return the thumb length.
    */
    int thumbLength() const {return d_thumbLength;}
    /*!
      \return the thumb width.
    */
    int thumbWidth() const {return d_thumbWidth;}
    /*!
      \return the border width.
    */
    int borderWidth() const {return d_borderWidth;}

    void setThumbLength(int l);
    void setThumbWidth(int w);
    void setBorderWidth(int bw);
    void setMargins(int x, int y);

    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;
    virtual QSizePolicy sizePolicy() const;
    
protected:
    virtual double getValue(const QPoint &p);
    virtual void getScrollMode(const QPoint &p, 
        int &scrollMode, int &direction);

    void draw(QPainter *p, const QRect& update_rect);
    virtual void drawSlider (QPainter *p, const QRect &r);
    virtual void drawThumb(QPainter *p, const QRect &, int pos);

    virtual void resizeEvent(QResizeEvent *e);
    virtual void paintEvent (QPaintEvent *e);

    virtual void valueChange();
    virtual void rangeChange();
    virtual void scaleChange();
    virtual void fontChange(const QFont &oldFont);

    void layoutSlider( bool update = TRUE );
    int xyPosition(double v) const;

private:
    QRect d_sliderRect;

    int d_thumbLength;
    int d_thumbWidth;
    int d_borderWidth;
    int d_scaleDist;
    int d_xMargin;
    int d_yMargin;
    
    ScalePos d_scalePos;
    BGSTYLE d_bgStyle;
};

#endif

// Local Variables:
// mode: C++
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
