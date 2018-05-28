/*****************************************************************************
 *
 * Copyright (C) 2009 - 2017  Florian Pose <fp@igh-essen.com>
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

#ifndef DLS_MODEL_H
#define DLS_MODEL_H

#include <QList>
#include <QString>
#include <QAbstractItemModel>

/*****************************************************************************/

namespace LibDLS {
    class Directory;
}

namespace DLS {
    class Graph;
    class Section;
    class Layer;
}

namespace QtDls {

class Dir;
class Channel;

class Q_DECL_EXPORT Model:
    public QAbstractItemModel
{
    friend class DLS::Graph;
    friend class DLS::Section;
    friend class DLS::Layer;
    friend class Dir;

    public:
        Model();
        ~Model();

        void addLocalDir(LibDLS::Directory *);
        void removeDir(LibDLS::Directory *);
        void clear();

        void update();

        bool hasUnusedDirs(DLS::Graph *) const;
        void removeUnusedDirs(DLS::Graph *);

        enum NodeType { InvalidNode, DirNode, JobNode, ChannelNode };
        NodeType nodeType(const QModelIndex &) const;

        LibDLS::Directory *dir(const QModelIndex &);

        Channel *getChannel(QUrl);

        class Exception
        {
            public:
                Exception(const QString &);
                QString msg;
        };

        // from QAbstractItemModel
        int rowCount(const QModelIndex &) const;
        int columnCount(const QModelIndex &) const;
        QModelIndex index(int, int, const QModelIndex &) const;
        QModelIndex parent(const QModelIndex &) const;
        QVariant data(const QModelIndex &, int) const;
        QVariant headerData(int, Qt::Orientation, int) const;
        Qt::ItemFlags flags(const QModelIndex &) const;
        QStringList mimeTypes() const;
        QMimeData *mimeData(const QModelIndexList &) const;

    protected:
        void prepareLayoutChange();
        void finishLayoutChange();

    private:
        QList<Dir *> dirs;
};

} // namespace

#endif

/****************************************************************************/
