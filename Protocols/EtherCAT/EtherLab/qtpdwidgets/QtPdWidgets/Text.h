/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2009 - 2013  Florian Pose <fp@igh-essen.com>
 *
 * This file is part of the QtPdWidgets library.
 *
 * The QtPdWidgets library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * The QtPdWidgets library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the QtPdWidgets Library. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

#ifndef PD_TEXT_H
#define PD_TEXT_H

#include <QFrame>
#include <QHash>
#include <QTimer>

#include "Export.h"
#include "ScalarSubscriber.h"
#include "ScalarVariable.h"

namespace Pd {

class Text;

/** Condition type.
 */
class TextCondition:
    public BoolVariable
{
    Q_OBJECT

    public:
        TextCondition(Text *);

        void setText(const QString &);
        const QString &getText() const { return text; }

        void setInvert(bool);
        bool getInvert() const { return invert; }

    private:
        Text * const parent;
        QString text;
        bool invert;

        TextCondition();
};

/****************************************************************************/

/** Text display widget.
 *
 * \todo Default text property.
 */
class QDESIGNER_WIDGET_EXPORT Text:
    public QFrame, public ScalarSubscriber
{
    Q_OBJECT

    friend class TextCondition;

    Q_PROPERTY(Qt::Alignment alignment
            READ getAlignment WRITE setAlignment RESET resetAlignment)
    Q_PROPERTY(QString prefix
            READ getPrefix WRITE setPrefix RESET resetPrefix)
    Q_PROPERTY(QString suffix
            READ getSuffix WRITE setSuffix RESET resetSuffix)
    Q_PROPERTY(bool conditionActive READ getConditionActive)

    public:
        Text(QWidget *parent = 0);
        virtual ~Text();

        Qt::Alignment getAlignment() const { return alignment; }
        void setAlignment(Qt::Alignment);
        void resetAlignment();

        const QString &getPrefix() const { return prefix; }
        void setPrefix(const QString &);
        void resetPrefix();

        const QString &getSuffix() const {return suffix; }
        void setSuffix(const QString &);
        void resetSuffix();

        void clearData(); // pure-virtual from ScalarSubscriber

        int getValue() const { return processValue; }
        void setValue(int);

        /** Value type.
         *
         * A tuple of text, text color and font, that can be provided for each
         * process value.
         */
        struct Value {
            QString text; /**< Displayed text. */
            QColor color; /**< Text color. */
            QFont font; /**< Font to use. */

            Value(
                    const QString &t = QString(), /**< Text. */
                    QColor c = Qt::black, /**< Color. */
                    const QFont &f = QFont() /**< Font. */
                    ):
                text(t), color(c), font(f) {}
        };

        /** Value hash type.
         *
         * This hash shall contain a value object for each possible value to
         * display.
         */
        class Q_DECL_EXPORT Hash: public QHash<int, Value> {
            public:
                void insert(
                        int position,
                        const QString &t = QString(),
                        QColor c = Qt::black,
                        const QFont &f = QFont());
        };
        void setHash(const Hash *);

        void updateValueText();

        TextCondition *addCondition(PdCom::Variable *,
                const QString & = QString(), bool = false);
        void clearConditions();

        bool getConditionActive() const { return conditionActive; }

    protected:
        bool event(QEvent *);
        void paintEvent(QPaintEvent *);

    private:
        Qt::Alignment alignment; /**< Text alignment. */
        QString prefix; /**< Prefix to display before the #displayValue text.
                         */
        QString suffix; /**< Suffix to display after the #displayValue text.
                         */
        int processValue; /**< The current value from the process. */
        bool dataPresent; /**< There is a process value to display. */
        const Hash *hash; /**< The hash to use. */
        Value displayValue; /**< Value object to display. */
        QString displayText; /**< Display text. */
        QColor displayColor; /**< Display color. */
        QFont displayFont; /**< Display font. */
        QList<TextCondition *> conditions;
        int conditionIndex;
        QTimer conditionTimer;
        bool conditionActive;

        void notify(PdCom::Variable *); // pure-virtual from PdCom::Subscriber

        void updateDisplayText();
        void findCondition();

        void retranslate();

    private slots:
        void conditionChanged();
        void conditionTimeout();
};

/****************************************************************************/

} // namespace

#endif
