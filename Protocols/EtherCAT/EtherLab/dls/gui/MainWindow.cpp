/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2012  Florian Pose <fp@igh-essen.com>
 *
 * This file is part of the data logging service (DLS).
 *
 * DLS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * DLS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with DLS. If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

#include <iostream>
using namespace std;

#include <QDebug>
#include <QFileDialog>
#include <QSettings>
#include <QCloseEvent>
#include <QMessageBox>
#include <QUrl>

#define RENDERTEST 0

#if RENDERTEST
#include <QPainter>
#endif

#include "MainWindow.h"
#include "SettingsDialog.h"
#include "AboutDialog.h"

#define MODELTEST 0

#if MODELTEST
#include "modeltest.h"
#endif

#define DEBUG_SCRIPTS 0

/****************************************************************************/

MainWindow::MainWindow(
        const QString &fileName,
        bool newView,
        QWidget *parent
        ):
    QMainWindow(parent),
    uriDialog(this),
    scriptActions(NULL),
    scriptProcess(this),
    menuDir(NULL)
{
    setupUi(this);

    connect(dlsGraph, SIGNAL(logMessage(const QString &)),
            this, SLOT(loggingCallback(const QString &)));
    connect(&scriptProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(scriptFinished(int, QProcess::ExitStatus)));
    connect(&scriptProcess, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(scriptError(QProcess::ProcessError)));

    updateWindowTitle();
    setWindowIcon(QPixmap(":/images/dlsgui.svg"));

    QSettings settings;
    restore = settings.value("RestoreOnStartup", true).toBool();
    recentFiles = settings.value("RecentFiles").toStringList();
    if (settings.contains("WindowHeight") &&
            settings.contains("WindowWidth")) {
        resize(settings.value("WindowWidth").toInt(),
                settings.value("WindowHeight").toInt());
    }
    if (settings.value("WindowMaximized", false).toBool()) {
        setWindowState(windowState() | Qt::WindowMaximized);
    }
    if (settings.contains("SplitterSizes")) {
        QVariantList vl(settings.value("SplitterSizes").toList());
        QList<int> list;
        list << vl[0].toInt() << vl[1].toInt();
        splitter->setSizes(list);
    }

    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActions[i] = new QAction(this);
        recentFileActions[i]->setVisible(false);
        connect(recentFileActions[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
        menuRecentFiles->addAction(recentFileActions[i]);
    }

    updateRecentFileActions();

    int size = settings.beginReadArray("Scripts");
    if (size > 0) {
        scriptActions = new QAction *[size];

        for (int i = 0; i < size; ++i) {
            settings.setArrayIndex(i);
            Script s;
            s.command = settings.value("Command").toString();
            s.alias = settings.value("Alias").toString();
            QString dialect = settings.value("Dialect").toString();
            if (dialect == "Yaml" || dialect == "") {
                s.dialect = Script::Yaml;
            }
            else if (dialect == "Octave") {
                s.dialect = Script::Octave;
            }
            else if (dialect == "Dls") {
                s.dialect = Script::Dls;
            }
            else {
                qWarning() <<
                    tr("Unknown dialect \"%1\": Defaulting to Yaml.")
                    .arg(dialect);
                s.dialect = Script::Yaml;
            }

            scriptActions[i] = new QAction(this);
            if (s.alias != "") {
                scriptActions[i]->setText(s.alias);
            }
            else {
                scriptActions[i]->setText(s.command);
            }
            scriptActions[i]->setData(i);
            connect(scriptActions[i], SIGNAL(triggered()),
                    this, SLOT(execScript()));
            menuScripts->addAction(scriptActions[i]);

            scripts.append(s);
        }
    }
    settings.endArray();

    menuScripts->menuAction()->setVisible(size > 0);
    updateScriptActions();

#if MODELTEST
    new ModelTest(&model);
#endif

    dlsGraph->setDropModel(&model);

    treeView->setModel(&model);
    treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(treeView, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(treeViewCustomContextMenu(const QPoint &)));

    QString fileToLoad;

    if (newView) {
        // pass
    }
    else if (!fileName.isEmpty()) {
        fileToLoad = fileName;
    }
    else if (restore && recentFiles.size() > 0) {
        fileToLoad = recentFiles.front();
    }

    if (!fileToLoad.isEmpty()) {
        if (dlsGraph->load(fileToLoad, &model)) {
            currentFileName = fileToLoad;
            updateWindowTitle();
            addRecentFile(currentFileName);
        }
        else {
            qWarning() << "failed to load" << fileToLoad;
        }
    }
}

/****************************************************************************/

MainWindow::~MainWindow()
{
    model.clear();

    for (QList<LibDLS::Directory *>::iterator dir = dirs.begin();
            dir != dirs.end(); dir++) {
        delete *dir;
    }

    for (int i = 0; i < scripts.size(); ++i) {
        delete scriptActions[i];
    }

    delete [] scriptActions;
}

/********************** HACK: QTBUG-16507 workaround ************************/

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    QString filePath = windowFilePath();
    if (!filePath.isEmpty()) {
        setWindowFilePath(filePath + "x");
        setWindowFilePath(filePath);
    }
}

