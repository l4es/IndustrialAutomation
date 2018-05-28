/*****************************************************************************
 *
 * Copyright (C) 2009 - 2017  Florian Pose <fp@igh-essen.com>
 *
 * This file is part of the DLS widget library.
 *
 * The DLS widget library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * The DLS widget library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the DLS widget library. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

#ifndef DLS_GRAPH_H
#define DLS_GRAPH_H

#include <QFrame>
#include <QThread>
#include <QMutex>
#include <QSvgRenderer>
#include <QAction>
#include <QScrollBar>
#include <QReadWriteLock>
#include <QTouchEvent>
#include <QUrl>
#include <QDir>

#ifndef DLS_NO_DESIGNER
#if QT_VERSION >= 0x050000
#include <QtUiPlugin/QDesignerExportWidget>
#else
#include <QtDesigner/QDesignerExportWidget>
#endif
#else
# define QDESIGNER_WIDGET_EXPORT Q_DECL_EXPORT
#endif

#include <LibDLS/Job.h>

#include "Scale.h"

class QDomElement;

namespace LibDLS {
    class Data;
}

namespace QtDls {
    class Model;
    class Channel;
}

namespace DLS {

class Section;
class Graph;

/****************************************************************************/

/** Working class hero.
 */
class GraphWorker:
    public QObject {
    Q_OBJECT

    public:
        GraphWorker(Graph *);
        ~GraphWorker();

        void clearData();

        static int dataCallback(LibDLS::Data *, void *);

        int width;

        const QList<LibDLS::Data *> &genData() const { return genericData; }
        const QList<LibDLS::Data *> &minData() const { return minimumData; }
        const QList<LibDLS::Data *> &maxData() const { return maximumData; }

    public slots:
        void doWork();

    signals:
        void notifySection(Section *section);
        void finished();

    private:
        Graph * const graph;
        QList<LibDLS::Data *> genericData;
        QList<LibDLS::Data *> minimumData;
        QList<LibDLS::Data *> maximumData;
        QList<LibDLS::Job::Message> messages;

        void newData(LibDLS::Data *);
        static void clearDataList(QList<LibDLS::Data *> &);
};

/****************************************************************************/

/** Graph widget.
 */
