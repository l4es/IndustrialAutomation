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

#include "PushButtonPlugin.h"

#include "WidgetContainer.h"
#include "DataSlot.h"
#include "DataModel.h"
#include "DataNode.h"

#include <QtPdWidgets/PushButton.h>

#include <QJsonObject>

/****************************************************************************/

QString PushButtonPlugin::name() const
{
    return tr("Push Button");
}

/****************************************************************************/

QIcon PushButtonPlugin::icon() const
{
    return QIcon(":/images/plugin-button.svg");
}

/****************************************************************************/

int PushButtonPlugin::getFreeSlots(const SlotModel &slotModel) const
{
    int used = slotModel.getChildCount();
    return !used ? 1 : 0;
}

/****************************************************************************/

void PushButtonPlugin::appendVariable(QWidget *widget, SlotModel *slotModel,
        DataNode *dataNode) const
{
    Pd::PushButton *button = dynamic_cast<Pd::PushButton *>(widget);
    if (!button) {
        qWarning() << __func__ << "failed to cast";
        return;
    }

    DataSlot *slot = new DataSlot(slotModel);
    slot->url = dataNode->nodeUrl();

    PdCom::Variable *pv(dataNode->getVariable());
    if (pv) {
        button->setVariable(pv, slot->period, slot->scale, slot->offset,
                slot->tau);
    }
}

/****************************************************************************/

void PushButtonPlugin::connectDataSlots(QWidget *widget,
        const SlotModel *slotModel, const DataModel *dataModel) const
{
    Pd::PushButton *button = dynamic_cast<Pd::PushButton *>(widget);
    if (!button) {
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
            button->setVariable(pv, slot->period, slot->scale, slot->offset,
                    slot->tau);
        }
    }
    else {
        qWarning()
            << tr("Url not found: %1").arg(slot->url.toString());
    }
}

/****************************************************************************/

void PushButtonPlugin::clearVariables(QWidget *widget) const
{
    Pd::PushButton *button = dynamic_cast<Pd::PushButton *>(widget);
    button->clearVariable();
}

/****************************************************************************/

QWidget *PushButtonPlugin::createWidget(QWidget *parent) const
{
    return new Pd::PushButton(parent);
}

/****************************************************************************/
