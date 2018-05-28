/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2009 - 2015  Florian Pose <fp@igh-essen.com>
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

#ifndef QTDLS_CHANNEL_H
#define QTDLS_CHANNEL_H

#include <QList>
#include <QReadWriteLock>

#include <LibDLS/Time.h>
#include <LibDLS/Chunk.h> // for DataCallback
#include <LibDLS/Channel.h>

#include "Node.h"

/*****************************************************************************/

namespace LibDLS {
    class Export;
    class Job;
}

namespace QtDls {

class Channel:
   public Node
{
    public:
        Channel(Node *, LibDLS::Channel *);
        ~Channel();

        QUrl url() const;
        Model::NodeType type() const { return Model::ChannelNode; }
        QString name() const;
        LibDLS::Job *job() const { return ch->getJob(); }
        unsigned int dirIndex() const { return ch->dir_index(); }

        class Exception
        {
            public:
                Exception(const QString &);
                QString msg;
        };

        void fetchData(LibDLS::Time, LibDLS::Time, unsigned int,
                LibDLS::DataCallback,
                void *, unsigned int);
        bool beginExport(LibDLS::Export *, const QString &);

        struct TimeRange
        {
            LibDLS::Time start;
            LibDLS::Time end;
        };
        std::vector<TimeRange> chunkRanges();
        bool getRange(LibDLS::Time &, LibDLS::Time &);

        int rowCount() const;
        QVariant data(const QModelIndex &, int) const;
        void *child(int) const;
        int row(void *) const;
        Qt::ItemFlags flags() const;

    private:
        LibDLS::Channel * const ch;
        QReadWriteLock rwlock;
        std::vector<TimeRange> lastRanges;

        static bool range_before(const TimeRange &, const TimeRange &);

        Channel();
};

} // namespace

#endif

/****************************************************************************/
