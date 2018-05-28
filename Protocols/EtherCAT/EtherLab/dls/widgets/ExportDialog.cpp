/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2012-2013  Florian Pose <fp@igh-essen.com>
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

#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>

#include <LibDLS/Export.h>

#include "ExportDialog.h"
#include "DlsWidgets/Graph.h"
#include "Channel.h"

using DLS::ExportDialog;
using DLS::ExportWorker;
using QtDls::Channel;

/****************************************************************************/

/** Constructor.
 */
ExportDialog::ExportDialog(
        Graph *graph,
        QThread *thread,
        QSet<Channel *> channels
        ):
    QDialog(graph),
    graph(graph),
    worker(channels, graph->getStart(), graph->getEnd()),
    dirCreated(false),
    now(LibDLS::Time::now()),
    workerBusy(false)
{
    setupUi(this);

    worker.moveToThread(thread);

    connect(&worker, SIGNAL(updateProgress()), this, SLOT(updateProgress()));
    connect(&worker, SIGNAL(finished()), this, SLOT(workerFinished()));

    QString num;
    num.setNum(channels.size());
    labelNumber->setText(num);

    labelBegin->setText(graph->getStart().to_real_time().c_str());
    labelEnd->setText(graph->getEnd().to_real_time().c_str());

    labelDuration->setText(
            graph->getStart().diff_str_to(graph->getEnd()).c_str());

    char *env;
    QString envExport, envExportFormat;

    if ((env = getenv("DLS_EXPORT"))) {
        envExport = env;
    }
    else {
        envExport = ".";
    }

    if ((env = getenv("DLS_EXPORT_FMT"))) {
        envExportFormat = env;
    }
    else {
        envExportFormat = "dls-export-%Y-%m-%d-%H-%M-%S";
    }

    dir.setPath(envExport + "/" + now.format_time(
                envExportFormat.toLocal8Bit().constData()).c_str());
    labelDir->setText(QDir::cleanPath(dir.path()));
}

/****************************************************************************/

/** Destructor.
 */
ExportDialog::~ExportDialog()
{
}

/****************************************************************************/

void ExportDialog::accept()
{
    dirCreated = !dir.exists();

    // create unique directory
    if (!dir.mkpath(dir.absolutePath())) {
        QMessageBox box(this);
        box.setIcon(QMessageBox::Critical);
        box.setText(tr("Failed to create export directory %1.")
                .arg(dir.absolutePath()));
        box.exec();
        return;
    }

    // create info file
    QString path = dir.filePath("dls_export_info.txt");
    QFile infoFile(path);
    if (!infoFile.open(QIODevice::WriteOnly)) {
        QMessageBox box(this);
        box.setIcon(QMessageBox::Critical);
        box.setText(tr("Failed to open %1.").arg(path));
        box.exec();
        return;
    }

    QTextStream str(&infoFile);

    str << "\n"
        << "This is a DLS export directory.\n\n"
        << "Exported on: " << now.to_rfc811_time().c_str() << "\n\n"
        << "Exported range from: "
        << graph->getStart().to_real_time().c_str() << "\n"
        << "                 to: "
        << graph->getEnd().to_real_time().c_str() << "\n"
        << "           duration: "
        << graph->getStart().diff_str_to(graph->getEnd()).c_str() << "\n\n";

    QSet<Channel *>::const_iterator channel;
    for (channel = worker.getChannels().begin();
            channel != worker.getChannels().end(); channel++) {
        str << "channel" << (*channel)->dirIndex()
            << ": " << (*channel)->name() << "\n";
    }

    infoFile.close();

    worker.setDirectory(dir);
    worker.setDecimation(spinBoxDecimation->value());

    if (checkBoxAscii->isChecked()) {
        LibDLS::ExportAscii *exp = new LibDLS::ExportAscii();
        worker.addExporter(exp);
    }

    if (checkBoxMatlab->isChecked()) {
        LibDLS::ExportMat4 *exp = new LibDLS::ExportMat4();
        worker.addExporter(exp);
    }

    QPushButton *ok = buttonBox->button(QDialogButtonBox::Ok);
    ok->setEnabled(false);
    pushButtonDir->setEnabled(false);
    checkBoxAscii->setEnabled(false);
    checkBoxMatlab->setEnabled(false);

    workerBusy = true;
    QMetaObject::invokeMethod(&worker, "doWork", Qt::QueuedConnection);
}

