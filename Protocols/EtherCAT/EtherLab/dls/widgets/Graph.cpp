/*****************************************************************************
 *
 * Copyright (C) 2012 - 2017  Florian Pose <fp@igh.de>
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

#include <QtGui>
#include <QDomDocument>
#include <QPrinter>
#include <QPrintDialog>
#include <QMenu>

#include "DlsWidgets/Graph.h"
#include "DlsWidgets/Section.h"
#include "DlsWidgets/Layer.h"
#include "Channel.h"
#include "DlsWidgets/Model.h"
#include "SectionDialog.h"
#include "DatePickerDialog.h"
#include "ExportDialog.h"
#include "FilterDialog.h"

using DLS::Graph;
using DLS::GraphWorker;
using DLS::Section;
using QtDls::Model;

#define DROP_TOLERANCE 10
#define MSG_ROW_HEIGHT 16
#define MSG_LINES_HEIGHT 3
#define MIN_TOUCH_HEIGHT 20

/****************************************************************************/

QColor Graph::messageColor[] = {
    Qt::blue,
    Qt::darkYellow,
    Qt::red,
    Qt::red,
    Qt::black
};

/****************************************************************************/

QString Graph::messagePixmap[] = {
    ":/DlsWidgets/images/dialog-information.svg",
    ":/DlsWidgets/images/dialog-warning.svg",
    ":/DlsWidgets/images/dialog-error.svg",
    ":/DlsWidgets/images/dialog-error.svg",
    ":/DlsWidgets/images/dialog-information.svg"
};

/****************************************************************************/

/** Constructor.
 */
Graph::Graph(
        QWidget *parent /**< parent widget */
        ): QFrame(parent),
    scale(this),
    autoRange(true),
    dropModel(NULL),
    dropSection(NULL),
    dropLine(-1),
    dropRemaining(-1),
    zooming(false),
    interaction(Pan),
    panning(false),
    measuring(false),
    thread(this),
    worker(this),
    workerBusy(false),
    reloadPending(false),
    pendingWidth(0),
    busySvg(QString(":/DlsWidgets/images/view-refresh.svg"), this),
    fixMeasuringAction(this),
    removeMeasuringAction(this),
    prevViewAction(this),
    nextViewAction(this),
    loadDataAction(this),
    zoomAction(this),
    panAction(this),
    measureAction(this),
    zoomInAction(this),
    zoomOutAction(this),
    zoomResetAction(this),
    pickDateAction(this),
    gotoTodayAction(this),
    gotoYesterdayAction(this),
    gotoThisWeekAction(this),
    gotoLastWeekAction(this),
    gotoThisMonthAction(this),
    gotoLastMonthAction(this),
    gotoThisYearAction(this),
    gotoLastYearAction(this),
    sectionPropertiesAction(this),
    removeSectionAction(this),
    clearSectionsAction(this),
    messagesAction(this),
    filterAction(this),
    printAction(this),
    exportAction(this),
    selectedSection(NULL),
    splitterWidth(
            QApplication::style()->pixelMetric(QStyle::PM_SplitterWidth)),
    splitterSection(NULL),
    movingSection(NULL),
    startHeight(0),
    scrollBar(this),
    scrollBarNeeded(false),
    scaleWidth(0),
    currentView(views.begin()),
    showMessages(false),
    messageAreaHeight(55),
    mouseOverMsgSplitter(false),
    movingMsgSplitter(false),
    touchX0(0),
    touchPanning(false),
    touchZooming(false)
{
    LibDLS::set_logging_callback(staticLoggingCallback, this);

#ifdef DEBUG_MT_IN_FILE
    debugFile.setFileName("\\\\10.202.246.121\\transfer\\dlsdebug.txt");
    debugFile.open(QIODevice::WriteOnly);
#endif

    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(60, 50);
    setAcceptDrops(true);
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    setAttribute(Qt::WA_AcceptTouchEvents);

    scrollBar.setVisible(false);
    scrollBar.setCursor(Qt::ArrowCursor);
    scrollBar.raise();
    connect(&scrollBar, SIGNAL(valueChanged(int)),
            this, SLOT(sliderValueChanged(int)));

    updateCursor();

    thread.start();
    connect(&worker, SIGNAL(notifySection(Section *)),
            this, SLOT(updateSection(Section *)));
    connect(&worker, SIGNAL(finished()), this, SLOT(workerFinished()));

    fixMeasuringAction.setText(tr("&Fix measuring line"));
    fixMeasuringAction.setStatusTip(
            tr("Fix the measuring line at the current time."));
    connect(&fixMeasuringAction, SIGNAL(triggered()),
            this, SLOT(fixMeasuringLine()));

    removeMeasuringAction.setText(tr("&Remove measuring line"));
    removeMeasuringAction.setStatusTip(tr("Remove the measuring line."));
    connect(&removeMeasuringAction, SIGNAL(triggered()),
            this, SLOT(removeMeasuringLine()));

    prevViewAction.setText(tr("&Previous view"));
    prevViewAction.setShortcut(Qt::ALT | Qt::Key_Left);
    prevViewAction.setStatusTip(tr("Return to previous view."));
    prevViewAction.setIcon(QIcon(":/DlsWidgets/images/edit-undo.svg"));
    connect(&prevViewAction, SIGNAL(triggered()), this, SLOT(previousView()));

    nextViewAction.setText(tr("&Next view"));
    nextViewAction.setShortcut(Qt::ALT | Qt::Key_Right);
    nextViewAction.setStatusTip(tr("Proceed to next view."));
    nextViewAction.setIcon(QIcon(":/DlsWidgets/images/edit-redo.svg"));
    connect(&nextViewAction, SIGNAL(triggered()), this, SLOT(nextView()));

    loadDataAction.setText(tr("&Update"));
    loadDataAction.setShortcut(Qt::Key_F5);
    loadDataAction.setStatusTip(tr("Update displayed data."));
    loadDataAction.setIcon(QIcon(":/DlsWidgets/images/view-refresh.svg"));
    connect(&loadDataAction, SIGNAL(triggered()), this, SLOT(loadData()));

    zoomAction.setText(tr("&Zoom"));
    zoomAction.setShortcut(Qt::Key_Z);
    zoomAction.setStatusTip(tr("Set mouse interaction to zooming."));
    zoomAction.setIcon(QIcon(":/DlsWidgets/images/system-search.svg"));
    connect(&zoomAction, SIGNAL(triggered()), this, SLOT(interactionSlot()));

    panAction.setText(tr("&Pan"));
    panAction.setShortcut(Qt::Key_P);
    panAction.setStatusTip(tr("Set mouse interaction to panning."));
    panAction.setIcon(QIcon(":/DlsWidgets/images/go-next.svg"));
    connect(&panAction, SIGNAL(triggered()), this, SLOT(interactionSlot()));

    measureAction.setText(tr("&Measure"));
    measureAction.setShortcut(Qt::Key_M);
    measureAction.setStatusTip(tr("Set mouse interaction to measuring."));
    measureAction.setIcon(QIcon(":/DlsWidgets/images/measure.svg"));
    connect(&measureAction, SIGNAL(triggered()),
            this, SLOT(interactionSlot()));

    zoomInAction.setText(tr("Zoom in"));
    zoomInAction.setShortcut(Qt::Key_Plus);
    zoomInAction.setStatusTip(tr("Zoom the current view in to half"
                " of the time around the center."));
    zoomInAction.setIcon(QIcon(":/DlsWidgets/images/system-search.svg"));
    connect(&zoomInAction, SIGNAL(triggered()), this, SLOT(zoomIn()));

    zoomOutAction.setText(tr("Zoom out"));
    zoomOutAction.setShortcut(Qt::Key_Minus);
    zoomOutAction.setStatusTip(tr("Zoom the current view out the double"
                " time around the center."));
    zoomOutAction.setIcon(QIcon(":/DlsWidgets/images/system-search.svg"));
    connect(&zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOut()));

    zoomResetAction.setText(tr("Auto range"));
    zoomResetAction.setStatusTip(
            tr("Automatically zoom to the data extent."));
    zoomResetAction.setIcon(QIcon(":/DlsWidgets/images/view-fullscreen.svg"));
    connect(&zoomResetAction, SIGNAL(triggered()), this, SLOT(zoomReset()));

    pickDateAction.setText(tr("Choose date..."));
    pickDateAction.setStatusTip(
            tr("Open a dialog for date picking."));
    connect(&pickDateAction, SIGNAL(triggered()), this, SLOT(pickDate()));

    gotoTodayAction.setText(tr("Today"));
    gotoTodayAction.setStatusTip(
            tr("Set the date range to today."));
    connect(&gotoTodayAction, SIGNAL(triggered()),
            this, SLOT(gotoDate()));

    gotoYesterdayAction.setText(tr("Yesterday"));
    gotoYesterdayAction.setStatusTip(
            tr("Set the date range to yesterday."));
    connect(&gotoYesterdayAction, SIGNAL(triggered()),
            this, SLOT(gotoDate()));

    gotoThisWeekAction.setText(tr("This week"));
    gotoThisWeekAction.setStatusTip(
            tr("Set the date range to this week."));
    connect(&gotoThisWeekAction, SIGNAL(triggered()),
            this, SLOT(gotoDate()));

    gotoLastWeekAction.setText(tr("Last week"));
    gotoLastWeekAction.setStatusTip(
            tr("Set the date range to last week."));
    connect(&gotoLastWeekAction, SIGNAL(triggered()),
            this, SLOT(gotoDate()));

    gotoThisMonthAction.setText(tr("This month"));
    gotoThisMonthAction.setStatusTip(
            tr("Set the date range to this month."));
    connect(&gotoThisMonthAction, SIGNAL(triggered()),
            this, SLOT(gotoDate()));

    gotoLastMonthAction.setText(tr("Last month"));
    gotoLastMonthAction.setStatusTip(
            tr("Set the date range to last month."));
    connect(&gotoLastMonthAction, SIGNAL(triggered()),
            this, SLOT(gotoDate()));

    gotoThisYearAction.setText(tr("This year"));
    gotoThisYearAction.setStatusTip(
            tr("Set the date range to this year."));
    connect(&gotoThisYearAction, SIGNAL(triggered()),
            this, SLOT(gotoDate()));

    gotoLastYearAction.setText(tr("Last year"));
    gotoLastYearAction.setStatusTip(
            tr("Set the date range to last year."));
    connect(&gotoLastYearAction, SIGNAL(triggered()),
            this, SLOT(gotoDate()));

    sectionPropertiesAction.setText(tr("Section properties..."));
    sectionPropertiesAction.setStatusTip(tr("Open the section configuration"
                " dialog."));
    sectionPropertiesAction.setIcon(
            QIcon(":/DlsWidgets/images/document-properties.svg"));
    connect(&sectionPropertiesAction, SIGNAL(triggered()),
            this, SLOT(sectionProperties()));

    removeSectionAction.setText(tr("Remove section"));
    removeSectionAction.setStatusTip(tr("Remove the selected section."));
    removeSectionAction.setIcon(QIcon(":/DlsWidgets/images/list-remove.svg"));
    connect(&removeSectionAction, SIGNAL(triggered()),
            this, SLOT(removeSelectedSection()));

    clearSectionsAction.setText(tr("Clear sections"));
    clearSectionsAction.setStatusTip(tr("Remove all sections."));
    clearSectionsAction.setIcon(QIcon(":/DlsWidgets/images/list-remove.svg"));
    connect(&clearSectionsAction, SIGNAL(triggered()),
            this, SLOT(clearSections()));

    messagesAction.setText(tr("Show Messages"));
    messagesAction.setStatusTip(tr("Show process messages."));
    messagesAction.setIcon(QIcon(":/DlsWidgets/images/messages.svg"));
    messagesAction.setCheckable(true);
    connect(&messagesAction, SIGNAL(changed()),
            this, SLOT(showMessagesChanged()));

    filterAction.setText(tr("Filter messages..."));
    filterAction.setStatusTip(tr("Filter messages by regular expressions."));
    filterAction.setIcon(QIcon(":/DlsWidgets/images/messages.svg"));
    connect(&filterAction, SIGNAL(triggered()), this, SLOT(filterTriggered()));

    printAction.setText(tr("Print..."));
    printAction.setStatusTip(tr("Open the print dialog."));
    printAction.setIcon(QIcon(":/DlsWidgets/images/document-print.svg"));
    connect(&printAction, SIGNAL(triggered()), this, SLOT(print()));

    exportAction.setText(tr("Export..."));
    exportAction.setStatusTip(tr("Open the export dialog."));
    exportAction.setIcon(QIcon(":/DlsWidgets/images/document-save.svg"));
    connect(&exportAction, SIGNAL(triggered()), this, SLOT(showExport()));

    updateActions();
}

