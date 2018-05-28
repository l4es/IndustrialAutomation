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

#include "DoubleSpinBoxPlugin.h"
#include "WidgetContainer.h"
#include "DataSlot.h"
#include "DataModel.h"
#include "DataNode.h"

#include <QtPdWidgets/DoubleSpinBox.h>

#include <QJsonObject>

/****************************************************************************/

QString DoubleSpinBoxPlugin::name() const
{
    return tr("Parameter Editor");
}

/****************************************************************************/

QIcon DoubleSpinBoxPlugin::icon() const
{
    return QIcon(":/images/plugin-spinbox.svg");
}

/****************************************************************************/

int DoubleSpinBoxPlugin::getFreeSlots(const SlotModel &slotModel) const
{
    int used = slotModel.getChildCount();
    return !used ? 1 : 0;
}

/****************************************************************************/

void DoubleSpinBoxPlugin::appendVariable(QWidget *widget,
        SlotModel *slotModel, DataNode *dataNode) const
{
    Pd::DoubleSpinBox *spinBox = dynamic_cast<Pd::DoubleSpinBox *>(widget);
    if (!spinBox) {
        qWarning() << __func__ << "failed to cast";
        return;
    }

    DataSlot *slot = new DataSlot(slotModel);
    slot->url = dataNode->nodeUrl();

    PdCom::Variable *pv(dataNode->getVariable());
    if (pv) {
        spinBox->setVariable(pv, slot->period, slot->scale, slot->offset,
                slot->tau);
    }
}

/****************************************************************************/

void DoubleSpinBoxPlugin::connectDataSlots(QWidget *widget,
        const SlotModel *slotModel, const DataModel *dataModel) const
{
    Pd::DoubleSpinBox *spinBox = dynamic_cast<Pd::DoubleSpinBox *>(widget);
    if (!spinBox) {
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
            spinBox->setVariable(pv, slot->period, slot->scale, slot->offset,
                    slot->tau);
        }
    }
    else {
        qWarning()
            << tr("Url not found: %1").arg(slot->url.toString());
    }
}

/****************************************************************************/

void DoubleSpinBoxPlugin::clearVariables(QWidget *widget) const
{
    Pd::DoubleSpinBox *spinBox = dynamic_cast<Pd::DoubleSpinBox *>(widget);
    spinBox->clearVariable();
}

/****************************************************************************/

QWidget *DoubleSpinBoxPlugin::createWidget(QWidget *parent) const
{
    return new Pd::DoubleSpinBox(parent);
}

/****************************************************************************/
