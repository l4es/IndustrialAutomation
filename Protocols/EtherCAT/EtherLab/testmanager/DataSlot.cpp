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

#include "DataSlot.h"
#include "DataModel.h"
#include "DataSource.h"

#include <pdcom/Variable.h>

#include <QVariant>
#include <QJsonObject>
#include <QJsonArray>
#include <QIcon>
#include <QLocale>

/****************************************************************************/

DataSlot::DataSlot(SlotNode *parent):
    SlotNode(parent),
    period(0.0),
    scale(1.0),
    offset(0.0),
    tau(0.0)
{
}

/****************************************************************************/

QIcon DataSlot::getIcon(const DataModel *dataModel) const
{
    if (!dataModel) {
        return QIcon();
    }

    DataNode *dataNode(dataModel->findDataNode(url));
    if (!dataNode) {
        return QIcon();
    }

    if (dataNode->getPeriod() > 0.0) {
        return QIcon(
                ":/images/utilities-system-monitor.png");
    }
    else {
        return QIcon(":/images/preferences-system.svg");
    }
}

/****************************************************************************/

void DataSlot::fromJson(const QJsonValue &value)
{
    QJsonObject obj(value.toObject());

    url = obj["url"].toString();
    period = obj["period"].toDouble();
    if (obj.contains("scale")) {
        scale = obj["scale"].toDouble();
    }
    offset = obj["offset"].toDouble();
    tau = obj["tau"].toDouble();
    color = obj["color"].toString();
}

/****************************************************************************/

QJsonValue DataSlot::toJson() const
{
    QJsonObject obj;

    obj["url"] = url.toString();

    if (period) {
        obj["period"] = period;
    }

    if (scale != 1.0) {
        obj["scale"] = scale;
    }

    if (offset) {
        obj["offset"] = offset;
    }

    if (tau) {
        obj["tau"] = tau;
    }

    if (color.isValid()) {
        obj["color"] = color.name();
    }

    return obj;
}

/****************************************************************************/

QVariant DataSlot::nodeData(int role, int section,
        const DataModel *dataModel) const
{
    QVariant ret;

    switch (role) {
        case Qt::DisplayRole:
            switch (section) {
                case 0:
                    ret = url;
                    break;
                case 1:
                    ret = QLocale().toString(period);
                    break;
                case 2:
                    ret = QLocale().toString(scale);
                    break;
                case 3:
                    ret = QLocale().toString(offset);
                    break;
                case 4:
                    ret = QLocale().toString(tau);
                    break;
                case 5:
                    ret = color.name();
                    break;
                default:
                    break;
            }
            break;

        case Qt::EditRole:
            switch (section) {
                case 1:
                    ret = QLocale().toString(period);
                    break;
                case 2:
                    ret = QLocale().toString(scale);
                    break;
                case 3:
                    ret = QLocale().toString(offset);
                    break;
                case 4:
                    ret = QLocale().toString(tau);
                    break;
                case 5:
                    ret = color.rgb();
                    break;
                default:
                    break;
            }
            break;

        case Qt::DecorationRole:
            switch (section) {
                case 0:
                    return getIcon(dataModel);
                    break;

                case 5:
                    ret = color;
                    break;
            }
            break;

        case Qt::ToolTipRole:
            switch (section) {
                case 0:
                    ret = url;
                    break;
            }
            break;

        default:
            break;
    }

    return ret;
}

/****************************************************************************/

void DataSlot::nodeFlags(Qt::ItemFlags &flags, int section) const
{
    if (section > 0 && section < 6) {
        flags |= Qt::ItemIsEditable;
    }
}

/****************************************************************************/

bool DataSlot::nodeSetData(int section, const QVariant &value)
{
    switch (section) {
        case 1:
            period = QLocale().toDouble(value.toString());
            return true;

        case 2:
            scale = QLocale().toDouble(value.toString());
            return true;

        case 3:
            offset = QLocale().toDouble(value.toString());
            return true;

        case 4:
            tau = QLocale().toDouble(value.toString());
            return true;

        case 5:
            {
                QColor c(QColor().fromRgb(value.toInt()));
                if (c.isValid()) {
                    color = c;
                    return true;
                }
            }
            break;

        default:
            break;
    }

    return false;
}

/****************************************************************************/

void DataSlot::countColors(QMap<QRgb, unsigned int> &colorUsage) const
{
    auto colorIt = colorUsage.find(color.rgba());
    if (colorIt != colorUsage.end()) {
        colorIt.value()++;
    }
}

/****************************************************************************/

void DataSlot::appendDataSources(DataModel *dataModel) const
{
    QUrl dataSourceUrl(url.adjusted(QUrl::RemovePassword |
                QUrl::RemovePath | QUrl::RemoveQuery |
                QUrl::RemoveFragment));
    if (!dataModel->hasDataSource(dataSourceUrl)) {
        DataSource *dataSource = new DataSource(dataModel, dataSourceUrl);
        dataModel->append(dataSource);
    }

    SlotNode::appendDataSources(dataModel);
}

/****************************************************************************/
