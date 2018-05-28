/*****************************************************************************
 *
 * $Id$
 *
 ****************************************************************************/

#include <QDebug>
#include <QUrl>
#include <QIcon>

#include <algorithm>

#include <LibDLS/Export.h>

#include "Channel.h"

using namespace QtDls;

/*****************************************************************************/

Channel::Channel(
        Node *parent,
        LibDLS::Channel *channel
        ):
    Node(parent),
    ch(channel)
{
}

/****************************************************************************/

Channel::~Channel()
{
}

/****************************************************************************/

QUrl Channel::url() const
{
    QUrl u = parent()->url();
    QString path = u.path();
    path += ch->name().c_str();
    u.setPath(path);
    return u;
}

/****************************************************************************/

QString Channel::name() const
{
    return ch->name().c_str();
}

/****************************************************************************/

void Channel::fetchData(LibDLS::Time start, LibDLS::Time end,
        unsigned int min_values, LibDLS::DataCallback callback, void *priv,
        unsigned int decimation)
{
    rwlock.lockForWrite();
    ch->fetch_chunks();
    rwlock.unlock();

    rwlock.lockForRead();
    ch->fetch_data(start, end, min_values, callback, priv, decimation);
    rwlock.unlock();
}

/****************************************************************************/

bool Channel::beginExport(LibDLS::Export *exporter, const QString &path)
{
    rwlock.lockForRead();

    try {
        exporter->begin(*ch, path.toLocal8Bit().constData());
    }
    catch (LibDLS::ExportException &e) {
        rwlock.unlock();
        qDebug() << "export begin failed: " << e.msg.c_str();
        return false;
    }

    rwlock.unlock();
    return true;
}

/****************************************************************************/

std::vector<Channel::TimeRange> Channel::chunkRanges()
{
    std::vector<TimeRange> ranges;

    if (rwlock.tryLockForRead()) {
        for (LibDLS::Channel::ChunkMap::const_iterator c =
                ch->chunks().begin();
                c != ch->chunks().end(); c++) {
            TimeRange r;
            r.start = c->second.start();
            r.end = c->second.end();
            ranges.push_back(r);
        }

        rwlock.unlock();

        sort(ranges.begin(), ranges.end(), range_before);

        lastRanges = ranges;
    }
    else {
        /* FIXME workaround for unnecessary locking. */
        ranges = lastRanges;
    }

    return ranges;
}

/****************************************************************************/

bool Channel::getRange(LibDLS::Time &start, LibDLS::Time &end)
{
    bool ret;

    rwlock.lockForRead();

    if (ch->chunks().empty()) {
        ret = false;
    }
    else {
        start = ch->start();
        end = ch->end();
        ret = true;
    }

    rwlock.unlock();

    return ret;
}

/****************************************************************************/

int Channel::rowCount() const
{
    return 0;
}

/****************************************************************************/

QVariant Channel::data(const QModelIndex &index, int role) const
{
    QVariant ret;

    switch (index.column()) {
        case 0:
            switch (role) {
                case Qt::DisplayRole:
                    ret = QString(ch->name().c_str());
                    break;
                case Qt::DecorationRole:
                    ret = QIcon(":/DlsWidgets/images/"
                            "utilities-system-monitor.svg");
                    break;
            }
            break;
    }

    return ret;
}

/****************************************************************************/

void *Channel::child(int row) const
{
    Q_UNUSED(row);

    Node *ret = NULL;

    return ret;
}

/****************************************************************************/

int Channel::row(void *n) const
{
    Q_UNUSED(n)
    return 0;
}

/****************************************************************************/

Qt::ItemFlags Channel::flags() const
{
    return Qt::ItemIsDragEnabled | Qt::ItemIsSelectable;
}

/****************************************************************************/

bool Channel::range_before(
        const TimeRange &range1,
        const TimeRange &range2
        )
{
    return range1.start < range2.start;
}

/****************************************************************************/
