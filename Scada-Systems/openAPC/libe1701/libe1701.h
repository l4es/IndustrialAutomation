#ifndef E1701_H
#define E1701_H

#define E1701_DLL_VERSION 1

#define E1701_OK                        0 // operation could be finished successfully
#define E1701_ERROR_INVALID_CARD      101 // wrong/illegal card number specified
#define E1701_ERROR_NO_CONNECTION     102 // could not connect to card
#define E1701_ERROR_NO_MEMORY         103 // not enough memory available
#define E1701_ERROR_UNKNOWN_FW        104 // unknown/incompatible firmware version
#define E1701_ERROR                   105 // unknown/unspecified error
#define E1701_ERROR_TRANSMISSION      106 // transmission of data failed
#define E1701_ERROR_FILEOPEN          107 // opening a file failed
#define E1701_ERROR_FILEWRITE         108 // writing data to a file failed
#define E1701_ERROR_BORD_NA           109 // a base- or extension board that would be required for a function is not available
#define E1701_ERROR_INVALID_DATA      110 // a function was called with invalid data or by using an operation mode where this function is not used/allowed
#define E1701_ERROR_UNKNOWN_BOARD     111 // trying to access a board that is not a scanner controller
#define E1701_ERROR_FILENAME          112 // there is an error with the given filename (too long, too many subdirectories, illegal characters,...)
#define E1701_ERROR_NOT_SUPPORTED     113 // the requested feature is not supported by the current firmware version
#define E1701_ERROR_NO_DATA_AVAILABLE 114 // tried to receive some data but there are none avilable yet

#define E1701_MAX_HEAD_NUM         16 // maximum number of cards that can be controlled

#define E1701_LASER_FPK         0x90000000 // for backwards compatibility set two bits here
#define E1701_LASER_FREQ_ON1    0x40000000 // switch on-frequency/q-switch immediately
#define E1701_LASER_FREQ_ON2    0x20000000 // switch on-frequency/q-switch after yag3QTime; in mode YAG 2 it is equal to FPK time
//#define E1701_LASER_FPK         0x10000000
#define E1701_LASER_FREQ_OFF    0x08000000 // output a stand-by frequency during jumps
#define E1701_LASER_FREQ_DUAL   0x04000000 // output a second frequency at LaserB permanently

#define E1701_LASERMODE_CO2     (1|E1701_LASER_FREQ_ON1|E1701_LASER_FREQ_OFF)
#define E1701_LASERMODE_YAG1    (2|E1701_LASER_FREQ_ON1|E1701_LASER_FREQ_OFF|E1701_LASER_FPK)
#define E1701_LASERMODE_YAG2    (3|E1701_LASER_FREQ_ON2|E1701_LASER_FREQ_OFF|E1701_LASER_FPK)
#define E1701_LASERMODE_YAG3    (4|E1701_LASER_FREQ_ON2|E1701_LASER_FREQ_OFF|E1701_LASER_FPK)
#define E1701_LASERMODE_CRF     (5|E1701_LASER_FREQ_ON1) // laser mode with continuously running frequency
#define E1701_LASERMODE_DFREQ   (6|E1701_LASER_FREQ_ON1|E1701_LASER_FREQ_OFF|E1701_LASER_FREQ_DUAL) // dual frequency laser mode which emits a second frequency at LaserB
#define E1701_LASERMODE_unused  (7)
#define E1701_LASERMODE_IPG     (8|E1701_LASER_FREQ_ON1|E1701_LASER_FREQ_OFF)

#define E1701_CSTATE_MARKING            0x00001
#define E1701_CSTATE_START_PRESSED      0x00002
#define E1701_CSTATE_STOP_PRESSED       0x00004
#define E1701_CSTATE_WAS_START_PRESSED  0x00008
#define E1701_CSTATE_WAS_STOP_PRESSED   0x00010
#define E1701_CSTATE_ERROR              0x00020 // REMOVE?
#define E1701_CSTATE_WAS_EXTTRIGGER     0x00040 // internal use only, do not check this flag
#define E1701_CSTATE_PROCESSING         0x00080
#define E1701_CSTATE_EMITTING           0x00100
#define E1701_CSTATE_FILE_WRITE_ERROR   0x00200
#define E1701_CSTATE_WAIT_EXTTRIGGER    0x00400
#define E1701_CSTATE_WAS_SILENTTRIGGER  0x00800 // internal use only, do not check this flag
#define E1701_CSTATE_FILEMODE_ACTIVE    0x01000 // internal use only, do not check this flag
#define E1701_CSTATE_HALTED             0x02000
#define E1701_CSTATE_WRITING_DATA       0x04000
#define E1701_CSTATE_WRITING_DATA_ERROR 0x08000

