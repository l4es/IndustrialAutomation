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

#ifndef DATA_NODE_H
#define DATA_NODE_H

/****************************************************************************/

#include <pdcom/Variable.h>
#include <pdcom/Subscriber.h>

#include <QObject>
#include <QTimer>

/****************************************************************************/

class DataSource;
class DataModel;

/****************************************************************************/

class DataNode:
    public QObject,
    public PdCom::Subscriber
{
    Q_OBJECT

    public:
        DataNode(DataModel *, DataSource *, DataNode * = NULL,
                const QString & = QString());
        virtual ~DataNode();

        void setNodeName(const QString &);
        void addChild(DataNode *);
        void setVariable(PdCom::Variable *);
        void clearVariables();

        virtual QVariant nodeData(int, int);
        virtual void nodeFlags(Qt::ItemFlags &, int) const;
        virtual bool nodeSetData(const QVariant &) const;
        virtual QString nodeMimeText() const;
        virtual QUrl nodeUrl() const;
        virtual void addUrl(QList<QUrl> &) const;

        DataNode *getParentNode() const {
            return parent;
        }

        const QString &getName() const {
            return nodeName;
        }

        PdCom::Variable *getVariable() const {
            return variable;
        }

        double getPeriod() const {
            return period;
        }

        DataNode *getChildNode(int) const;
        int getRow();

        int displayIndex(DataNode *child) const {
            return display.indexOf(child);
        }

        int displaySize() const {
            return display.size();
        }

        DataNode *findChild(const QString &) const;

        virtual void print(int) const;

        static void loadIcons();

    protected:
        DataModel * const dataModel;
        DataSource * const dataSource;
        DataNode * const parent;

        void sortChildren();
        void showAll(bool);
        void showAncestors();
        void applyFilter(const QRegExp &);
        void updateDisplay();

    private:
        QString nodeName;
        QList<DataNode *> children;
        QList<DataNode *> display; // children to display

        PdCom::Variable *variable;
        double period;
        QString path;
        bool writable;
        PdCom::Variable::Dimension dim;

        bool subscribed;
        bool hasData;
        double value;

        bool show; // filter state

        static QIcon signalIcon;
        static QIcon parameterIcon;
        static QIcon folderIcon;

        // virtual from PdCom::Subscriber
        void notify(PdCom::Variable *);
        void notifyDelete(PdCom::Variable *);

        QString dimensionString() const;

        DataNode();
};

/****************************************************************************/

#endif
