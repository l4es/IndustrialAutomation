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

#include <QtPlugin>

#include "Plugin.h"

/****************************************************************************/

/** Plugin constructor.
 */
Plugin::Plugin(
        const QString &name, /**< Class name. */
        QObject *parent /**< parent object */
        ): QObject(parent),
    initialized(false),
    widgetClassName(name)
{
}

/****************************************************************************/

/** Initializes static plugin objects.
 */
void Plugin::initialize(
        QDesignerFormEditorInterface * /**< core */
        )
{
    if (initialized) {
        return;
    }

    initialized = true;
}

/****************************************************************************/

/** Returns the initialized state.
 */
bool Plugin::isInitialized() const
{
    return initialized;
}

/****************************************************************************/

/** Returns the name of the Plugin class.
 */
QString Plugin::name() const
{
    return "DLS::" + widgetClassName;
}

/****************************************************************************/

/** Returns the name of the plugin group to display in the list of plugins.
 */
QString Plugin::group() const
{
    return "DLS Widgets";
}

/****************************************************************************/

/** Returns the icon of the plugin.
 */
QIcon Plugin::icon() const
{
    return QIcon(); // empty icon
}

/****************************************************************************/

/** Returns the plugin's tooltip.
 */
QString Plugin::toolTip() const
{
    return "";
}

/****************************************************************************/

/** Returns the text for the "What is this?" function.
 */
QString Plugin::whatsThis() const
{
    return "";
}

/****************************************************************************/

/** Returns, if the widget is a container.
 */
bool Plugin::isContainer() const
{
    return false;
}

/****************************************************************************/

/** Returns the recommended default setting for the plugin as an XML tree.
 */
QString Plugin::domXml() const
{
    QString xml;

#if QT_VERSION >= 0x050000
    xml = "<ui language=\"c++\">\n"
        "  <widget class=\"DLS::" + widgetClassName + "\" "
        "name=\"dls" + widgetClassName + "\" />\n"
        "</ui>\n";
#else
    xml = "<widget class=\"DLS::" + widgetClassName + "\" "
        "name=\"dls" + widgetClassName + "\" />\n";
#endif

    return xml;
}

/****************************************************************************/

/** Returns the name of the header needed to use the plugin class.
 */
QString Plugin::includeFile() const
{
    return "DlsWidgets/" + widgetClassName + ".h";
}

/****************************************************************************/