/****************************************************************************/

/** Destructor.
 */
Graph::~Graph()
{
    thread.quit();
    thread.wait();
    clearSections();
#ifdef DEBUG_MT_IN_FILE
    debugFile.close();
#endif
}

/****************************************************************************/

/** Sets the model used for drop operations.
 */
void Graph::setDropModel(QtDls::Model *model)
{
    dropModel = model;
}

/****************************************************************************/

/** Gives a hint aboute the optimal size.
 */
QSize Graph::sizeHint() const
{
    return QSize(300, 100);
}

/****************************************************************************/

/** Loads sections and settings from an XML file.
 */
bool Graph::load(const QString &path, Model *model)
{
    clearSections();

    QFile file(path);
    QFileInfo fi(path);
    dir = fi.absoluteDir();

    if (!file.open(QFile::ReadOnly)) {
        qWarning() << tr("Failed to open %1!").arg(path);
        return false;
    }

    QDomDocument doc;
    QString errorMessage;
    int errorRow, errorColumn;

    if (!doc.setContent(&file, false,
                &errorMessage, &errorRow, &errorColumn)) {
        qWarning() << tr("Failed to parse XML, line %2, column %3: %4")
            .arg(errorRow).arg(errorColumn).arg(errorMessage);
        return false;
    }

    QDomElement docElem = doc.documentElement();
    QDomNodeList children = docElem.childNodes();
    int64_t start = 0LL, end = 0LL;
    bool hasStart = false, hasEnd = false, val;

    for (int i = 0; i < children.size(); i++) {
        QDomNode node = children.item(i);
        if (!node.isElement()) {
            continue;
        }

        QDomElement child = node.toElement();

        if (child.tagName() == "Start") {
            QString text = child.text();
            bool ok;
            start = text.toLongLong(&ok, 10);
            if (!ok) {
                qWarning() << "Invalid start time";
                return false;
            }
            hasStart = true;
        }
        else if (child.tagName() == "End") {
            QString text = child.text();
            bool ok;
            end = text.toLongLong(&ok, 10);
            if (!ok) {
                qWarning() << "Invalid end time";
                return false;
            }
            hasEnd = true;
        }
        else if (child.tagName() == "ShowMessages") {
            QString text = child.text();
            bool ok;
            val = text.toInt(&ok, 10) != 0;
            if (!ok) {
                qWarning() << "Invalid value for ShowMessages";
                return false;
            }
            setShowMessages(val);
        }
        else if (child.tagName() == "MessageAreaHeight") {
            QString text = child.text();
            bool ok;
            int num = text.toInt(&ok, 10);
            if (!ok) {
                qWarning() << "Invalid value for MessageAreaHeight";
                return false;
            }
            messageAreaHeight = num;
        }
        else if (child.tagName() == "MessageFilter") {
            setMessageFilter(child.text());
        }
        else if (child.tagName() == "Sections") {
            loadSections(child, model, dir);
        }
    }

    if (!hasStart || !hasEnd) {
        qWarning() << "start or end tag missing!";
        return false;
    }

    scale.setRange(start, end);
    autoRange = false;
    newView();
    loadData();

    updateScrollBar();
    updateActions();
    return true;
}

/****************************************************************************/

/** Saves sections and settings to an XML file.
 */
bool Graph::save(const QString &path)
{
    QFile file(path);

    if (!file.open(QFile::WriteOnly)) {
        qWarning() << tr("Failed to open %1 for writing!").arg(path);
        return false;
    }

    QDomDocument doc;
    QDomElement root = doc.createElement("DlsView");
    doc.appendChild(root);

    QString num;

    QDomElement startElem = doc.createElement("Start");
    num.setNum(scale.getStart().to_int64());
    QDomText text = doc.createTextNode(num);
    startElem.appendChild(text);
    root.appendChild(startElem);

    QDomElement endElem = doc.createElement("End");
    num.setNum(scale.getEnd().to_int64());
    text = doc.createTextNode(num);
    endElem.appendChild(text);
    root.appendChild(endElem);

    QDomElement msgElem = doc.createElement("ShowMessages");
    num.setNum(showMessages);
    text = doc.createTextNode(num);
    msgElem.appendChild(text);
    root.appendChild(msgElem);

    QDomElement msgHeightElem = doc.createElement("MessageAreaHeight");
    num.setNum(messageAreaHeight);
    text = doc.createTextNode(num);
    msgHeightElem.appendChild(text);
    root.appendChild(msgHeightElem);

    QDomElement filterElem = doc.createElement("MessageFilter");
    text = doc.createTextNode(messageFilter);
    filterElem.appendChild(text);
    root.appendChild(filterElem);

    QDomElement secElem = doc.createElement("Sections");
    root.appendChild(secElem);

    rwLockSections.lockForRead();

    for (QList<Section *>::const_iterator s = sections.begin();
            s != sections.end(); s++) {
        (*s)->save(secElem, doc);
    }

    rwLockSections.unlock();

    QByteArray ba = doc.toByteArray(2);
    if (file.write(ba) != ba.size()) {
        return false;
    }

    file.close();
    return true;
}

/****************************************************************************/

bool Graph::renderPage(
        QPainter &painter,
        const QRect &rect,
        unsigned int pageNum,
        RenderFlags flags
        )
{
    bool ret = false;

    int displayHeight = renderCommon(painter, rect);

    rwLockSections.lockForRead();

    QList<Section *>::const_iterator first = sections.begin();
    unsigned int page = 0;

    while (first != sections.end()) {
        QList<Section *>::const_iterator last =
            lastSectionOnPage(first, displayHeight);
        if (page == pageNum) {
            renderSections(painter, rect, first, last, displayHeight, flags);
            ret = true;
            break;
        }
        first = last + 1;
        page++;
    }

    rwLockSections.unlock();

    return ret;
}

/****************************************************************************/

/** Tries to connect layers without channels to the given model.
 */
void Graph::connectChannels(Model *model)
{
    rwLockSections.lockForRead();

    for (QList<Section *>::const_iterator s = sections.begin();
            s != sections.end(); s++) {
        (*s)->connectChannels(model, dir);
    }

    rwLockSections.unlock();
}

/****************************************************************************/

/** Returns, if a directory is used by some layer.
 */
bool Graph::dirInUse(const LibDLS::Directory *d)
{
    bool inUse;

    rwLockSections.lockForRead();

    for (QList<Section *>::const_iterator s = sections.begin();
            s != sections.end(); s++) {
        inUse = (*s)->dirInUse(d);
        if (inUse) {
            break;
        }
    }

    rwLockSections.unlock();

    return inUse;
}

/****************************************************************************/

Section *Graph::appendSection()
{
    rwLockSections.lockForWrite();

    Section *s = new Section(this);
    sections.append(s);

    rwLockSections.unlock();

    updateScrollBar();
    updateActions();
    return s;
}

/****************************************************************************/

Section *Graph::insertSectionBefore(Section *before)
{
    rwLockSections.lockForWrite();

    int index = sections.indexOf(before);
    Section *s = new Section(this);

    if (index > -1) {
        sections.insert(index, s);
    }
    else {
        sections.append(s);
    }

    rwLockSections.unlock();

    updateScrollBar();
    updateActions();

    return s;
}

