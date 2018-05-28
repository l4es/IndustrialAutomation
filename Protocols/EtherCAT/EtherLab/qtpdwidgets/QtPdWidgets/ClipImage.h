/*****************************************************************************
 *
 * $Id$
 *
 ****************************************************************************/

#ifndef PD_CLIPIMAGE_H
#define PD_CLIPIMAGE_H

#include <QFrame>
#include <QSvgRenderer>

#include <pdcom/Subscriber.h>
#include <pdcom/Variable.h>

#include "Export.h"

namespace Pd {

/****************************************************************************/

class QDESIGNER_WIDGET_EXPORT ClipImage:
    public QFrame,
    public PdCom::Subscriber
{
    Q_OBJECT
    Q_ENUMS(ClipMode)
    Q_PROPERTY(QString background
            READ getBackground WRITE setBackground RESET resetBackground)
    Q_PROPERTY(QString foreground
            READ getForeground WRITE setForeground RESET resetForeground)
    Q_PROPERTY(ClipMode clipMode
            READ getClipMode WRITE setClipMode RESET resetClipMode)

    public:
        ClipImage(QWidget * = 0);
        ~ClipImage();

        QSize sizeHint() const;

        const QString &getBackground() const { return backgroundPath; }
        void setBackground(const QString &);
        void resetBackground();

        const QString &getForeground() const { return foregroundPath; }
        void setForeground(const QString &);
        void resetForeground();

        enum ClipMode {
            Bar,
            Clock
        };
        ClipMode getClipMode() const { return clipMode; }
        void setClipMode(ClipMode);
        void resetClipMode();

        /** Subscribe to a process variable.
         */
        void setVariable(
                PdCom::Variable *pv, /**< Process variable. */
                double sampleTime = 0.0, /**< Sample time. */
                double scale = 1.0, /**< Scale factor. */
                double offset = 0.0, /**< Offset (applied after scaling). */
                double tau = 0.0 /**< PT1 filter time constant. A value less
                                    or equal to 0.0 means, that no filter is
                                    applied. */
                );

        void clearVariable();

    private:
        QString backgroundPath;
        QString foregroundPath;
        ClipMode clipMode;

        PdCom::Variable *var;
        PdCom::Variable::Scale scale; /**< Scale vector. */
        double filterConstant;
        bool dataPresent;
        double value;

        QSvgRenderer backgroundRenderer;
        bool backgroundLoaded;
        QSvgRenderer foregroundRenderer;
        bool foregroundLoaded;

        double scaleFactor;

        void resizeEvent(QResizeEvent *);
        void paintEvent(QPaintEvent *);
        void notify(PdCom::Variable *);
        void notifyDelete(PdCom::Variable *);

        void updateScale();
};

} // namespace

/****************************************************************************/

#endif
