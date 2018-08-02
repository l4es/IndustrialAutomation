// vim: expandtab

#if defined(_MSC_VER) /* MSVC Compiler */
#pragma warning ( disable : 4786 )
#endif

#include <qvaluelist.h>
#include <qmime.h>
#include <qdragobject.h>

#include "qwtplugin.h"

#include "qwt_counter.h"
#include "qwt_plot.h"
#include "qwt_wheel.h"
#include "qwt_thermo.h"
#include "qwt_knob.h"
#include "qwt_scale.h"
#include "qwt_slider.h"
#include "qwt_analog_clock.h"
#include "qwt_compass.h"
//apa+++
#include "single_point_led.h"
#include "double_point_led.h"
#include "ptoggle.h"
#include "pthermometer.h"
#include "ptank.h"
#include "pswitch.h"
#include "psinglepointled.h"
#include "pmeter.h"
#include "plcdnumber.h"
#include "pdoublepointled.h"
#include "elswitch.h"

namespace
{
    struct Entry
    {
        Entry() {}
        Entry( QString _classname, QString _header, QString  _pixmap,
                QString _tooltip, QString _whatshis):       
                classname(_classname),
                header(_header),
                pixmap(_pixmap),
                tooltip(_tooltip),
                whatshis(_whatshis)
        {}

        QString classname;
        QString header;
        QString pixmap;
        QString tooltip;
        QString whatshis;
    };

    QValueList<Entry> vec;

    const Entry *entry(const QString& str)
    {
        for ( uint i = 0; i < vec.count(); i++ )
        {
            if (str == vec[i].classname)
                return &vec[i];
        }
        return NULL;
    }
}

QwtPlugin::QwtPlugin()
{
    vec.append(Entry("QwtPlot", "qwt_plot.h",
        "qwtplot.png", "QwtPlot", "whatsthis"));
    vec.append(Entry("QwtAnalogClock", "qwt_analog_clock.h", 
        "qwtanalogclock.png", "QwtAnalogClock", "whatsthis"));
    vec.append(Entry("QwtCompass", "qwt_compass.h",
        "qwtcompass.png", "QwtCompass", "whatsthis"));
    vec.append(Entry("QwtCounter", "qwt_counter.h", 
        "qwtcounter.png", "QwtCounter", "whatsthis"));
    vec.append(Entry("QwtDial", "qwt_dial.h", 
        "qwtdial.png", "QwtDial", "whatsthis"));
    vec.append(Entry("QwtKnob", "qwt_knob.h",
        "qwtknob.png", "QwtKnob", "whatsthis"));
    vec.append(Entry("QwtPushButton", "qwt_push_button.h",
        "qwtpushbutton.png", "QwtPushButton", "whatsthis"));
    vec.append(Entry("QwtScale", "qwt_scale.h",
        "qwtscale.png", "QwtScale", "whatsthis"));
    vec.append(Entry("QwtSlider", "qwt_slider.h",
        "qwtslider.png", "QwtSlider", "whatsthis"));
    vec.append(Entry("QwtThermo", "qwt_thermo.h",
        "qwtthermo.png", "QwtThermo", "whatsthis"));
    vec.append(Entry("QwtWheel", "qwt_wheel.h",
        "qwtwheel.png", "QwtWheel", "whatsthis"));
    vec.append(Entry("SinglePointLed", "single_point_led.h",
        "led_rg0000.png", "SinglePointLed", "whatsthis"));
    vec.append(Entry("DoublePointLed", "double_point_led.h",
        "led_rg0001.png", "DoublePointLed", "whatsthis"));
	vec.append(Entry("PSinglePointLed", "psinglepointled.h",
        "led_by0000.png", "PSinglePointLed", "whatsthis"));
    vec.append(Entry("PDoublePointLed", "pdoublepointled.h",
        "led_by0001.png", "PDoublePointLed", "whatsthis"));
	vec.append(Entry("PToggle", "ptoggle.h",
        "ptoggleicon.png", "PToggle", "whatsthis"));
    vec.append(Entry("PThermometer", "pthermometer.h",
        "pthermometericon.png", "PThermometer", "whatsthis"));
	vec.append(Entry("PTank", "ptank.h",
        "ptankicon.png", "PTank", "whatsthis"));
    vec.append(Entry("PSwitch", "pswitch.h",
        "pswitchicon.png", "PSwitch", "whatsthis"));
	vec.append(Entry("PMeter", "pmeter.h",
        "pmetericon.png", "PMeter", "whatsthis"));
    vec.append(Entry("PLCDNumber", "plcdnumber.h",
        "plcdnumbericon.png", "PLCDNumber", "whatsthis"));
	vec.append(Entry("Breaker", "elswitch.h",
        "pswitchicon.png", "Breaker", "whatsthis"));

}

