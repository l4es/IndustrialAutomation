#ifndef E1803_H
#define E1803_H

#define E1803_DLL_VERSION 1

#define E1803_OK                        0 // operation could be finished successfully
#define E1803_ERROR_INVALID_CARD      101 // wrong/illegal card number specified
#define E1803_ERROR_NO_CONNECTION     102 // could not connect to card
#define E1803_ERROR_NO_MEMORY         103 // not enough memory available
#define E1803_ERROR_UNKNOWN_FW        104 // unknown/incompatible firmware version
#define E1803_ERROR                   105 // unknown/unspecified error
#define E1803_ERROR_TRANSMISSION      106 // transmission of data failed
#define E1803_ERROR_FILEOPEN          107 // opening a file failed
#define E1803_ERROR_FILEWRITE         108 // writing data to a file failed
#define E1803_ERROR_BORD_NA           109 // a required extension board is not available
#define E1803_ERROR_INVALID_DATA      110 // a function was called with invalid data or by using an operation mode where this function is not used/allowed
#define E1803_ERROR_UNKNOWN_BOARD     111 // trying to access a board that is not a scanner controller
#define E1803_ERROR_FILENAME          112 // there is an error with the given filename (too long, too many subdirectories, illegal characters,...)
#define E1803_ERROR_NOT_SUPPORTED     113 // the requested feature is not supported by the current firmware version
#define E1803_ERROR_NO_DATA_AVAILABLE 114 // tried to receive some data but there are none avilable yet

#define E1803_MAX_HEAD_NUM         50 // maximum number of cards that can be controlled

#define E1803_LASER_FPK       0x90000000 // for backwards compatibility set two bits here
#define E1803_LASER_FREQ_ON1  0x40000000 // switch on-frequency/q-switch immediately
#define E1803_LASER_FREQ_ON2  0x20000000 // switch on-frequency/q-switch after yag3QTime; in mode YAG 2 it is equal to FPK time
//#define E1803_LASER_FPK       0x10000000
#define E1803_LASER_FREQ_OFF  0x08000000 // output a stand-by frequency during jumps
#define E1803_LASER_FREQ_DUAL 0x04000000 // output a second frequency at LaserB permanently

#define E1803_LASERMODE_CO2   (1|E1803_LASER_FREQ_ON1|E1803_LASER_FREQ_OFF)
#define E1803_LASERMODE_YAG1  (2|E1803_LASER_FREQ_ON1|E1803_LASER_FREQ_OFF|E1803_LASER_FPK)
#define E1803_LASERMODE_YAG2  (3|E1803_LASER_FREQ_ON2|E1803_LASER_FREQ_OFF|E1803_LASER_FPK)
#define E1803_LASERMODE_YAG3  (4|E1803_LASER_FREQ_ON2|E1803_LASER_FREQ_OFF|E1803_LASER_FPK)
#define E1803_LASERMODE_CRF   (5|E1803_LASER_FREQ_ON1) // laser mode with continuously running frequency
#define E1803_LASERMODE_DFREQ (6|E1803_LASER_FREQ_ON1|E1803_LASER_FREQ_OFF|E1803_LASER_FREQ_DUAL) // dual frequency laser mode which emits a second frequency at LaserB
#define E1803_LASERMODE_IPG   (7|E1803_LASER_FREQ_ON1)

#define E1803_CSTATE_MARKING            0x00001
#define E1803_CSTATE_START_PRESSED      0x00002
#define E1803_CSTATE_STOP_PRESSED       0x00004
#define E1803_CSTATE_WAS_START_PRESSED  0x00008
#define E1803_CSTATE_WAS_STOP_PRESSED   0x00010
#define E1803_CSTATE_ERROR              0x00020 // REMOVE?
#define E1803_CSTATE_WAS_EXTTRIGGER     0x00040 // internal use only, do not check this flag
#define E1803_CSTATE_PROCESSING         0x00080
#define E1803_CSTATE_EMITTING           0x00100
#define E1803_CSTATE_FILE_WRITE_ERROR   0x00200
#define E1803_CSTATE_WAIT_EXTTRIGGER    0x00400
#define E1803_CSTATE_WAS_SILENTTRIGGER  0x00800 // internal use only, do not check this flag
#define E1803_CSTATE_FILEMODE_ACTIVE    0x01000 // internal use only, do not check this flag
#define E1803_CSTATE_HALTED             0x02000
#define E1803_CSTATE_WRITING_DATA       0x04000
#define E1803_CSTATE_WRITING_DATA_ERROR 0x08000
#define E1803_CSTATE_WAS_MOTION_STARTED 0x10000
#define E1803_CSTATE_WAS_MOTION_STOPPED 0x20000

