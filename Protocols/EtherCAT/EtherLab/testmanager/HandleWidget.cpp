/*****************************************************************************
 *
 * Testmanager - Graphical Automation and Visualisation Tool
 *
 * Copyright (C) 2018  Florian Pose <fp@igh.de>
 *
 * This file is part of Testmanager.
 *
 * Testmanager is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Testmanager is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with Testmanager. If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

#include "HandleWidget.h"

#include "Plugin.h"
#include "MainWindow.h"
#include "DataModel.h"
#include "WidgetContainer.h"

#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QMimeData>
#include <QPainter>
#include <QDebug>

/****************************************************************************/

HandleWidget::HandleWidget(
        WidgetContainer *parent
        ):
    QWidget(parent),
    parent(parent),
    resizeDir(None),
    move(false),
    referenceContainer(parent),
    duplicateOnMove(false),
    highlight(Standard)
{
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    setMouseTracking(true);

    for (int i = 0; i < NumHandles; i++) {
        handle[i].setSize(QSize(5, 5));
    }

    area[0].setWidth(10); // left
    area[1].setHeight(10); // top
    area[2].setWidth(10); // right
    area[3].setHeight(10); // bottom
}

/****************************************************************************/

void HandleWidget::setHighlight(Highlight hl)
{
    if (highlight == hl) {
        return;
    }

    highlight = hl;
    update();
}

/****************************************************************************/

void HandleWidget::resizeEvent(QResizeEvent *)
{
    // top right
    handle[1].moveRight(contentsRect().right());

    // bottom left
    handle[2].moveBottom(contentsRect().bottom());

    // bottom right
    handle[3].moveRight(contentsRect().right());
    handle[3].moveBottom(contentsRect().bottom());

    // left, right
    area[0].setHeight(contentsRect().height());
    area[2].setHeight(contentsRect().height());
    area[2].moveRight(contentsRect().right());

    // top, bottom
    area[1].setWidth(contentsRect().width());
    area[3].setWidth(contentsRect().width());
    area[3].moveBottom(contentsRect().bottom());
}

/****************************************************************************/

void HandleWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QColor fillColor;

    switch (highlight) {
        default:
        case Standard:
            fillColor = Qt::darkBlue;
            break;
        case DropAccepted:
            fillColor = Qt::green;
            break;
        case DropDenied:
            fillColor = Qt::red;
            break;
    }
    fillColor.setAlpha(16);
    painter.fillRect(contentsRect(), fillColor);

    if (highlight == DropDenied) {
        painter.fillRect(contentsRect(),
                QBrush(Qt::red, Qt::BDiagPattern));
    }

    if (parent->isSelected()) {
        for (int i = 0; i < NumHandles; i++) {
            if (event->region().intersects(handle[i])) {
                painter.fillRect(handle[i], Qt::darkBlue);
            }
        }
    }

    if (resizeDir) {
        QString text(QString("%1 × %2").arg(width()).arg(height()));
        QFontMetrics fm(painter.fontMetrics());
        QRect bound(fm.boundingRect(contentsRect(), Qt::AlignCenter, text));
        QColor bgColor(255, 255, 255, 200);
        painter.fillRect(bound.adjusted(-2, -2, 2, 2), bgColor);
        painter.drawText(contentsRect(), Qt::AlignCenter, text);
    }
}

/****************************************************************************/

void HandleWidget::mouseMoveEvent(QMouseEvent *event)
{
    bool over[NumHandles];
    for (int i = 0; i < NumHandles; i++) {
        over[i] = area[i].contains(event->pos());
    }

    if (!resizeDir and !move) {
        if (over[0] && !over[1] && !over[3]) {
            setCursor(Qt::SizeHorCursor);
        }
        else if (over[1] && !over[2] && !over[0]) {
            setCursor(Qt::SizeVerCursor);
        }
        else if (over[2] && !over[3] && !over[1]) {
            setCursor(Qt::SizeHorCursor);
        }
        else if (over[3] && !over[0] && !over[2]) {
            setCursor(Qt::SizeVerCursor);
        }
        else if (over[0] && over[1]) {
            setCursor(Qt::SizeFDiagCursor);
        }
        else if (over[1] && over[2]) {
            setCursor(Qt::SizeBDiagCursor);
        }
        else if (over[2] && over[3]) {
            setCursor(Qt::SizeFDiagCursor);
        }
        else if (over[3] && over[0]) {
            setCursor(Qt::SizeBDiagCursor);
        }
        else {
            setCursor(Qt::SizeAllCursor);
        }
    }
    else { // resizing or moving

        int gridStep(parent->getMainWindow()->getGridStep());
        QPoint absDiff(event->globalPos() - startGlobalPos);

        QPoint absTopLeft(startRect.topLeft() + absDiff);
        QPoint targetTopLeft;
        if (event->modifiers() & Qt::ShiftModifier) {
            targetTopLeft = absTopLeft;
        }
        else {
            // snap top/left to grid
            targetTopLeft = QPoint(
                    qRound((double) absTopLeft.x() / gridStep) * gridStep,
                    qRound((double) absTopLeft.y() / gridStep) * gridStep
                    );
        }
        QPoint topLeftDiff(targetTopLeft -
                referenceContainer->geometry().topLeft());

        QPoint absBottomRight(startRect.bottomRight() + absDiff);
        QPoint targetBottomRight;
        if (event->modifiers() & Qt::ShiftModifier) {
            targetBottomRight = absBottomRight;
        }
        else {
            // snap bottom/right to grid
            targetBottomRight = QPoint(
                    qRound((double) absBottomRight.x() / gridStep) * gridStep,
                    qRound((double) absBottomRight.y() / gridStep) * gridStep
                    );
        }
        QPoint bottomRightDiff(targetBottomRight -
                referenceContainer->geometry().bottomRight());

#if 0
        if (move || resizeDir) {
            qDebug() << absDiff << startRect;
            qDebug() << "tl" << absTopLeft << targetTopLeft << topLeftDiff;
            qDebug() << "br" << absBottomRight << targetBottomRight
                << bottomRightDiff;
        }
#endif

        if (resizeDir & Left) {
            parent->adjustLeft(topLeftDiff.x());
        }

        if (resizeDir & Top) {
            parent->adjustTop(topLeftDiff.y());
        }

        if (resizeDir & Right) {
            parent->adjustWidth(bottomRightDiff.x());
        }

        if (resizeDir & Bottom) {
            parent->adjustHeight(bottomRightDiff.y());
        }

        if (move) {
            if (duplicateOnMove) {
                duplicateOnMove = false;
                referenceContainer = parent->duplicateSelected();
            }
            parent->moveSelected(topLeftDiff);
        }
    }
}

/****************************************************************************/

void HandleWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        bool shift = event->modifiers() & Qt::ShiftModifier;
        duplicateOnMove = event->modifiers() & Qt::ControlModifier;

        if (parent->isSelected()) {
            if (shift) {
                parent->deselect();
            }
        }
        else {
            if (!shift) {
                parent->deselectAll();
            }
            parent->select();
        }

        resizeDir = None;

        for (int i = 0; i < NumHandles; i++) {
            if (area[i].contains(event->pos())) {
                resizeDir |= (1 << i);
            }
        }

        if (!resizeDir) {
            move = true;
        }

        startGlobalPos = event->globalPos();
        referenceContainer = parent;
        startRect = referenceContainer->geometry();
        update();
    }
}

/****************************************************************************/

void HandleWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        resizeDir = None;
        move = false;
        update();
    }
}

/****************************************************************************/
