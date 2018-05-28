/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2012  Florian Pose <fp@igh-essen.com>
 *
 * This file is part of the data logging service (DLS).
 *
 * The DLS is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * The DLS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the DLS. If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

#include <QMainWindow>
#include <QProcess>

#include "ui_MainWindow.h"
#include "LogWindow.h"

#include "LibDLS/Dir.h"

#include "DlsWidgets/Model.h"
#include "UriDialog.h"

/****************************************************************************/

class MainWindow:
    public QMainWindow,
    public Ui::MainWindow
{
    Q_OBJECT

    public:
        MainWindow(const QString &fileName = QString(), bool = false,
                QWidget * = 0);
        ~MainWindow();

    private:
        QList<LibDLS::Directory *> dirs;
        QtDls::Model model;
        bool restore;
        QStringList recentFiles;
        QString currentFileName;
        LogWindow logWindow;
        UriDialog uriDialog;
        struct Script {
            QString command;
            QString alias;
            enum Dialect {
                Yaml,
                Octave,
                Dls
            };
            Dialect dialect;
        };
        QList<Script> scripts;

        enum { MaxRecentFiles = 10 };
        QAction *recentFileActions[MaxRecentFiles];

        QAction **scriptActions;
        QProcess scriptProcess;

        LibDLS::Directory *menuDir;

        void showEvent(QShowEvent *);
        void closeEvent(QCloseEvent *);
        void addRecentFile(const QString &);
        void updateRecentFileActions();
        void updateScriptActions();

        static QStringList viewFilters();

    private slots:
        void on_actionNew_triggered();
        void on_actionLoad_triggered();
        void openRecentFile();
        void on_actionSave_triggered();
        void on_actionSaveAs_triggered();
        void on_actionAddLocalDataSource_triggered();
        void on_actionAddRemoteDataSource_triggered();
        void on_actionSettings_triggered();
        void on_actionLogWindow_triggered();
        void on_actionAboutDlsgui_triggered();

        void on_toolButtonNewDir_clicked();
        void on_toolButtonNewUrl_clicked();

        void loggingCallback(const QString &);
        void execScript();
        void scriptFinished(int, QProcess::ExitStatus);
        void scriptError(QProcess::ProcessError);

        void treeViewCustomContextMenu(const QPoint &);
        void updateDirectory();
        void removeDirectory();
        void removeUnusedDirectories();
        void updateWindowTitle();
};

/****************************************************************************/
