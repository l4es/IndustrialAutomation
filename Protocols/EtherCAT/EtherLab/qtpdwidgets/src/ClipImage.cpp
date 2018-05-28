
/*****************************************************************************
 *
 * $Id$
 *
 ****************************************************************************/

#include <QtGui>

#include "QtPdWidgets/ClipImage.h"

using namespace Pd;

#define DEFAULT_CLIPMODE Bar

/****************************************************************************/

ClipImage::ClipImage(
        QWidget *parent
        ):
    QFrame(parent),
    clipMode(DEFAULT_CLIPMODE),
    var(NULL),
    dataPresent(false),
    backgroundRenderer(this),
    backgroundLoaded(false),
    foregroundRenderer(this),
    foregroundLoaded(false),
    scaleFactor(0.0)
{
    updateScale();
}

/****************************************************************************/

ClipImage::~ClipImage()
{
    clearVariable();
}

/****************************************************************************/

/** Gives a hint aboute the optimal size.
 */
QSize ClipImage::sizeHint() const
{
    return QSize(100, 100);
}

/****************************************************************************/

void ClipImage::setBackground(const QString &path)
{
    if (backgroundPath == path) {
        return;
    }

    backgroundPath = path;

    if (path.isEmpty()) {
        backgroundRenderer.load(QByteArray());
        backgroundLoaded = false;
    }
    else {
        backgroundLoaded = backgroundRenderer.load(path);
    }

    updateScale();
}

/****************************************************************************/

void ClipImage::resetBackground()
{
    setBackground(QString());
}

/****************************************************************************/

void ClipImage::setForeground(const QString &path)
{
    if (foregroundPath == path) {
        return;
    }

    foregroundPath = path;

    if (path.isEmpty()) {
        foregroundRenderer.load(QByteArray());
        foregroundLoaded = false;
    }
    else {
        foregroundLoaded = foregroundRenderer.load(path);
    }

    update();
}

/****************************************************************************/

void ClipImage::resetForeground()
{
    setForeground(QString());
}

/****************************************************************************/

/** Sets the #clipMode.
 */
    void ClipImage::setClipMode(ClipMode m)
{
    if (m != clipMode) {
        clipMode = m;
    }
}

/****************************************************************************/

/** Resets the #clipMode.
 */
void ClipImage::resetClipMode()
{
    setClipMode(DEFAULT_CLIPMODE);
}

/****************************************************************************/

/** Subscribes to a ProcessVariable.
 */
void ClipImage::setVariable(
        PdCom::Variable *pv,
        double sampleTime,
        double gain,
        double offset,
        double tau
        )
{
    clearVariable();

    if (!pv) {
        return;
    }

    scale.gain = gain;
    scale.offset = offset;

    if (tau > 0.0 && sampleTime > 0.0) {
        filterConstant = sampleTime / tau;
    } else {
        filterConstant = 0.0;
    }

    try {
        pv->subscribe(this, sampleTime);
    } catch (PdCom::Exception &e) {
        qCritical() << QString("Failed to subscribe to variable"
                " \"%1\" with sample time %2: %3")
            .arg(QString(pv->path.c_str()))
            .arg(sampleTime)
            .arg(e.what());
        return;
    }

    var = pv;

    if (!sampleTime) {
        pv->poll(this); // poll once to get initial value
    }
}

/****************************************************************************/

/** Unsubscribe from a Variable.
 */
void ClipImage::clearVariable()
{
    if (var) {
        var->unsubscribe(this);
        var = NULL;
        update();
    }
}

/****************************************************************************/

void ClipImage::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    updateScale();
}

/****************************************************************************/

void ClipImage::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);

    QPainter p(this);

    p.setRenderHint(QPainter::Antialiasing);

    QRect renderRect = contentsRect();
    backgroundRenderer.render(&p, renderRect);

    QRectF clipRect(contentsRect());
    double x = 0.0;

    if (dataPresent) {
        x = value;

        if (x < 0.0) {
            x = 0.0;
        }
        else if (x > 1.0) {
            x = 1.0;
        }
    }

    if (clipMode == Bar) {
        //qDebug() << contentsRect() << clipRect << x;
        clipRect.setTop(clipRect.bottom() - x * clipRect.height());
        p.setClipRect(clipRect);
    }
    else if (clipMode == Clock) {
        QPainterPath path;
        path.moveTo(clipRect.center());
        path.lineTo(clipRect.left() + clipRect.width() / 2.0, clipRect.top());
        path.arcTo(clipRect, 90.0, -360.0 * x);
        p.setClipPath(path);
    }

    foregroundRenderer.render(&p, renderRect);
}

/****************************************************************************/

/** This virtual method is called by the ProcessVariable, if its value
 * changes.
 */
void ClipImage::notify(PdCom::Variable *pv)
{
    if (pv == var) {
        double v;
        pv->getValue(&v, 1, &scale);

        if (dataPresent) {
            double newValue;

            if (filterConstant > 0.0) {
                newValue =
                    filterConstant * (v - value) + value;
            } else {
                newValue = v;
            }

            value = newValue;
        } else {
            value = v; // bypass filter
            dataPresent = true;
        }

        update();
    }
}

/****************************************************************************/

/** Notification for variable deletion.
 *
 * This virtual function is called by the Variable, when it is about to be
 * destroyed.
 */
void ClipImage::notifyDelete(PdCom::Variable *pv)
{
    if (pv == var) {
        var = NULL;
        dataPresent = false;
        update();
    }
}

/****************************************************************************/

void ClipImage::updateScale()
{
    /* workaround for designer not accepting QString properties as resources.
     * try to reload SVG data on next opportunity. */

    if (!backgroundPath.isEmpty() && !backgroundLoaded) {
        backgroundLoaded = backgroundRenderer.load(backgroundPath);
    }
    if (!foregroundPath.isEmpty() && !foregroundLoaded) {
        foregroundLoaded = foregroundRenderer.load(foregroundPath);
    }

    update();
}

/****************************************************************************/
