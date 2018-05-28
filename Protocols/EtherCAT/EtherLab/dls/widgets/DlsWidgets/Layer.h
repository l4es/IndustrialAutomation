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

#ifndef DLS_LAYER_H
#define DLS_LAYER_H

#include <QColor>
#include <QMutex>

namespace LibDLS {
    class Channel;
    class Data;
}

namespace QtDls {
    class Channel;
}

namespace DLS {

class Section;
class GraphWorker;

/****************************************************************************/

/** Graph section layer.
 */
class Q_DECL_EXPORT Layer
{
    Q_DECLARE_TR_FUNCTIONS(Layer)

    public:
        Layer(Section *);
        Layer(const Layer &, Section *);
        virtual ~Layer();

        void load(const QDomElement &, QtDls::Model *, const QDir &);
        void save(QDomElement &, QDomDocument &) const;

        void connectChannel(QtDls::Model *, const QDir &);
        bool dirInUse(const LibDLS::Directory *) const;

        void setChannel(QtDls::Channel *);
        QtDls::Channel *getChannel() const { return channel; };

        const QString &getUrlString() const { return urlString; }

        void setName(const QString &);
        const QString &getName() const { return name; }
        void setUnit(const QString &);
        const QString &getUnit() const { return unit; }
        void setColor(QColor);
        QColor getColor() const { return color; }
        void setScale(double);
        double getScale() const { return scale; }
        void setOffset(double);
        double getOffset() const { return offset; }
        void setPrecision(int);
        int getPrecision() const { return precision; }

        void loadData(const LibDLS::Time &, const LibDLS::Time &, int,
                GraphWorker *, std::set<LibDLS::Job *> &);

        struct MeasureData {
            const Layer *layer;
            int x;
            double minimum;
            double maximum;
            int minY;
            int maxY;
            int meanY;
            unsigned int group;
            int movedY;
            bool found;

            bool operator<(const MeasureData &other) const {
                return minimum < other.minimum;
            }
        };

        void draw(QPainter &, const QRect &, double, double, double,
                MeasureData * = NULL);

        double getMinimum() const { return minimum; }
        double getMaximum() const { return maximum; }
        double getExtremaValid() const { return extremaValid; }

        QString title() const;
        QString formatValue(double) const;

        class Exception {
            public:
                Exception(const QString &msg):
                    msg(msg) {}
                QString msg;
        };
    private:
        Section * const section;
        QtDls::Channel *channel;
        QString urlString;
        QString name;
        QString unit;
        QColor color;
        double scale;
        double offset;
        int precision;

        QMutex dataMutex;
        QList<LibDLS::Data *> genericData;
        QList<LibDLS::Data *> minimumData;
        QList<LibDLS::Data *> maximumData;
        double minimum;
        double maximum;
        bool extremaValid;

        void newData(LibDLS::Data *);
        void clearDataList(QList<LibDLS::Data *> &);
        void copyDataList(QList<LibDLS::Data *> &,
                const QList<LibDLS::Data *> &);
        void updateExtrema();
        void updateExtremaList(const QList<LibDLS::Data *> &, bool *);
        void drawGaps(QPainter &, const QRect &, double) const;

        Layer(); // private
};

/****************************************************************************/

} // namespace

#endif