#define E1701_MAX_CORRECTION_TABLES 16

#define E1701_FILEMODE_OFF   0xFFFFFFFF
#define E1701_FILEMODE_LOCAL 0
#define E1701_FILEMODE_SEND  1

#define E1701_BSTATE_XY2_100_BB        0x0001
#define E1701_BSTATE_ILDA_BB           0x0002
#define E1701_BSTATE_LP8_EB            0x0100
#define E1701_BSTATE_DIGI_EB           0x0200
#define E1701_BSTATE_LY001_BB          0x0400

#define E1701_BSTATE_BB_MASK          (E1701_BSTATE_XY2_100_BB|E1701_BSTATE_ILDA_BB|E1701_BSTATE_LY001_BB)

#define E1701_TUNE_EXTTRIG_DIGIIN7     0x000001
#define E1701_TUNE_2D_MOTF             0x000002
#define E1701_TUNE_SAVE_SERIAL_STATES  0x000004 // when this option is set the current state of serial numbers is stored during marking and does not get los on power cycle
#define E1701_TUNE_INVERT_LASERGATE    0x000008
#define E1701_TUNE_INVERT_LASERA       0x000010
#define E1701_TUNE_INVERT_LASERB       0x000020
#define E1701_TUNE_LASERA_GPO          0x000040
#define E1701_TUNE_LASERB_GPO          0x000080
#define E1701_TUNE_10V_ANALOGUE_XYZ    0x000100
#define E1701_TUNE_USE_A1_AS_Z         0x000200
#define E1701_TUNE_STUPI2D_XY2         0x000400
//#define E1701_TUNE_EXACT3D_XY2         0x0800 // default since FW version 30
#define E1701_TUNE_XY2_18BIT           0x001000
#define E1701_TUNE_reserved            0x002000 // used by E1701P firmware part
#define E1701_TUNE_DISABLE_TEST        0x004000
#define E1701_TUNE_INVERT_MIP          0x008000
#define E1701_TUNE_INVERT_WET          0x010000
#define E1701_TUNE_EXTTRIG_DIGIIN6     0x020000

#define E1701_COMMAND_FLAG_STREAM      0x0001 // command has to be enqueued in stream
#define E1701_COMMAND_FLAG_DIRECT      0x0002 // command has to be executed directly and immediately
#define E1701_COMMAND_FLAG_PASSIVE     0x0004 // do not send a request ot the hardware but use data which are already buffered

#define E1701_COMMAND_FLAG_WRITE_MASK             0x0F00
#define E1701_COMMAND_FLAG_WRITE_LP8MO            0x0100
#define E1701_COMMAND_FLAG_WRITE_LP8LATCH         0x0200
#define E1701_COMMAND_FLAG_WRITE_LASERA_GPO       0x0300
#define E1701_COMMAND_FLAG_WRITE_LASERB_GPO       0x0400
#define E1701_COMMAND_FLAG_WRITE_LASERGATE        0x0500
//#define E1701_COMMAND_FLAG_WRITE_SLICETHICKNESS   0x0600
#define E1701_COMMAND_FLAG_WRITE_SPOTSIZE         0x0700

#define E1701_COMMAND_FLAG_DYNDATA_MARK_FONTENTRY 0x0100

#define E1701_COMMAND_FLAG_MOTF_WAIT_INCS         0x0100
#define E1701_COMMAND_FLAG_MOTF_WAIT_BITS         0x0200

#define E1701_COMMAND_FLAG_XYCORR_FLIPXY       0x0100
#define E1701_COMMAND_FLAG_XYCORR_MIRRORX      0x0200
#define E1701_COMMAND_FLAG_XYCORR_MIRRORY      0x0400

#define E1701_COMMAND_FLAG_SCANNER_VAR_POLYDELAY  0x0100

#define E1701_PIXELMODE_NO_JUMPS           0x0001 // do not perform jumps also in case power is 0%; when this flag is set, power threshold is ignored
#define E1701_PIXELMODE_JUMP_N_SHOOT       0x0002 // no continuous movement, jump to the marking position and shoot there for laseroff minus laseron time
#define E1701_PIXELMODE_HW_POWER_CONTROL   0x0004 // power control is done by hardware internally, this is currently supported for E1701_LASERMODE_IPG only
#define E1701_PIXELMODE_GATE_POWER_CONTROL 0x0008 // special mode suitable for b/w bitmaps, laser gate is switched on/off depending on power >= or < 50%

#define E1701_FREE_SPACE_PRIMARY   0
#define E1701_FREE_SPACE_SECONDARY 1

struct oapc_bin_struct_dyn_data; //forward declaration in case oapc_libio.h is not used

