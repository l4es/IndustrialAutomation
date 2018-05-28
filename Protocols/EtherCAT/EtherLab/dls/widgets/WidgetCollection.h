/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2012-2014  Florian Pose <fp@igh-essen.com>
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

#ifndef DLS_WIDGETCOLLECTION_H
#define DLS_WIDGETCOLLECTION_H

#include <QtDesigner>
#include <qplugin.h>

/****************************************************************************/

/** Container class for the list of provided plugins.
 */
class WidgetCollection:
    public QObject,
    public QDesignerCustomWidgetCollectionInterface
{
    Q_OBJECT
#ifdef DLS_QT5
    /* DLS_QT5 defined in project file, thus interpreted by moc */
    Q_PLUGIN_METADATA(IID
            "org.qt-project.Qt.QDesignerCustomWidgetCollectionInterface")
#endif
    Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)

    public:
        WidgetCollection(QObject *parent = 0);
        virtual ~WidgetCollection();

        virtual QList<QDesignerCustomWidgetInterface *> customWidgets() const;

    private:
        /** The list of custom widgets for the designer. */
        QList<QDesignerCustomWidgetInterface *> widgets;
};

/****************************************************************************/

#endif