#define E1803_MAX_CORRECTION_TABLES 16

#define E1803_FILEMODE_OFF   0xFFFFFFFF
#define E1803_FILEMODE_LOCAL 0
#define E1803_FILEMODE_SEND  1

#define E1803_BSTATE_MOTION_EB         0x0001

#define E1803_TUNE_EXTTRIG_DIGIIN7     0x000001
#define E1803_TUNE_2D_MOTF             0x000002
#define E1803_TUNE_SAVE_SERIAL_STATES  0x000004 // when this option is set the current state of serial numbers is stored during marking and does not get los on power cycle
#define E1803_TUNE_INVERT_LASERGATE    0x000008
#define E1803_TUNE_INVERT_LASERA       0x000010
#define E1803_TUNE_INVERT_LASERB       0x000020
//#define E1803_TUNE_LASERA_GPO          0x0040
//#define E1803_TUNE_LASERB_GPO          0x0080
#define E1803_TUNE_10V_ANALOGUE_XYZ    0x000100
#define E1803_TUNE_USE_A1_AS_Z         0x000200
#define E1803_TUNE_STUPI2D_XY2         0x000400
//#define E1803_TUNE_EXACT3D_XY2         0x0800 this is the default operation mode
#define E1803_TUNE_XY2_18BIT           0x001000
#define E1803_TUNE_XY2_20BIT           0x002000
#define E1803_TUNE_DISABLE_TEST        0x004000
#define E1803_TUNE_INVERT_MIP          0x008000
#define E1803_TUNE_INVERT_WET          0x010000
#define E1803_TUNE_EXTTRIG_DIGIIN6     0x020000

#define E1803_COMMAND_FLAG_STREAM      0x00000001 // command has to be enqueued in stream
#define E1803_COMMAND_FLAG_DIRECT      0x00000002 // command has to be executed directly and immediately, library waits for response
#define E1803_COMMAND_FLAG_ASYNC       0x00000004 // command has to be executed whenever there is some time for it, for read-functions this means a value may be returned only after several calls
#define E1803_COMMAND_FLAG_DONOTWAIT   0x00000008 // do not wait for the end of the command but execute it in parallel

#define E1803_COMMAND_FLAG_ANA_AOUT0   0x00000100 // analogue output
#define E1803_COMMAND_FLAG_ANA_AOUT1   0x00000200 // analogue output
#define E1803_COMMAND_FLAG_ANA_AIN0    0x00002000 // analogue input
#define E1803_COMMAND_FLAG_ANA_AIN1    0x00004000 // analogue input
#define E1803_COMMAND_FLAG_ANA_AIN2    0x00010000 // analogue input
#define E1803_COMMAND_FLAG_ANA_MASK    (E1803_COMMAND_FLAG_ANA_AOUT0|E1803_COMMAND_FLAG_ANA_AOUT1|E1803_COMMAND_FLAG_ANA_AIN0|E1803_COMMAND_FLAG_ANA_AIN1|E1803_COMMAND_FLAG_ANA_AIN2)

#define E1803_COMMAND_FLAG_AXIS_0      0x00000100
#define E1803_COMMAND_FLAG_AXIS_1      0x00000200
#define E1803_COMMAND_FLAG_AXIS_2      0x00000400
#define E1803_COMMAND_FLAG_AXIS_3      0x00000800
#define E1803_COMMAND_FLAG_AXIS_MASK   (E1803_COMMAND_FLAG_AXIS_0|E1803_COMMAND_FLAG_AXIS_1|E1803_COMMAND_FLAG_AXIS_2|E1803_COMMAND_FLAG_AXIS_3)
#define E1803_COMMAND_FLAG_AXIS_SHIFT  8

