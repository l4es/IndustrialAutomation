/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2009 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */
/*
Header for: validated widgets
*/
#include "valedit.h"
#include <fstream.h>
#include <stdlib.h>
#include <ctype.h>
#include "DateNavigator.h"
/*
*Function:VALEDIT::keyPressEvent(QKeyEvent *e)
*Inputs:none
*Outputs:none
*Returns:none
*/
void VALEDIT::keyPressEvent(QKeyEvent *e)
{
	bool ignore = false;
	switch(e->key())
	{
		case Key_Delete: 
		case Key_Left://APA to be commented?
		case Key_Backspace://APA to be commented?
		case Key_Right:
		break;
		default:
		{
			if(CharValid.contains(e->text()) == 0)
			{
				ignore = true;
			};
			break;
		};
	};
	if(!ignore)
	{
		QLineEdit::keyPressEvent(e);
	};
}; 
/*
*Function:NAMEEDIT::keyPressEvent(QKeyEvent *e)
*Inputs:none
*Outputs:none
*Returns:none
*/
void NAMEEDIT::keyPressEvent(QKeyEvent *e)
{
	bool ignore = false;
	switch(e->key())
	{
		case Key_Delete: 
		case Key_Left:
		case Key_Backspace:
		case Key_Right:
		break;
		default:
		{
			if(!strchr("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",e->ascii()))
			{
				ignore = true;
			}
			else
			{
				if(!cursorPosition())
				{
					if(isdigit(e->ascii()))
					{
						ignore = true;
					};
				};
			};
		};
		break;
	};
	if(!ignore)
	{
		QLineEdit::keyPressEvent(e);
	};
}; 
/*
*Function:void INTEDIT::keyPressEvent(QKeyEvent *e)
*Inputs:none
*Outputs:none
*Returns:none
*/
void INTEDIT::keyPressEvent(QKeyEvent *e)
{
	switch(e->key())
	{
		case Key_Minus:
		{
			if(cursorPosition() != 0)
			{
				return;
			};			
		};
		break;
		default:
		break;
	};
	VALEDIT::keyPressEvent(e);
};

/*
*Function:void INTEDIT::keyPressEvent(QKeyEvent *e)
*Inputs:none
*Outputs:none
*Returns:none
*/
void FPEDIT::keyPressEvent(QKeyEvent *e)
{
	switch(e->key())
	{
		case Key_Colon:
			return;
		case Key_Minus:
		{
			if(cursorPosition() != 0)
			{
				return;
			};			
		};
		break;
		//removed on 06-10-2003
//		case Key_Period:
//		{
//			dot_counter++;
//			if(dot_counter > 1)
//			{
//				return;
//			};			
//		};
//		break;
		default:
		break;
	};
	VALEDIT::keyPressEvent(e);
};

