/****************************************************************************
**
** This file is part of PLCEdit, an open-source cross-platform editor 
** for PLC source files (e.g. from Moeller, CoDeSys and Siemens).
** Copyright (C) 2005-2010  M. Rehfeldt
**
** This software uses classes of Trolltech Qt toolkit and is freeware. 
** This file may be used under the terms of the GNU General Public License 
** version 2.0 or (at your option) any later version as published by the 
** Free Software Foundation and appearing in the file LICENSE.GPL included 
** in the packaging of this file. 
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** Contact e-mail: M. Rehfeldt <info@plcedit.org>
** Program URL   : http://www.plcedit.org
**
****************************************************************************/


#ifndef HIGHLIGHTER_INS_H
#define HIGHLIGHTER_INS_H

#include <QSyntaxHighlighter>

#include <QHash>
#include <QTextCharFormat>

class QTextDocument;


class Highlighter_Ins : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    Highlighter_Ins(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text);

private:
    struct HighlightingRule
    { //!< highlighting rule structure
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules; //!< vector

    QRegExp commentStartExpression; 			//!< regular expression for comment section start item
    QRegExp commentEndExpression; 				//!< regular expression for comment section end item

    QTextCharFormat keywordFormat;				//!< text format for keyword
    QTextCharFormat singleLineCommentFormat;	//!< text format for single comment
    QTextCharFormat multiLineCommentFormat;		//!< text format for multi line comment

};

#endif
