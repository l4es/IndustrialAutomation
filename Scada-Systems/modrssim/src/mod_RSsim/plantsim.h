/////////////////////////////////////////////////////////////////////////////
//
// FILE; plantsim.h ; headder file
//
// See "_README.CPP"
// built-in plant simulation module constants
//
/////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////
// Plant Simulation section
#define AMBIENT      2100     // 21 deg C.
#define TEMPMAX      11000    // 110 deg C.

// PS or PlantSim registers
#define PS_MASK_INPUT1     0x01
#define PS_MASK_INPUT2     0x02
#define PS_MASK_DRAIN      0x04
#define PS_MASK_HEATER     0x08
#define PS_MASK_RESET      0x10
#define PS_MASK_LOWLEVEL   0x20
#define PS_MASK_AUTO       0x40
#define PS_STIRRER_START   0x80


#define PS_CONTROLBITS     0     // register 1
#define PS_FEEDBACKBITS    1     // reflected PS_CONTROLBITS
#define PS_MIXERTIME       2 
#define PS_LEVEL           3     //
#define PS_MAXLEVEL        4     //
#define PS_TANK1LEVEL      5 
#define PS_MAXTANK1LEVEL   6 
#define PS_TANK2LEVEL      7 
#define PS_MAXTANK2LEVEL   8 
#define PS_TEMP            9     // register 5
#define PS_TEMPSETPOINT    10
#define PS_HEATERAMPS      11
#define PS_HEATEREFFECT    12    // heater effectiveness
#define PS_MILLISECONDS    13
#define PS_SECONDS         14
#define PS_MINUTES         15
#define PS_HOURS           16
#define PS_DAY             17
#define PS_WEEKDAY         18    // 0=sunday
#define PS_RANDOM          19
#define PS_LASTREGISTER    (PS_RANDOM)

#define PS_AUTO_HYSTERESIS    200   // 1 degrees

extern char *plantSim_registerNames[PS_LASTREGISTER+1];

