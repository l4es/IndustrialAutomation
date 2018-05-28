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

#include "Plugin.h"

#include "PropertyGroup.h"
#include "Property.h"
#include "DataModel.h"

#include <QMetaProperty>
#include <QUrl>
#include <QDebug>

/****************************************************************************/

QMap<QString, QSet<QString> > Plugin::whiteLists;

/****************************************************************************/

Plugin::Plugin()
{
    fillWhiteLists();
}

/****************************************************************************/

QWidget *Plugin::create(QWidget *parent)
{
    QWidget *widget(createWidget(parent));

    if (getPropertyGroups().empty()) {
        collectProperties(widget);
    }

    return widget;
}

/****************************************************************************/

void Plugin::initProperties(WidgetContainer *) const
{
}

/****************************************************************************/

QIcon Plugin::icon() const
{
    return QIcon();
}

/****************************************************************************/

int Plugin::getFreeSlots(const SlotModel &) const
{
    return 0;
}

/****************************************************************************/

void Plugin::appendVariable(QWidget *, SlotModel *, DataNode *) const
{
}

/****************************************************************************/

void Plugin::connectDataSlots(QWidget *, const SlotModel *,
        const DataModel *) const
{
}

/****************************************************************************/

void Plugin::clearVariables(QWidget *) const
{
}

/****************************************************************************/

void Plugin::collectProperties(const QObject *object)
{
    const QMetaObject *metaObject = object->metaObject();

    do {
        PropertyGroup *group = new PropertyGroup(metaObject->className());
        propertyGroups.insert(0, group);

        QSet<QString> whiteList;
        bool whiteListing = whiteLists.contains(metaObject->className());
        if (whiteListing) {
            whiteList = whiteLists[metaObject->className()];
        }

        for (int i = metaObject->propertyOffset();
                i < metaObject->propertyCount(); i++) {
            QMetaProperty metaProp = metaObject->property(i);

            if (!metaProp.isDesignable() or !metaProp.isWritable()) {
                continue;
            }

            if (whiteListing and not whiteList.contains(metaProp.name())) {
                continue;
            }

            new Property(group, metaProp);
        }
        metaObject = metaObject->superClass();
    }
    while (metaObject);

    int colorIndex = 0;
    foreach (PropertyGroup *group, propertyGroups) {
        group->setColorIndex(colorIndex++);
    }
}

/****************************************************************************/

void Plugin::fillWhiteLists()
{
    if (!whiteLists.empty()) {
        return;
    }

    QSet<QString> set;

    set.insert("enabled");
    set.insert("geometry");
    set.insert("toolTip");
    set.insert("toolTipDuration");
    set.insert("statusTip");
    set.insert("whatsThis");
    set.insert("styleSheet");
    whiteLists["QWidget"] = set;
}

/****************************************************************************/
