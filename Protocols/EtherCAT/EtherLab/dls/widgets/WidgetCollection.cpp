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

#include "Plugin.h"
#include "WidgetCollection.h"

#include "DlsWidgets/Graph.h"

using namespace DLS;

/****************************************************************************/

class GraphPlugin:
    public Plugin
{
    public:
        GraphPlugin(QObject *parent = 0):
            Plugin("Graph", parent) {}
        QWidget *createWidget(QWidget *parent) {
            return new Graph(parent);
        }
};

/****************************************************************************/

/** Constructor.
 *
 * Fills the list of the provided plugins.
 */
WidgetCollection::WidgetCollection(
        QObject *parent /**> parent object */
        ): QObject(parent)
{
    widgets.append(new GraphPlugin(this));
}

/****************************************************************************/

/** Destructor.
 */
WidgetCollection::~WidgetCollection()
{
    for (int i = 0; i < widgets.size(); i++) {
        delete widgets[i];
    }
}

/****************************************************************************/

/** Returns the list of the provided plugins.
 */
QList<QDesignerCustomWidgetInterface *>
WidgetCollection::customWidgets() const
{
    return widgets;
}

/****************************************************************************/

// first parameter must be the same as the TARGET variable in the .pro file.

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(DlsWidgets, WidgetCollection)
#endif

/****************************************************************************/