#ifdef __cplusplus
extern "C"
{
#endif
   typedef int (*E1701_power_callback)(unsigned char n,double power,void *userData); /** type definition for the callback function that has to be provided by client for setting power */
#ifdef __cplusplus
};
#endif

#ifndef ENV_E1701
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the E1701_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// E1701_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef ENV_LINUX
 #ifdef E1701_EXPORTS
  #define E1701_API __attribute ((visibility ("default")))
 #else
  #define E1701_API
 #endif
#else
 #ifdef E1701_EXPORTS
  #define E1701_API __declspec(dllexport)
 #else
  #define E1701_API __declspec(dllimport)
 #endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif
   // ***** E1701 easy interface functions *************************************************************************************
   // base functions
   E1701_API unsigned char E1701_set_connection(const char *address);
   E1701_API unsigned char E1701_set_connection_(const char *address);
   E1701_API void          E1701_set_password(const unsigned char n,const char *ethPwd);
   E1701_API int           E1701_set_filepath(unsigned char n,const char *fname,unsigned int mode);
   E1701_API int           E1701_set_logfile(unsigned char n,const char *path); // DEPRECATED;
   E1701_API int           E1701_set_debug_logfile(const unsigned char n,const char *path,const unsigned char flags); // for logging of local function calls, suitable for debugging of own application
   E1701_API int           E1701_write_debug_logfile(const unsigned char n,const char *format,...); // for writing own debug texts into log
   E1701_API void          E1701_close(unsigned char n);
   E1701_API int           E1701_load_correction(unsigned char n, const char* filename,unsigned char tableNum);
   E1701_API int           E1701_switch_correction(unsigned char n,unsigned char tableNum);
   E1701_API int           E1701_set_xy_correction(unsigned char n, double gainX, double gainY,double rot,int offsetX,int offsetY); // DEPRECATED, use E1701_set_xy_correction3 instead!
   E1701_API int           E1701_set_xy_correction2(const unsigned char n, const double gainX, const double gainY, const double rot,const int offsetX, const int offsetY,const double slantX, const double slantY); // DEPRECATED, use E1701_set_xy_correction3 instead!
   E1701_API int           E1701_set_xy_correction3(const unsigned char n,const unsigned int flags,const double gainX, const double gainY, const double rot, const int offsetX, const int offsetY, const double slantX, const double slantY);
   E1701_API int           E1701_set_z_correction(const unsigned char n,const unsigned int h,const double xy_to_z_ratio,const int res);
   E1701_API int           E1701_tune(const unsigned char n,const unsigned int tuneFlags);
   E1701_API int           E1701_set_speeds(unsigned char n, double jumpspeed,double markspeed);
   E1701_API int           E1701_set_laser_delays(unsigned char n,double ondelay,double offdelay);
   E1701_API int           E1701_set_laser_mode(unsigned char n, unsigned int mode);
   E1701_API int           E1701_set_laser(const unsigned char n,const unsigned int flags,const char on);
   E1701_API int           E1701_set_wobble(unsigned char n,unsigned int x,unsigned int y,double freq);
   E1701_API int           E1701_set_scanner_delays(unsigned char n,double jumpdelay,double markdelay,double polydelay);// DEPRECATED, use E1701_set_scanner_delays2 instead!
   E1701_API int           E1701_set_scanner_delays2(const unsigned char n,const unsigned int flags,const double jumpdelay,const double markdelay,const double polydelay);
   E1701_API int           E1701_jump_abs(unsigned char n,int x,int y,int z);
   E1701_API int           E1701_mark_abs(unsigned char n,int x,int y,int z);
   E1701_API int           E1701_set_pixelmode(const unsigned char n,const unsigned int mode,const double powerThres,const unsigned int res);
   E1701_API int           E1701_mark_pixelline(const unsigned char n,int x,int y,int z,const int pixWidth,const int pixHeight,const int pixDepth,unsigned int pixNum,const double *pixels,E1701_power_callback power_callback,void *userData);
   E1701_API int           E1701_set_pos(unsigned char n,int x,int y,int z,unsigned char laserOn);
   E1701_API int           E1701_set_matrix(unsigned char n, double m11, double m12, double m21, double m22);// DEPRECATED, use E1701_set_xy_correction3 instead!
   E1701_API int           E1701_set_matrix2(const unsigned char n,const unsigned int flags,const double m11,const double m12,const double m21,const double m22);
   E1701_API int           E1701_set_trigger_point(unsigned char n);
   E1701_API int           E1701_release_trigger_point(unsigned char n);
   E1701_API int           E1701_set_sync(const unsigned char n,const unsigned int flags,const unsigned int value);
   E1701_API unsigned int  E1701_get_sync(const unsigned char n);
   E1701_API int           E1701_execute(unsigned char n);
   E1701_API int           E1701_stop_execution(unsigned char n);
   E1701_API int           E1701_halt_execution(unsigned char n,unsigned char halt);
   E1701_API int           E1701_delay(unsigned char n,double delay);
   E1701_API int           E1701_dynamic_data(unsigned char n,struct oapc_bin_struct_dyn_data *dynData);
   E1701_API int           E1701_dynamic_mark(const unsigned char n,const unsigned int flags,const void *value);
   E1701_API unsigned int  E1701_get_startstop_state(unsigned char n);
   E1701_API unsigned int  E1701_get_card_state(unsigned char n); // DEPRECATED: use E1701_get_card_state2() instead!
   E1701_API int           E1701_get_card_state2(const unsigned char n, unsigned int *state);
   E1701_API unsigned int  E1701_get_card_info(unsigned char n);
   E1701_API int           E1701_set_laser_timing(unsigned char n,double frequency,double pulse);
   E1701_API int           E1701_set_laserb(const unsigned char n,const double frequency,const double pulse);
   E1701_API int           E1701_set_standby(unsigned char n,double frequency,double pulse);
   E1701_API int           E1701_set_fpk(unsigned char n,double fpk,double yag3QTime);
   E1701_API int           E1701_set_sky_params(unsigned char n,double angle, unsigned int fadeIn,unsigned int fadeOut);
   E1701_API int           E1701_get_free_space(unsigned char n,int buffer);
   E1701_API void          E1701_get_version(unsigned char n,unsigned short *hwVersion,unsigned short *fwVersion);
   E1701_API int           E1701_get_library_version();
   E1701_API int           E1701_write(unsigned char n,unsigned int flags,unsigned int value);

   // LP8 extension board functions
   E1701_API int           E1701_lp8_write(unsigned char n,unsigned char value); // DEPRECATED; please use E1701_lp8_write2() instead!
   E1701_API int           E1701_lp8_write2(unsigned char n,unsigned int flags,unsigned char value);
   E1701_API int           E1701_lp8_write_latch(unsigned char n,unsigned char on,double delay1,unsigned char value,double delay2,double delay3);
   E1701_API int           E1701_lp8_a0(unsigned char n,unsigned char value); // DEPRECATED; please use E1701_lp8_a0_2() instead!
   E1701_API int           E1701_lp8_a0_2(unsigned char n,unsigned int flags,unsigned char value);
   E1701_API int           E1701_lp8_write_mo(unsigned char n,unsigned char on);

   // DIGI I/O extension board functions
   E1701_API int           E1701_digi_write(unsigned char n,unsigned int value);        // DEPRECATED; please use E1701_digi_write2() instead!
   E1701_API int           E1701_digi_write2(unsigned char n,unsigned int flags,unsigned int value,unsigned int mask);
   E1701_API int           E1701_digi_pulse(const unsigned char n,const unsigned int flags,const unsigned int in_value,const unsigned int mask,const unsigned int pulses,const double delayOn,const double delayOff);
   E1701_API unsigned int  E1701_digi_read(unsigned char n);                            // DEPRECATED; please use E1701_digi_read3() instead!
   E1701_API int           E1701_digi_read2(const unsigned char n,unsigned int *value); // DEPRECATED; please use E1701_digi_read3() instead!
   E1701_API int           E1701_digi_read3(const unsigned char n,const unsigned int flags,unsigned int *value);
   E1701_API int           E1701_digi_wait(unsigned char n,unsigned long value,unsigned long mask);
   E1701_API int           E1701_digi_set_motf(unsigned char n,double motfX,double motfY);
   E1701_API int           E1701_digi_set_motf_sim(unsigned char n,double motfX,double motfY);
   E1701_API int           E1701_digi_wait_motf(const unsigned char n,const unsigned int flags,const double dist);
   E1701_API int           E1701_digi_set_mip_output(unsigned char n,unsigned int value,unsigned int flags);
   E1701_API int           E1701_digi_set_wet_output(const unsigned char n,const unsigned int value,const unsigned int flags);

   // Analogue baseboard specific functions
   E1701_API int           E1701_ana_a123(const unsigned char n,const unsigned short r,const unsigned short g,const unsigned short b);

   // Miscellaneous internal-only functions
   E1701_API unsigned int  E1701_send_data(unsigned char n,unsigned int flags,const char *sendData,unsigned int length,unsigned int *sentLength);
   E1701_API unsigned int  E1701_recv_data(unsigned char n,unsigned int flags,char *recvData,unsigned int maxLength);

   // ***** end of E1701 easy interface functions ******************************************************************************
#ifdef __cplusplus
};
#endif // __cplusplus
#endif

#endif //E1701_H