class QDESIGNER_WIDGET_EXPORT Graph:
    public QFrame
{
    Q_OBJECT

    friend class GraphWorker;
    friend class Section; // FIXME

    public:
        Graph(QWidget *parent = 0);
        virtual ~Graph();

        void setDropModel(QtDls::Model *);

        virtual QSize sizeHint() const;

        bool load(const QString &, QtDls::Model *);
        bool save(const QString &);

        enum RenderFlag {
            Plain = 0,
            MeasuringLine = 1,
            All = MeasuringLine
        };
        Q_DECLARE_FLAGS(RenderFlags, RenderFlag)
        bool renderPage(QPainter &, const QRect &, unsigned int = 0,
                RenderFlags = All);

        void connectChannels(QtDls::Model *);
        bool dirInUse(const LibDLS::Directory *);

        Section *appendSection();
        Section *insertSectionBefore(Section *);
        void removeSection(Section *);

        void updateRange();
        void setRange(const LibDLS::Time &, const LibDLS::Time &);
        const LibDLS::Time &getStart() const { return scale.getStart(); };
        const LibDLS::Time &getEnd() const { return scale.getEnd(); };
        QSet<QUrl> urls();
        const QString &getMessageFilter() const { return messageFilter; }

        struct ChannelInfo {
            QUrl url;
            unsigned int jobId;
            unsigned int dirIndex;
        };
        QList<ChannelInfo> channelInfo();

        LibDLS::Time getMeasureTime() const { return measureTime; }

        enum Interaction {
            Zoom,
            Pan,
            Measure
        };

        enum NamedRange {
            Today,
            Yesterday,
            ThisWeek,
            LastWeek,
            ThisMonth,
            LastMonth,
            ThisYear,
            LastYear
        };

    signals:
        void logMessage(const QString &);

    public slots:
        void previousView();
        void nextView();
        void loadData();
        void zoomIn();
        void zoomOut();
        void zoomReset();
        void setInteraction(Interaction);
        void setNamedRange(NamedRange);
        void pan(double);
        void print();
        void setShowMessages(bool);
        void setMessageFilter(const QString &);
        void clearSections();

    protected:
        bool event(QEvent *);
        void mousePressEvent(QMouseEvent *);
        void mouseReleaseEvent(QMouseEvent *);
        void mouseMoveEvent(QMouseEvent *);
        void leaveEvent(QEvent *);
        void keyPressEvent(QKeyEvent *event);
        void resizeEvent(QResizeEvent *);
        void paintEvent(QPaintEvent *);
        void contextMenuEvent(QContextMenuEvent *);
        void dragEnterEvent(QDragEnterEvent *);
        void dragLeaveEvent(QDragLeaveEvent *);
        void dragMoveEvent(QDragMoveEvent *);
        void dropEvent(QDropEvent *);
        void wheelEvent(QWheelEvent *);

    private:
        Scale scale;
        QDir dir; /**< Directory for resolving relative channel paths. */
        QList<Section *> sections; /**< List of data sections. */
        QReadWriteLock rwLockSections;
        bool autoRange;
        QtDls::Model *dropModel;
        Section *dropSection;
        int dropLine;
        int dropRemaining;
        QPoint startPos;
        QPoint endPos;
        LibDLS::Time dragStart;
        LibDLS::Time dragEnd;
        bool zooming;
        Interaction interaction;
        bool panning;
        bool measuring;
        LibDLS::Time measureTime;

        QThread thread;
        GraphWorker worker;
        bool workerBusy;
        bool reloadPending;
        int pendingWidth;
        QSvgRenderer busySvg;

        QAction fixMeasuringAction;
        QAction removeMeasuringAction;
        QAction prevViewAction;
        QAction nextViewAction;
        QAction loadDataAction;
        QAction zoomAction;
        QAction panAction;
        QAction measureAction;
        QAction zoomInAction;
        QAction zoomOutAction;
        QAction zoomResetAction;
        QAction pickDateAction;
        QAction gotoTodayAction;
        QAction gotoYesterdayAction;
        QAction gotoThisWeekAction;
        QAction gotoLastWeekAction;
        QAction gotoThisMonthAction;
        QAction gotoLastMonthAction;
        QAction gotoThisYearAction;
        QAction gotoLastYearAction;
        QAction sectionPropertiesAction;
        QAction removeSectionAction;
        QAction clearSectionsAction;
        QAction messagesAction;
        QAction filterAction;
        QAction printAction;
        QAction exportAction;
        Section *selectedSection;
        const int splitterWidth;
        Section *splitterSection;
        Section *movingSection;
        int startHeight;
        QScrollBar scrollBar;
        bool scrollBarNeeded;
        int scaleWidth;

        struct View {
            LibDLS::Time start;
            LibDLS::Time end;
        };
        QList<View> views;
        QList<View>::iterator currentView;

        // Message display
        bool showMessages;
        int messageAreaHeight;
        bool mouseOverMsgSplitter;
        bool movingMsgSplitter;
        QList<LibDLS::Job::Message> messages;
        QMutex msgMutex;
        static QColor messageColor[];
        static QString messagePixmap[];
        QString messageFilter;

        QMutex loggingMutex;

        int touchX0;
        bool touchPanning;

        LibDLS::Time touchT0;
        LibDLS::Time touchT1;
        bool touchZooming;

        QFile debugFile;
        QString debugString;

        void updateDragging(QPoint);
        void resetDragging();
        void updateCursor();
        void updateActions();
        void updateMeasuring();
        void updateScrollBar();
        Section *sectionFromPos(const QPoint &);
        Section *splitterSectionFromPos(const QPoint &);
        static void drawDropRect(QPainter &, const QRect &);
        void newView();
        bool loadSections(const QDomElement &, QtDls::Model *, const QDir &);
        void drawMessages(QPainter &, const QRect &);
        static void staticLoggingCallback(const char *, void *);
        void loggingCallback(const char *);
        void updateTouch(QTouchEvent *);
        bool touchPanStart(const QPoint &);
        void touchPanUpdate(const QTouchEvent::TouchPoint &);
        void touchZoomStart(int, int);
        void touchZoomUpdate(int, int);
        int getDataWidth() const;
        QSet<QtDls::Channel *> displayedChannels();
        QList<Section *>::const_iterator lastSectionOnPage(
                QList<Section *>::const_iterator, int) const;
        int renderCommon(QPainter &, const QRect &) const;
        void renderSections(QPainter &, const QRect &,
                QList<Section *>::const_iterator,
                QList<Section *>::const_iterator, int, RenderFlags);

    private slots:
        void interactionSlot();
        void removeSelectedSection();
        void sectionProperties();
        void sliderValueChanged(int);
        void pickDate();
        void gotoDate();
        void workerFinished();
        void updateSection(Section *section);
        void showMessagesChanged();
        void filterTriggered();
        void showExport();
        void fixMeasuringLine();
        void removeMeasuringLine();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Graph::RenderFlags)

/****************************************************************************/

} // namespace

#endif
