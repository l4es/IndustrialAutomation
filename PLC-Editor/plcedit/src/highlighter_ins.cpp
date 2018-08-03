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

#include "highlighter_ins.h"

Highlighter_Ins::Highlighter_Ins(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{ //! creating function
  //! \param QTextDocument

  //taken from the Qt 4.2/4.5 examples
  
  
  HighlightingRule rule;

  keywordFormat.setForeground(Qt::blue);
  keywordFormat.setFontWeight(QFont::Normal);
  QStringList keywordPatterns;
  keywordPatterns 
    				//some commands lower case
                    << "\\bld\\b"   << "\\bldn\\b"  << "\\bst\\b"   << "\\bstn\\b"   << "\\bs\\b"   << "\\br\\b"
                    << "\\bcal\\b"  << "\\bjmp\\b"  << "\\bjmpc\\b" << "\\bjmpcn\\b" << "\\bret\\b"
                    << "\\band\\b"  << "\\bandn\\b" << "\\bor\\b"   << "\\born\\b"   << "\\bxor\\b" << "\\bxorn\\b"
                    << "\\blt\\b"   << "\\ble\\b"   << "\\beq\\b"   << "\\bne\\b"    << "\\bge\\b"  << "\\bgt\\b"
                    << "\\badd\\b"  << "\\bsub\\b"  << "\\bmul\\b"  << "\\bdiv\\b"   << "\\bmod\\b"  
                    << "\\babs\\b"  << "\\bnot\\b"  << "\\brol\\b"  << "\\bror\\b"   << "\\bshl\\b" << "\\bshr\\b"
                    
    				//Std AWL
                    //register ops
                    << "\\bLD\\b"   << "\\bLDN\\b"  << "\\bST\\b"  	<< "\\bSTN\\b"  
                    << "\\bR\\b"    << "\\bS\\b"
                    //calls and jumps
                    << "\\bCAL\\b"  << "\\bCALC\\b" << "\\bCALCN\\b" 
                    << "\\bJMP\\b"  << "\\bJMPC\\b" << "\\bJMPCN\\b"
                    //logical ops
                    << "\\bAND\\b"  << "\\bANDN\\b" 
                    << "\\bOR\\b"   << "\\bORN\\b"  << "\\bXOR\\b"  << "\\bXORN\\b"  	<< "\\bXOR_bit\\b" 
                    << "\\bLT\\b"   << "\\bLE\\b"   << "\\bGE\\b"   << "\\bGT\\b"   	<< "\\bEQ\\b"  		<< "\\bNE\\b"  
                    << "\\bROL\\b"  << "\\bROR\\b"  << "\\bSHL\\b"  << "\\bSHR\\b"
                    << "\\bABS\\b"  << "\\bNOT\\b"  
                    //arithmetic ops  
                    << "\\bADD\\b"  << "\\bSUB\\b"  << "\\bMUL\\b"  << "\\bDIV\\b"  	<< "\\bMOD\\b"
                    //diverse
                    << "\\bRET\\b"  << "\\bRETC\\b"	

                    //Siemens AWL 
                    << "\\bCALL\\b" << "\\bSPA\\b"  << "\\bSPB\\b"  << "\\bSPBN\\b" 
                    << "\\bU\\b"    << "\\bO\\b"      
                    << "\\bTITLE\\b"<< "\\bNETWORK\\b" 
                     
                    //ST language
                    << "\\bIF\\b"    << "\\bTHEN\\b"   << "\\bELSE\\b"  	<< "\\bEND_IF\\b" 
                    << "\\bCASE\\b"  << "\\bOF\\b"     << "\\bEND_CASE\\b"
                    << "\\bFOR\\b"   << "\\bTO\\b"     << "\\bDO\\b"    	<< "\\bEND_FOR\\b"
                    << "\\bWHILE\\b" << "\\bEND_WHILE\\b"
                    << "\\bREPEAT\\b"<< "\\bUNTIL\\b"  << "\\bEND_REPEAT\\b"
                    << "\\bEXIT\\b"  << "\\bTRUE\\b"   << "\\bFALSE\\b"  	<< "\\bRETURN\\b"
                    
                    //Fcn.s
                    //selection ops
                    << "\\bSEL\\b"  		<< "\\bMIN\\b"  			<< "\\bMAX\\b"
                    << "\\bLIMIT\\b"		<< "\\bMUX\\b"
                    //string ops
                    << "\\bFIND\\b"             << "\\bMID\\b"  			<< "\\bREPLACE\\b"
                    << "\\bLEN\\b"              << "\\bLEFT\\b" 			<< "\\bDELETE\\b"
                    << "\\bRIGHT\\b"            << "\\bINSERT\\b"
                    //BCD
                    << "\\bBCD_TO_INT\\b"     << "\\bBCD_TO_DINT\\b"    << "\\bBCD_TO_STR\\b"
                    //BOOL_TO
                    << "\\bBOOL_TO_BYTE\\b"   << "\\bBOOL_TO_WORD\\b"   << "\\bBOOL_TO_DWORD\\b"
                    << "\\bBOOL_TO_UINT\\b"   << "\\bBOOL_TO_USINT\\b"  << "\\bBOOL_TO_SINT\\b" 
                    << "\\bBOOL_TO_INT\\b"    << "\\bBOOL_TO_DINT\\b"   << "\\bBOOL_TO_UDINT\\b"    
                    //BYTE_TO
                    << "\\bBYTE_TO_BOOL\\b"   << "\\bBYTE_TO_WORD\\b"   << "\\bBYTE_TO_DWORD\\b"
                    << "\\bBYTE_TO_UINT\\b"   << "\\bBYTE_TO_USINT\\b"  << "\\bBYTE_TO_SINT\\b" 
                    << "\\bBYTE_TO_INT\\b"    << "\\bBYTE_TO_DINT\\b"   << "\\bBYTE_TO_UDINT\\b"    
                    //WORD_TO
                    << "\\bWORD_TO_BOOL\\b"   << "\\bWORD_TO_BYTE\\b"   << "\\bWORD_TO_USINT\\b"   
                    << "\\bWORD_TO_UINT\\b"   << "\\bWORD_TO_SINT\\b"   << "\\bWORD_TO_INT\\b" 
                    << "\\bWORD_TO_DINT\\b"   << "\\bWORD_TO_UDINT\\b"  << "\\bWORD_TO_DWORD\\b" 
                    << "\\bWORD_TO_EXP\\b"
                    //DWORD_TO
                    << "\\bDWORD_TO_BOOL\\b"  << "\\bDWORD_TO_BYTE\\b"  << "\\bDWORD_TO_USINT\\b"   
                    << "\\bDWORD_TO_UINT\\b"  << "\\bDWORD_TO_SINT\\b"  << "\\bDWORD_TO_INT\\b" 
                    << "\\bDWORD_TO_DINT\\b"  << "\\bDWORD_TO_UDINT\\b" << "\\bDWORD_TO_WORD\\b" 
                    << "\\bDWORD_TO_EXP\\b"
                    //UINT_TO
                    << "\\bUINT_TO_BCD\\b"    << "\\bUINT_TO_BOOL\\b"   << "\\bUSINT_TO_BYTE\\b" 
                    << "\\bUINT_TO_USINT\\b"  << "\\bUINT_TO_SINT\\b"   << "\\bUINT_TO_INT\\b" 
                    << "\\bUINT_TO_DINT\\b"   << "\\bUINT_TO_UDINT\\b"  << "\\bUINT_TO_WORD\\b" 
                    << "\\bUINT_TO_DWORD\\b"  << "\\bUINT_TO_REAL\\b"
                    //USINT_TO
                    << "\\bUSINT_TO_BCD\\b"   << "\\bUSINT_TO_BOOL\\b"  << "\\bUSINT_TO_BYTE\\b"    
                    << "\\bUSINT_TO_UINT\\b"  << "\\bUSINT_TO_SINT\\b"  << "\\bUSINT_TO_INT\\b" 
                    << "\\bUSINT_TO_DINT\\b"  << "\\bUSINT_TO_UDINT\\b" << "\\bUSINT_TO_WORD\\b" 
                    << "\\bUSINT_TO_DWORD\\b" << "\\bUSINT_TO_REAL\\b"
                    //SINT_TO
                    << "\\bSINT_TO_BCD\\b"    << "\\bSINT_TO_BYTE\\b"   << "\\bSINT_TO_UINT\\b"   
                    << "\\bSINT_TO_USINT\\b"  << "\\bSINT_TO_INT\\b"    << "\\bSINT_TO_DINT\\b"  
                    << "\\bSINT_TO_UDINT\\b"  << "\\bSINT_TO_WORD\\b"   << "\\bSINT_TO_DWORD\\b" 
                    << "\\bSINT_TO_REAL\\b"
                    //INT_TO
                    << "\\bINT_TO_BCD\\b"     << "\\bINT_TO_BOOL\\b"    << "\\bINT_TO_BYTE\\b"    
                    << "\\bINT_TO_SINT\\b"    << "\\bINT_TO_USINT\\b"   << "\\bINT_TO_DINT\\b" 
                    << "\\bINT_TO_UDINT\\b"   << "\\bINT_TO_WORD\\b"    << "\\bINT_TO_DWORD\\b" 
                    << "\\bINT_TO_REAL\\b"    << "\\bINT_TO_TIME\\b"    << "\\bINT_TO_STRING\\b"
                    //DINT_TO
                    << "\\bDINT_TO_BCD\\b"    << "\\bDINT_TO_BOOL\\b"   << "\\bDINT_TO_BYTE\\b"    
                    << "\\bDINT_TO_SINT\\b"   << "\\bDINT_TO_USINT\\b"  << "\\bDINT_TO_INT\\b" 
                    << "\\bDINT_TO_UDINT\\b"  << "\\bDINT_TO_WORD\\b"   << "\\bDINT_TO_DWORD\\b" 
                    << "\\bDINT_TO_REAL\\b"
                    //UDINT_TO
                    << "\\bUDINT_TO_BCD\\b"   << "\\bUDINT_TO_BOOL\\b"  << "\\bUDINT_TO_BYTE\\b"    
                    << "\\bUDINT_TO_SINT\\b"  << "\\bUDINT_TO_USINT\\b" << "\\bUDINT_TO_DINT\\b" 
                    << "\\bUDINT_TO_INT\\b"   << "\\bUDINT_TO_WORD\\b"  << "\\bUDINT_TO_DWORD\\b" 
                    << "\\bUDINT_TO_REAL\\b"
                    //REAL_TO
                    << "\\bREAL_TO_UINT\\b"   << "\\bREAL_TO_USINT\\b"  << "\\bREAL_TO_SINT\\b" 
                    << "\\bREAL_TO_INT\\b"    << "\\bREAL_TO_DINT\\b"   << "\\bREAL_TO_UDINT\\b"    
                    << "\\bREAL_TO_STRING\\b" << "\\bTRUNC\\b"
                    //TIME_TO
                    << "\\bTIME_TO_BOOL\\b"   << "\\bTIME_TO_USINT\\b"  << "\\bTIME_TO_SINT\\b" 
                    << "\\bTIME_TO_INT\\b"    << "\\bTIME_TO_UINT\\b"   << "\\bTIME_TO_DINT\\b" 
                    << "\\bTIME_TO_WORD\\b"   << "\\bTIME_TO_DWORD\\b"  << "\\bTIME_TO_STRING\\b" 
                    //STRING_TO
                    << "\\bSTRING_TO_BCD\\b"  << "\\bSTRING_TO_BOOL\\b" << "\\bSTRING_TO_INT\\b" 
                    << "\\bSTRING_TO_DINT\\b" << "\\bSTRING_TO_WORD\\b" << "\\bSTRING_TO_DWORD\\b" 
                    << "\\bSTRING_TO_REAL\\b" << "\\bSTRING_TO_TIME\\b"
                    ;
                    
                    
  foreach (const QString &pattern, keywordPatterns) 
  {
    rule.pattern = QRegExp(pattern);
    rule.format = keywordFormat;
    highlightingRules.append(rule);
  }

  singleLineCommentFormat.setForeground(Qt::darkGreen);
  rule.pattern = QRegExp("\\#\\*[^\n]\\*\\#");
  //rule.pattern = QRegExp("//[^\n]*");
  rule.format = singleLineCommentFormat;
  highlightingRules.append(rule);
    
  multiLineCommentFormat.setForeground(Qt::darkGreen);
  commentStartExpression = QRegExp("\\(\\*");
  commentEndExpression = QRegExp("\\*\\)");

}


void Highlighter_Ins::highlightBlock(const QString &text)
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
