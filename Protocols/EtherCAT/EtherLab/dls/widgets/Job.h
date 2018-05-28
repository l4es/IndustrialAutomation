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

#ifndef DLS_JOB_H
#define DLS_JOB_H

#include <QList>

#include "Node.h"

/*****************************************************************************/

namespace LibDLS {
    class Job;
}

namespace QtDls {

class Channel;

class Job:
   public Node
{
    public:
        Job(Node *, LibDLS::Job *);
        ~Job();

        QUrl url() const;
        Model::NodeType type() const { return Model::JobNode; }

        Channel *findChannel(const QString &);

        class Exception
        {
            public:
                Exception(const QString &);
                QString msg;
        };

        int rowCount() const;
        QVariant data(const QModelIndex &, int) const;
        void *child(int) const;
        int row(void *) const;

        LibDLS::Job *getJob() const { return job; }

    private:
        LibDLS::Job * const job;
        QList<Channel *> channels;

        Job();
};

} // namespace

#endif

/****************************************************************************/