/****************************************************************************/

void Graph::removeSection(Section *section)
{
    rwLockSections.lockForWrite();

    if (dropSection == section) {
        dropSection = NULL;
    }

    if (selectedSection == section) {
        selectedSection = NULL;
    }

    if (splitterSection == section) {
        splitterSection = NULL;
    }

    if (movingSection == section) {
        movingSection = NULL;
    }

    int num = sections.removeAll(section);

    rwLockSections.unlock();

    updateScrollBar();
    updateActions();

    delete section;

    if (num > 0) {
        update();
    }
}

/****************************************************************************/

void Graph::updateRange()
{
    if (!autoRange) {
        return;
    }

    LibDLS::Time start, end;
    bool valid = false;

    rwLockSections.lockForRead();

    for (QList<Section *>::const_iterator s = sections.begin();
            s != sections.end(); s++) {
        (*s)->getRange(valid, start, end);
    }

    rwLockSections.unlock();

    if (valid) {
        bool scaleChanged =
            scale.getStart() != start || scale.getEnd() != end;
        scale.setRange(start, end);
        newView();
        if (scaleChanged) {
            // FIXME avoid infinite loop: 1) Fetch chunks from all
            // channels, 2) Update range, 3) load data
            loadData();
        }
        update();
    }
}

/****************************************************************************/

void Graph::setRange(const LibDLS::Time &start, const LibDLS::Time &end)
{
    scale.setRange(start, end);
    autoRange = false;

    newView();
    loadData();
}

/****************************************************************************/

QSet<QUrl> Graph::urls()
{
    QSet<QtDls::Channel *> chs = displayedChannels(); // FIXME race
    QSet<QUrl> ret;

    for (QSet<QtDls::Channel *>::const_iterator ch = chs.begin();
            ch != chs.end(); ch++) {
        ret += (*ch)->url();
    }

    return ret;
}

/****************************************************************************/

QList<Graph::ChannelInfo> Graph::channelInfo()
{
    QSet<QtDls::Channel *> chs = displayedChannels(); // FIXME race
    QList<ChannelInfo> ret;
    ChannelInfo ci;

    for (QSet<QtDls::Channel *>::const_iterator ch = chs.begin();
            ch != chs.end(); ch++) {
        ci.url = (*ch)->url();
        ci.jobId = (*ch)->job()->id();
        ci.dirIndex = (*ch)->dirIndex();
        ret += ci;
    }

    return ret;
}

/****************************************************************************/

void Graph::previousView()
{
    if (views.empty() || currentView == views.begin()) {
        return;
    }

    currentView--;
    scale.setRange(currentView->start, currentView->end);
    autoRange = false;
    updateActions();
    loadData();
}

/****************************************************************************/

void Graph::nextView()
{
    if (views.empty() || currentView + 1 == views.end()) {
        return;
    }

    currentView++;
    scale.setRange(currentView->start, currentView->end);
    autoRange = false;
    updateActions();
    loadData();
}

/****************************************************************************/

void Graph::loadData()
{
    rwLockSections.lockForRead();

    // mark all sections as busy
    for (QList<Section *>::iterator s = sections.begin();
            s != sections.end(); s++) {
        (*s)->setBusy(true);
    }

    rwLockSections.unlock();

    update(); // FIXME update busy rect only

    if (workerBusy) {
        reloadPending = true;
        pendingWidth = getDataWidth();
    }
    else {
        worker.width = getDataWidth();
        workerBusy = true;
        QMetaObject::invokeMethod(&worker, "doWork", Qt::QueuedConnection);
    }
}

/****************************************************************************/

void Graph::zoomIn()
{
    if (getEnd() <= getStart()) {
        return;
    }

    LibDLS::Time diff;
    diff.from_dbl_time((getEnd() - getStart()).to_dbl_time() / 4.0);
    setRange(getStart() + diff, getEnd() - diff);
}

/****************************************************************************/

void Graph::zoomOut()
{
    if (getEnd() <= getStart()) {
        return;
    }

    LibDLS::Time diff;
    diff.from_dbl_time((getEnd() - getStart()).to_dbl_time() / 2.0);
    setRange(getStart() - diff, getEnd() + diff);
}

/****************************************************************************/

void Graph::zoomReset()
{
    autoRange = true;
    updateActions();
    updateRange();
    loadData();
}

/****************************************************************************/

void Graph::setInteraction(Interaction i)
{
    interaction = i;

    if (zooming && interaction != Zoom) {
        zooming = false;
    }
    if (panning && interaction != Pan) {
        panning = false;
    }
    if (measuring && interaction != Measure) {
        measureTime.set_null();
    }

    updateMeasuring();
    updateActions();
    updateCursor();
    update();
}

/****************************************************************************/

void Graph::setNamedRange(NamedRange r)
{
    LibDLS::Time now, start, end;
    int day;

    now.set_now();

    switch (r) {
        case Today:
            start.set_date(now.year(), now.month(), now.day());
            end.set_date(now.year(), now.month(), now.day() + 1);
            setRange(start, end);
            break;
        case Yesterday:
            start.set_date(now.year(), now.month(), now.day() - 1);
            end.set_date(now.year(), now.month(), now.day());
            setRange(start, end);
            break;
        case ThisWeek:
            day = now.day() - now.day_of_week() + 1;
            start.set_date(now.year(), now.month(), day);
            end.set_date(now.year(), now.month(), day + 7);
            setRange(start, end);
            break;
        case LastWeek:
            day = now.day() - now.day_of_week() + 1;
            start.set_date(now.year(), now.month(), day - 7);
            end.set_date(now.year(), now.month(), day);
            setRange(start, end);
            break;
        case ThisMonth:
            start.set_date(now.year(), now.month(), 1);
            end.set_date(now.year(), now.month() + 1, 1);
            setRange(start, end);
            break;
        case LastMonth:
            start.set_date(now.year(), now.month() - 1, 1);
            end.set_date(now.year(), now.month(), 1);
            setRange(start, end);
            break;
        case ThisYear:
            start.set_date(now.year(), 1, 1);
            end.set_date(now.year() + 1, 1, 1);
            setRange(start, end);
            break;
        case LastYear:
            start.set_date(now.year() - 1, 1, 1);
            end.set_date(now.year(), 1, 1);
            setRange(start, end);
            break;
    }
}

/****************************************************************************/

void Graph::pan(double fraction)
{
    if (getEnd() <= getStart()) {
        return;
    }

    LibDLS::Time diff;
    diff.from_dbl_time((getEnd() - getStart()).to_dbl_time() * fraction);
    setRange(getStart() + diff, getEnd() + diff);
}

/****************************************************************************/

void Graph::print()
{
    QPrinter printer;
    printer.setOrientation(QPrinter::Landscape);
    printer.setPaperSize(QPrinter::A4);

#ifdef __unix__
    /* the native windows printing dialog does not support pdf output.
     * http://qt-project.org/forums/viewthread/6088 */
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName("dls-export.pdf");
#endif

    QPrintDialog dialog(&printer, this);
    int ret = dialog.exec();

    if (ret != QDialog::Accepted) {
        return;
    }

    QPainter painter;

    if (!painter.begin(&printer)) {
        qWarning("failed to open file, is it writable?");
        return;
    }

    QRect rect(printer.pageRect());
    rect.moveTo(0, 0);

    rwLockSections.lockForRead();

    QList<Section *>::const_iterator first = sections.begin();
    while (first != sections.end()) {
        int displayHeight = renderCommon(painter, rect);
        QList<Section *>::const_iterator last =
            lastSectionOnPage(first, displayHeight);
        renderSections(painter, rect, first, last, displayHeight, All);
        first = last + 1;
        if (first != sections.end()) {
            printer.newPage();
        }
    }

    rwLockSections.unlock();

    painter.end();
}

/****************************************************************************/

/** Set whether to display messages.
 */
void Graph::setShowMessages(
        bool show
        )
{
    if (show != showMessages) {
        showMessages = show;
        messagesAction.setChecked(show);
        update();
    }
}

/****************************************************************************/

/** Set the message filter regexp.
 */
void Graph::setMessageFilter(
        const QString &filter
        )
{
    if (filter != messageFilter) {
        messageFilter = filter;
        update();
    }
}

/****************************************************************************/

/** Clears the list of sections.
 */
void Graph::clearSections()
{
    rwLockSections.lockForWrite();

    dropSection = NULL;
    selectedSection = NULL;
    splitterSection = NULL;
    movingSection = NULL;

    for (QList<Section *>::iterator s = sections.begin();
            s != sections.end(); s++) {
        delete *s;
    }

    sections.clear();

    rwLockSections.unlock();

    updateScrollBar();
    updateActions();
    update();
}

/****************************************************************************/

/** Event handler.
 */
bool Graph::event(
        QEvent *event /**< Paint event flags. */
        )
{
    switch (event->type()) {
        case QEvent::MouseButtonDblClick:
            fixMeasuringLine();
            return true;

        case QEvent::LanguageChange:
            break;

        case QEvent::TouchBegin:
        case QEvent::TouchUpdate:
        case QEvent::TouchEnd:
#if QT_VERSION >= 0x050000
        case QEvent::TouchCancel:
#endif
            updateTouch(static_cast<QTouchEvent *>(event));
            return true;

        default:
            break;
    }

    return QWidget::event(event);
}

/****************************************************************************/

/** Mouse press event.
 */
void Graph::mousePressEvent(QMouseEvent *event)
{
    if (!(event->button() & Qt::LeftButton)) {
        return;
    }

    startPos = event->pos();
    endPos = event->pos();
    dragStart = getStart();
    dragEnd = getEnd();

    if (mouseOverMsgSplitter) {
        movingMsgSplitter = true;
        startHeight = messageAreaHeight;
        event->accept();
        updateCursor();
        update();
        return;
    }
    else if (splitterSection) {
        movingSection = splitterSection;
        startHeight = splitterSection->getHeight();
        event->accept();
        updateCursor();
        update();
        return;
    }
    else if (interaction == Zoom && !touchZooming) {
        zooming = true;
        event->accept();
        updateCursor();
        update();
        return;
    }
    else if (interaction == Pan && !touchZooming) {
        panning = true;
        event->accept();
        updateCursor();
        update();
        return;
    }
}

