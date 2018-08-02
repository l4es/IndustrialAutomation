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
#include <qt.h>
#include <ctype.h>
#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "calendar.h"
#define INITIAL_TIME 750
#define REPEAT_TIME 100
static char* months[] = { "January", "February", "March", "April",
	"May", "June", "July", "August", "September", "October",
	"November", "December" 
};

//16-06-2003 TODO:correct the path fo language support
//static char* months[] = { "Gennaio", "Febbraio", "Marzo", "Aprile",
//	"Maggio", "Giugno", "Luglio", "Agosto", "Settembre", "Ottobre",
//	"Novembre", "Dicembre" 
//};

// Calendar
Calendar::Calendar(QWidget* p, const char *name)
:QListBox(p,name)
{
	dateinput=(DateInput*)p;
	curyear=98;
	curmonth=9;
	curday=25;
	calrow=calcol=startdow=stopdow=numrows=-1;
	direction=initial=0;
	setMouseTracking(TRUE);
	setFixedWidth(130);
	timer=new QTimer(this);
	CHECK_PTR(timer);
	connect(timer,SIGNAL(timeout()),SLOT(timerEvent()));
	tip=new CalendarTip(this,this);
	CHECK_PTR(tip);
}
Calendar::~Calendar(void)
{
	delete timer;
}
void Calendar::show()
{
	// Since our parent is a QComboBox, which attempts to filter
	// it's child's events, we must filter our own events first in
	// order to receive the events we need for this widget
	installEventFilter(this);
	setMouseTracking(TRUE);
	// get curyear, curmonth, curday from string
	QString str=dateinput->currentText();
	dateinput->validator()->fixup(str);
	dateinput->changeItem(str,0);
	char* tempstr=strdup(dateinput->currentText());
	curmonth=atoi(strtok(tempstr,"/"))-1;
	curday=atoi(strtok(NULL,"/"));
	curyear=atoi(strtok(NULL,"/"));
	if (curyear>=1900) curyear-=1900;
	free(tempstr);
	recalc();
	QListBox::show();
}
void Calendar::hide()
{
	setMouseTracking(FALSE);
	removeEventFilter(this);
	QListBox::hide();
}
void Calendar::recalc(void)
{
	// Calculate some time stuff...
	struct tm tm1;
	tm1.tm_sec=0;
	tm1.tm_min=0;
	tm1.tm_hour=12;
	tm1.tm_mday=1;
	tm1.tm_mon=curmonth;
	tm1.tm_year=curyear;
	time_t temp=mktime(&tm1);
	struct tm* ptr=localtime(&temp);
	startdow=ptr->tm_wday;
	int nextmonth=curmonth+1;
	int nextyear=curyear;
	if (nextmonth>11) {
		nextmonth=0;
		nextyear++;
	}
	tm1.tm_sec=0;
	tm1.tm_min=0;
	tm1.tm_hour=12;
	tm1.tm_mday=1;
	tm1.tm_mon=nextmonth;
	tm1.tm_year=nextyear;
	temp=mktime(&tm1);
	temp-=3600*24;	// 1 day
	ptr=localtime(&temp);
	numdaysinmonth=ptr->tm_mday;
	stopdow=ptr->tm_wday;
	numrows=(numdaysinmonth+startdow-1)/7;
	// Get day coloring information, if present
	for (int i=0;i<31;i++)
	daycolor[i]=NoColor;
	emit(classifyRequest(curmonth,curyear,daycolor));
	resize(width(),numrows*15+55);
}
int Calendar::mouseLeftArrow(QMouseEvent* qme)
{
	if (qme->x()>=10 && qme->x()<=20 && qme->y()>=5 && qme->y()<=15)
	return 1;
	return 0;
}
int Calendar::mouseRightArrow(QMouseEvent* qme)
{
	if (qme->x()>=width()-20 && qme->x()<=width()-10 && qme->y()>=5 && 
	qme->y()<=15)
	return 1;
	return 0;
}
bool Calendar::eventFilter(QObject*, QEvent* event)
{
	if (event->type() == QEvent::MouseMove) {
		mouseMoveEvent((QMouseEvent*)event);
		return TRUE;
		} else if (event->type() == QEvent::MouseButtonDblClick) {
		if (mouseLeftArrow((QMouseEvent*)event))
		return TRUE;
		if (mouseRightArrow((QMouseEvent*)event))
		return TRUE;
		} else if (event->type() == QEvent::MouseButtonRelease) {
		QMouseEvent* qme=(QMouseEvent*)event;
		if (mouseLeftArrow(qme)) {
			mouseReleaseEvent(qme);
			return TRUE;
		}
		if (mouseRightArrow(qme)) {
			mouseReleaseEvent(qme);
			return TRUE;
		}
	} 
	return FALSE;
}
void Calendar::paintEvent(QPaintEvent*)
{
	QPainter paint;
	paint.begin(this);
	QColorGroup g=colorGroup();
	paint.setPen(g.text());
	paint.fillRect(0,0,width(),height(),white);
	paint.fillRect(0,0,width(),20,yellow);
	paint.setPen(black);
	paint.drawLine(0,0,width()-1,0);
	paint.drawLine(0,0,0,height()-1);
	paint.drawLine(width()-1,0,width()-1,height()-1);
	paint.drawLine(0,height()-1,width()-1,height()-1);
	QFont font("Helvetica",10);
	font.setBold(TRUE);
	paint.setFont(font);
	char txt[30];
	sprintf(txt,"%s, %i",months[curmonth],curyear+1900);
	paint.drawText(0,0,width(),20,AlignCenter,txt,strlen(txt));
	QFont font1("Helvetica",8);
	paint.setFont(font1);
	int w=width()/7;
	static char* dow[]={"Su","M","Tu","W","Th","F","Sa"};
	{
		for (int i=0;i<7;i++) {
			paint.drawText(i*w+2,20,15,15,AlignCenter,dow[i],strlen(dow[i]));
		}
	};
	{
		for (int i=1;i<=numdaysinmonth;i++) {
			char txt[3];
			sprintf(txt,"%i",i);
			if (daycolor[i-1] == Color1) {
				paint.fillRect(((i+startdow-1)%7)*w+4,((i+startdow-1)/7)*15+36,13,13,
				gray);
			}
			if (daycolor[i-1] == Color2) {
				paint.fillRect(((i+startdow-1)%7)*w+4,((i+startdow-1)/7)*15+36,13,13,
				red);
			}
			if (i==curday) {
				paint.drawRect(((i+startdow-1)%7)*w+3,((i+startdow-1)/7)*15+35,15,15);
			}
			paint.drawText(((i+startdow-1)%7)*w+3,
			((i+startdow-1)/7)*15+35,15,15,AlignCenter,txt,strlen(txt));
		}
	};
	QPointArray points(3);
	points.setPoints(3,10,10,20,5,20,15);
	paint.setBrush(black);
	paint.drawPolygon(points,TRUE);
	points.setPoints(3,width()-10,10,width()-20,5,width()-20,15);
	paint.drawPolygon(points,TRUE);
	paint.end();
}
void Calendar::mousePressEvent(QMouseEvent* qme)
{
	char str[20];
	if (mouseLeftArrow(qme)) {
		curmonth--;
		if (curmonth<0) {
			curmonth=11;
			curyear--;
		}
		direction=0;
		initial=1;
		timer->start(INITIAL_TIME);
		sprintf(str,"%i/%i/%i",curmonth+1,curday,curyear+1900);
		dateinput->changeItem(str,0);
		recalc();
		repaint();
		} else if (mouseRightArrow(qme)) {
		curmonth++;
		if (curmonth>11) {
			curmonth=0;
			curyear++;
		}
		direction=1;
		initial=1;
		timer->start(INITIAL_TIME);
		sprintf(str,"%i/%i/%i",curmonth+1,curday,curyear+1900);
		dateinput->changeItem(str,0);
		recalc();
		repaint();
		} else {
		int row=int(qme->y()>35?(qme->y()-35)/15:-1);
		int col=int(qme->x()/(width()/7));
		if (!((row==0 && col<startdow) || (row>numrows) || (col>6) ||
		(row==numrows && col>stopdow)) && col>=0 && row>=0) {
			curday=row*7+col+1-startdow;
			char text[20];
			sprintf(text,"%i/%i/%i",curmonth+1,curday,curyear+1900);
			dateinput->changeItem(text,0);
			/*
			cerr<<curmonth+1<<"/"<<curday<<"/"<<curyear+1900<<" selected!"<<endl;
			cerr<<"count="<<count()<<endl;
			*/
			calrow=calcol=-1;
		}
	}
}
void Calendar::mouseMoveEvent(QMouseEvent* qme)
{
	int row=int(qme->y()>35?(qme->y()-35)/15:-1);
	int col=int(qme->x()/(width()/7));
	if ((row==0 && col<startdow) || (row>numrows) || (col>6) ||
	(row==numrows && col>stopdow)) {
		row=col=-99;
	}
	QPainter paint;
	paint.begin(this);
	if (row!=calrow || col!=calcol) {
		if (curday == calrow*7+calcol+1-startdow) {
			paint.setPen(black);
			} else {
			paint.setPen(white);
		}
		paint.drawRect(calcol*(width()/7)+3,calrow*15+35,15,15);
		if (row>=0 && col>=0) { 
			calrow=row;
			calcol=col;
			QColorGroup g=colorGroup();
			int x=calcol*(width()/7)+3;
			int y=calrow*15+35;
			paint.setPen(g.midlight());
			paint.drawLine(x,y,x,y+14);
			paint.drawLine(x,y,x+14,y);
			paint.setPen(g.dark());
			paint.drawLine(x+14,y,x+14,y+14);
			paint.drawLine(x,y+14,x+14,y+14);
			} else {
			calrow=calcol=-99;
		}
	}
	paint.end();
}
void Calendar::mouseReleaseEvent(QMouseEvent*)
{
	timer->stop();
}
void Calendar::timerEvent()
{
	if (initial) {
		initial=0;
		timer->start(REPEAT_TIME);
	}
	if (direction) {
		curmonth++;
		if (curmonth>11) {
			curmonth=0;
			curyear++;
		}
		} else {
		curmonth--;
		if (curmonth<0) {
			curmonth=11;
			curyear--;
		}
	}
	char str[20];
	sprintf(str,"%i/%i/%i",curmonth+1,curday,curyear+1900);
	dateinput->changeItem(str,0);
	recalc();
	repaint();
}
void Calendar::getDayLabel(int m, int d, int y, char* t)
{
	// default to null if not connected...
	strcpy(t,"");
	emit(dayLabel(m,d,y,t));
}
// CalendarTip
CalendarTip::CalendarTip(QWidget* p, Calendar* c)
:QToolTip(p)
{
	cal=c;
}
void CalendarTip::maybeTip(const QPoint& p)
{
	int row=int(p.y()>35?(p.y()-35)/15:-1);
	int col=int(p.x()/(cal->width()/7));
	int curday;
	if (!((row==0 && col<cal->startdow) || (row>cal->numrows) || (col>6) ||
	(row==cal->numrows && col>cal->stopdow)) && col>=0 && row>=0) {
		curday=row*7+col+1-cal->startdow;
		QRect rect(p.x()-5,p.y()-5,10,10);
		char tiptext[100],text[100];
		cal->getDayLabel(cal->curmonth,curday,cal->curyear+1900,text);
		if (strcmp(text,""))
		sprintf(tiptext,"%s %i, %i\n%s",months[cal->curmonth],curday,
		cal->curyear+1900,text);
		else
		sprintf(tiptext,"%s %i, %i",months[cal->curmonth],curday,
		cal->curyear+1900);
		tip(rect,tiptext);
	} 
}
// DateValidator
DateValidator::DateValidator(QWidget* p)
:QValidator(p)
{
}
QValidator::State DateValidator::validate(QString& str, int&) const
{
	#if 0
	int tempmon=1;
	int tempday=1;
	int tempyear=1970;
	// Every character must be either a digit or a slash
	for (unsigned int i=0;i< str.length();i++)
	if (!isdigit(str[i]) && str[i]!='/')
	return Invalid;
	// Must have exactly two slashes
	char* firstslash=strchr(str,'/');
	if (firstslash==NULL) 
	return Valid;
	else {
		tempmon=atoi(str);
	}
	char* secondslash=strchr(firstslash+1,'/');
	if (secondslash==NULL)
	return Valid;
	else {
		tempday=atoi(firstslash+1);
		tempyear=atoi(secondslash+1);
	}
	// Month, day and year must be in an acceptable range
	if (tempmon>12 || tempday>31 || tempyear>2100)
	return Invalid;
	if (tempmon<1 || tempday<1 || tempyear<1970)
	return Valid;
	#endif
	return Acceptable;
}
void DateValidator::fixup(QString& str)
{
	#if 0
	int tempmon=1;
	int tempday=1;
	int tempyear=1970;
	str.detach();
	char* firstslash=strchr(str,'/');
	if (firstslash != NULL) {
		tempmon=atoi(str);
		char* secondslash=strchr(firstslash+1,'/');
		if (secondslash != NULL) {
			tempday=atoi(firstslash+1);
			tempyear=atoi(secondslash+1);
		}
	}
	if (tempmon<=0) tempmon=1;
	if (tempday<1) tempday=1;
	if (tempyear<1970) tempyear=1970;
	str.sprintf("%i/%i/%i",tempmon,tempday,tempyear);
	#endif
}
// DateInput
DateInput::DateInput(QWidget* p, char* txt)
:QComboBox(TRUE,p)
{
	cal=new Calendar(this);
	CHECK_PTR(cal);
	connect(cal,SIGNAL(classifyRequest(int,int,char*)),
	SLOT(slotClassifyRequest(int,int,char*)));
	connect(cal,SIGNAL(dayLabel(int,int,int,char*)),
	SLOT(slotDayLabel(int,int,int,char*)));
	setListBox(cal);
	dv=new DateValidator(this);
	CHECK_PTR(dv);
	setValidator(dv);
	setMaxCount(1);
	setInsertionPolicy(AtCurrent);
	setAutoCompletion(TRUE);
	insertItem(txt);
	setCurrentItem(0);
}
DateInput::~DateInput(void)
{
	delete dv;
}
const char* DateInput::getDate(void)
{
	return cal->text(0);
}
void DateInput::slotClassifyRequest(int m,int y,char* c)
{
	emit(classifyRequest(m,y,c));
}
void DateInput::slotDayLabel(int m, int d, int y, char* t)
{
	emit(dayLabel(m,d,y,t));
}

