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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Plugin.h"
#include "DataModel.h"

#include <QMainWindow>

#include <QSocketNotifier>

#include "ui_MainWindow.h"

/****************************************************************************/

class PropertyModel;
class PropertyNode;
class PropertyDelegate;

struct _object;
typedef struct _object PyObject;

/****************************************************************************/

class MainWindow:
    public QMainWindow,
    public Ui::MainWindow
{
    Q_OBJECT

    public:
        MainWindow(const QString &, bool, QWidget *parent = 0);
        ~MainWindow();

        static PyObject *pythonMethodTabCount(PyObject *, PyObject *);
        static PyObject *pythonMethodMaximize(PyObject *, PyObject *);

        DataModel *getDataModel() const {
            return dataModel;
        }
        PropertyModel *getPropertyModel() const {
            return propertyModel;
        }
        const PluginMap &getPluginMap() const { return pluginMap; }
        bool getEditMode() const { return editMode; }

        int getGridStep() const { return gridStep; }

    public slots:
        void signalRaised(int);

    private:
        DataModel *dataModel;
        PropertyModel *propertyModel;
        PropertyDelegate *propertyDelegate;
        wchar_t *programName;
        PluginMap pluginMap;
        bool editMode;
        int gridStep;
        bool restore;
        QStringList recentFiles;
        QString filePath;
        PropertyNode *propertyNode; // selected node
        int pythonStdOut[2];
        int pythonStdErr[2];
        QSocketNotifier *pythonStdOutNotifier;
        QSocketNotifier *pythonStdErrNotifier;

        QList<QString> pythonHistory;
        int pythonHistoryIndex;

        enum { MaxRecentFiles = 10 };
        QAction *recentFileActions[MaxRecentFiles];

        QLabel *labelIn;
        QLabel *labelOut;

        static MainWindow *singleton;

        QPersistentModelIndex dataIndex;

        void initPython();
        void loadLayout(const QString &);
        void saveLayout();
        void updateWindowTitle();
        void pythonBenchmark();
        bool eventFilter(QObject *, QEvent *);
        void pythonHistoryStep(int);
        void showEvent(QShowEvent *);
        void closeEvent(QCloseEvent *);
        void addRecentFile(const QString &);
        void updateRecentFileActions();
        void expandChildren(const QModelIndex &, QTreeView *, int);
        void updateEditMode();

    private slots:
        void on_actionNew_triggered();
        void on_actionLoad_triggered();
        void openRecentFile();
        void on_actionSave_triggered();
        void on_actionSaveAs_triggered();
        void on_actionClose_triggered();
        void on_actionConnectAll_triggered();
        void on_actionConnect_triggered();
        void on_actionEditMode_toggled(bool);
        void on_actionGlobalStyleSheet_triggered();
        void on_actionAddTab_triggered();
        void on_actionTest_triggered();
        void on_actionAboutTestManager_triggered();
        void on_tabWidget_tabBarDoubleClicked(int);
        void on_lineEditFilter_textChanged(const QString &);
        void on_toolButtonClearFilter_clicked();
        void sourceTreeCustomContextMenu(const QPoint &);
        void connectDataSource();
        void disconnectDataSource();
        void removeDataSource();
        void expandData();
        void expandProperties();
        void propertyTreeCustomContextMenu(const QPoint &);
        void resetProperty();
        void tabCloseRequested(int);
        void pythonStdOutDataAvailable(int);
        void pythonStdErrDataAvailable(int);
        void on_lineEditPython_returnPressed();
        void connectionStateChanged(DataModel::ConnectionState);
        void connectDataSlots();
        void statsChanged();
};

/****************************************************************************/

#endif