/****************************************************************************/

/** Mouse press event.
 */
void Graph::mouseMoveEvent(QMouseEvent *event)
{
    endPos = event->pos();

    if (movingMsgSplitter) {
        int dh = endPos.y() - startPos.y();
        messageAreaHeight = startHeight - dh;
        if (messageAreaHeight < MSG_LINES_HEIGHT) {
            messageAreaHeight = MSG_LINES_HEIGHT;
        }
        if (dh) {
            update();
        }
        updateScrollBar();
    }

    if (movingSection) {
        int dh = endPos.y() - startPos.y();
        int h = startHeight + dh;
        if (h < 0) {
            h = 0;
        }
        movingSection->setHeight(h);
        updateScrollBar();
    }

    if (zooming) {
        update();
    }

    if (panning) {
        int dataWidth = getDataWidth();
        LibDLS::Time range = getEnd() - getStart();

        if (range > 0.0 && dataWidth > 0) {
            double xScale = range.to_dbl_time() / dataWidth;

            LibDLS::Time diff;
            diff.from_dbl_time((endPos.x() - startPos.x()) * xScale);
            scale.setRange(dragStart - diff, dragEnd - diff);
            autoRange = false;
            updateActions();
            update();
        }
    }

    updateMeasuring();

    QRect msgSplitterRect(contentsRect());
    msgSplitterRect.setTop(
            contentsRect().bottom() + 1 - messageAreaHeight - splitterWidth);
    msgSplitterRect.setHeight(std::max(splitterWidth, MIN_TOUCH_HEIGHT));

    bool last = mouseOverMsgSplitter;
    mouseOverMsgSplitter =
        msgSplitterRect.contains(event->pos()) && showMessages;
    if (mouseOverMsgSplitter != last) {
        update(msgSplitterRect);
    }

    rwLockSections.lockForRead();

    Section *sec = splitterSectionFromPos(event->pos());
    if (splitterSection != sec) {
        splitterSection = sec;
        update();
    }

    rwLockSections.unlock();

    updateCursor();
}

/****************************************************************************/

/** Mouse release event.
 */
void Graph::mouseReleaseEvent(QMouseEvent *event)
{
    bool wasZooming = zooming;
    bool wasPanning = panning;
    int dataWidth = getDataWidth();
    LibDLS::Time range = getEnd() - getStart();

    movingMsgSplitter = false;
    zooming = false;
    panning = false;
    movingSection = NULL;
    updateCursor();
    update();

    if (startPos.x() == endPos.x() || dataWidth <= 0 || range <= 0.0) {
        return;
    }

    double xScale = range.to_dbl_time() / dataWidth;

    if (wasZooming) {
        LibDLS::Time diff;
        int offset = contentsRect().left() + scaleWidth;
        diff.from_dbl_time((startPos.x() - offset) * xScale);
        LibDLS::Time newStart = getStart() + diff;
        diff.from_dbl_time((event->pos().x() - offset) * xScale);
        LibDLS::Time newEnd = getStart() + diff;
        setRange(newStart, newEnd);
    }
    else if (wasPanning) {
        LibDLS::Time diff;
        diff.from_dbl_time((endPos.x() - startPos.x()) * xScale);
        setRange(dragStart - diff, dragEnd - diff);
    }
}

/****************************************************************************/

/** Mouse release event.
 */
void Graph::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    measuring = false;
    update();
}

/****************************************************************************/

void Graph::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
        case Qt::Key_Left:
            if (event->modifiers() & Qt::AltModifier) {
                previousView();
            }
            else {
                pan(-0.125);
            }
            break;
        case Qt::Key_Right:
            if (event->modifiers() & Qt::AltModifier) {
                nextView();
            }
            else {
                pan(0.125);
            }
            break;
        case Qt::Key_F5:
            loadData();
            break;
        case Qt::Key_Z:
            setInteraction(Zoom);
            break;
        case Qt::Key_P:
            setInteraction(Pan);
            break;
        case Qt::Key_M:
            setInteraction(Measure);
            break;
        case Qt::Key_Plus:
            zoomIn();
            break;
        case Qt::Key_Minus:
            zoomOut();
            break;
        case Qt::Key_PageUp:
            pan(1.0);
            break;
        case Qt::Key_PageDown:
            pan(-1.0);
            break;
        default:
            QWidget::keyPressEvent(event);
            break;
    }
}

/****************************************************************************/

/** Handles the widget's resize event.
 */
void Graph::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    scale.setLength(contentsRect().width());
    updateScrollBar();

    int sectionWidth = contentsRect().width();
    if (scrollBarNeeded) {
        sectionWidth -= scrollBar.width();
    }

    rwLockSections.lockForRead();

    for (QList<Section *>::iterator s = sections.begin();
            s != sections.end(); s++) {
        (*s)->resize(sectionWidth);
    }

    rwLockSections.unlock();

    loadData();
}

/****************************************************************************/

/** Paint function.
 */
void Graph::paintEvent(
        QPaintEvent *event /**< paint event flags */
        )
{
    QFrame::paintEvent(event);

    QPainter painter(this);

    QRect timeScaleRect(contentsRect());
    int height = scale.getOuterLength() + 1;
    scaleWidth = 0;

    rwLockSections.lockForRead();

    for (QList<Section *>::iterator s = sections.begin();
            s != sections.end(); s++) {
        height += (*s)->getHeight() + splitterWidth;
        if ((*s)->getShowScale()) {
            int w = (*s)->getScaleWidth();
            if (w > scaleWidth) {
                scaleWidth = w;
            }
        }
    }

    rwLockSections.unlock();

    if (height > contentsRect().height()) {
        height = contentsRect().height();
    }

    if (showMessages) {
        height = contentsRect().height();
    }

    timeScaleRect.setLeft(scaleWidth);
    timeScaleRect.setHeight(height);
    scale.draw(painter, timeScaleRect);

    // Horizontal line above top section
    QPen horLinePen;
    painter.setPen(horLinePen);
    painter.drawLine(contentsRect().left(),
            contentsRect().top() + scale.getOuterLength(),
            contentsRect().right(),
            contentsRect().top() + scale.getOuterLength());

    int top = contentsRect().top() + scale.getOuterLength() + 1;
    LibDLS::Time range = getEnd() - getStart();
    int measurePos = -1;
    if (!measureTime.is_null()
            && measureTime >= getStart() && measureTime < getEnd()
            && range > 0.0) {
        double xScale = getDataWidth() / range.to_dbl_time();
        measurePos = (measureTime - getStart()).to_dbl_time() * xScale + 0.5;
    }
    QRect dataRect(contentsRect());
    dataRect.setTop(top);
    if (showMessages) {
        dataRect.setBottom(contentsRect().bottom() -
                messageAreaHeight - splitterWidth);
    }
    if (scrollBarNeeded) {
        dataRect.setWidth(contentsRect().width() - scrollBar.width());
        // FIXME don't move scrollbar during paint, better during resize?
        scrollBar.move(dataRect.right() + 1, top);
        scrollBar.resize(scrollBar.width(), dataRect.height());
    }

    painter.setClipRect(dataRect);

    rwLockSections.lockForRead();

    for (QList<Section *>::iterator s = sections.begin();
            s != sections.end(); s++) {
        QRect sectionRect(dataRect);
        sectionRect.setTop(top - scrollBar.value());
        sectionRect.setHeight((*s)->getHeight());
        (*s)->draw(painter, sectionRect, measurePos, scaleWidth, true);

        QRect splitterRect(sectionRect);
        splitterRect.setTop(top + (*s)->getHeight() - scrollBar.value());
        splitterRect.setHeight(splitterWidth);
        painter.fillRect(splitterRect, palette().window());

        QStyleOption styleOption;
        styleOption.initFrom(this);
        styleOption.rect = splitterRect;
        QStyle::State state = QStyle::State_MouseOver;
        styleOption.state &= ~state;
        if (splitterSection == *s && !zooming && !panning) {
            styleOption.state |= state;
        }
        QStyle *style = QApplication::style();
        style->drawControl(QStyle::CE_Splitter, &styleOption, &painter, this);

        if ((*s == dropSection && dropLine < 0) || *s == selectedSection) {
            drawDropRect(painter, sectionRect);
        }

        top += (*s)->getHeight() + splitterWidth;
    }

    rwLockSections.unlock();

    painter.setClipping(false);

    if (showMessages) {
        QRect msgRect(contentsRect());
        msgRect.setLeft(contentsRect().left() + scaleWidth);
        msgRect.setTop(dataRect.bottom() + 1 + splitterWidth);
        msgRect.setHeight(messageAreaHeight);

        painter.drawLine(msgRect.left(), msgRect.top() - 1,
                msgRect.right(), msgRect.top() - 1);

        QRect msgSplitterRect(contentsRect());
        msgSplitterRect.setTop(contentsRect().bottom() + 1 -
                messageAreaHeight - splitterWidth);
        msgSplitterRect.setHeight(splitterWidth);

        painter.fillRect(msgSplitterRect, palette().window());

        QStyleOption styleOption;
        styleOption.initFrom(this);
        styleOption.rect = msgSplitterRect;
        QStyle::State state = QStyle::State_MouseOver;
        styleOption.state &= ~state;
        if (mouseOverMsgSplitter) {
            styleOption.state |= state;
        }
        QStyle *style = QApplication::style();
        style->drawControl(QStyle::CE_Splitter, &styleOption, &painter, this);

        drawMessages(painter, msgRect);
    }

    if (dropLine >= 0) {
        QPen pen;
        pen.setColor(Qt::blue);
        pen.setWidth(5);
        painter.setPen(pen);

        painter.drawLine(5, dropLine, dataRect.width() - 10, dropLine);
    }
    else if (dropRemaining >= 0) {
        QRect remRect(contentsRect());
        remRect.setTop(dropRemaining);
        drawDropRect(painter, remRect);
    }

    if (zooming) {
        QPen pen;
        pen.setColor(Qt::blue);
        painter.setPen(pen);

        QRect zoomRect(startPos.x(), contentsRect().top(),
                endPos.x() - startPos.x() + 1, contentsRect().height());
        painter.fillRect(zoomRect, QColor(0, 0, 255, 63));
        painter.drawLine(zoomRect.topLeft(), zoomRect.bottomLeft());
        painter.drawLine(zoomRect.topRight(), zoomRect.bottomRight());
    }

    if (measurePos != -1) {
        int xp = contentsRect().left() + scaleWidth + measurePos;
        QPen pen;
        pen.setColor(Qt::darkBlue);
        painter.setPen(pen);

        painter.drawLine(xp, contentsRect().top(),
                xp, contentsRect().bottom());

        QRect textRect(contentsRect());
        textRect.setLeft(xp + 3);
        textRect.setTop(contentsRect().top() + 2);
        textRect.setHeight(contentsRect().height() - 4);
        QString label(measureTime.to_real_time().c_str());
        QFontMetrics fm(painter.font());
        QSize s = fm.size(0, label);
        if (s.width() <= textRect.width()) {
            painter.fillRect(
                    QRect(textRect.topLeft(), s).adjusted(-2, 0, 2, 0),
                    Qt::white);
            painter.drawText(textRect, Qt::AlignLeft, label);
        }
        else {
            textRect.setLeft(contentsRect().left());
            textRect.setRight(xp - 3);
            if (s.width() <= textRect.width()) {
                painter.fillRect(QRect(
                            QPoint(textRect.right() + 1 - s.width(),
                            textRect.top()), s).adjusted(-2, 0, 2, 0),
                        Qt::white);
                painter.drawText(textRect, Qt::AlignRight, label);
            }
        }
    }

#ifdef DEBUG_MT_ON_SCREEN
    painter.setClipping(false);
    QPen pen;
    pen.setColor(Qt::darkRed);
    painter.setPen(pen);
    painter.drawText(contentsRect(), Qt::AlignLeft | Qt::AlignBottom,
            "DEBUG on");

    if (!debugString.isEmpty()) {
        painter.drawText(contentsRect(), Qt::AlignRight | Qt::AlignBottom,
                debugString);
    }
#endif
}

