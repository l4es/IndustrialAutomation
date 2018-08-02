/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2014 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */

#include <qobject.h>
#include <qdialog.h>
#include "general_defines.h"

#ifdef USE_STD_MAP
typedef std::map<QString, QObject*, std::less<QString> > WidgetDict;
#endif

class HMI_manager : public QObject
{
    Q_OBJECT

	enum {
		tUnit = 1, tTagLimits
	}; // transaction codes

	QString sample_point_name;
	QString value_for_command;
	WidgetDict QwtThermo_dictionary;
	WidgetDict QLCDNumber_dictionary;
	WidgetDict PLCDNumber_dictionary;
	WidgetDict SinglePointLed_dictionary;
	WidgetDict DoublePointLed_dictionary;
	WidgetDict PSinglePointLed_dictionary;
	WidgetDict PDoublePointLed_dictionary;
	WidgetDict PSwitch_dictionary;
	WidgetDict Breaker_dictionary;
	WidgetDict PTank_dictionary;
	WidgetDict PThermometer_dictionary;
	WidgetDict QPushButton_dictionary;

	void doUpdateTags(QString &s, double &v, WidgetDict &dict);
	void doUpdateSamplePoint(QString &s, int state, int ack_flag, WidgetDict &dict);

public:
    void setParent( QDialog *parent );
	void setInitialValuesAndLimits();
	void get_utc_host_time(struct cp56time2a* time);
	
	
public slots:
    void sendCommand();
	void pSwitchToggledSendCommand();
	void RightClicked(QString &class_name, QString &widget_name);
	void UpdateTags(); // update tag values
	void UpdateSamplePoint(); // handle updated sample points
	void QueryResponse (QObject *, const QString &, int, QObject*); // handles database responses
	void DoButtonCommand();
	void Do_pSwitchCommand();
	void ReceivedNotify(int, const char *);
private:
    QDialog *dialog_parent;
};
