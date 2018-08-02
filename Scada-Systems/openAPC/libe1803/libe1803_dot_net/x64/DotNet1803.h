// DotNet1803.h

/********************************************************************************************

These sources are distributed in the hope that they will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. All
the information given here, within the interface descriptions and within the specification
are subject to change without notice. Errors and omissions excepted.

Note:	Expects ..\..\lib32 to contain libe1803.lib, libe1803.dll, and libe1803.h
		Targets .NET Frame v4.0.  Tested with v4.6.2.
		Requires WinSock v2.2(Vista) or higher

History:	2017-03-23	Author Jason Baginski(nxtwothou@hotmail.com) initial release
			2017-03-27	Realign from HALaserDotNet to libe1803_dot_net namespace.  Remove 
						unnecesssary project bits to shrink DLL and force x86 or x64 based
						on project.
			2017-04-12  Added executemode execute functions and executeMarkAsync

*********************************************************************************************/

#pragma once

#include "../../libe1803.h"

using namespace System;


namespace libe1803_dot_net {

	public ref class DotNet1803
	{
	public:
		bool bOnErrorThrowException = false; // if you prefer to get an exception thrown rather than a true/false complete result, set to this to be true.
		String^ ErrorDetail = "";

		~DotNet1803();

		// set_connection must be first command after.  If ethernet, must use set_password after set_connection.  To then connect to controller, use load_correction
		DotNet1803();
		// Connects to IP address("192.168.1.254") or COMx("COM3") with no correction file
		DotNet1803(String^ address);
		// Connects to IP address("192.168.1.254") or COMx("COM3") ethPwd must match card e1803.cfg file with parameter "passwd".  Connects with no correction file
		DotNet1803(String^ address, String^ ethPwd);
		// Connects to IP address("192.168.1.254") or COMx("COM3") ethPwd must match card e1803.cfg file with parameter "passwd".
		DotNet1803(String^ address, String^ ethPwd, String^ correctionfilename);
		// Connects to IP address("192.168.1.254") or COMx("COM3") ethPwd must match card e1803.cfg file with parameter "passwd".  correctiontableNum 0-16 for switch_correction
		DotNet1803(String^ address, String^ ethPwd, String^ correctionfilename, unsigned char correctiontableNum);

		// This function has to be called as very first.  IP address("192.168.1.254") or COMx("COM3")
		bool set_connection(String^ address);
		// This function has to be called as very first.  IP address("192.168.1.254") or COMx("COM3") ethPwd must match card e1803.cfg file with parameter "passwd"
		bool set_connection(String^ address, String^ ethPwd);
		// Sets a password that is used for Ethernet connection.  ethPwd must match card e1803.cfg file with parameter "passwd"
		void set_password(String^ ethPwd);
		bool set_filepath(unsigned char n, String^ fname, unsigned int mode);
		// write all function calls into a logfile so that it is possible to evaluate the real order of commands.  Full logging
		bool set_debug_logfile(String^ path);
		// write all function calls into a logfile so that it is possible to evaluate the real order of commands.  flags combine 0x00 min log size, 0x01 include all motion, 0x02 include all check state
		bool set_debug_logfile(String^ path, unsigned char flags);
		// Closes the connection to a card and releases all related resources.
		void close();
		// Opens connection to the card.  Must be called at least once to establish connection to card.
		bool load_correction();
		// Opens connection to the card.  Must be called at least once to establish connection to card.
		bool load_correction(String^ filename);
		// Opens connection to the card. tableNum 0-16, for use with switch_correction. Must be called at least once to establish connection to card.
		bool load_correction(String^ filename, unsigned char tableNum);
		//Switches between up to 16 correction tables on the fly.
		bool switch_correction(unsigned char tableNum);
		// Sets size correction factor and offset for X and Y direction of working area as well as a rotation.  Will overwrite set_matrix().
		bool set_xy_correction(unsigned int flags, double gainX, double gainY, double rot, int offsetX, int offsetY, double slantX, double slantY);
		// Set additional Z correction parameter. h - vertical height from last mirror to working area.  xy_to_z_ratio - ratio between max size and max z(100x100mm working area with 40mm z would be 100/40=2.5)
		bool set_z_correction(unsigned int h, double xy_to_z_ratio, int res2);
		// Enables special functions and features.
		bool tune(unsigned int tuneFlags);
		// Set scanner speed values to be used for all following vector data and until not replaced by other speed values.  Values are bits/msec 1..4294960000
		bool set_speeds(double jumpspeed, double markspeed);
		// Set laser delay values to be used for all following vector data and until not replaced by other delay values.  Values are microseconds
		bool set_laser_delays(double ondelay, double offdelay);
		// Sets the laser mode to be used for all following operations, this value influences the signals emitted at the connectors of the card.This function has to be called prior to setting any other laser parameters
		bool set_laser_mode(unsigned int mode);
		enum class lasermode {
			LASERMODE_CO2 = 0
			, LASERMODE_YAG1 = 1
			, LASERMODE_YAG2 = 2
			, LASERMODE_YAG3 = 3
			, LASERMODE_CRF = 4
			, LASERMODE_DFREQ = 5
			, LASERMODE_IPG = 6
		};
		// Sets the laser mode to be used for all following operations, this value influences the signals emitted at the connectors of the card.This function has to be called prior to setting any other laser parameters
		bool set_laser_mode(lasermode mode);
		// Switches the laser on or off independent fro many mark or jump commands
		bool set_laser(unsigned int flags, char on);
		// Switches the laser on or off in IMMEDIATE mode
		bool set_laser(bool bOn);
		// Switches the laser on or off independent fro many mark or jump commands
		bool set_laser(unsigned int flags, bool bOn);
		// gives the possibility to not to let the laser beam follow the given path directly but to rotate around the specified path and lasers current position.  x and y-1..10000000, freq - Hz in range 1..25000
		bool set_wobble(unsigned int x, unsigned int y, double freq);
		// Set stream scanner delays in unit microseconds. Smallest possible value and resolution is 0.5 microseconds
		bool set_scanner_delays(double jumpdelay, double markdelay, double polydelay);
		// Set stream scanner delays in unit microseconds. Smallest possible value and resolution is 0.5 microseconds  flags-0x0100(E1803_COMMAND_FLAG_SCANNER_VAR_POLYDELAY)
		bool set_scanner_delays(unsigned int flags, double jumpdelay, double markdelay, double polydelay);
		// Perform a stream jump(movement with laser turned off) to the given position -33554431..33554432
		bool jump_abs(int x, int y);
		// Perform a stream jump(movement with laser turned off) to the given position -33554431..33554432
		bool jump_abs(int x, int y, int z);
		// Perform a stream mark(movement with laser turned on) to the given position -33554431..33554432
		bool mark_abs(int x, int y);
		// Perform a stream mark(movement with laser turned on) to the given position -33554431..33554432
		bool mark_abs(int x, int y, int z);
		enum class pixelmode {
			PIXELMODE_DEFAULT = 0
			, PIXELMODE_NO_JUMPS = 1
			, PIXELMODE_JUMP_N_SHOOT = 2
			, PIXELMODE_HW_POWER_CONTROL = 4
			, PIXELMODE_GATE_POWER_CONTROL = 8
		};
		// Set the operational mode for mark_pixelline().
		bool set_pixelmode(pixelmode mode, double powerThres);
		// Set the operational mode for mark_pixelline().
		bool set_pixelmode(unsigned int mode, double powerThres, unsigned int res);
		// Mark a single line of a bitmap image.  User is responsible for making sure memory is not garabage collected until end due to callbacks.
		bool mark_pixelline(int x, int y, int z, int pixWidth, int pixHeight, int pixDepth, unsigned int pixNum, double *pixels, E1803_power_callback power_callback, void *userData);
		// Perform a stream raw, immediate movement to the given position with laser off.  DANGEROUS, moves at MAX speed and large movements may damage motors.  -33554431..33554432
		bool set_pos(int x, int y);
		// Perform a stream raw, immediate movement to the given position.  DANGEROUS, moves at MAX speed and large movements may damage motors.  -33554431..33554432
		bool set_pos(int x, int y, bool bLaserOn);
		// Perform a stream raw, immediate movement to the given position.  DANGEROUS, moves at MAX speed and large movements may damage motors.  -33554431..33554432
		bool set_pos(int x, int y, int z, unsigned char laserOn);
		// Specify a stream 2x2 matrix that contains scaling and rotation corrections for the output.  Overwrites all corrections specified with set_xy_correction().
		bool set_matrix(double m11, double m12, double m21, double m22);
		// Specifies a point in stream where execution has to stop until an external trigger signal (rising edge on ExtStart) or calling of release_trigger_point()
		bool set_trigger_point();
		// Immediately unpauses place where set_trigger_point() was set
		bool release_trigger_point();
		// Starts execution of all previously sent commands.  Always call this function after all data is sent.
		bool execute();
		const int executeDefaultTimeout = 30000;
		const int executeWaitLoopSleep = 50;
		enum class executemode {
			EXECMODE_IMMEDIATE = 0
			, EXECMODE_WAITUNTILSTARTMARK = 1
			, EXECMODE_FINISHMARK = 2
		};
		// Starts execution of all previously sent commands.  Always call this function after all data is sent.
		bool execute(executemode mode);
		// Starts execution of all previously sent commands.  Always call this function after all data is sent.  For Mode 1 or 2, timeoutDelay specifies when to return
		bool execute(executemode mode,int timeout);		
		delegate void executeMarkAsyncCompleteEvent(bool bSuccess);
		virtual event executeMarkAsyncCompleteEvent^ executeMarkAsyncComplete;
		bool executeMarkAsync();
		bool executeMarkAsync(int timeout);
		// Stops the currently running execution as fast as possible and drops all marking data and sent-but-not-processed parameters.  After calling, please resend marking/scanning parameters
		bool stop_execution();
		// bPause true pauses execution of commands.  bPause false resumes operations
		bool halt_execution(bool bPause);
		// Halts or continues the processing and output of marking data.
		bool halt_execution(unsigned char halt);
		// Stream pause marking for the given time
		bool delay(double delay);
		bool dynamic_data(struct oapc_bin_struct_dyn_data *dynData);
		// Returns a bit pattern that informs about state of the start and stop input pins
		unsigned int get_startstop_state();
		// Returns a bit pattern that informs about cards current operational state
		unsigned int get_card_state();
		unsigned int get_card_info();
		// Returns head status information in case the connected scanhead provides such data via STATUS signal of XY2 - 100 interface.
		unsigned int get_head_state();
		// Returns head status information in case the connected scanhead provides such data via STATUS signal of XY2 - 100 interface.
		unsigned int get_head_state(unsigned int flags);
		// Stream set the frequency and pulse-width to be used during marking at LaserA output of laser connector.  frequency - 25..20000000 Hz, pulse – pulse width in usec.
		bool set_laser_timing(double frequency, double pulse);
		// Stream set the frequency and pulse-width to be used at LaserB output of laser connector. To use LaserB as second frequency output, a laser mode with flag E1803_LASERMODE_DUAL has to be configured. frequency - 25..20000000 Hz, pulse – pulse width in usec
		bool set_laserb(double frequency, double pulse);
		// Stream set the frequency and pulse-width to be used during jumps, as stand-by frequency or as continuously running frequency at LaserA output of laser connector.
		bool set_standby(double frequency, double pulse);
		// Set the parameters for first pulse killer signal that is emitted via laser connector whenever the laser is turned on; this applies to YAG - modes only and is emitted as one single pulse at LaserB output.
		bool set_fpk(double fpk, double yag3QTime);
		// This function returns the combined space (in unit "commands") that is free.  -1 if both failed, -2 if only primary failed, -3 if only secondary failed
		int get_free_space();
		// This function returns the space (in unit "commands") that is free in primary buffers. -1 if failed
		int get_free_space_primary();
		// This function returns the space (in unit "commands") that is free in secondary buffers. -1 if failed
		int get_free_space_secondary();
		// This function returns the space (in unit "commands") that is free in one of the buffers.  buffer may be E1803_FREE_SPACE_PRIMARY or E1803_FREE_SPACE_SECONDARY.  Returns -1 if fail
		int get_free_space(int buffer);
		// Get the hardware version of the used board.
		unsigned short get_version_hardware();
		// Get the software version of the used board.
		unsigned short get_version_firmware();
		// Get the hardware and software version of the used board.
		void get_version(unsigned short *hwVersion, unsigned short *fwVersion);
		// Returns string version of underlying library "0.0.0"
		String^ get_library_version_string();
		// Returns an integer value which is an identifier specifying the version shared library. "Mmmrrr" where "M" is the major version, "m" the minor version number	and "r" the release count.
		unsigned int get_library_version();
		// Writes some specific data to outputs at E1803DLight controller. Here flags decides which output to use and value specifies what has to be written to this output
		bool write(unsigned int flags, unsigned int value);
		enum class commandbehavior {
			Stream = 1
			, Direct = 2 // immediate
		};
		// Sets the LP8_0..LP8_7 outputs of 8 bit laser port of laser interface connector without touching the related latch output.Total execution time of this command is 1 usec.  flags determines stream or immediate
		bool lp8_write(commandbehavior behavior, unsigned char value);
		// Sets the LP8_0..LP8_7 outputs of 8 bit laser port of laser interface connector without touching the related latch output.Total execution time of this command is 1 usec.  flags determines stream or immediate
		bool lp8_write(unsigned int flags, unsigned char value);
		// Stream sets the LP8 8 bit laser port of laser interface connector with freely definable delays and toggles the related latch output automatically
		bool lp8_write_latch(unsigned char on, double delay1, unsigned char value, double delay2, double delay3);
		// Stream sets the master oscillator output MO of laser interface connector to be used with e.g. fiber lasers.
		bool lp8_write_mo(unsigned char on);
		// Sets the 8 bit digital output port.
		bool digi_write(commandbehavior behavior, unsigned int value, unsigned int mask);
		// Sets the 8 bit digital output port.  flags determine stream or immediate
		bool digi_write(unsigned int flags, unsigned int value, unsigned int mask);
		// Immediately reads the 8 bit digital input port.
		unsigned int digi_read();
		// Stop execution and output of data until the given bitpattern(value) was detected at digital inputs(mask) of digital interface connector.
		bool digi_wait(unsigned long value, unsigned long mask);
		// Disables or enables marking on-the-fly functionality and specifies factors for X- and Y-direction.  Set both values to 0 to disable marking on-the-fly.
		bool digi_set_motf(double motfX, double motfY);
		// Disables or enables simulated marking on-the-fly functionality and specifies factors for X- and Ydirection.  Set both values to 0 to disable simulated marking on-the-fly.
		bool digi_set_motf_sim(double motfX, double motfY);
		// Halts the current marking operation for a given distance of the on-the-fly encoder.  flags determine encoder increments or distance
		bool digi_wait_motf(unsigned int flags, double dist);
		// This function can be used to specify which of the digital outputs has to be used for signalling "marking	in progress".  value - 0..7 of digital output or 0xFFFFFFFF for disabled.
		bool digi_set_mip_output(unsigned int value, unsigned int flags);
		// This function can be used to specify which of the digital outputs has to be used for signalling "waiting for external trigger". value - 0..7 of digital output or 0xFFFFFFFF for disabled.
		bool digi_set_wet_output(unsigned int value, unsigned int flags);
		// UNUSED with E1803
		bool ana_write(unsigned int flags, unsigned short a);
		// Immediately send data to RS232/RS485 serial interface using the serial interface parameters which are configured	in e1803.cfg configuration file.
		bool uart_write(String^ sendData);
		// Immediately send data to RS232/RS485 serial interface using the serial interface parameters which are configured	in e1803.cfg configuration file.
		bool uart_write(String^ sendData, unsigned int in_length);
		// Immediately send data to RS232/RS485 serial interface using the serial interface parameters which are configured	in e1803.cfg configuration file.
		bool uart_write(String^ sendData, unsigned int in_length, unsigned int *sentLength);
		// Immediately receive data from RS232/RS485 serial interface using the serial interface parameters which are configured in e1803.cfg configuration file.  Please make sure recvData is not garabage collected.
		bool uart_read(char *recvData, unsigned int maxLength, unsigned int *receivedLength);
		// Immediately receive data from RS232/RS485 serial interface using the serial interface parameters which are configured in e1803.cfg configuration file.  Please make sure recvData is not garabage collected.
		bool uart_read(unsigned int flags, char *recvData, unsigned int maxLength, unsigned int *receivedLength);

		unsigned int send_data(unsigned int flags, char *sendData, unsigned int length, unsigned int *sentLength);
		unsigned int recv_data(unsigned int flags, char *recvData, unsigned int maxLength);


		const int CSTATE_MARKING = E1803_CSTATE_MARKING;
		const int CSTATE_START_PRESSED = E1803_CSTATE_START_PRESSED;
		const int CSTATE_STOP_PRESSED = E1803_CSTATE_STOP_PRESSED;
		const int CSTATE_WAS_START_PRESSED = E1803_CSTATE_WAS_START_PRESSED;
		const int CSTATE_WAS_STOP_PRESSED = E1803_CSTATE_WAS_STOP_PRESSED;
		const int CSTATE_ERROR = E1803_CSTATE_ERROR;
		const int CSTATE_WAS_EXTTRIGGER = E1803_CSTATE_WAS_EXTTRIGGER;
		const int CSTATE_PROCESSING = E1803_CSTATE_PROCESSING;
		const int CSTATE_EMITTING = E1803_CSTATE_EMITTING;
		const int CSTATE_FILE_WRITE_ERROR = E1803_CSTATE_FILE_WRITE_ERROR;
		const int CSTATE_WAIT_EXTTRIGGER = E1803_CSTATE_WAIT_EXTTRIGGER;
		const int CSTATE_WAS_SILENTTRIGGER = E1803_CSTATE_WAS_SILENTTRIGGER;
		const int CSTATE_FILEMODE_ACTIVE = E1803_CSTATE_FILEMODE_ACTIVE;
		const int CSTATE_HALTED = E1803_CSTATE_HALTED;

		const int FILEMODE_OFF = E1803_FILEMODE_OFF;
		const int FILEMODE_LOCAL = E1803_FILEMODE_LOCAL;
		const int FILEMODE_SEND = E1803_FILEMODE_SEND;

		const int TUNE_EXTTRIG_DIGIIN7 = E1803_TUNE_EXTTRIG_DIGIIN7;
		const int TUNE_2D_MOTF = E1803_TUNE_2D_MOTF;
		const int TUNE_SAVE_SERIAL_STATES = E1803_TUNE_SAVE_SERIAL_STATES;
		const int TUNE_INVERT_LASERGATE = E1803_TUNE_INVERT_LASERGATE;
		const int TUNE_INVERT_LASERA = E1803_TUNE_INVERT_LASERA;
		const int TUNE_INVERT_LASERB = E1803_TUNE_INVERT_LASERB;
		const int TUNE_10V_ANALOGUE_XYZ = E1803_TUNE_10V_ANALOGUE_XYZ;
		const int TUNE_USE_A1_AS_Z = E1803_TUNE_USE_A1_AS_Z;
		const int TUNE_STUPI2D_XY2 = E1803_TUNE_STUPI2D_XY2;
		const int TUNE_XY2_18BIT = E1803_TUNE_XY2_18BIT;
		const int TUNE_XY2_20BIT = E1803_TUNE_XY2_20BIT;

		const int COMMAND_FLAG_STREAM = E1803_COMMAND_FLAG_STREAM;
		const int COMMAND_FLAG_DIRECT = E1803_COMMAND_FLAG_DIRECT;

		const int COMMAND_FLAG_ANA_MASK = E1803_COMMAND_FLAG_ANA_MASK;
		const int COMMAND_FLAG_ANA_A0 = E1803_COMMAND_FLAG_ANA_A0;
		const int COMMAND_FLAG_ANA_A1 = E1803_COMMAND_FLAG_ANA_A1;

		const int COMMAND_FLAG_WRITE_MASK = E1803_COMMAND_FLAG_WRITE_MASK;
		const int COMMAND_FLAG_WRITE_LP8MO = E1803_COMMAND_FLAG_WRITE_LP8MO;
		const int COMMAND_FLAG_WRITE_LP8LATCH = E1803_COMMAND_FLAG_WRITE_LP8LATCH;
		const int COMMAND_FLAG_WRITE_LASERGATE = E1803_COMMAND_FLAG_WRITE_LASERGATE;
		const int COMMAND_FLAG_WRITE_unused1 = E1803_COMMAND_FLAG_WRITE_unused1;
		const int COMMAND_FLAG_WRITE_unused2 = E1803_COMMAND_FLAG_WRITE_unused2;

		const int COMMAND_FLAG_MOTF_WAIT_INCS = E1803_COMMAND_FLAG_MOTF_WAIT_INCS;
		const int COMMAND_FLAG_MOTF_WAIT_BITS = E1803_COMMAND_FLAG_MOTF_WAIT_BITS;

		const int COMMAND_FLAG_XYCORR_FLIPXY = E1803_COMMAND_FLAG_XYCORR_FLIPXY;
		const int COMMAND_FLAG_XYCORR_MIRRORX = E1803_COMMAND_FLAG_XYCORR_MIRRORX;
		const int COMMAND_FLAG_XYCORR_MIRRORY = E1803_COMMAND_FLAG_XYCORR_MIRRORY;

		const int COMMAND_FLAG_SCANNER_VAR_POLYDELAY = E1803_COMMAND_FLAG_SCANNER_VAR_POLYDELAY;
		const int COMMAND_FLAG_UART1 = E1803_COMMAND_FLAG_UART1;
		const int COMMAND_FLAG_UART2 = E1803_COMMAND_FLAG_UART2;

	private:
		String^ ReturnValueToString(int ret);
		bool TestReturn(int ret);
		// loops after an execute
		bool executeMarkLoop(executemode mode, int timeout, bool useEvents);
		void executeAsyncThread(Object^ timeout);
		bool bCancelExecWait;
	};
}
