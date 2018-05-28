/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2009 - 2014  Florian Pose <fp@igh-essen.com>
 *
 * This file is part of the QtPdWidgets library.
 *
 * The QtPdWidgets library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * The QtPdWidgets library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the QtPdWidgets Library. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

#include <QtGui>

#include "QtPdWidgets/Image.h"

using Pd::Image;

/****************************************************************************/

class Image::Transformation
{
    public:
        Transformation(Pd::Image *image): image(image) {}
        virtual ~Transformation() {}

        virtual void push(QPainter &) = 0;

    protected:
        Image * const image;
};

/****************************************************************************/

class Image::FixedTranslation:
    public Image::Transformation
{
    public:
        FixedTranslation(Image *image, qreal x, qreal y):
            Transformation(image),
            x(x),
            y(y)
        {}

        virtual void push(QPainter &p) {
            p.translate(x, y);
        }

    private:
        qreal x;
        qreal y;
};

/****************************************************************************/

class Image::VariableTranslation:
    public Image::Transformation,
    public ScalarSubscriber
{
    public:
        VariableTranslation(
                Image *image,
                Image::Axis axis,
                PdCom::Variable *pv,
                double sampleTime,
                double scale,
                double offset,
                double tau
                ):
            Transformation(image),
            axis(axis),
            value(0.0)
        {
            setVariable(pv, sampleTime, scale, offset, tau);
        }

        virtual void push(QPainter &p) {
            switch (axis) {
                case X:
                    p.translate(value, 0.0);
                    break;
                case Y:
                    p.translate(0.0, value);
                    break;
            }
        }

    private:
        Image::Axis axis;
        double value;

        void notify(PdCom::Variable *pv) {
            pv->getValue(&value, 1, &scale);
            image->update();
        }
};

/****************************************************************************/

class Image::FixedRotation:
    public Image::Transformation
{
    public:
        FixedRotation(Image *image, qreal alpha):
            Transformation(image),
            alpha(alpha)
        {}

        virtual void push(QPainter &p) {
            p.rotate(alpha);
        }

    private:
        qreal alpha;
};

/****************************************************************************/

class Image::VariableRotation:
    public Image::Transformation,
    public ScalarSubscriber
{
    public:
        VariableRotation(
                Image *image,
                PdCom::Variable *pv,
                double sampleTime,
                double scale,
                double offset,
                double tau
                ):
            Transformation(image),
            value(0.0)
        {
            setVariable(pv, sampleTime, scale, offset, tau);
        }

        virtual void push(QPainter &p) {
            p.rotate(value);
        }

    private:
        double value;

        void notify(PdCom::Variable *pv) {
            pv->getValue(&value, 1, &scale);
            image->update();
        }
};

/****************************************************************************/

/** Constructor.
 */
Image::Image(
        QWidget *parent /**< Parent widget. */
        ):
    QFrame(parent),
    value(0),
    dataPresent(false),
    pixmapHash(NULL),
    angle(0.0)
{
    retranslate();
}

/****************************************************************************/

/** Destructor.
 */
Image::~Image()
{
    clearTransformations();
}

/****************************************************************************/

void Image::clearData()
{
    dataPresent = false;
    updatePixmap();
}

/****************************************************************************/

/** Sets the current #value.
 *
 * If the value has changed, updatePixmap() is called.
 */
void Image::setValue(int value)
{
    if (value != Image::value || !dataPresent) {
        Image::value = value;
        dataPresent = true;
        updatePixmap();
    }
}

/****************************************************************************/

/** Sets the #PixmapHash to use.
 */
void Image::setPixmapHash(const PixmapHash *ph)
{
    if (ph != pixmapHash) {
        pixmapHash = ph;
        updatePixmap();
    }
}

/****************************************************************************/

/** Sets the #defaultPixmap.
 */
void Image::setDefaultPixmap(const QPixmap &p)
{
    defaultPixmap = p;
    updatePixmap();
}

/****************************************************************************/

/** Resets the #defaultPixmap.
 */
void Image::resetDefaultPixmap()
{
    setDefaultPixmap(QPixmap());
}

/****************************************************************************/

/** Sets the #angle to use.
 */
void Image::setAngle(qreal a)
{
    if (a != angle) {
        angle = a;
        update();
    }
}

/****************************************************************************/

/** Resets the #angle.
 */
void Image::resetAngle()
{
    setAngle(0.0);
}

/****************************************************************************/

void Image::clearTransformations()
{
    for (QList<Transformation *>::iterator i =
            transformationList.begin();
            i != transformationList.end();
            i++) {
        delete *i;
    }

    transformationList.clear();
}

/****************************************************************************/

void Image::translate(qreal x, qreal y)
{
    FixedTranslation *t = new FixedTranslation(this, x, y);
    transformationList.append(t);
}

/****************************************************************************/

void Image::translate(
        Axis axis,
        PdCom::Variable *pv,
        double sampleTime,
        double scale,
        double offset,
        double tau
        )
{
    VariableTranslation *t = new VariableTranslation(this, axis, pv,
            sampleTime, scale, offset, tau);
    transformationList.append(t);
}

/****************************************************************************/

void Image::rotate(qreal alpha)
{
    FixedRotation *t = new FixedRotation(this, alpha);
    transformationList.append(t);
}

/****************************************************************************/

void Image::rotate(
        PdCom::Variable *pv,
        double sampleTime,
        double scale,
        double offset,
        double tau
        )
{
    VariableRotation *t = new VariableRotation(this, pv, sampleTime, scale,
            offset, tau);
    transformationList.append(t);
}

/****************************************************************************/

/** Event handler.
 */
bool Image::event(
        QEvent *event /**< Paint event flags. */
        )
{
    if (event->type() == QEvent::LanguageChange) {
        retranslate();
    }

    return QFrame::event(event);
}

/****************************************************************************/

void Image::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter p(this);

    p.setRenderHints(
            QPainter::Antialiasing |
            QPainter::SmoothPixmapTransform);

    QRectF targetRect(displayPixmap.rect());
    QRectF sourceRect(displayPixmap.rect());
    targetRect.moveLeft((contentsRect().width() - sourceRect.width()) / 2.0);
    targetRect.moveTop((contentsRect().height() - sourceRect.height()) / 2.0);

    QPointF center = targetRect.center();
    p.translate(center);
    p.rotate(angle);

    for (QList<Transformation *>::const_iterator i =
            transformationList.begin();
            i != transformationList.end();
            i++) {
        (*i)->push(p);
    }

    p.translate(-center);

    p.drawPixmap(targetRect, displayPixmap, sourceRect);
}

/****************************************************************************/

/** This virtual method is called by the ProcessVariable, if its value
 * changes.
 */
void Image::notify(
        PdCom::Variable *pv
        )
{
    int32_t value;
    pv->getValue(&value);
    setValue(value);
}

/****************************************************************************/

/** Displays a new Pixmap depending on the process #value.
 *
 * \todo default image?
 */
void Image::updatePixmap()
{
    if (dataPresent && pixmapHash && pixmapHash->contains(value)) {
        displayPixmap = pixmapHash->value(value);
    }
    else {
        displayPixmap = defaultPixmap;
    }

    update();
}

/****************************************************************************/

/** Retranslate the widget.
 */
void Image::retranslate()
{
    setWindowTitle(Pd::Image::tr("Image"));
}

/****************************************************************************/
