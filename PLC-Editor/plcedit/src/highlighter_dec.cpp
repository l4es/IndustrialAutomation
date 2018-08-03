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


#include <QtGui>

#include "highlighter_dec.h"

Highlighter_Dec::Highlighter_Dec(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{ //! creating function
  //! \param QTextDocument

  //taken from the Qt 4.2/4.5 examples
  
  
  HighlightingRule rule;

  keywordFormat.setForeground(Qt::blue);
  keywordFormat.setFontWeight(QFont::Normal);
  QStringList keywordPatterns;
  keywordPatterns //data types
                    << "\\bBOOL\\b" 	<< "\\bBYTE\\b"         << "\\bWORD\\b" 	<< "\\bDWORD\\b"        << "\\bLWORD\\b"
                    << "\\bSINT\\b" 	<< "\\bINT\\b"      	<< "\\bDINT\\b" 	<< "\\bLINT\\b" 	
                    << "\\bUSINT\\b"	<< "\\bUINT\\b" 	<< "\\bUDINT\\b" 	<< "\\bULINT\\b"
                    << "\\bTIME\\b" 	<< "\\bDATE\\b"		<< "\\bDATE_AND_TIME\\b"                        << "\\bTIME_OF_DAY\\b"
                    << "\\bDT\\b"
                    << "\\bREAL\\b" 	<< "\\bLREAL\\b"	<< "\\bARRAY\\b"	<< "\\bOF\\b"
                    << "\\bSTRING\\b"   << "\\bENUM\\b"	<< "\\bPOINTER TO\\b"
                    //block marks
                    << "\\bVAR\\b"      << "\\bVAR_GLOBAL\\b" 	<< "\\bVAR_EXTERNAL\\b" 
                    << "\\bVAR_INPUT\\b"<< "\\bVAR_OUTPUT\\b" 	<< "\\bVAR_IN_OUT\\b"
                    << "\\bEND_VAR\\b"  
                    << "\\bTYPE\\b"     << "\\bEND_TYPE\\b"	<< "\\bSTRUCT\\b" 	<< "\\bEND_STRUCT\\b"
                    //diverse
                    << "\\bAT\\b"   	 << "\\bTRUE\\b" 	<< "\\bFALSE\\b"
                    //IEC61131 functions
                    << "\\bTON\\b"   	 << "\\bTOF\\b" 	<< "\\bTP\\b"
                    << "\\bR_TRIG\\b"   << "\\bF_TRIG\\b" 	<< "\\bR_EDGE\\b"       << "\\bF_EDGE\\b"
                    ;
                
                    
  foreach (const QString &pattern, keywordPatterns) 
  {
    rule.pattern = QRegExp(pattern);
    rule.format = keywordFormat;
    highlightingRules.append(rule);
  }

  singleLineCommentFormat.setForeground(Qt::darkGreen);
  rule.pattern = QRegExp("//[^\n]*");
  rule.format = singleLineCommentFormat;
  highlightingRules.append(rule);
    
  multiLineCommentFormat.setForeground(Qt::darkGreen);
  commentStartExpression = QRegExp("\\(\\*");
  commentEndExpression = QRegExp("\\*\\)");

}


void Highlighter_Dec::highlightBlock(const QString &text)
{ //! set highlighting block
  
  foreach (const HighlightingRule &rule, highlightingRules) 
  {
    QRegExp expression(rule.pattern);
    int index = expression.indexIn(text);
    while (index >= 0) 
    {
      int length = expression.matchedLength();
      setFormat(index, length, rule.format);
      index = expression.indexIn(text, index + length);
    }
  }
  setCurrentBlockState(0);

  int startIndex = 0;
  if (previousBlockState() != 1)
    startIndex = commentStartExpression.indexIn(text);

  while (startIndex >= 0) 
  {
    int endIndex = commentEndExpression.indexIn(text, startIndex);
    int commentLength;
    if (endIndex == -1) 
    {
      setCurrentBlockState(1);
      commentLength = text.length() - startIndex;
    } 
    else 
    {
      commentLength = endIndex - startIndex + commentEndExpression.matchedLength();
    }
    
    setFormat(startIndex, commentLength, multiLineCommentFormat);
    startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
  }
  
}