/****************************************************************************/

/** Shows the context menu.
 */
void Graph::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    QMenu gotoMenu(this);

    rwLockSections.lockForWrite();

    removeMeasuringAction.setEnabled(
            interaction != Measure && !measureTime.is_null());
    selectedSection = sectionFromPos(event->pos());
    removeSectionAction.setEnabled(selectedSection);
    clearSectionsAction.setEnabled(!sections.isEmpty());
    sectionPropertiesAction.setEnabled(selectedSection);

    rwLockSections.unlock();

    menu.addAction(&fixMeasuringAction);
    menu.addAction(&removeMeasuringAction);
    menu.addSeparator();
    menu.addAction(&prevViewAction);
    menu.addAction(&nextViewAction);
    menu.addSeparator();
    menu.addAction(&loadDataAction);
    menu.addSeparator();
    menu.addAction(&zoomAction);
    menu.addAction(&panAction);
    menu.addAction(&measureAction);
    menu.addSeparator();
    menu.addAction(&zoomInAction);
    menu.addAction(&zoomOutAction);
    menu.addAction(&zoomResetAction);
    menu.addSeparator();
    QAction *gotoMenuAction = menu.addMenu(&gotoMenu);
    gotoMenuAction->setText(tr("Go to date"));
    menu.addSeparator();
    menu.addAction(&sectionPropertiesAction);
    menu.addAction(&removeSectionAction);
    menu.addAction(&clearSectionsAction);
    menu.addAction(&messagesAction);
    menu.addAction(&filterAction);
    menu.addSeparator();
    menu.addAction(&printAction);
    menu.addAction(&exportAction);

    gotoMenu.addAction(&pickDateAction);
    gotoMenu.addSeparator();
    gotoMenu.addAction(&gotoTodayAction);
    gotoMenu.addAction(&gotoYesterdayAction);
    gotoMenu.addSeparator();
    gotoMenu.addAction(&gotoThisWeekAction);
    gotoMenu.addAction(&gotoLastWeekAction);
    gotoMenu.addSeparator();
    gotoMenu.addAction(&gotoThisMonthAction);
    gotoMenu.addAction(&gotoLastMonthAction);
    gotoMenu.addSeparator();
    gotoMenu.addAction(&gotoThisYearAction);
    gotoMenu.addAction(&gotoLastYearAction);

    menu.exec(event->globalPos());

    rwLockSections.lockForWrite();
    selectedSection = NULL;
    rwLockSections.unlock();
}

/****************************************************************************/

void Graph::dragEnterEvent(QDragEnterEvent *event)
{
    if (!dropModel || !event->mimeData()->hasFormat("text/uri-list")) {
        return;
    }

    updateDragging(event->pos());

    event->acceptProposedAction();
}

/****************************************************************************/

void Graph::dragLeaveEvent(QDragLeaveEvent *event)
{
    Q_UNUSED(event);

    resetDragging();
}

/****************************************************************************/

void Graph::dragMoveEvent(QDragMoveEvent *event)
{
    updateDragging(event->pos());
}

/****************************************************************************/

void Graph::dropEvent(QDropEvent *event)
{
    if (!dropModel) {
        return;
    }

    Section *s = NULL;

    updateDragging(event->pos());

    if (dropSection) {
        if (dropLine >= 0) {
            /* insert before dropSection */
            s = insertSectionBefore(dropSection);
        }
        else {
            s = dropSection;
        }
    }
    else {
        s = appendSection(); // FIXME do not release lock inbetween!
    }

    QList<QUrl> urls = event->mimeData()->urls();

    rwLockSections.lockForWrite();

    for (QList<QUrl>::iterator url = urls.begin(); url != urls.end(); url++) {
        if (!url->isValid()) {
            qWarning() << "Not a valid URL:" << *url;
            continue;
        }

        QtDls::Channel *ch;

        try {
            ch = dropModel->getChannel(*url);
        }
        catch (Model::Exception &e) {
            qWarning() << tr("Failed to get channel %1: %2")
                .arg(url->toString())
                .arg(e.msg);
            continue;
        }

        if (ch) {
            s->appendLayer(ch);
        }
        else {
            qWarning() << QString("Failed to get channel %1!")
                .arg(url->toString());
        }
    }

    rwLockSections.unlock();

    resetDragging();
    event->acceptProposedAction();

    loadData();
}

/****************************************************************************/

void Graph::wheelEvent(QWheelEvent *event)
{
    if (event->delta() > 0) { // zoom in
        zoomIn();
    }
    else { // zoom out
        zoomOut();
    }
}

/****************************************************************************/

void Graph::updateDragging(QPoint p)
{
    int top = contentsRect().top() + scale.getOuterLength() + 1 -
        scrollBar.value(), y = p.y();

    resetDragging();

    rwLockSections.lockForRead();

    for (QList<Section *>::iterator s = sections.begin();
            s != sections.end(); s++) {
        if (y <= top + DROP_TOLERANCE) {
            dropSection = *s;
            dropLine = top;
            break;
        } else if (y <= top + (*s)->getHeight() - DROP_TOLERANCE) {
            dropSection = *s;
            break;
        }

        top += (*s)->getHeight() + splitterWidth;
    }

    if (!dropSection) {
        dropRemaining = top;
    }

    rwLockSections.unlock();

    update();
}

/****************************************************************************/

void Graph::resetDragging()
{
    dropSection = NULL;
    dropLine = -1;
    dropRemaining = -1;
    update();
}

/****************************************************************************/

void Graph::updateCursor()
{
    QCursor cur;

    if (zooming) {
        cur = Qt::ArrowCursor;
    }
    else if (panning) {
        cur = Qt::ClosedHandCursor;
    }
    else if (mouseOverMsgSplitter) {
        cur = Qt::SizeVerCursor;
    }
    else if (splitterSection) {
        cur = Qt::SizeVerCursor;
    }
    else if (interaction == Pan) {
        cur = Qt::OpenHandCursor;
    }

    setCursor(cur);
}

/****************************************************************************/

void Graph::updateActions()
{
    bool rangeValid = getEnd() > getStart();

    prevViewAction.setEnabled(!views.empty() && currentView != views.begin());
    nextViewAction.setEnabled(
            !views.empty() && currentView + 1 != views.end());
    loadDataAction.setEnabled(!sections.empty() && rangeValid);

    zoomAction.setEnabled(interaction != Zoom);
    panAction.setEnabled(interaction != Pan);
    measureAction.setEnabled(interaction != Measure);

    zoomInAction.setEnabled(rangeValid);
    zoomOutAction.setEnabled(rangeValid);
    zoomResetAction.setEnabled(!autoRange);
    printAction.setEnabled(rangeValid);
    exportAction.setEnabled(rangeValid);
}

/****************************************************************************/

/** Updates the measuring flag.
 */
void Graph::updateMeasuring()
{
    if (interaction != Measure) {
        measuring = false;
        return;
    }

    LibDLS::Time range = getEnd() - getStart();
    QRect measureRect(contentsRect());
    measureRect.setLeft(contentsRect().left() + scaleWidth);
    int dataWidth = getDataWidth();

    if (range <= 0.0 || !measureRect.isValid() ||
            !measureRect.contains(endPos) || dataWidth <= 0) {
        measuring = false;
    }
    else {
        double xScale = range.to_dbl_time() / dataWidth;

        int measurePos = endPos.x() - measureRect.left();
        measureTime.from_dbl_time(measurePos * xScale);
        measureTime += getStart();
        measuring = true;
    }

    update();
}