/****************************************************************************/

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;

    settings.setValue("RestoreOnStartup", restore);
    settings.setValue("RecentFiles", recentFiles);
    settings.setValue("WindowWidth", width());
    settings.setValue("WindowHeight", height());
    settings.setValue("WindowMaximized", isMaximized());
    QVariantList list;
    list << splitter->sizes()[0] << splitter->sizes()[1];
    settings.setValue("SplitterSizes", list);

    logWindow.close();
    event->accept();
}

/****************************************************************************/

void MainWindow::addRecentFile(const QString &path)
{
    QDir cur = QDir::currentPath();
    QString absPath = cur.absoluteFilePath(path);
    absPath = QDir::cleanPath(absPath);
    recentFiles.removeAll(absPath);
    recentFiles.prepend(absPath);
    updateRecentFileActions();
}

/****************************************************************************/

void MainWindow::updateRecentFileActions()
{
    int numRecentFiles = qMin(recentFiles.size(), (int) MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(recentFiles[i]);
        recentFileActions[i]->setText(text);
        recentFileActions[i]->setData(recentFiles[i]);
        recentFileActions[i]->setVisible(true);
    }

    for (int j = numRecentFiles; j < MaxRecentFiles; ++j) {
        recentFileActions[j]->setVisible(false);
    }

    menuRecentFiles->setEnabled(numRecentFiles > 0);
}

/****************************************************************************/

void MainWindow::updateScriptActions()
{
    bool enabled = scriptProcess.state() == QProcess::NotRunning;

    for (int i = 0; i < scripts.size(); ++i) {
        scriptActions[i]->setEnabled(enabled);
    }
}

/****************************************************************************/

QStringList MainWindow::viewFilters()
{
    QStringList filters;
    filters << tr("DLS Views (*.dlsv)");
    filters << tr("XML files (*.xml)");
    filters << tr("All files (*.*)");
    return filters;
}

/****************************************************************************/

void MainWindow::on_actionNew_triggered()
{
    currentFileName = "";
    updateWindowTitle();
    dlsGraph->clearSections();
    dlsGraph->setMessageFilter("");
}

/****************************************************************************/

void MainWindow::on_actionLoad_triggered()
{
    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setDefaultSuffix("dlsv");
    dialog.setNameFilters(viewFilters());
    dialog.selectFile(currentFileName);

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    QString path = dialog.selectedFiles()[0];

    if (path.isEmpty()) {
        return;
    }

    if (dlsGraph->load(path, &model)) {
        currentFileName = path;
        updateWindowTitle();
        addRecentFile(currentFileName);
    }
}

/****************************************************************************/

void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (!action) {
        return;
    }

    QString path = action->data().toString();

    if (dlsGraph->load(path, &model)) {
        currentFileName = path;
        updateWindowTitle();
        addRecentFile(currentFileName);
    }
}

/****************************************************************************/

void MainWindow::on_actionSave_triggered()
{
    QString path;

    if (currentFileName.isEmpty()) {
        QFileDialog dialog(this);
        dialog.setAcceptMode(QFileDialog::AcceptSave);
        dialog.setFileMode(QFileDialog::AnyFile);
        dialog.setDefaultSuffix("dlsv");
        dialog.setNameFilters(viewFilters());

        if (dialog.exec() != QDialog::Accepted) {
            return;
        }

        path = dialog.selectedFiles()[0];
    }
    else {
        path = currentFileName;
    }

    if (dlsGraph->save(path)) {
        currentFileName = path;
        updateWindowTitle();
        addRecentFile(currentFileName);
    }
}

/****************************************************************************/

void MainWindow::on_actionSaveAs_triggered()
{
    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setDefaultSuffix("dlsv");
    dialog.setNameFilters(viewFilters());
    dialog.selectFile(currentFileName);

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    QString path = dialog.selectedFiles()[0];

    if (path.isEmpty()) {
        return;
    }

    if (dlsGraph->save(path)) {
        currentFileName = path;
        updateWindowTitle();
        addRecentFile(currentFileName);
    }
}

