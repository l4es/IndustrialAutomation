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

#include "GraphPlugin.h"
#include "WidgetContainer.h"
#include "DataSlot.h"
#include "DataModel.h"
#include "DataNode.h"

#include <QtPdWidgets/Graph.h>

#include <QJsonObject>

/****************************************************************************/

QString GraphPlugin::name() const
{
    return tr("Graph");
}

/****************************************************************************/

QIcon GraphPlugin::icon() const
{
    return QIcon(":/images/plugin-graph.svg");
}

/****************************************************************************/

int GraphPlugin::getFreeSlots(const SlotModel &) const
{
    return -1; // unlimited
}

/****************************************************************************/

void GraphPlugin::appendVariable(QWidget *widget, SlotModel *slotModel,
        DataNode *dataNode) const
{
    Pd::Graph *graph = dynamic_cast<Pd::Graph *>(widget);
    if (!graph) {
        qWarning() << __func__ << "failed to cast";
        return;
    }

    DataSlot *slot = new DataSlot(slotModel);
    slot->url = dataNode->nodeUrl();
    slot->period = 0.1;
    slot->color = slotModel->nextColor();

    PdCom::Variable *pv(dataNode->getVariable());
    if (pv) {
        graph->addVariable(pv, slot->period, slot->scale, slot->offset,
                slot->tau, slot->color);
    }
}

/****************************************************************************/

void GraphPlugin::connectDataSlots(QWidget *widget,
        const SlotModel *slotModel, const DataModel *dataModel) const
{
    Pd::Graph *graph = dynamic_cast<Pd::Graph *>(widget);
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
                        slot->offset, slot->tau, slot->color);
            }
        }
        else {
            qWarning()
                << tr("Url not found: %1").arg(slot->url.toString());
        }
    }
}

/****************************************************************************/

void GraphPlugin::clearVariables(QWidget *widget) const
{
    Pd::Graph *graph = dynamic_cast<Pd::Graph *>(widget);
    graph->clearVariables();
}

/****************************************************************************/

QWidget *GraphPlugin::createWidget(QWidget *parent) const
{
    return new Pd::Graph(parent);
}

/****************************************************************************/
