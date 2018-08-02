
// General preferences
#define PF_CURRENT_LIMIT		26		// A
#define PF_POWER_LIMIT			0.5		// W
#define PF_ENERGY_LIMIT			16		// uJ
#define PF_CURRENT				75		// %
#define PF_THERMATRACK			6150
#define PF_FREQUENCY			25000	// Hz
#define PF_TRIGGER_MODE			1		// vector = 0 = internal; bitmap = 1 = external
#define PF_PULSE_CONTROL		4		// vector = 2 = ThermEQ; bitmap = 4 = Pulse Track
#define PF_SHG_TEMPERATURE		151.7	// deg C
#define PF_FHG_TEMPERATURE		116.2	// deg C
#define PF_AVIA_HOST			"192.168.12.235"
#define PF_AVIA_PORT			4001
#define PF_TIMEOUT				60

/*
PF_CURRENT_LIMIT        [10, 60], 33, A, current limit at which the laser power stops loading in. The driver keeps on loading the laser until this current value is reached.
PF_POWER_LIMIT          [0.1, 3.5], 1.8, W, power limit at which the laser power stops loading in. The driver keeps loading the laser until this power value is reached.
PF_ENERGY_LIMIT         [0.1, 150], 40, uJ, energy limit at which the laser power stops loading in. The driver keeps loading the laser until this energy value is reached.
PF_CURRENT              [0, 100], 85, %, rate (slope) of the laser power load capacitor. From Avia 266 documentation: "Sets the diode current to percentage from 0.1% (10 Amps) to 100% (maximum allowable current). A setting of 0% will be zero current. =.1% increments.
PF_THERMATRACK          [0, 9000], 3125, -, thermatrack setting as calculated on thermatrack operation on laser frontpanel (23 deg = 3800, 27 deg = 4100 @ 33 kHz)
PF_FREQUENCY            [5000?, 100000], 12500, Hz, frequency of laser pulse
PF_TRIGGER_MODE         [0 or 1], 1, -, vector = 0 = internal; bitmap = 1 = external
PF_PULSE_CONTROL        [2 or 4], 4, -, vector = 2 = ThermEQ; bitmap = 4 = Pulse Track
PF_SHG_TEMPERATURE      [50-170], 151.7, �C, SHG temperature
PF_FHG_TEMPERATURE      [50-175], 116.2, �C, FHG temperature
*/

// Avia RS232 read commands

#define RD_SYSTEM_OK			"?F"
#define	RD_SHUTTER				"?S"
#define	RD_KEY					"?K"
#define	RD_DIODE				"?DO"
#define	RD_PULSE				"?PC"
#define	RD_LOCK					"?LF"
#define	RD_CURRENT				"?C"
#define	RD_UV_POWER				"?UP"
#define	RD_UV_ENERGY			"?UE"
#define	RD_FREQUENCY			"?RR"
#define	RD_THERMATRACK			"?MD"
#define	RD_TRIGGER_MODE			"?TM"
#define	RD_ROD_SERVO_STATUS		"?LRSS"
#define	RD_DIODE_1_SERVO_STATUS	"?D1SS"
#define	RD_DIODE_2_SERVO_STATUS	"?D2SS"
#define	RD_SHG_SERVO_STATUS		"?SHGSS"
#define	RD_FHG_SERVO_STATUS		"?FHGSS"
#define	RD_PULSE_CONTROL		"?PD"
#define	RD_BASE_TEMPERATURE		"?BT"
#define	RD_ROD_TEMPERATURE		"?LRT"
#define	RD_SHG_TEMPERATURE		"?SHGT"
#define	RD_FHG_TEMPERATURE		"?FHGT"
#define	RD_DIODE_1_TEMPERATURE	"?D1T"
#define	RD_DIODE_2_TEMPERATURE	"?D2T"
#define	RD_ECHO					"?ECHO"
#define	RD_PROMPT				"?PROMPT"
#define	RD_FHG_OPTIMIZE			"?FHGCO"
#define	RD_SHG_OPTIMIZE			"?SHGO"
#define	RD_THERMATRACK_OPTIMIZE	"?DOP"
#define	RD_SHG_TEMPERATURE_SET	"?SHGST"
#define	RD_FHG_TEMPERATURE_SET	"?FHGST"
#define	RD_DIODE_1_TEMPERATURE_SET	"?D1ST"
#define	RD_DIODE_2_TEMPERATURE_SET	"?D2ST"
// unfrequent commands
#define RD_POWER_SUPPLY_SN		"?PSSN"
#define RD_POWER_SUPPLY_SOFTWARE	"?SV"
#define RD_HEAD_SN				"?HSN"
#define RD_HEAD_SOFTWARE		"?HSV"
#define RD_HEAD_HOURS			"?HH"
#define RD_BATTERY_VOLTAGE		"?BV"
#define RD_CRYSTAL_WARNING		"?CWE"
#define RD_DIODE_1_HOURS		"?D1H"
#define RD_DIODE_1_SET_TEMPERATURE	"?D1ST"
#define RD_DIODE_2_HOURS		"?D2H"
#define RD_DIODE_2_SET_TEMPERATURE	"?D2ST"
#define RD_SPOT_LOCATION		"?SP"
#define RD_SPOT_HOURS			"?SPH:"
#define RD_MOVING				"?DMM"

// Avia RS232 write commands
#define	WR_SHUTTER				"S="
#define	WR_KEY					"K="
#define	WR_DIODE				"DO="
#define	WR_PULSE				"PC="
#define	WR_LOCK					"LF="
#define	WR_CURRENT				"C="
#define	WR_FREQUENCY			"RR="
#define	WR_THERMATRACK			"MD="
#define	WR_TRIGGER_MODE			"TM="
#define	WR_PULSE_CONTROL		"PD="
#define	WR_SHG_TEMPERATURE		"SHGT="
#define	WR_FHG_TEMPERATURE		"FHGT="
#define	WR_DIODE_1_TEMPERATURE	"D1T="
#define	WR_DIODE_2_TEMPERATURE	"D2T="
#define	WR_ECHO					"ECHO="
#define	WR_PROMPT				"PROMPT="
#define	WR_FHG_OPTIMIZE			"FHGCO="
#define	WR_SHG_OPTIMIZE			"SHGO="
#define	WR_THERMATRACK_OPTIMIZE	"DOP="
#define	WR_SHG_TEMPERATURE_SET	"SHGST="
#define	WR_FHG_TEMPERATURE_SET	"FHGST="
#define	WR_DIODE_1_TEMPERATURE_SET	"D1ST="
#define	WR_DIODE_2_TEMPERATURE_SET	"D2ST="
