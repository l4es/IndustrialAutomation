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

#include "XYGraphPlugin.h"

#include "WidgetContainer.h"
#include "DataSlot.h"
#include "DataModel.h"
#include "DataNode.h"

#include <QtPdWidgets/XYGraph.h>

#include <QJsonObject>
#include <QDebug>

/****************************************************************************/

QString XYGraphPlugin::name() const
{
    return tr("XY Graph");
}

/****************************************************************************/

QIcon XYGraphPlugin::icon() const
{
    return QIcon(":/images/plugin-xygraph.svg");
}

/****************************************************************************/

int XYGraphPlugin::getFreeSlots(const SlotModel &) const
{
    return -1; // unlimited
}

/****************************************************************************/

void XYGraphPlugin::appendVariable(QWidget *widget, SlotModel *slotModel,
        DataNode *dataNode) const
{
    Pd::XYGraph *graph = dynamic_cast<Pd::XYGraph *>(widget);
    if (!graph) {
        qWarning() << __func__ << "failed to cast";
        return;
    }

    DataSlot *slot = new DataSlot(slotModel);
    slot->url = dataNode->nodeUrl();
    slot->period = 0.1;

    PdCom::Variable *pv(dataNode->getVariable());
    if (pv) {
        graph->addVariable(pv, slot->period, slot->scale, slot->offset);
    }
}

/****************************************************************************/

void XYGraphPlugin::connectDataSlots(QWidget *widget,
        const SlotModel *slotModel, const DataModel *dataModel) const
{
    Pd::XYGraph *graph = dynamic_cast<Pd::XYGraph *>(widget);
    if (!graph) {
        qWarning() << __func__ << "failed to cast";
        return;
    }

    for (int i = 0; i < slotModel->getChildCount(); i++) {
        DataSlot *slot(dynamic_cast<DataSlot *>(slotModel->getChildNode(i)));
        if (!slot) {
            continue;
        }

        DataNode *dataNode(dataModel->findDataNode(slot->url));
        if (dataNode) {
            PdCom::Variable *pv(dataNode->getVariable());
            if (pv) {
                graph->addVariable(pv, slot->period, slot->scale,
                        slot->offset);
            }
        }
        else {
            qWarning()
                << tr("Url not found: %1").arg(slot->url.toString());
        }
    }
}

/****************************************************************************/

void XYGraphPlugin::clearVariables(QWidget *widget) const
{
    Pd::XYGraph *graph = dynamic_cast<Pd::XYGraph *>(widget);
    graph->clearVariables();
}

/****************************************************************************/

QWidget *XYGraphPlugin::createWidget(QWidget *parent) const
{
    return new Pd::XYGraph(parent);
}

/****************************************************************************/
