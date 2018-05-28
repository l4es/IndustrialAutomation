/*****************************************************************************
 *
 * $Id$
 *
 ****************************************************************************/

#include <QDebug>
#include <QStringList>
#include <QMimeData>
#include <QUrl>

#include <LibDLS/Dir.h>

#include "DlsWidgets/Model.h"
#include "DlsWidgets/Graph.h"
#include "Dir.h"
#include "Channel.h"

using namespace QtDls;

/*****************************************************************************/

Model::Model()
{
}

/****************************************************************************/

Model::~Model()
{
    clear();
}

/****************************************************************************/

void Model::addLocalDir(
        LibDLS::Directory *d
        )
{
    Dir *dir = new Dir(this, d);
    beginInsertRows(QModelIndex(), dirs.count(), dirs.count());
    dirs.push_back(dir);
    endInsertRows();
}

/****************************************************************************/

void Model::removeDir(
        LibDLS::Directory *remDir
        )
{
    int row = 0;

    for (QList<Dir *>::iterator d = dirs.begin(); d != dirs.end();
            d++, row++) {
        if (remDir == (*d)->getDir()) {
            beginRemoveRows(QModelIndex(), row, row);
            dirs.removeAt(row);
            delete remDir;
            endRemoveRows();
            return;
        }
    }
}

/****************************************************************************/

void Model::clear()
{
    if (dirs.empty()) {
        return;
    }

    beginRemoveRows(QModelIndex(), 0, dirs.count() - 1);

    while (!dirs.empty()) {
        delete dirs.front();
        dirs.pop_front();
    }

    endRemoveRows();
}

/****************************************************************************/

void Model::update()
{
    for (QList<Dir *>::iterator d = dirs.begin(); d != dirs.end(); d++) {
        (*d)->getDir()->import();
    }
}

/****************************************************************************/

bool Model::hasUnusedDirs(DLS::Graph *graph) const
{
    bool hasUnused = false;

    for (QList<Dir *>::const_iterator d = dirs.begin(); d != dirs.end(); d++) {
        hasUnused = !graph->dirInUse((*d)->getDir());
        if (hasUnused) {
            break;
        }
    }

    return hasUnused;
}

/****************************************************************************/

void Model::removeUnusedDirs(DLS::Graph *graph)
{
    QList<Dir *> unused;

    for (QList<Dir *>::const_iterator d = dirs.begin(); d != dirs.end(); d++) {
        if (!graph->dirInUse((*d)->getDir())) {
            unused.push_back(*d);
        }
    }

    for (QList<Dir *>::const_iterator d = unused.begin(); d != unused.end();
            d++) {
        removeDir((*d)->getDir());
    }
}

/****************************************************************************/

struct ChannelLocator {
    QString dirPath;
    unsigned int jobId;
    QString channelName;
    bool dirExists;
};

QtDls::Channel *Model::getChannel(QUrl url)
{
    if (!url.scheme().isEmpty() && url.scheme() != "file"
            && url.scheme() != "dls") {
        QString err = QString("URL scheme \"%1\" is not supported!")
            .arg(url.scheme());
        throw Exception(err);
    }

    /* the jobNNN component can show up multiple times in the URL path. To
     * determine, which one corresponds to the job directory, we must try, if
     * there is an existing dir for each one. If no existing directory was
     * found, we have to search again for the next occurrence of jobNNN. */

    QList<ChannelLocator> locList;
    QStringList comp = url.path().split('/');

    for (int i = 0; i < comp.size(); i++) {
        if (!comp[i].startsWith("job")) {
            continue;
        }
        QString rem = comp[i].mid(3);
        ChannelLocator loc;
        loc.dirExists = false;
        bool ok;
        loc.jobId = rem.toUInt(&ok, 10);
        if (!ok) {
            continue;
        }
        QStringList dirPathComp = comp.mid(0, i);
        loc.dirPath = dirPathComp.join("/");
        QStringList channelNameComp = comp.mid(i + 1);
        loc.channelName = "/" + channelNameComp.join("/");
        locList.append(loc);
#if 0
        qDebug() << "Locator" << loc.dirPath
            << loc.jobId << loc.channelName;
#endif
    }

    if (locList.empty()) {
        QString err = QString("URL %1 invalid because of missing job!")
            .arg(url.toString());
        throw Exception(err);
    }

    // try to find an existing dir with matching path

    for (QList<ChannelLocator>::iterator loc = locList.begin();
            loc != locList.end(); loc++) {
        for (QList<Dir *>::iterator d = dirs.begin(); d != dirs.end(); d++) {
            QString dirPath = (*d)->getDir()->path().c_str();
            if (loc->dirPath != dirPath) {
                continue;
            }

            loc->dirExists = true;

            QtDls::Channel *ch =
                (*d)->findChannel(loc->jobId, loc->channelName);
            if (ch) {
                return ch;
            }
        }
    }

    // try to create a new dir for each valid combination

    for (QList<ChannelLocator>::iterator loc = locList.begin();
            loc != locList.end(); loc++) {
        if (loc->dirExists) {
            continue;
        }

        LibDLS::Directory *d = new LibDLS::Directory();
        QString uriText = url.toString(QUrl::RemovePath) + loc->dirPath;
#if 0
        qDebug() << "Trying new dir" << uriText;
#endif

        try {
            d->set_uri(uriText.toUtf8().constData()); // FIXME enc?
        }
        catch (LibDLS::DirectoryException &e) {
            qWarning() << "Invalid URL found: " << e.msg.c_str();
            delete d;
            continue;
        }

        qDebug() << "Adding directory" << uriText;

        Dir *dir = new Dir(this, d);
        beginInsertRows(QModelIndex(), dirs.count(), dirs.count());
        dirs.push_back(dir);
        endInsertRows();

        try {
            d->import();
        }
        catch (LibDLS::DirectoryException &e) {
            continue;
        }

        Channel *ch = dir->findChannel(loc->jobId, loc->channelName);

        if (!ch) {
            continue;
        }

        return ch;
    }

    QString err = QString("Channel %1 not found!").arg(url.toString());
    throw Exception(err);
}

