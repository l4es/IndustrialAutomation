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
#ifndef __CALENDAR_H
#define __CALENDAR_H
#include <qt.h>
class Calendar;
class CalendarTip;
class DateInput;
class QMouseEvent;
class QPaintEvent;
class QResizeEvent;
typedef enum { NoColor, Color1, Color2, Color3 } DayColors;
	class Calendar : public QListBox {
		Q_OBJECT
		public:
		Calendar(QWidget*, const char * = 0);
		~Calendar(void);
		void show();
		void hide();
		signals:
		void classifyRequest(int mon, int yr, char*);
		void dayLabel(int, int, int, char*);
		protected:
		bool eventFilter(QObject*,QEvent*);
		void paintEvent(QPaintEvent*);
		void mousePressEvent(QMouseEvent*);
		void mouseMoveEvent(QMouseEvent*);
		void mouseReleaseEvent(QMouseEvent*);
		protected slots:
		void timerEvent();
		private:
		void getDayLabel(int, int, int, char*);
		void recalc(void);
		int mouseLeftArrow(QMouseEvent*);
		int mouseRightArrow(QMouseEvent*);
		int initial,direction;
		int curyear,curmonth,curday;
		int calrow,calcol,startdow,stopdow,numrows;
		int numdaysinmonth;
		char daycolor[31];
		QTimer* timer;
		DateInput* dateinput;
		CalendarTip* tip;
		friend CalendarTip;
	};
	class CalendarTip : public QToolTip {
		public:
		CalendarTip(QWidget*,Calendar*);
		protected:
		void maybeTip(const QPoint&);
		private:
		Calendar* cal;
	};
	class DateValidator : public QValidator {
		public:
		DateValidator(QWidget*);
		State validate(QString&,int&) const;
		void fixup(QString&);
	};
	class DateInput : public QComboBox {
		Q_OBJECT
		public:
		DateInput(QWidget*,char*);
		~DateInput(void);
		const char* getDate(void);
		signals:
		void classifyRequest(int,int,char*);
		void dayLabel(int, int, int, char*);
		protected slots:
		void slotDayLabel(int, int, int, char*);
		void slotClassifyRequest(int,int,char*);
		private:
		Calendar* cal;
		DateValidator* dv;
	};
	#endif
	
