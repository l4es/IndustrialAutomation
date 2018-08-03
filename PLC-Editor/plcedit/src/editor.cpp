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


#include "editor.h"


    //	acceptable colors :
    //	(255, 255, 192) - light yellow 		(yellow)
    //	(255, 250, 205) - lemon chiffon 	(yellow)
    //	(222, 184, 135) - burly wood 		(brown)
    //	(255, 192, 192) - light red 		(red)
    // 	(255, 225, 255) - thistle1  		(red)
    //	(240, 230, 140) - khaki 			(green)
    // 	(193, 255, 193) - DarkSeaGreen1		(green)
    //	(224, 255, 255) - light cyan 		(blue)
    //	(202, 225, 255) - light steel blue1 (blue)
    //	(192, 192, 192) - grey 				(grey)
    //	(240, 255, 240) - honney dew 		(grey)



Editor::Editor(QWidget *parent) 
  : QTextEdit(parent)
{ //! creating function

  //Thanks to: QtSoftware, S. Weinzierl and J. L. Biord
 
 
  setLineWrapMode(QTextEdit::NoWrap);			//set line wrap mode to NO
  setUndoRedoEnabled(true);						//set undo/redo enabled
  
  prefWidth = 45; 								//max. width of line numbers column
  setViewportMargins(prefWidth, 0, 0, 0);
  
  connect(verticalScrollBar(), 	SIGNAL(valueChanged(int)), 			this, SLOT(update()));
  connect(this, 				SIGNAL(textChanged()), 				this, SLOT(update()));
  connect(this, 				SIGNAL(cursorPositionChanged()),	this, SLOT(cursorPositionChangedOperations()));
  connect(this->document(),		SIGNAL(modificationChanged(bool)), 	this, SIGNAL(contentModified(bool)));
  
  retranslateUi();
}


Editor::~Editor()
{ //! destructor

}




//=============================================================================
// - - - protected Fcn.s - - -
/*
void Editor::contextMenuEvent(QContextMenuEvent *event) 
{ //! reimplementation of context menu event to insert 'compare with...' item
 
  QList<QAction*> actionList;										//create action list
  compareWithAction = new QAction(this);							//create compare with action
  compareWithAction->setText(str_contextmenu_comparewith);
  connect(compareWithAction, SIGNAL(triggered()), this, SIGNAL(compareWith()));
  compareWithNextAction = new QAction(this);						//create compare with next action
  compareWithNextAction->setText(str_contextmenu_comparewithnext);
  connect(compareWithNextAction, SIGNAL(triggered()), this, SIGNAL(compareWithNext()));
  compareWithPrevAction = new QAction(this);						//create compare with previous action
  compareWithPrevAction->setText(str_contextmenu_comparewithprev);
  connect(compareWithPrevAction, SIGNAL(triggered()), this, SIGNAL(compareWithPrev()));

  QMenu* contextMenu = createStandardContextMenu();					//create standard context menu
  
  actionList = contextMenu->actions();								//fill action list
  contextMenu->insertAction(actionList[0],compareWithAction);		//insert compare with action
  contextMenu->insertAction(actionList[0],compareWithNextAction);	//insert compare with next action
  contextMenu->insertAction(actionList[0],compareWithPrevAction);	//insert compare with previous action
  contextMenu->insertSeparator(actionList[0]);						//inser seperator
  contextMenu->exec(event->globalPos());							//execute context menu
  delete contextMenu;												//delete context menu 
}
*/

bool Editor::event(QEvent *event) 
{ //! line numbers block paint event

  if (event->type() == QEvent::Paint) 
  {
    QPainter painter(this);

    //painting basic line numbering area
    painter.fillRect(prefWidth, 0, 3, height(), QColor(192, 192, 192)); //small sep. rect. in grey as line numbering area limiter
    painter.fillRect(0, 0, prefWidth, height(), QColor(hiLineColor));   //filling line numbering rect., lemon chiffon; standard color setting for line numbering area: QColor(255, 250, 205)

    //coloring line numbering area at curent cursor position
    if (enableHighlightLines == true) //prevent colors which would hide the line numbers
        painter.fillRect(0, cursorRect().y(), prefWidth, cursorRect().height(), QColor(hiLineColor)); //current lines rect. painted with hiLineColor

    //framing line numbering area at curent cursor position
    painter.fillRect(0, cursorRect().y()+1, prefWidth, 1, QColor(192, 192, 192)); //small sep. rect. in grey above current line numer
    painter.fillRect(0, cursorRect().y()+cursorRect().height(), prefWidth, 1, QColor(192, 192, 192)); //small sep. rect. in grey below current line number

    //get scroll area data
    int vscrollValue = verticalScrollBar()->value();
    qreal pageBottom = vscrollValue + viewport()->height();
    int lineNumber(1);
    //int currentLine = textCursor().blockNumber()+1; Not used

    //get font metrics
    const QFontMetrics fm=fontMetrics();
    const int fontAscent = fontMetrics().ascent()+1;

    //writing line numbers
    for (QTextBlock block = document()->begin(); block.isValid(); block = block.next(), lineNumber++) 
    {
      QTextLayout *layout = block.layout();
      const QRectF boundingRect = layout->boundingRect();
      QPointF position = layout->position();
      
      if ( position.y() + boundingRect.height() < vscrollValue )
        continue;
      if ( position.y() > pageBottom ) 
        break;
       
      const QString str = QString::number(lineNumber);
      painter.drawText(prefWidth - fm.width(str)-3, qRound(position.y()) - vscrollValue + fontAscent, str);
    }
  }

  return QTextEdit::event(event);
}




//=============================================================================
// - - - private Fcn.s - - -
void Editor::paintEvent(QPaintEvent *event)
{ //! painter event

  //set painter
  QPainter painter(viewport());
 
  //set lines highlighter
  if (enableHighlightLines == true)
  {
    QRect r = cursorRect();
    r.setX(0);
    r.setWidth(viewport()->width());
    painter.fillRect(r, QBrush(hiLineColor));
  }

  //set print area indicator
  if (printArea > 0) 
  {
    int x1=(printArea * 8), x2=x1, y1=0, y2= viewport()->height();
    //test line: 1111111111222222222233333333334444444444555555555566666666667777777777888888888899999999990000000000
    QLine line = QLine(x1, y1, x2, y2);
    painter.setPen(Qt::DotLine);
    painter.drawLine(line);
  }
 
  painter.end();
  QTextEdit::paintEvent(event);
}




//=============================================================================
// - - - public Fcn.c - - -
bool Editor::contentIsModified()
{ //! get modified status of document

  return(this->document()->isModified());
}


void Editor::retranslateUi()
{ //! retranslate Ui

  str_contextmenu_comparewith = tr("&Compare with...");
  str_contextmenu_comparewithnext = tr("Compare with next");
  str_contextmenu_comparewithprev = tr("Compare with previous");

}


void Editor::setContentModified(bool modified)
{ //! set modification satus of document

  this->document()->setModified(modified);
}




//=============================================================================
// - - - private SLOTs - - -
void Editor::cursorPositionChangedOperations()
{ //! cursor position changed handling to update the viewport and line numbers column
	
  viewport()->update(); //update viewport, needed for correct lines highlighter
  /*disabled because of inhibition of repainting framing area in line numbering area
  if (enableHighlightLines == true) //only repaint line numbers column if enabled
  */
  repaint();			//repaint, needed for updating line numbers column with current lines rect.
}