/****************************************************************************/

void MainWindow::on_actionAddLocalDataSource_triggered()
{
    on_toolButtonNewDir_clicked();
}

/****************************************************************************/

void MainWindow::on_actionAddRemoteDataSource_triggered()
{
    on_toolButtonNewUrl_clicked();
}

/****************************************************************************/

void MainWindow::on_actionSettings_triggered()
{
    SettingsDialog dialog(restore, this);

    dialog.exec();
}

/****************************************************************************/

void MainWindow::on_actionLogWindow_triggered()
{
    logWindow.show();
}

/****************************************************************************/

void MainWindow::on_actionAboutDlsgui_triggered()
{
    AboutDialog dialog(this);

    dialog.exec();

#if RENDERTEST
    QPixmap pixmap(600, 400);
    QPainter painter(&pixmap);
    pixmap.fill();
    int page = 0;

    while (1) {
        if (!dlsGraph->renderPage(painter, pixmap.rect(),
                    page, DLS::Graph::All)) {
            break;
        }

        pixmap.save(QString("shot-%1.png").arg(page));
        pixmap.fill();
        page++;
    }
#endif
}

/****************************************************************************/

void MainWindow::on_toolButtonNewDir_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this,
            tr("Open data directory"), "/vol/data/dls_data");

    if (path.isEmpty()) {
        return;
    }

    LibDLS::Directory *dir = new LibDLS::Directory();

    try {
        dir->set_uri(path.toLocal8Bit().constData());
    } catch (LibDLS::DirectoryException &e) {
        qWarning() << e.msg.c_str();
        delete dir;
        return;
    }

    model.addLocalDir(dir);

    try {
        dir->import();
    } catch (LibDLS::DirectoryException &e) {
        qWarning() << e.msg.c_str();
    }
}

/****************************************************************************/

void MainWindow::on_toolButtonNewUrl_clicked()
{
    if (uriDialog.exec() != QDialog::Accepted) {
        return;
    }

    LibDLS::Directory *dir = new LibDLS::Directory();

    try {
        dir->set_uri(uriDialog.getUri().toString().toLatin1().constData());
    } catch (LibDLS::DirectoryException &e) {
        qWarning() << e.msg.c_str();
        delete dir;
        return;
    }

    try {
        dir->import();
    } catch (LibDLS::DirectoryException &e) {
        qWarning() << e.msg.c_str();
    }

    model.addLocalDir(dir);
}

/****************************************************************************/

void MainWindow::loggingCallback(const QString &msg)
{
    logWindow.log(msg);
}

/****************************************************************************/

void MainWindow::execScript()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (!action) {
        return;
    }

    int index = action->data().toInt();
    if (index < 0 || index >= scripts.size()) {
        return;
    }

    QString command = scripts[index].command;

    if (command == "") {
        qWarning() << "Command is empty.";
        return;
    }

#if DEBUG_SCRIPTS
    qDebug() << QString("Executing \"%1\" with dialect %2")
        .arg(scripts[index].command)
        .arg(scripts[index].dialect);
#endif

    scriptProcess.start(scripts[index].command);

    if (!scriptProcess.waitForStarted()) {
        return;
    }

    QString out;
    QList<DLS::Graph::ChannelInfo> channelInfo = dlsGraph->channelInfo();
    LibDLS::Time measure = dlsGraph->getMeasureTime();

    switch (scripts[index].dialect) {
        case Script::Octave:
            out += QString(
                    "dls.start = [{'%1'}, %2];\n"
                    "dls.end = [{'%3'}, %4];\n")
                .arg(dlsGraph->getStart().to_iso_time().c_str())
                .arg(dlsGraph->getStart().to_dbl_time(), 0, 'f')
                .arg(dlsGraph->getEnd().to_iso_time().c_str())
                .arg(dlsGraph->getEnd().to_dbl_time(), 0, 'f');
            if (!measure.is_null()) {
                out += QString("dls.measure = [{'%1'}, %2];\n")
                    .arg(measure.to_iso_time().c_str())
                    .arg(measure.to_dbl_time(), 0, 'f');
            }
            out += "dls.channels = [\n";
            for (QList<DLS::Graph::ChannelInfo>::const_iterator ci =
                    channelInfo.begin(); ci != channelInfo.end(); ci++) {
                out += QString("    {'url: %1'}, %2, %3;\n")
                    .arg(ci->url.toString())
                    .arg(ci->jobId)
                    .arg(ci->dirIndex);
            }
            out += "];\n";
            break;

        case Script::Dls:
            out += QString(
                    "-s \"%1\"\n"
                    "-e \"%2\"\n")
                .arg(dlsGraph->getStart().to_iso_time().c_str())
                .arg(dlsGraph->getEnd().to_iso_time().c_str());
            for (QList<DLS::Graph::ChannelInfo>::const_iterator ci =
                    channelInfo.begin(); ci != channelInfo.end(); ci++) {
                out += QString("    -j %1 -c %2\n")
                    .arg(ci->jobId)
                    .arg(ci->dirIndex);
            }
            break;

        default: // Yaml
            out += QString("---\nstart: %1\nend: %2\n")
                .arg(dlsGraph->getStart().to_iso_time().c_str())
                .arg(dlsGraph->getEnd().to_iso_time().c_str());
            if (!measure.is_null()) {
                out += QString("measure: %1\n")
                    .arg(measure.to_iso_time().c_str());
            }
            out += "channels:\n";
            for (QList<DLS::Graph::ChannelInfo>::const_iterator ci =
                    channelInfo.begin(); ci != channelInfo.end(); ci++) {
                out += QString("   - url: %1\n").arg(ci->url.toString());
                out += QString("     job: %1\n").arg(ci->jobId);
                out += QString("   index: %1\n").arg(ci->dirIndex);
            }
    }

    scriptProcess.write(out.toLocal8Bit());
    scriptProcess.closeWriteChannel();

    updateScriptActions();
}