/****************************************************************************/

/** Updates the scroll bar.
 */
void Graph::updateScrollBar()
{
    int height = 0;

    rwLockSections.lockForRead();

    for (QList<Section *>::iterator s = sections.begin();
            s != sections.end(); s++) {
        height += (*s)->getHeight() + splitterWidth;
    }

    rwLockSections.unlock();

    int displayHeight = contentsRect().height() - scale.getOuterLength() - 1;
    if (showMessages) {
        displayHeight -= messageAreaHeight + splitterWidth;
    }
    bool needed = height > displayHeight;

    if (needed) {
        scrollBar.setMaximum(height - displayHeight);
        scrollBar.setPageStep(displayHeight);
    }
    else {
        scrollBar.setMaximum(0);
    }

    if (needed != scrollBarNeeded) {
        scrollBarNeeded = needed;
        scrollBar.setVisible(needed);
        update();
    }
}

/****************************************************************************/

Section *Graph::sectionFromPos(const QPoint &pos)
{
    if (!contentsRect().contains(pos)) {
        return NULL;
    }

    QRect timeScaleRect(contentsRect());
    timeScaleRect.setHeight(scale.getOuterLength());
    if (timeScaleRect.contains(pos)) {
        return NULL;
    }

    int top = contentsRect().top() + scale.getOuterLength() + 1 -
        scrollBar.value();

    for (QList<Section *>::iterator s = sections.begin();
            s != sections.end(); s++) {
        QRect rect(contentsRect().left(), top,
                contentsRect().width(), (*s)->getHeight());
        if (rect.contains(pos)) {
            return *s;
        }
        top += (*s)->getHeight() + splitterWidth;
    }

    return NULL;
}

/****************************************************************************/

/** If the position is on a section splitter, return the section.
 */
Section *Graph::splitterSectionFromPos(const QPoint &pos)
{
    Section *sec = NULL;

    int top = contentsRect().top() + scale.getOuterLength() + 1 -
        scrollBar.value();
    QRect splitterRect(contentsRect());
    if (scrollBarNeeded) {
        splitterRect.setWidth(contentsRect().width() - scrollBar.width());
    }

    for (QList<Section *>::iterator s = sections.begin();
            s != sections.end(); s++) {
        if (top > pos.y()) {
            break;
        }
        splitterRect.moveTop(top + (*s)->getHeight());
        QList<Section *>::iterator next = s + 1;
        int height = splitterWidth;
        if (next != sections.end()) {
            height += std::min((*next)->getHeight(), (*next)->legendHeight());
        }
        else { // last section
            height = std::max(height, MIN_TOUCH_HEIGHT);
        }
        splitterRect.setHeight(height);
        if (splitterRect.contains(pos)) {
            sec = *s;
            break;
        }
        top += (*s)->getHeight() + splitterWidth;
    }

    return sec;
}

/****************************************************************************/

/** Draws a drop target rectangle.
 */
void Graph::drawDropRect(QPainter &painter, const QRect &rect)
{
    QPen pen;
    QBrush brush;

    pen.setColor(Qt::blue);
    painter.setPen(pen);

    brush.setColor(QColor(0, 0, 255, 63));
    brush.setStyle(Qt::SolidPattern);
    painter.setBrush(brush);

    painter.drawRect(rect);
}

/****************************************************************************/

/** Removes remaining views and places the current view at the end.
 */
void Graph::newView()
{
    if (!views.empty() && currentView != views.end()) {
        // erase all views after the current
        views.erase(currentView + 1, views.end());
    }

    View v;
    v.start = scale.getStart();
    v.end = scale.getEnd();
    currentView = views.insert(views.end(), v);

    updateActions();
}

/****************************************************************************/

bool Graph::loadSections(const QDomElement &elem, Model *model,
        const QDir &dir)
{
    QDomNodeList children = elem.childNodes();

    for (int i = 0; i < children.size(); i++) {
        QDomNode node = children.item(i);
        if (!node.isElement()) {
            continue;
        }

        QDomElement child = node.toElement();
        if (child.tagName() != "Section") {
            continue;
        }

        Section *section = new Section(this);

        try {
            section->load(child, model, dir);
        } catch (Section::Exception &e) {
            delete section;
            clearSections();
            qWarning() << tr("Failed to load section: %1").arg(e.msg);
            return false;
        }

        rwLockSections.lockForWrite();
        sections.append(section);
        rwLockSections.unlock();
    }

    return true;
}

/****************************************************************************/

void Graph::drawMessages(QPainter &painter, const QRect &rect)
{
    QFontMetrics fm(painter.font());
    int rows = 0, textAreaHeight = rect.height() - 5;
    int rowHeight = MSG_ROW_HEIGHT;
    if (fm.height() > MSG_ROW_HEIGHT) {
        rowHeight = fm.height();
    }
    if (textAreaHeight > 0) {
        rows = textAreaHeight / rowHeight;
    }
    int *feed = NULL;
    if (rows > 0) {
        feed = new int[rows];
    }
    for (int i = 0; i < rows; i++) {
        // allow icon drawing before time scale start
        feed[i] = -scaleWidth;
    }

    int messageWidth = contentsRect().width() - scaleWidth;
    LibDLS::Time range = getEnd() - getStart();

    if (range > 0.0 && messageWidth > 0) {
        double xScale = messageWidth / range.to_dbl_time();
        LibDLS::Job::Message::Type types[messageWidth];
        for (int i = 0; i < messageWidth; i++) {
            types[i] = LibDLS::Job::Message::Unknown;
        }

        msgMutex.lock();

        for (QList<LibDLS::Job::Message>::const_iterator msg =
                messages.begin(); msg != messages.end(); msg++) {
            LibDLS::Time dt = msg->time - getStart();
            double xv = dt.to_dbl_time() * xScale;
            if (xv < 0) {
                continue;
            }
            int xc = (int) (xv + 0.5);
            if (xc >= messageWidth) {
                break;
            }
            if (msg->type > types[xc]) {
                types[xc] = msg->type;
            }

            for (int i = 0; i < rows; i++) {
                if (feed[i] > xc - MSG_ROW_HEIGHT / 2) {
                    continue;
                }

                if (xc + MSG_ROW_HEIGHT / 2 > rect.width()) {
                    break;
                }

                if (msg->type != LibDLS::Job::Message::Unknown) {
                    QRect pixRect;
                    pixRect.setLeft(rect.left() + xc - MSG_ROW_HEIGHT / 2);
                    pixRect.setTop(rect.top() + 5 + i * rowHeight);
                    pixRect.setWidth(MSG_ROW_HEIGHT);
                    pixRect.setHeight(MSG_ROW_HEIGHT);
                    QSvgRenderer svg(messagePixmap[msg->type]);
                    svg.render(&painter, pixRect);
                    feed[i] = xc + MSG_ROW_HEIGHT / 2 + 2;
                }

                if (xc + MSG_ROW_HEIGHT / 2 + 2 <= rect.width()) {
                    QString label = QString::fromUtf8(msg->text.c_str());
                    QRect textRect(rect);
                    textRect.setLeft(
                            rect.left() + xc + MSG_ROW_HEIGHT / 2 + 2);
                    textRect.setTop(rect.top() + MSG_LINES_HEIGHT + 2 +
                            i * rowHeight);
                    textRect.setHeight(rowHeight);
                    label = fm.elidedText(label, Qt::ElideRight,
                            textRect.width(), Qt::AlignLeft);
                    QRect bound = fm.boundingRect(textRect,
                            Qt::AlignLeft | Qt::AlignVCenter, label);
                    QRect back = bound.adjusted(-1, -1, 1, 1);
                    back = back.intersected(textRect);
                    painter.fillRect(back, Qt::white);
                    if (msg->type != LibDLS::Job::Message::Unknown) {
                        painter.setPen(messageColor[msg->type]);
                    }
                    else {
                        painter.setPen(Qt::magenta);
                    }

                    painter.setPen(messageColor[msg->type]);
                    painter.drawText(textRect,
                            Qt::AlignLeft | Qt::AlignVCenter, label);
                    feed[i] = xc + MSG_ROW_HEIGHT / 2 + 2 + bound.width() + 2;
                }
                break;
            }
        }

        msgMutex.unlock();

        for (int i = 0; i < messageWidth; i++) {
            if (types[i] == LibDLS::Job::Message::Unknown) {
                continue;
            }

            painter.setPen(messageColor[types[i]]);
            painter.drawLine(rect.left() + i, rect.top(),
                    rect.left() + i, rect.top() + MSG_LINES_HEIGHT);
        }
    }

    if (feed) {
        delete [] feed;
    }
}

/****************************************************************************/

void Graph::staticLoggingCallback(const char *msg, void *data)
{
    Graph *g = (Graph *) data;
    g->loggingCallback(msg);
}

/****************************************************************************/

void Graph::loggingCallback(const char *msg)
{
    loggingMutex.lock();
    emit logMessage(QString(msg));
    loggingMutex.unlock();
}

/****************************************************************************/

#ifdef DEBUG_MT_IN_FILE
static int counter = 0;
#endif