/****************************************************************************/

Model::NodeType Model::nodeType(const QModelIndex &index) const
{
    if (index.isValid()) {
        Node *n = (Node *) index.internalPointer();
        return n->type();
    } else {
        return InvalidNode;
    }
}

/****************************************************************************/

LibDLS::Directory *Model::dir(const QModelIndex &index)
{
    LibDLS::Directory *dir = NULL;

    if (nodeType(index) == DirNode) {
        Node *n = (Node *) index.internalPointer();
        Dir *d = dynamic_cast<Dir *>(n);
        dir = d->getDir();
    }

    return dir;
}

/****************************************************************************/

/** Implements the model interface.
 */
int Model::rowCount(const QModelIndex &index) const
{
    int ret = 0;

    if (index.isValid()) {
        if (index.column() == 0 && index.internalPointer()) {
            Node *n = (Node *) index.internalPointer();
            ret = n->rowCount();
        }
    } else {
        ret = dirs.count();
    }

    return ret;
}

/****************************************************************************/

/** Implements the model interface.
 *
 * \returns Number of columns.
 */
int Model::columnCount(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return 1;
}

/****************************************************************************/

/** Implements the Model interface.
 */
QModelIndex Model::index(int row, int col, const QModelIndex &parent) const
{
    QModelIndex ret;

    if (row < 0 || col < 0) {
        return ret;
    }

    if (parent.isValid()) {
        Node *n = (Node *) parent.internalPointer();
        ret = createIndex(row, col, n->child(row));
    } else {
        if (row < dirs.count()) {
            ret = createIndex(row, col, dirs[row]);
        }
    }

    return ret;
}

/****************************************************************************/

/** Implements the Model interface.
 */
QModelIndex Model::parent(const QModelIndex &index) const
{
    QModelIndex ret;

    if (index.isValid()) {
        Node *n = (Node *) index.internalPointer();
        Node *p = n->parent();
        if (p) {
            int row;
            Node *pp = p->parent(); // grandparent to get parent row
            if (pp) {
                row = pp->row(p);
            } else {
                Dir *d = dynamic_cast<Dir *>(p);
                row = dirs.indexOf(d);
            }
            ret = createIndex(row, 0, p);
        }
    }

    return ret;
}

/****************************************************************************/

/** Implements the Model interface.
 */
QVariant Model::data(const QModelIndex &index, int role) const
{
    QVariant ret;

    if (index.isValid()) {
        Node *n = (Node *) index.internalPointer();
        ret = n->data(index, role);
    }

    return ret;
}

/****************************************************************************/

/** Implements the Model interface.
 */
QVariant Model::headerData(
        int section,
        Qt::Orientation o,
        int role
        ) const
{
    Q_UNUSED(section);
    Q_UNUSED(o);
    Q_UNUSED(role);
    return QVariant();
}

/****************************************************************************/

/** Implements the Model interface.
 */
Qt::ItemFlags Model::flags(const QModelIndex &index) const
{
    Qt::ItemFlags f;

    if (index.isValid()) {
        f |= Qt::ItemIsEnabled;
        Node *n = (Node *) index.internalPointer();
        f |= n->flags();
    }

    return f;
}

/*****************************************************************************/

QStringList Model::mimeTypes() const
{
    QStringList types;
    types << "text/uri-list";
    return types;
}

/*****************************************************************************/

QMimeData *Model::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QList<QUrl> urls;

    foreach (QModelIndex index, indexes) {
        if (index.isValid()) {
            Node *n = (Node *) index.internalPointer();
            Channel *c = dynamic_cast<Channel *>(n);
            urls.append(c->url());
        }
    }

    mimeData->setUrls(urls);
    return mimeData;
}

/*****************************************************************************/

void Model::prepareLayoutChange()
{
    emit layoutAboutToBeChanged();
}

/*****************************************************************************/

void Model::finishLayoutChange()
{
    emit layoutChanged();
}

/*****************************************************************************/

Model::Exception::Exception(const QString &what):
    msg(what)
{
}

/****************************************************************************/