/****************************************************************************/

void MainWindow::scriptFinished(
        int exitCode,
        QProcess::ExitStatus exitStatus
        )
{
#if DEBUG_SCRIPTS
    QByteArray result = scriptProcess.readAll();
    qDebug() << "finished" << exitCode << exitStatus << result;
#else
    Q_UNUSED(exitCode);
    Q_UNUSED(exitStatus);
#endif

    updateScriptActions();
}

/****************************************************************************/

void MainWindow::scriptError(
        QProcess::ProcessError error
        )
{
    QString msg;

    switch (error) {
        case QProcess::FailedToStart:
            msg = tr("Failed to start process.");
            break;
        case QProcess::Crashed:
            msg = tr("Script crashed.");
            break;
        case QProcess::Timedout:
            msg = tr("Script timed out.");
            break;
        case QProcess::WriteError:
            msg = tr("Failed to write to script.");
            break;
        case QProcess::ReadError:
            msg = tr("Failed to read from script.");
            break;
        default:
            msg = tr("Unknown error.");
            break;
    }

    QMessageBox::critical(this, tr("Script"),
            tr("Failed to execute script: %1").arg(msg));

    updateScriptActions();
}

/****************************************************************************/

void MainWindow::treeViewCustomContextMenu(const QPoint &point)
{
    QMenu *menu = new QMenu;
    QModelIndex index = treeView->indexAt(point);

    menuDir = model.dir(index);

    QAction *a = menu->addAction(tr("Update"), this, SLOT(updateDirectory()));
    a->setIcon(QIcon(":/images/view-refresh.svg"));
    a->setEnabled(menuDir);

    a = menu->addAction(tr("Remove"), this, SLOT(removeDirectory()));
    a->setIcon(QIcon(":/images/Edit-delete.svg"));
    a->setEnabled(menuDir && !dlsGraph->dirInUse(menuDir));

    a = menu->addAction(tr("Remove unused directories"),
            this, SLOT(removeUnusedDirectories()));
    a->setIcon(QIcon(":/images/Edit-delete.svg"));
    a->setEnabled(model.hasUnusedDirs(dlsGraph));

    menu->exec(treeView->viewport()->mapToGlobal(point));
}

/****************************************************************************/

void MainWindow::updateDirectory()
{
    if (!menuDir) {
        return;
    }

    try {
        menuDir->import();
    } catch (LibDLS::DirectoryException &e) {
        qWarning() << e.msg.c_str();
    }

    dlsGraph->connectChannels(&model);
    dlsGraph->loadData();
}

/****************************************************************************/

void MainWindow::removeDirectory()
{
    if (!menuDir) {
        return;
    }

    model.removeDir(menuDir);
    menuDir = NULL;
}

/****************************************************************************/

void MainWindow::removeUnusedDirectories()
{
    model.removeUnusedDirs(dlsGraph);
}

/****************************************************************************/

void MainWindow::updateWindowTitle()
{
    if (currentFileName.isEmpty()) {
        setWindowFilePath("");
        setWindowTitle(QCoreApplication::applicationName());
    }
    else {
        setWindowTitle("");
        setWindowFilePath(currentFileName);
    }
}

/****************************************************************************/
