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

#include "DialPlugin.h"
#include "WidgetContainer.h"
#include "DataSlot.h"
#include "SlotModel.h"
#include "DataModel.h"
#include "DataNode.h"

#include <QtPdWidgets/Dial.h>

#include <QJsonObject>

/****************************************************************************/

QString DialPlugin::name() const
{
    return tr("Dial / Analog Gauge");
}

/****************************************************************************/

QIcon DialPlugin::icon() const
{
    return QIcon(":/images/plugin-dial.svg");
}

/****************************************************************************/

int DialPlugin::getFreeSlots(const SlotModel &slotModel) const
{
    int ret = 2 - slotModel.getChildCount();
    if (ret < 0) {
        ret = 0;
    }
    return ret;
}

/****************************************************************************/

void DialPlugin::appendVariable(QWidget *widget, SlotModel *slotModel,
        DataNode *dataNode) const
{
    Pd::Dial *dial = dynamic_cast<Pd::Dial *>(widget);
    if (!dial) {
        qWarning() << __func__ << "failed to cast";
        return;
    }

    if (slotModel->getChildCount() == 0) { // FIXME
        // append as display variable
        SlotNode *node = new SlotNode(slotModel, tr("Display"));
        DataSlot *slot = new DataSlot(node);
        slot->url = dataNode->nodeUrl();
        slot->period = 0.1;

        PdCom::Variable *pv(dataNode->getVariable());
        if (pv) {
            dial->currentValue.setVariable(pv, slot->period,
                    slot->scale, slot->offset, slot->tau);
        }
    }
    else {
        // append as setpoint variable
        SlotNode *node = new SlotNode(slotModel, tr("Setpoint"));
        DataSlot *slot = new DataSlot(node);
        slot->url = dataNode->nodeUrl();

        PdCom::Variable *pv(dataNode->getVariable());
        if (pv) {
            dial->setpointValue.setVariable(pv, slot->period,
                    slot->scale, slot->offset, slot->tau);
        }
    }
}

/****************************************************************************/

void DialPlugin::connectDataSlots(QWidget *widget,
        const SlotModel *slotModel, const DataModel *dataModel) const
{
    Pd::Dial *dial = dynamic_cast<Pd::Dial *>(widget);
    if (!dial) {
        qWarning() << __func__ << "failed to cast";
        return;
    }

    if (slotModel->getChildCount() > 0) { // display
        SlotNode *node = slotModel->getChildNode(0);
        if (node->getChildCount() > 0) {
            DataSlot *slot =
                dynamic_cast<DataSlot *>(node->getChildNode(0));
            if (slot) {
                DataNode *dataNode(dataModel->findDataNode(slot->url));
                if (dataNode) {
                    PdCom::Variable *pv(dataNode->getVariable());
                    if (pv) {
                        dial->currentValue.setVariable(pv, slot->period,
                                slot->scale, slot->offset, slot->tau);
                    }
                }
                else {
                    qWarning()
                        << tr("Url not found: %1").arg(slot->url.toString());
                }
            }
        }
    }

    if (slotModel->getChildCount() > 1) { // setpoint
        SlotNode *node = slotModel->getChildNode(1);
        if (node->getChildCount() > 0) {
            DataSlot *slot =
                dynamic_cast<DataSlot *>(node->getChildNode(0));
            if (slot) {
                DataNode *dataNode(dataModel->findDataNode(slot->url));
                if (dataNode) {
                    PdCom::Variable *pv(dataNode->getVariable());
                    if (pv) {
                        dial->setpointValue.setVariable(pv, slot->period,
                                slot->scale, slot->offset, slot->tau);
                    }
                }
                else {
                    qWarning()
                        << tr("Url not found: %1").arg(slot->url.toString());
                }
            }
        }
    }
}

/****************************************************************************/

void DialPlugin::clearVariables(QWidget *widget) const
{
    Pd::Dial *dial = dynamic_cast<Pd::Dial *>(widget);
    if (dial) {
        dial->currentValue.clearVariable();
        dial->setpointValue.clearVariable();
    }
}

/****************************************************************************/

QWidget *DialPlugin::createWidget(QWidget *parent) const
{
    return new Pd::Dial(parent);
}

/****************************************************************************/
