/*****************************************************************************
 *
 * $Id$
 *
 ****************************************************************************/

#include <QDebug>
#include <QUrl>
#include <QApplication>

#include <LibDLS/Job.h>

#include "Job.h"
#include "Channel.h"

using namespace QtDls;

/*****************************************************************************/

Job::Job(
        Node *parent,
        LibDLS::Job *job
        ):
    Node(parent),
    job(job)
{
    job->fetch_channels();

    for (std::list<LibDLS::Channel>::iterator ch = job->channels().begin();
            ch != job->channels().end(); ch++) {
        Channel *c = new Channel(this, &*ch);
        channels.push_back(c);
    }
}

/****************************************************************************/

Job::~Job()
{
}

/****************************************************************************/

QUrl Job::url() const
{
    QUrl u = parent()->url();
    QString path = u.path();
    path += QString("/job%1").arg(job->id());
    u.setPath(path);
    return u;
}

/****************************************************************************/

Channel *Job::findChannel(const QString &name)
{
    for (QList<Channel *>::iterator c = channels.begin();
            c != channels.end(); c++) {
        QString cName((*c)->name());
        if (name != cName) {
            continue;
        }

        return (*c);
    }

    return NULL;
}

/****************************************************************************/

int Job::rowCount() const
{
    return channels.size();
}

/****************************************************************************/

QVariant Job::data(const QModelIndex &index, int role) const
{
    QVariant ret;

    switch (index.column()) {
        case 0:
            switch (role) {
                case Qt::DisplayRole: {
                        QString text =
                            QApplication::translate("Job", "Job %1")
                            .arg(job->id());

                        QString desc(job->preset().description().c_str());
                        if (!desc.isEmpty()) {
                            text += ", \"" + desc + "\"";
                        }

                        ret = text;
                    }
                    break;
            }
            break;
    }

    return ret;
}

/****************************************************************************/

void *Job::child(int row) const
{
    Q_UNUSED(row);

    Node *ret = NULL;

    if (row >= 0 && row < channels.size()) {
        ret = channels[row];
    }

    return ret;
}

/****************************************************************************/

int Job::row(void *n) const
{
    return channels.indexOf((Channel *) n);
}

/****************************************************************************/