/****************************************************************************/

void ExportDialog::reject()
{
    if (workerBusy) {
        worker.cancel();
    }
    else {
        done(Rejected);
    }
}

/****************************************************************************/

bool ExportDialog::removeRecursive(const QString &dirName)
{
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(
                    QDir::NoDotAndDotDot | QDir::System
                    | QDir::Hidden  | QDir::AllDirs
                    | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeRecursive(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }

        result = dir.rmdir(dirName);
    }

    return result;
}

/****************************************************************************/

void ExportDialog::updateProgress()
{
    progressBar->setValue(worker.progress());
}

/****************************************************************************/

void ExportDialog::workerFinished()
{
    workerBusy = false;

    if (worker.cancelled() || !worker.successful()) {
        if (dirCreated) {
            removeRecursive(dir.absolutePath());
        }
    }

    if (worker.cancelled()) {
        done(Rejected);
        return;
    }

    if (worker.successful()) {
        done(Accepted);
    }
}

/****************************************************************************/

void ExportDialog::on_pushButtonDir_clicked()
{
    QFileDialog dialog(this);

    QString path = QFileDialog::getSaveFileName(this, tr("Target Directory"),
            dir.path(), QString(), NULL, QFileDialog::ShowDirsOnly);

    if (path.isEmpty()) {
        return;
    }

    dir.setPath(path);
    labelDir->setText(QDir::cleanPath(dir.path()));
}

/****************************************************************************/

ExportWorker::ExportWorker(
        QSet<Channel *> channels,
        LibDLS::Time start,
        LibDLS::Time end
        ):
    start(start),
    end(end),
    decimation(1),
    channels(channels),
    totalProgress(0.0),
    channelProgress(0.0),
    cancelRequested(false),
    success(false)
{
}

/****************************************************************************/

ExportWorker::~ExportWorker()
{
    for (QList<LibDLS::Export *>::const_iterator exp = exporters.begin();
            exp != exporters.end(); exp++) {
        delete *exp;
    }
}

/****************************************************************************/

void ExportWorker::addExporter(LibDLS::Export *exporter)
{
    exporters += exporter;
}

/****************************************************************************/

void ExportWorker::doWork()
{
    totalProgress = 0.0;
    channelProgress = 0.0;
    bool beginSuccessful;

    success = false;

    QSet<Channel *>::const_iterator channel;

    for (channel = channels.begin(); channel != channels.end(); channel++) {

        beginSuccessful = true;

        for (QList<LibDLS::Export *>::const_iterator exp = exporters.begin();
                exp != exporters.end(); exp++) {
            if (!(*channel)->beginExport(*exp, dir.path())) {
                beginSuccessful = false;
                break;
            }
        }

        if (!beginSuccessful || cancelRequested) {
            break;
        }

        (*channel)->fetchData(start, end, 0, dataCallback, this, decimation);

        if (cancelRequested) {
            break;
        }

        for (QList<LibDLS::Export *>::const_iterator exp = exporters.begin();
                exp != exporters.end(); exp++) {
            (*exp)->end();
        }

        channelProgress += 100.0 / channels.size();
        totalProgress = channelProgress;
        emit updateProgress();

        if (cancelRequested) {
            break;
        }
    }

    if (channel == channels.end()) {
        success = true;
    }

    emit finished();
}

/****************************************************************************/

int ExportWorker::dataCallback(LibDLS::Data *data, void *priv)
{
    ((ExportWorker *) priv)->newData(data);
    return 0; // not adopted
}

/****************************************************************************/

void ExportWorker::newData(LibDLS::Data *data)
{
    for (QList<LibDLS::Export *>::const_iterator exp = exporters.begin();
            exp != exporters.end(); exp++) {
        (*exp)->data(data);
    }

    // display progress
    double diff_time = (data->end_time() - start).to_dbl();
    totalProgress = channelProgress +
        diff_time * 100.0 / channels.size() / (end - start).to_dbl();
    emit updateProgress();
}

/****************************************************************************/