#define E1803_MOTION_REFSTEP_N           0x0001
#define E1803_MOTION_REFSTEP_P           0x0002
#define E1803_MOTION_REFSTEP_INV_SWITCH  0x8000  // invert the logic of reference switch
#define E1803_MOTION_MAX_AXES                (4) // maximum number of axes that can be controlled


#define E1803_COMMAND_FLAG_WRITE_MASK           0x0F00
#define E1803_COMMAND_FLAG_WRITE_LP8MO          0x0100
#define E1803_COMMAND_FLAG_WRITE_LP8LATCH       0x0200
#define E1803_COMMAND_FLAG_WRITE_LASERGATE      0x0500
#define E1803_COMMAND_FLAG_WRITE_unused1        0x0600
#define E1803_COMMAND_FLAG_WRITE_unused2        0x0700

#define E1803_COMMAND_FLAG_MOTF_WAIT_INCS      0x0100
#define E1803_COMMAND_FLAG_MOTF_WAIT_BITS      0x0200

#define E1803_COMMAND_FLAG_XYCORR_FLIPXY       0x0100
#define E1803_COMMAND_FLAG_XYCORR_MIRRORX      0x0200
#define E1803_COMMAND_FLAG_XYCORR_MIRRORY      0x0400

#define E1803_COMMAND_FLAG_SCANNER_VAR_POLYDELAY 0x0100

#define E1803_COMMAND_FLAG_DYNDATA_MARK_FONTENTRY 0x0100

#define E1803_PIXELMODE_NO_JUMPS           0x0001 // do not perform jumps also in case power is 0%; when this flag is set, power threshold is ignored
#define E1803_PIXELMODE_JUMP_N_SHOOT       0x0002 // no continuous movement, jump to the marking position and shoot there for laseroff minus laseron time
#define E1803_PIXELMODE_HW_POWER_CONTROL   0x0004 // power control is done by hardware internally, this is currently supported for E1803_LASERMODE_IPG only
#define E1803_PIXELMODE_GATE_POWER_CONTROL 0x0008 // special mode suitable for b/w bitmaps, laser gate is switched on/off depending on power >= or < 50%
#define E1803_FREE_SPACE_PRIMARY   0
#define E1803_FREE_SPACE_SECONDARY 1

#define HEAD_STATE_MASK    0xFE0000 // AND-concatenate this with the return value of E1803_get_head_state() to evaluate the type of head:
#define HEAD_STATE_2D_HEAD 0x060000
#define HEAD_STATE_3D_HEAD 0x020000
#define HEAD_STATE_ERROR   0xFE0000 // no or invalid head data received

#define E1803_COMMAND_FLAG_UART1 0x0100 // RS232/RS485 interface on E1803 baseboard (naming troubles, this is UART0 from hardwares point of view)
#define E1803_COMMAND_FLAG_UART2 0x0200 // currently unused (naming troubles, this is UART1 from hardwares point of view)

#define E1803_COMMAND_FLAG_HEAD_STATE_RAW 0x0100


struct oapc_bin_struct_dyn_data; //forward declaration in case oapc_libio.h is not used

#ifdef __cplusplus
extern "C"
{
#endif
   typedef int (*E1803_power_callback)(unsigned char n,double power,void *userData); /** type definition for the callback function that has to be provided by client for setting power */
#ifdef __cplusplus
};
#endif

#ifndef ENV_E1803
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the E1803_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// E1803_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef ENV_LINUX
 #ifdef E1803_EXPORTS
  #define E1803_API __attribute ((visibility ("default")))
 #else
  #define E1803_API
 #endif
