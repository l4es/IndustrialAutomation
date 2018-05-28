/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2012-2014  Florian Pose <fp@igh-essen.com>
 *
 * This file is part of the QtPdWidgets library.
 *
 * The QtPdWidgets library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * The QtPdWidgets library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the QtPdWidgets Library. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

#include "Plugin.h"
#include "WidgetCollection.h"

#include "QtPdWidgets/Bar.h"
#include "QtPdWidgets/CheckBox.h"
#include "QtPdWidgets/ClipImage.h"
#include "QtPdWidgets/Dial.h"
#include "QtPdWidgets/Digital.h"
#include "QtPdWidgets/DoubleSpinBox.h"
#include "QtPdWidgets/Graph.h"
#include "QtPdWidgets/Image.h"
#include "QtPdWidgets/Led.h"
#include "QtPdWidgets/MultiLed.h"
#include "QtPdWidgets/NoPdTouchEdit.h"
#include "QtPdWidgets/PushButton.h"
#include "QtPdWidgets/RadioButton.h"
#include "QtPdWidgets/SpinBox.h"
#include "QtPdWidgets/TableView.h"
#include "QtPdWidgets/Text.h"
#include "QtPdWidgets/Time.h"
#include "QtPdWidgets/TouchEdit.h"
#include "QtPdWidgets/XYGraph.h"

using namespace Pd;

/****************************************************************************/

class BarPlugin:
    public Plugin
{
    public:
        BarPlugin(QObject *parent = 0):
            Plugin("Bar", parent) {}
        QWidget *createWidget(QWidget *parent) {
            return new Bar(parent);
        }
};

/****************************************************************************/

class CheckBoxPlugin:
    public Plugin
{
    public:
        CheckBoxPlugin(QObject *parent = 0):
            Plugin("CheckBox", parent) {}
        QWidget *createWidget(QWidget *parent) {
            return new CheckBox(parent);
        }
};

/****************************************************************************/

class ClipImagePlugin:
    public Plugin
{
    public:
        ClipImagePlugin(QObject *parent = 0):
            Plugin("ClipImage", parent) {}
        QWidget *createWidget(QWidget *parent) {
            return new ClipImage(parent);
        }
};

/****************************************************************************/

class DialPlugin:
    public Plugin
{
    public:
        DialPlugin(QObject *parent = 0):
            Plugin("Dial", parent) {}
        QWidget *createWidget(QWidget *parent) {
            return new Dial(parent);
        }
};

/****************************************************************************/

class DigitalPlugin:
    public Plugin
{
    public:
        DigitalPlugin(QObject *parent = 0):
            Plugin("Digital", parent) {}
        QWidget *createWidget(QWidget *parent) {
            return new Digital(parent);
        }
};

/****************************************************************************/

class DoubleSpinBoxPlugin:
    public Plugin
{
    public:
        DoubleSpinBoxPlugin(QObject *parent = 0):
            Plugin("DoubleSpinBox", parent) {}
        QWidget *createWidget(QWidget *parent) {
            return new DoubleSpinBox(parent);
        }
};

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

class ImagePlugin:
    public Plugin
{
    public:
        ImagePlugin(QObject *parent = 0):
            Plugin("Image", parent) {}
        QWidget *createWidget(QWidget *parent) {
            return new Image(parent);
        }
};

/****************************************************************************/

class LedPlugin:
    public Plugin
{
    public:
        LedPlugin(QObject *parent = 0):
            Plugin("Led", parent) {}
        QWidget *createWidget(QWidget *parent) {
            return new Led(parent);
        }
};

/****************************************************************************/

class MultiLedPlugin:
    public Plugin
{
    public:
        MultiLedPlugin(QObject *parent = 0):
            Plugin("MultiLed", parent) {}
        QWidget *createWidget(QWidget *parent) {
            return new MultiLed(parent);
        }
};

/****************************************************************************/

class PushButtonPlugin:
    public Plugin
{
    public:
        PushButtonPlugin(QObject *parent = 0):
            Plugin("PushButton", parent) {}
        QWidget *createWidget(QWidget *parent) {
            return new PushButton(parent);
        }
};

/****************************************************************************/

class RadioButtonPlugin:
    public Plugin
{
    public:
        RadioButtonPlugin(QObject *parent = 0):
            Plugin("RadioButton", parent) {}
        QWidget *createWidget(QWidget *parent) {
            return new RadioButton(parent);
        }
};

/****************************************************************************/

class SpinBoxPlugin:
    public Plugin
{
    public:
        SpinBoxPlugin(QObject *parent = 0):
            Plugin("SpinBox", parent) {}
        QWidget *createWidget(QWidget *parent) {
            return new SpinBox(parent);
        }
};

/****************************************************************************/

class TableViewPlugin:
    public Plugin
{
    public:
        TableViewPlugin(QObject *parent = 0):
            Plugin("TableView", parent) {}
        QWidget *createWidget(QWidget *parent) {
            return new TableView(parent);
        }
};

/****************************************************************************/

class TextPlugin:
    public Plugin
{
    public:
        TextPlugin(QObject *parent = 0):
            Plugin("Text", parent) {}
        QWidget *createWidget(QWidget *parent) {
            return new Text(parent);
        }
};

/****************************************************************************/

class TouchEditPlugin:
    public Plugin
{
    public:
        TouchEditPlugin(QObject *parent = 0):
            Plugin("TouchEdit", parent) {}
        QWidget *createWidget(QWidget *parent) {
            return new TouchEdit(parent);
        }
};

/****************************************************************************/

class NoPdTouchEditPlugin:
    public Plugin
{
    public:
        NoPdTouchEditPlugin(QObject *parent = 0):
            Plugin("NoPdTouchEdit", parent) {}
        QWidget *createWidget(QWidget *parent) {
            return new Pd::NoPdTouchEdit(parent);
        }
};

/****************************************************************************/

class TimePlugin:
    public Plugin
{
    public:
        TimePlugin(QObject *parent = 0):
            Plugin("Time", parent) {}
        QWidget *createWidget(QWidget *parent) {
            return new Time(parent);
        }
};

/****************************************************************************/

class XYGraphPlugin:
    public Plugin
{
    public:
        XYGraphPlugin(QObject *parent = 0):
            Plugin("XYGraph", parent) {}
        QWidget *createWidget(QWidget *parent) {
            return new XYGraph(parent);
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
    widgets.append(new BarPlugin(this));
    widgets.append(new CheckBoxPlugin(this));
    widgets.append(new ClipImagePlugin(this));
    widgets.append(new DialPlugin(this));
    widgets.append(new DigitalPlugin(this));
    widgets.append(new DoubleSpinBoxPlugin(this));
    widgets.append(new GraphPlugin(this));
    widgets.append(new ImagePlugin(this));
    widgets.append(new LedPlugin(this));
    widgets.append(new MultiLedPlugin(this));
    widgets.append(new NoPdTouchEditPlugin(this));
    widgets.append(new PushButtonPlugin(this));
    widgets.append(new RadioButtonPlugin(this));
    widgets.append(new SpinBoxPlugin(this));
    widgets.append(new TableViewPlugin(this));
    widgets.append(new TextPlugin(this));
    widgets.append(new TimePlugin(this));
    widgets.append(new TouchEditPlugin(this));
    widgets.append(new XYGraphPlugin(this));
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
Q_EXPORT_PLUGIN2(QtPdWidgets, WidgetCollection)
#endif

/****************************************************************************/