void Graph::updateTouch(QTouchEvent *event)
{
    int count = event->touchPoints().count();

#ifdef DEBUG_MT_ON_SCREEN
    int displayCount = count;
    if (event->type() == QEvent::TouchEnd
#if QT_VERSION >= 0x050000
            || event->type() == QEvent::TouchCancel
#endif
       ) {
        displayCount = 0;
    }
    debugString = QString("%1 TouchPoints:\n").arg(displayCount);
    for (int i = 0; i < displayCount; i++) {
        QTouchEvent::TouchPoint tp = event->touchPoints()[i];
        debugString += QString("%1: %2x%3 - %4x%5\n").arg(i)
            .arg(tp.startPos().x()).arg(tp.startPos().y())
            .arg(tp.pos().x()).arg(tp.pos().y());
    }
    update();
#endif

#ifdef DEBUG_MT_IN_FILE
    QString type;
    switch (event->type()) {
        case QEvent::TouchBegin:
            type = "TouchBegin";
            break;
        case QEvent::TouchUpdate:
            type = "TouchUpdate";
            break;
        case QEvent::TouchEnd:
            type = "TouchEnd";
            break;
#if QT_VERSION >= 0x050000
        case QEvent::TouchCancel:
            type = "TouchCancel";
            break;
#endif
        default:
            type = "???";
            break;
    }
    QString dbg = QString("\n%1: %2 count=%3\n")
        .arg(counter++).arg(type).arg(count);
    debugFile.write(dbg.toUtf8().constData());
    debugFile.flush();

    dbg = QString("touchZooming = %1\n").arg(touchZooming);
    for (int i = 0; i < event->touchPoints().count(); i++) {
        QTouchEvent::TouchPoint tp = event->touchPoints()[i];
        dbg += QString("%1 %2x%3 - %4x%5\n").arg(i)
            .arg(tp.startPos().x()).arg(tp.startPos().y())
            .arg(tp.pos().x()).arg(tp.pos().y());
    }
    debugFile.write(dbg.toUtf8().constData());
    debugFile.flush();
#endif

    switch (event->type()) {
        case QEvent::TouchBegin:
            if (count == 1) {
                QTouchEvent::TouchPoint tp0 = event->touchPoints()[0];

                rwLockSections.lockForRead();

                QPoint tp0Pos(tp0.pos().toPoint());
                Section *sec = splitterSectionFromPos(tp0Pos);
                if (sec) {
                    movingSection = sec;
                    startPos = tp0Pos;
                    startHeight = movingSection->getHeight();
                    rwLockSections.unlock();
                    event->accept();
                    updateCursor();
                    update();
                }
                else {
                    rwLockSections.unlock();

                    if (touchPanStart(tp0.pos().toPoint())) {
                        event->accept();
                    }
                }
            }
            else if (count == 2) {
                QTouchEvent::TouchPoint tp0 = event->touchPoints()[0];
                QTouchEvent::TouchPoint tp1 = event->touchPoints()[1];
                touchZoomStart(tp0.pos().x(), tp1.pos().x());
                event->accept();
            }
            break;

        case QEvent::TouchUpdate:
            event->accept();
            if (count == 1) {
                if (touchZooming) {
                    touchZooming = false;
                }
                QTouchEvent::TouchPoint tp0 = event->touchPoints()[0];

                rwLockSections.lockForRead();

                if (movingSection) {
                    int dh = tp0.pos().y() - startPos.y();
                    int h = startHeight + dh;
                    if (h < 0) {
                        h = 0;
                    }
                    movingSection->setHeight(h);
                    rwLockSections.unlock();
                    updateScrollBar();
                }
                else {
                    rwLockSections.unlock();

                    if (touchPanning) {
                        touchPanUpdate(tp0);
                    }
                    else {
                        touchPanStart(tp0.pos().toPoint());
                    }
                }
            }
            else if (count >= 2) {
                QTouchEvent::TouchPoint tp0 = event->touchPoints()[0];
                QTouchEvent::TouchPoint tp1 = event->touchPoints()[1];
                if (touchZooming) {
                    touchZoomUpdate(tp0.pos().x(), tp1.pos().x());
                }
                else {
                    touchZoomStart(tp0.pos().x(), tp1.pos().x());
                }
            }
            break;

        case QEvent::TouchEnd:
#if QT_VERSION >= 0x050000
        case QEvent::TouchCancel:
#endif
            event->accept();
            movingSection = NULL;
            if (touchPanning) {
                touchPanning = false;
                newView();
                loadData();
            }
            if (touchZooming) {
                touchZooming = false;
                newView();
                loadData();
            }

        default:
            break;
    }
}

/****************************************************************************/

bool Graph::touchPanStart(const QPoint &pos)
{
    QRect panRect(contentsRect());
    if (scrollBarNeeded) {
        panRect.setWidth(contentsRect().width() - scrollBar.width());
    }
    if (!panRect.contains(pos)) {
        return false;
    }

    startPos = pos;
    touchX0 = pos.x(); // FIXME unused; kept for binary compatibility
    touchPanning = true;
    panning = false;
    return true;
}

/****************************************************************************/

void Graph::touchPanUpdate(const QTouchEvent::TouchPoint &tp)
{
    int dataWidth = getDataWidth();
    LibDLS::Time range = getEnd() - getStart();

    if (scrollBarNeeded) {
        int dy = tp.pos().y() - tp.lastPos().y();
        scrollBar.setValue(scrollBar.value() - dy);
    }

    if (range > 0.0 && dataWidth > 0) {
        double xScale = range.to_dbl_time() / dataWidth;

        LibDLS::Time diff;
        diff.from_dbl_time((tp.pos().x() - touchX0) * xScale);
        touchX0 = tp.pos().x();
        scale.setRange(getStart() - diff, getEnd() - diff);
        autoRange = false;
        updateActions();
        update();
    }
}

/****************************************************************************/

void Graph::touchZoomStart(int x0, int x1)
{
    LibDLS::Time range = getEnd() - getStart();
    int width = x1 - x0;

    if (range <= 0.0 || width == 0) {
        return;
    }

    double xScale = range.to_dbl_time() / getDataWidth();
    int offset = contentsRect().left() + scaleWidth;
    LibDLS::Time d0, d1;
    d0.from_dbl_time((x0 - offset) * xScale);
    d1.from_dbl_time((x1 - offset) * xScale);
    if (d0 < d1) {
        touchT0 = getStart() + d0;
        touchT1 = getStart() + d1;
    }
    else {
        touchT0 = getStart() + d1;
        touchT1 = getStart() + d0;
    }
    touchZooming = true;
    touchPanning = false;
    panning = false;
    zooming = false;
    updateCursor();

#ifdef DEBUG_MT_IN_FILE
    QString dbg = QString("zooming1 %1 - %2\n")
        .arg(touchT0.to_real_time().c_str())
        .arg(touchT1.to_real_time().c_str());
    LibDLS::Time xrange = touchT1 - touchT0;
    dbg += QString("range=%1 width=%2 scale=%3\n")
        .arg(xrange.to_dbl_time())
        .arg(width)
        .arg(xScale);
    debugFile.write(dbg.toUtf8().constData());
    debugFile.flush();
#endif
}

/****************************************************************************/

void Graph::touchZoomUpdate(int x0, int x1)
{
    LibDLS::Time range = touchT1 - touchT0;
    int width = x1 - x0;

#ifdef DEBUG_MT_IN_FILE
    QString dbg = QString("range=%1 width=%2\n")
        .arg(range.to_dbl_time())
        .arg(width);
    debugFile.write(dbg.toUtf8().constData());
    debugFile.flush();
#endif

    if (width == 0 || range <= 0.0) {
        return;
    }

    if (width < 0) {
        width *= -1;
        x0 = x1; // x1 is not used later on
    }

    double newScale = range.to_dbl_time() / width;
    int offset = contentsRect().left() + scaleWidth;
    LibDLS::Time diff;
    diff.from_dbl_time((x0 - offset) * newScale);
    LibDLS::Time start = touchT0 - diff;
    diff.from_dbl_time(getDataWidth() * newScale);
    LibDLS::Time end = start + diff;
    scale.setRange(start, end);
    autoRange = false;
    update();

#ifdef DEBUG_MT_IN_FILE
    dbg = QString("update scale=%3 %4 - %5\n   %1 - %2\n")
        .arg(start.to_real_time().c_str())
        .arg(end.to_real_time().c_str())
        .arg(newScale)
        .arg(touchT0.to_real_time().c_str())
        .arg(touchT1.to_real_time().c_str());
    debugFile.write(dbg.toUtf8().constData());
    debugFile.flush();
#endif
}

/****************************************************************************/

int Graph::getDataWidth() const
{
    int dataWidth = contentsRect().width() - scaleWidth;

    if (scrollBarNeeded) {
        dataWidth -= scrollBar.width();
    }

    return dataWidth;
}

/****************************************************************************/

QSet<QtDls::Channel *> Graph::displayedChannels()
{
    QSet<QtDls::Channel *> channels;

    rwLockSections.lockForRead();

    for (QList<Section *>::const_iterator s = sections.begin();
            s != sections.end(); s++) {
        channels += (*s)->channels();
    }

    rwLockSections.unlock();

    return channels;
}

/****************************************************************************/

QList<Section *>::const_iterator Graph::lastSectionOnPage(
        QList<Section *>::const_iterator first,
        int displayHeight
        ) const
{
    QList<Section *>::const_iterator secIter = first;
    double heightSum = (*first)->relativeHeight(displayHeight);
    while (secIter != sections.end()) {
        QList<Section *>::const_iterator next = secIter + 1;
        if (next == sections.end()) {
            break;
        }
        double relHeight = (*next)->relativeHeight(displayHeight);
        if (heightSum + relHeight > 1.0) {
            break;
        }

        heightSum += relHeight;
        secIter = next;
    }

    return secIter;
}

/****************************************************************************/

int Graph::renderCommon(
        QPainter &painter,
        const QRect &rect
        ) const
{
    QRect timeScaleRect(rect);
    // FIXME scaleWidth taken from display!
    timeScaleRect.setLeft(rect.left() + scaleWidth);

    Scale printScale(this);
    printScale.setRange(scale.getStart(), scale.getEnd());
    printScale.setLength(timeScaleRect.width());
    printScale.draw(painter, timeScaleRect);
    int displayHeight = rect.height() - printScale.getOuterLength() - 1;

    // Horizontal line above top section
    QPen horLinePen;
    painter.setPen(horLinePen);
    painter.drawLine(rect.left(),
            rect.top() + printScale.getOuterLength(),
            rect.right(),
            rect.top() + printScale.getOuterLength());

    return displayHeight;
}

/****************************************************************************/