#else
 #ifdef E1803_EXPORTS
  #define E1803_API __declspec(dllexport)
 #else
  #define E1803_API __declspec(dllimport)
 #endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif
   struct oapc_bin_struct_dyn_data; //forward declaration

   // ***** E1803 easy interface functions *************************************************************************************
   // base functions
   E1803_API unsigned char E1803_set_connection(const char *address);
   E1803_API unsigned char E1803_set_connection_(const char *address);
   E1803_API void          E1803_set_password(const unsigned char n,const char *ethPwd);
   E1803_API int           E1803_set_filepath(unsigned char n,const char *fname,unsigned int mode);
   E1803_API int           E1803_set_debug_logfile(const unsigned char n,const char *path,const unsigned char flags); // for logging of local function calls, suitable for debugging of own application
   E1803_API int           E1803_write_debug_logfile(const unsigned char n,const char *format,...); // for writing own debug texts into log
   E1803_API void          E1803_close(unsigned char n);
   E1803_API int           E1803_load_correction(unsigned char n, const char* filename,unsigned char tableNum);
   E1803_API int           E1803_switch_correction(unsigned char n,unsigned char tableNum);
   E1803_API int           E1803_set_xy_correction(const unsigned char n,const unsigned int flags,const double gainX, const double gainY, const double rot,const int offsetX, const int offsetY,const double slantX, const double slantY);
   E1803_API int           E1803_set_z_correction(const unsigned char n,const unsigned int h,const double xy_to_z_ratio,const int res2);
   E1803_API int           E1803_tune(const unsigned char n,const unsigned int tuneFlags);
   E1803_API int           E1803_set_speeds(unsigned char n, double jumpspeed,double markspeed);
   E1803_API int           E1803_set_laser_delays(unsigned char n,double ondelay,double offdelay);
   E1803_API int           E1803_set_laser_mode(unsigned char n, unsigned int mode);
   E1803_API int           E1803_set_laser(const unsigned char n,const unsigned int flags,const char on);
   E1803_API int           E1803_set_wobble(unsigned char n,unsigned int x,unsigned int y,double freq);
   E1803_API int           E1803_set_scanner_delays(const unsigned char n,const unsigned int flags,const double jumpdelay,const double markdelay,const double polydelay);
   E1803_API int           E1803_jump_abs(unsigned char n,int x,int y,int z);
   E1803_API int           E1803_mark_abs(unsigned char n,int x,int y,int z);
   E1803_API int           E1803_set_pixelmode(const unsigned char n,const unsigned int mode,const double powerThres,const unsigned int res);
   E1803_API int           E1803_mark_pixelline(const unsigned char n,int x,int y,int z,const int pixWidth,const int pixHeight,const int pixDepth,unsigned int pixNum,const double *pixels,E1803_power_callback power_callback,void *userData);
   E1803_API int           E1803_set_pos(unsigned char n,int x,int y,int z,unsigned char laserOn);
   E1803_API int           E1803_set_matrix(unsigned char n, double m11, double m12, double m21, double m22);
   E1803_API int           E1803_set_trigger_point(unsigned char n);
   E1803_API int           E1803_release_trigger_point(unsigned char n);
   E1803_API int           E1803_set_sync(const unsigned char n,const unsigned int flags,const unsigned int value);
   E1803_API unsigned int  E1803_get_sync(const unsigned char n);
   E1803_API int           E1803_execute(unsigned char n);
   E1803_API int           E1803_stop_execution(unsigned char n);
   E1803_API int           E1803_halt_execution(unsigned char n,unsigned char halt);
   E1803_API int           E1803_delay(unsigned char n,double delay);
   E1803_API int           E1803_dynamic_data(unsigned char n,struct oapc_bin_struct_dyn_data *dynData);
   E1803_API int           E1803_dynamic_mark(const unsigned char n,const unsigned int flags,const void *value);
   E1803_API unsigned int  E1803_get_startstop_state(unsigned char n);
   E1803_API unsigned int  E1803_get_card_state(unsigned char n); // DEPRECATED, use E1803_get_card_state2() instead!
   E1803_API int           E1803_get_card_state2(const unsigned char n, unsigned int *state);
   E1803_API unsigned int  E1803_get_card_info(unsigned char n);
   E1803_API unsigned int  E1803_get_head_state(const unsigned char n,const unsigned int flags);
   E1803_API int           E1803_set_laser_timing(unsigned char n,double frequency,double pulse);
   E1803_API int           E1803_set_laserb(const unsigned char n,const double frequency,const double pulse);
   E1803_API int           E1803_set_standby(unsigned char n,double frequency,double pulse);
   E1803_API int           E1803_set_fpk(unsigned char n,double fpk,double yag3QTime);
   E1803_API int           E1803_get_free_space(unsigned char n,int buffer);
   E1803_API void          E1803_get_version(unsigned char n,unsigned short *hwVersion,unsigned short *fwVersion);
   E1803_API int           E1803_get_library_version();
   E1803_API int           E1803_write(unsigned char n,unsigned int flags,unsigned int value);

   E1803_API int           E1803_lp8_write(const unsigned char n,const unsigned int flags,const unsigned char value);
   E1803_API int           E1803_lp8_write_latch(unsigned char n,unsigned char on,double delay1,unsigned char value,double delay2,double delay3);
   E1803_API int           E1803_lp8_write_mo(unsigned char n,unsigned char on);

   E1803_API int           E1803_digi_write(const unsigned char n,const unsigned int flags,const unsigned int value,const unsigned int mask);
   E1803_API int           E1803_digi_pulse(const unsigned char n,const unsigned int flags,const unsigned int in_value,const unsigned int mask,const int unsigned pulses,const double delay1,const double delay2);
   E1803_API unsigned int  E1803_digi_read(unsigned char n); // DEPRECATED, use E1803_digi_read2() instead!
   E1803_API int           E1803_digi_read2(const unsigned char n, unsigned int *value);
   E1803_API int           E1803_digi_wait(unsigned char n,unsigned long value,unsigned long mask);
   E1803_API int           E1803_digi_set_motf(unsigned char n,double motfX,double motfY);
   E1803_API int           E1803_digi_set_motf_sim(unsigned char n,double motfX,double motfY);
   E1803_API int           E1803_digi_wait_motf(const unsigned char n,const unsigned int flags,const double dist);
   E1803_API int           E1803_digi_set_mip_output(unsigned char n,unsigned int value,unsigned int flags);
   E1803_API int           E1803_digi_set_wet_output(const unsigned char n,const unsigned int value,const unsigned int flags);

   E1803_API int           E1803_ana_write(const unsigned char n,const unsigned int flags,const unsigned short a);
   E1803_API int           E1803_ana_read(const unsigned char n,const unsigned int flags,unsigned short *a);

   E1803_API int           E1803_uart_write(const unsigned char n,const unsigned int flags,const char *sendData,const unsigned int in_length,unsigned int *sentLength);
   E1803_API int           E1803_uart_read(const unsigned char n,const unsigned int flags,char *recvData,const unsigned int maxLength,unsigned int *receivedLength);

   E1803_API int           E1803_motion_set_steps(const unsigned char n, const unsigned int flags,const double steps);
   E1803_API int           E1803_motion_set_limits(const unsigned char n,const unsigned int flags,const double llimit,const double hlimit,const double in_slimit);
   E1803_API int           E1803_motion_set_accel(const unsigned char n,const unsigned int flags,const double accel);
   E1803_API int           E1803_motion_set_speed(const unsigned char n,const unsigned int flags,double speed);
   E1803_API int           E1803_motion_reference(const unsigned char n,const unsigned int flags,const unsigned int axis,const unsigned int mode,const double leaveDist,double speedStep0,double speedStep1);
   E1803_API int           E1803_motion_move_abs(const unsigned char n,const unsigned int flags,const double pos0,const double pos1,const double pos2,const double pos3);
   E1803_API int           E1803_motion_move_rel(const unsigned char n,const unsigned int flags,const double pos0,const double pos1,const double pos2,const double pos3);
   E1803_API int           E1803_motion_stream_wait(const unsigned char n);
   E1803_API int           E1803_motion_stop(const unsigned char n);
   E1803_API int           E1803_motion_brake(const unsigned char n);
   E1803_API int           E1803_motion_set_pos(const unsigned char n,const unsigned int flags,const double pos);
   E1803_API int           E1803_motion_get_pos(const unsigned char n,const unsigned char axisNum,double *pos);

   E1803_API unsigned int  E1803_send_data(unsigned char n,unsigned int flags,const char *sendData,unsigned int length,unsigned int *sentLength);
   E1803_API unsigned int  E1803_recv_data(unsigned char n,unsigned int flags,char *recvData,unsigned int maxLength);

   // ***** end of E1803 easy interface functions ******************************************************************************
#ifdef __cplusplus
};
#endif // __cplusplus
#endif

#endif //E1803_H