QWidget* QwtPlugin::create(const QString &key, 
    QWidget* parent, const char* name)
{
    if ( key == "QwtPlot" )
        return new QwtPlot( parent, name );
    else if ( key == "QwtAnalogClock" )
        return new QwtAnalogClock( parent, name);
    else if ( key == "QwtCounter" )
        return new QwtCounter( parent, name);
    else if ( key == "QwtCompass" )
        return new QwtCompass( parent, name );
    else if ( key == "QwtDial" )
        return new QwtDial( parent, name);
    else if ( key == "QwtPushButton" )
        return new QwtPushButton(
            QString::fromLatin1("E=mc<sup>2</sup>"), parent, name );
    else if ( key == "QwtWheel" )
        return new QwtWheel( parent, name );
    else if ( key == "QwtThermo" )
        return new QwtThermo( parent, name );
    else if ( key == "QwtKnob" )
        return new QwtKnob( parent, name );
    else if ( key == "QwtScale" )
        return new QwtScale( QwtScale::Left, parent, name );
    else if ( key == "QwtSlider" )
        return new QwtSlider( parent, name );
	else if ( key == "SinglePointLed" )
        return new SinglePointLed(parent, name );
	else if ( key == "DoublePointLed" )
        return new DoublePointLed(parent, name );
	else if ( key == "PSinglePointLed" )
        return new PSinglePointLed(parent, name );
	else if ( key == "PDoublePointLed" )
        return new PDoublePointLed(parent, name );
	else if ( key == "PToggle" )
        return new PToggle(parent, name );
	else if ( key == "PThermometer" )
        return new PThermometer(parent, name );
	else if ( key == "PTank" )
        return new PTank(parent, name );
	else if ( key == "Breaker" )
        return new Breaker(parent, name );
	else if ( key == "PSwitch" )
        return new PSwitch(parent, name );
	else if ( key == "PMeter" )
        return new PMeter(parent, name );
	else if ( key == "PLCDNumber" )
        return new PLCDNumber(parent, name );

    return 0;
}


QStringList QwtPlugin::keys() const
{
    QStringList list;
    
    for (unsigned i = 0; i < vec.count(); i++)
        list += vec[i].classname;

    return list;
}

QString QwtPlugin::group( const QString& feature ) const
{
    if (entry(feature) != NULL )
        return QString("SCADA Widgets"); 
    return QString::null;
}

QIconSet QwtPlugin::iconSet( const QString& pmap) const
{
    QString pixmapKey("qwtwidget.png");
    if (entry(pmap) != NULL )
        pixmapKey = entry(pmap)->pixmap;

    const QMimeSource *ms =
        QMimeSourceFactory::defaultFactory()->data(pixmapKey);

    QPixmap pixmap;
    QImageDrag::decode(ms, pixmap);

    return QIconSet(pixmap);
}

QString QwtPlugin::includeFile( const QString& feature ) const
{
    if (entry(feature) != NULL)
        return entry(feature)->header;        
    return QString::null;
}

QString QwtPlugin::toolTip( const QString& feature ) const
{
    if (entry(feature) != NULL )
        return entry(feature)->tooltip;       
    return QString::null;
}

QString QwtPlugin::whatsThis( const QString& feature ) const
{
    if (entry(feature) != NULL)
        return entry(feature)->whatshis;      
    return QString::null;
}

bool QwtPlugin::isContainer( const QString& ) const
{
    return FALSE;
}


Q_EXPORT_PLUGIN( QwtPlugin )

// Local Variables:
// mode: C++
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