void Graph::renderSections(
        QPainter &painter,
        const QRect &rect,
        QList<Section *>::const_iterator first,
        QList<Section *>::const_iterator last,
        int displayHeight,
        RenderFlags flags
        )
{
    QList<Section *>::const_iterator secIter = first;
    double heightSum = (*first)->relativeHeight(displayHeight);
    unsigned int count = 1;
    while (secIter != last) {
        secIter++;
        heightSum += (*secIter)->relativeHeight(displayHeight);
        count++;
    }

    if (heightSum <= 0) {
        return;
    }

    int dataWidth = rect.width() - scaleWidth;
    LibDLS::Time range = getEnd() - getStart();
    int measurePos = -1;
    if ((flags & MeasuringLine) && !measureTime.is_null() && dataWidth > 0
            && measureTime >= getStart() && measureTime < getEnd()
            && range > 0.0) {
        double xScale = dataWidth / range.to_dbl_time();
        measurePos =
            (measureTime - getStart()).to_dbl_time() * xScale + 0.5;
    }

    std::set<LibDLS::Job *> jobSet;
    int top = rect.bottom() - displayHeight + 1;
    QRect dataRect(rect);
    dataRect.setTop(top);

    for (QList<Section *>::const_iterator s = first; s != last + 1; s++) {
        int height;
        if (s == last) {
            // use remaining height
            height = dataRect.bottom() - top;
        }
        else {
            height = (*s)->relativeHeight(displayHeight) *
                (dataRect.height() - count - 1) / heightSum;
        }
        QRect r(rect.left(), top, rect.width(), height);

        Section drawSection(**s);
        drawSection.setHeight(height);
        drawSection.resize(rect.width());
        drawSection.loadData(scale.getStart(), scale.getEnd(),
                dataWidth, &worker, jobSet);
        drawSection.draw(painter, r, measurePos, scaleWidth, false);

        QPen pen;
        painter.setPen(pen);
        painter.drawLine(rect.left(), top + height,
                rect.right(), top + height);

        top += height + 1;
    }

    if (measurePos != -1) {
        int xp = rect.left() + scaleWidth + measurePos;
        QPen pen;
        pen.setColor(Qt::darkBlue);
        painter.setPen(pen);

        painter.drawLine(xp, rect.top(), xp, rect.bottom());

        QRect textRect(rect);
        textRect.setLeft(xp + 3);
        textRect.setTop(rect.top() + 2);
        textRect.setHeight(rect.height() - 4);
        QString label(measureTime.to_real_time().c_str());
        QFontMetrics fm(painter.font());
        QSize s = fm.size(0, label);
        if (s.width() <= textRect.width()) {
            painter.fillRect(
                    QRect(textRect.topLeft(), s).adjusted(-2, 0, 2, 0),
                    Qt::white);
            painter.drawText(textRect, Qt::AlignLeft, label);
        }
        else {
            textRect.setLeft(rect.left());
            textRect.setRight(xp - 3);
            if (s.width() <= textRect.width()) {
                painter.fillRect(QRect(
                            QPoint(textRect.right() + 1 - s.width(),
                                textRect.top()), s).adjusted(-2, 0, 2, 0),
                        Qt::white);
                painter.drawText(textRect, Qt::AlignRight, label);
            }
        }
    }
}

/*****************************************************************************
 * private slots
 ****************************************************************************/

void Graph::interactionSlot()
{
    if (sender() == &zoomAction) {
        setInteraction(Zoom);
    }
    else if (sender() == &panAction) {
        setInteraction(Pan);
    }
    else if (sender() == &measureAction) {
        setInteraction(Measure);
    }
}

/****************************************************************************/

void Graph::removeSelectedSection()
{
    if (!selectedSection) {
        return;
    }

    removeSection(selectedSection);
    selectedSection = NULL;
}

/****************************************************************************/

void Graph::sectionProperties()
{
    if (!selectedSection) {
        return;
    }

    // FIXME section locking!

    SectionDialog *dialog = new SectionDialog(selectedSection, this);
    dialog->exec();
    delete dialog;
    selectedSection = NULL;
}

/****************************************************************************/

void Graph::sliderValueChanged(int value)
{
    Q_UNUSED(value);
    update();
}

/****************************************************************************/

void Graph::pickDate()
{
    DatePickerDialog *dialog = new DatePickerDialog(this);
    int result = dialog->exec();
    if (result == QDialog::Accepted) {
        setRange(dialog->getStart(), dialog->getEnd());
    }
    delete dialog;
}

/****************************************************************************/

void Graph::gotoDate()
{
    if (sender() == &gotoTodayAction) {
        setNamedRange(Today);
    }
    else if (sender() == &gotoYesterdayAction) {
        setNamedRange(Yesterday);
    }
    else if (sender() == &gotoThisWeekAction) {
        setNamedRange(ThisWeek);
    }
    else if (sender() == &gotoLastWeekAction) {
        setNamedRange(LastWeek);
    }
    else if (sender() == &gotoThisMonthAction) {
        setNamedRange(ThisMonth);
    }
    else if (sender() == &gotoLastMonthAction) {
        setNamedRange(LastMonth);
    }
    else if (sender() == &gotoThisYearAction) {
        setNamedRange(ThisYear);
    }
    else if (sender() == &gotoLastYearAction) {
        setNamedRange(LastYear);
    }
}

/****************************************************************************/

void Graph::workerFinished()
{
    update();

    if (reloadPending) {
        reloadPending = false;
        worker.width = pendingWidth;
        QMetaObject::invokeMethod(&worker, "doWork", Qt::QueuedConnection);
    }
    else {
        workerBusy = false;
    }
}

/****************************************************************************/

void Graph::updateSection(Section *section)
{
    bool updated = false;

    // look up section in the list: it could be deleted in the meantime!

    rwLockSections.lockForRead();

    if (sections.contains(section)) {
        section->update();
        updated = true;
    }

    rwLockSections.unlock();

    if (updated) {
        updateRange();
    }
    else {
        qWarning() << "Section" << (void *) section << "deleted.";
    }
}

/****************************************************************************/

void Graph::showMessagesChanged()
{
    setShowMessages(messagesAction.isChecked());
}

/****************************************************************************/

void Graph::filterTriggered()
{
    FilterDialog *dialog = new FilterDialog(this, messageFilter);
    int ret = dialog->exec();
    if (ret == QDialog::Accepted) {
        setMessageFilter(dialog->getRegex());
    }
    delete dialog;
}

/****************************************************************************/

void Graph::showExport()
{
    // FIXME channels() race
    ExportDialog *dialog =
        new ExportDialog(this, &thread, displayedChannels());
    dialog->exec();
    delete dialog;
}

/****************************************************************************/

void Graph::fixMeasuringLine()
{
    LibDLS::Time range = getEnd() - getStart();
    QRect measureRect(contentsRect());
    measureRect.setLeft(contentsRect().left() + scaleWidth);
    int dataWidth = getDataWidth();

    if (range > 0.0 && measureRect.isValid()
            && measureRect.contains(endPos) && dataWidth > 0) {

        if (interaction == Measure) {
            setInteraction(Pan);
        }

        double xScale = range.to_dbl_time() / dataWidth;
        int measurePos = endPos.x() - measureRect.left();
        measureTime.from_dbl_time(measurePos * xScale);
        measureTime += getStart();

        update();
    }
}

/****************************************************************************/

void Graph::removeMeasuringLine()
{
    measureTime.set_null();
    update();
}

/****************************************************************************/

GraphWorker::GraphWorker(Graph *graph):
    graph(graph)
{
    moveToThread(&graph->thread);
}

/****************************************************************************/

GraphWorker::~GraphWorker()
{
    clearData();
}

/****************************************************************************/

void GraphWorker::clearData()
{
    clearDataList(genericData);
    clearDataList(minimumData);
    clearDataList(maximumData);
    messages.clear();
}

/****************************************************************************/

void GraphWorker::doWork()
{
    std::set<LibDLS::Job *> jobSet;

    messages.clear();

    graph->rwLockSections.lockForRead();

    for (QList<Section *>::iterator s = graph->sections.begin();
            s != graph->sections.end(); s++) {
        (*s)->loadData(graph->scale.getStart(), graph->scale.getEnd(),
                width, this, jobSet);

        if (!graph->reloadPending) {
            (*s)->setBusy(false);
        }

        emit notifySection(*s);
    }

    graph->rwLockSections.unlock();

    // get system language
    QString lang = QLocale::system().name().left(2).toLower();
    if (lang == "c") {
        lang = "en";
    }

    for (std::set<LibDLS::Job *>::const_iterator job = jobSet.begin();
            job != jobSet.end(); job++) {
        std::list<LibDLS::Job::Message> msgs =
            (*job)->load_msg_filtered(graph->getStart(), graph->getEnd(),
                    graph->getMessageFilter().toUtf8().constData(),
                    lang.toLocal8Bit().constData());
        for (std::list<LibDLS::Job::Message>::const_iterator msg =
                msgs.begin(); msg != msgs.end(); msg++) {
            messages.append(*msg);
        }
    }

    qStableSort(messages);
    graph->msgMutex.lock();
    graph->messages = messages;
    graph->msgMutex.unlock();

    emit finished();
}

/****************************************************************************/

int GraphWorker::dataCallback(LibDLS::Data *data, void *cb_data)
{
    GraphWorker *worker = (GraphWorker *) cb_data;
    worker->newData(data);
    return 1; // adopt object
}

/****************************************************************************/

void GraphWorker::newData(LibDLS::Data *data)
{
    switch (data->meta_type()) {
        case LibDLS::MetaGen:
            genericData.push_back(data);
            break;
        case LibDLS::MetaMin:
            minimumData.push_back(data);
            break;
        case LibDLS::MetaMax:
            maximumData.push_back(data);
            break;
        default:
            break;
    }
}

/****************************************************************************/

void GraphWorker::clearDataList(QList<LibDLS::Data *> &list)
{
    for (QList<LibDLS::Data *>::iterator d = list.begin();
            d != list.end(); d++) {
        delete *d;
    }

    list.clear();
}

/****************************************************************************/
