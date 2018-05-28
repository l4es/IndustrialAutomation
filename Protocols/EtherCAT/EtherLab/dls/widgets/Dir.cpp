/*****************************************************************************
 *
 * $Id$
 *
 ****************************************************************************/

#include <list>

using namespace std;

#include <QDebug>
#include <QUrl>
#include <QIcon>
#include <QApplication>

#include <LibDLS/Dir.h>
#include <LibDLS/Job.h>

#include "Dir.h"
#include "Job.h"

using namespace QtDls;

/*****************************************************************************/

Dir::Dir(
        Model *model,
        LibDLS::Directory *dir
        ):
    Node(NULL),
    model(model),
    dir(dir)
{
    update_jobs();
    dir->attach_observer(this);
}

/****************************************************************************/

Dir::~Dir()
{
    dir->remove_observer(this);
    model->prepareLayoutChange();
    clear_jobs();
    model->finishLayoutChange();
}

/****************************************************************************/

QUrl Dir::url() const
{
    QUrl u;
    u.setUrl(dir->uri().c_str());
    return u;
}

/****************************************************************************/

Channel *Dir::findChannel(unsigned int job_id, const QString &name)
{
    for (QList<Job *>::iterator j = jobs.begin();
            j != jobs.end(); j++) {
        if ((*j)->getJob()->id() != job_id) {
            continue;
        }

        return (*j)->findChannel(name);
    }

    return NULL;
}

/****************************************************************************/

int Dir::rowCount() const
{
    return jobs.size();
}

/****************************************************************************/

QVariant Dir::data(const QModelIndex &index, int role) const
{
    QVariant ret;

    switch (index.column()) {
        case 0:
            switch (role) {
                case Qt::DisplayRole:
                    switch (dir->access()) {
                        case LibDLS::Directory::Local:
                            ret = QApplication::translate("Dir",
                                    "Local directory %1")
                                .arg(dir->path().c_str());
                            break;

                        case LibDLS::Directory::Network:
                            ret = QApplication::translate("Dir",
                                    "Remote directory %1")
                                .arg(url().toString());
                            break;

                        default:
                            break;
                    }
                    break;

                case Qt::DecorationRole:
                    switch (dir->access()) {
                        case LibDLS::Directory::Local:
                            ret = QIcon(":/DlsWidgets/images/"
                                    "drive-harddisk.svg");
                            break;

                        case LibDLS::Directory::Network:
                            if (dir->connected()) {
                                ret = QIcon(":/DlsWidgets/images/"
                                        "Network-idle.svg");
                            } else {
                                ret = QIcon(":/DlsWidgets/images/"
                                        "Network-error.svg");
                            }
                            break;

                        default:
                            break;
                    }
                    break;

                case Qt::ToolTipRole:
                    ret = dir->error_msg().c_str();
                    break;
            }
            break;
    }

    return ret;
}

/****************************************************************************/

void *Dir::child(int row) const
{
    Node *ret = NULL;

    if (row >= 0 && row < jobs.size()) {
        ret = jobs[row];
    }

    return ret;
}

/****************************************************************************/

int Dir::row(void *n) const
{
    return jobs.indexOf((Job *) n);
}

/****************************************************************************/

/** Update observers.
 */
void Dir::update()
{
    update_jobs();
}

/****************************************************************************/

void Dir::clear_jobs()
{
    for (QList<Job *>::iterator j = jobs.begin(); j != jobs.end(); j++) {
        delete *j;
    }

    jobs.clear();
}

/****************************************************************************/

void Dir::update_jobs()
{
    model->prepareLayoutChange();

    clear_jobs();

    for (list<LibDLS::Job *>::iterator j = dir->jobs().begin();
            j != dir->jobs().end(); j++) {
        Job *job = new QtDls::Job(this, *j);
        jobs.push_back(job);
    }

    model->finishLayoutChange();
}

/****************************************************************************/