/*
*Function:void TIMEEDIT::keyPressEvent(QKeyEvent *e)
*Inputs:none
*Outputs:none
*Returns:none
*/
void TIMEEDIT::keyPressEvent(QKeyEvent *e)
{
	if(text().length() != 8)
	{
		setText("00:00:00"); // handle cut and paste
	};
	switch(e->key())
	{
		case Key_Left:
		case Key_Right:
		QLineEdit::keyPressEvent(e);
		return;
		default:
		{
			switch(cursorPosition())
			{
				case 0: // 0 - 2
				{
					if((e->ascii() < '0') || (e->ascii() > '2'))
					{
						return;
					};
				};
				break;
				case 1: 
				{
					QString s = text(); 
					if(s[0] == '2')
					{
						if((e->ascii() < '0') || (e->ascii() > '3'))
						{
							return;
						};
					}
					else
					{
						if((e->ascii() < '0') || (e->ascii() > '9'))
						{
							return;
						};
					};
				};
				break;
				case 2: 
				case 5:
				{
					QKeyEvent ev(QEvent::KeyPress,Key_Colon,':',0);
					QLineEdit::keyPressEvent(&ev);
					cursorRight(false);
				};
				return; 
				case 3: 
				case 6:
				if((e->ascii() < '0') || (e->ascii() > '5'))
				{
					return;
				};
				break;
				case 4: 
				case 7:
				if((e->ascii() < '0') || (e->ascii() > '9'))
				{
					return;
				};
				break;
				default:
				return;
			};		
		};
		break;
	};
	del();	
	QLineEdit::keyPressEvent(e);
};
/*
*Function:void TIMEEDIT::mousePressEvent(QMouseEvent *e) 
*Inputs:none
*Outputs:none
*Returns:none
*/
void TIMEEDIT::mousePressEvent(QMouseEvent *e) 
{
if(e->button() != Qt::RightButton)
{
	QLineEdit::mousePressEvent(e);
};
};
/*
*Function: DATEEDIT::LineValidate()
*Inputs:none
*Outputs:none
*Returns:none
*/
static int dom[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
// 
// 
void DATEEDIT::LineValidate()
{
	bool Ok = true;
	QString s = text();
	const char *p = (const char *)s;
	//
	int d = atoi(p + 8);
	int m = atoi(p + 5);
	int y = atoi(p);
	if((m < 1) || (m > 12))
	{
		m = 1;
		Ok = false;
	}
	//
	if((d < 1) || (d > dom[m]))
	{
		if(m == 2)
		{
			if(d == 29)
			{
				if(y % 4)
				{
					d = 28; // not a leap year
					Ok = false;
				};
			}
			else
			{
				d = 28;
				Ok = false;
			};
		}
		else
		{
			d = dom[m];
			Ok = false;
		};
	};
	if(!Ok)
	{
		s.sprintf("%04d-%02d-%02d",y,m,d);
		setText(s);
	};
};
/*
*Function:void DATEEDIT::keyPressEvent(QKeyEvent *e)
*Inputs:none
*Outputs:none
*Returns:none
*/
void DATEEDIT::keyPressEvent(QKeyEvent *e)
{
	switch(e->key())
	{
		case Key_Left:
		case Key_Right:
		QLineEdit::keyPressEvent(e);
		return;
		default:
		{
			switch(cursorPosition())
			{
				case 8: // 0 - 2
				{
					QString s = QLineEdit::text(); 
					//
					// if the month is 02 then we limit the first digit to 2
					// 
					if(s[5] == '0' && s[6] == '2')
					{
						if((e->ascii() < '0') || (e->ascii() > '2'))
						{
							return;
						};
					}
					else
					{
						if((e->ascii() < '0') || (e->ascii() > '3'))
						{
							return;
						};
					};
				};
				break;
				case 9: // 0 - 9 or 0 - 3
				{
					QString s = QLineEdit::text(); // get the buffer
					if(s[8] == '3')
					{
						// if it is 3 then
						if((e->ascii() < '0') || (e->ascii() > '1'))
						{
							return;
						};
					}
					else
					{
						// if the first char is 0 or 1 then 
						if((e->ascii() < '0') || (e->ascii() > '9'))
						{
							return;
						};
					};
				};
				break;
				case 4: // must be slashes - skip
				case 7:
				{
					QKeyEvent ev(QEvent::KeyPress,Key_Colon,'-',0);
					QLineEdit::keyPressEvent(&ev);
					cursorRight(false);
				};
				return; 
				case 5: // only 0 - 1
				if((e->ascii() < '0') || (e->ascii() > '1'))
				{
					return;
				};
				break;
				case 6: // 0 - 9 or 0 - 2
				{
					QString s = QLineEdit::text(); // get the buffer
					if(s[5] == '1')
					{
						// if it is 1 then
						if((e->ascii() < '0') || (e->ascii() > '2'))
						{
							return;
						};
					}
					else
					{
						// if the first char is 0 or 1 then 
						if((e->ascii() < '0') || (e->ascii() > '9'))
						{
							return;
						};
					};
				};
				break;
				case 0:
				case 1:
				case 2:
				case 3:
				if((e->ascii() < '0') || (e->ascii() > '9'))
				{
					return;
				};
				break;
				default:
				return;
			};		
		};
		break;
	};
	del(); 
	QLineEdit::keyPressEvent(e);
}; 
/*
*Function:void DATEEDIT::mousePressEvent(QMouseEvent *e) 
*Inputs:none
*Outputs:none
*Returns:none
*/
void DATEEDIT::mousePressEvent(QMouseEvent *e) 
{
	if(e->button() == Qt::RightButton)
	{
		DateNavigator dlg(this);
		dlg.SetDate(text());
		if(dlg.exec())
		{
			setText(dlg.GetDate());
		};
	};
};
	
