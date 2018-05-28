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

#include "DigitalPlugin.h"
#include "WidgetContainer.h"
#include "DataSlot.h"
#include "DataModel.h"
#include "DataNode.h"

#include <QtPdWidgets/Digital.h>

#include <QJsonObject>

/****************************************************************************/

void DigitalPlugin::initProperties(WidgetContainer *container) const
{
    container->setProperty("styleSheet", "background-color: #cccccc;");
}

/****************************************************************************/

QString DigitalPlugin::name() const
{
    return tr("Digital Display");
}

/****************************************************************************/

QIcon DigitalPlugin::icon() const
{
    return QIcon(":/images/plugin-digital.svg");
}

/****************************************************************************/

int DigitalPlugin::getFreeSlots(const SlotModel &slotModel) const
{
    int used = slotModel.getChildCount();
    return !used ? 1 : 0;
}

/****************************************************************************/

void DigitalPlugin::appendVariable(QWidget *widget, SlotModel *slotModel,
        DataNode *dataNode) const
{
    Pd::Digital *digital = dynamic_cast<Pd::Digital *>(widget);
    if (!digital) {
        qWarning() << __func__ << "failed to cast";
        return;
    }

    DataSlot *slot = new DataSlot(slotModel);
    slot->url = dataNode->nodeUrl();

    if (dataNode->getPeriod() > 0.0) {
        slot->period = 0.2;
    }

    PdCom::Variable *pv(dataNode->getVariable());
    if (pv) {
        digital->setVariable(pv, slot->period, slot->scale, slot->offset,
                slot->tau);
    }
}

/****************************************************************************/

void DigitalPlugin::connectDataSlots(QWidget *widget,
        const SlotModel *slotModel, const DataModel *dataModel) const
{
    Pd::Digital *digital = dynamic_cast<Pd::Digital *>(widget);
    if (!digital) {
        qWarning() << __func__ << "failed to cast";
        return;
    }

    if (!slotModel->getChildCount()) {
        return;
    }

    DataSlot *slot(dynamic_cast<DataSlot *>(slotModel->getChildNode(0)));
    if (!slot) {
        return;
    }

    DataNode *dataNode(dataModel->findDataNode(slot->url));
    if (dataNode) {
        PdCom::Variable *pv(dataNode->getVariable());
        if (pv) {
            digital->setVariable(pv, slot->period, slot->scale, slot->offset,
                    slot->tau);
        }
    }
    else {
        qWarning()
            << tr("Url not found: %1").arg(slot->url.toString());
    }
}

/****************************************************************************/

void DigitalPlugin::clearVariables(QWidget *widget) const
{
    Pd::Digital *digital = dynamic_cast<Pd::Digital *>(widget);
    digital->clearVariable();
}

/****************************************************************************/

QWidget *DigitalPlugin::createWidget(QWidget *parent) const
{
    return new Pd::Digital(parent);
}

/****************************************************************************/
