#ifndef E1701P_H
#define E1701P_H

#define E1701P_DLL_VERSION 1

#define E1701P_OK                    0 // operation could be finished successfully
#define E1701P_ERROR_INVALID_CARD  101 // wrong/illegal card number specified
#define E1701P_ERROR_NO_CONNECTION 102 // could not connect to card
#define E1701P_ERROR_NO_MEMORY     103 // not enough memory available
#define E1701P_ERROR_UNKNOWN_FW    104 // unknown/incompatible firmware version
#define E1701P_ERROR               105 // unknown/unspecified error
#define E1701P_ERROR_TRANSMISSION  106 // transmission of data failed
#define E1701P_ERROR_FILEOPEN      107 // opening a file failed
#define E1701P_ERROR_FILEWRITE     108 // writing data to a file failed
#define E1701P_ERROR_BORD_NA       109 // a base- or extension board that would be required for a function is not available
#define E1701P_ERROR_INVALID_DATA  110 // a function was called with invalid data or by using an operation mode where this function is not used/allowed
#define E1701P_ERROR_UNKNOWN_BOARD 111 // trying to access a board that is not a scanner controller
#define E1701P_ERROR_FILENAME      112 // there is an error with the given filename (too long, too many subdirectories, illegal characters,...)
#define E1701P_ERROR_NOT_SUPPORTED 113 // the requested feature is not supported by the current firmware version

#define E1701P_MAX_HEAD_NUM         16 // maximum number of cards that can be controlled

#define E1701P_LASER_FPK      0x80000000
#define E1701P_LASER_FREQ_ON1 0x40000000 // switch on-frequency/q-switch immediately
#define E1701P_LASER_FREQ_ON2 0x20000000 // switch on-frequency/q-switch after yag3QTime; in mode YAG 2 it is equal to FPK time
#define E1701P_LASER_FREQ_OFF 0x08000000

#define E1701P_LASERMODE_CO2     (1|E1701P_LASER_FREQ_ON1|E1701P_LASER_FREQ_OFF)
#define E1701P_LASERMODE_YAG1    (2|E1701P_LASER_FREQ_ON1|E1701P_LASER_FREQ_OFF|E1701P_LASER_FPK)
#define E1701P_LASERMODE_YAG2    (3|E1701P_LASER_FREQ_ON2|E1701P_LASER_FREQ_OFF|E1701P_LASER_FPK)
#define E1701P_LASERMODE_YAG3    (4|E1701P_LASER_FREQ_ON2|E1701P_LASER_FREQ_OFF|E1701P_LASER_FPK)
#define E1701P_LASERMODE_CRF     (5|E1701P_LASER_FREQ_OFF) // laser mode with continuously running frequency
#define E1701P_LASERMODE_unused1 (6)
#define E1701P_LASERMODE_unused2 (7)
#define E1701P_LASERMODE_IPG     (8|E1701P_LASER_FREQ_ON1)

#define E1701P_CSTATE_START_PRESSED     0x0000001
#define E1701P_CSTATE_STOP_PRESSED      0x0000002
#define E1701P_CSTATE_WAS_START_PRESSED 0x0000004
#define E1701P_CSTATE_WAS_STOP_PRESSED  0x0000008
#define E1701P_CSTATE_ERROR             0x0000010
#define E1701P_CSTATE_WAS_EXTTRIGGER    0x0000020 // internal use only, do not check this flag
#define E1701P_CSTATE_PROCESSING        0x0000040
#define E1701P_CSTATE_FILE_WRITE_ERROR  0x0000080
#define E1701P_CSTATE_WAIT_EXTTRIGGER   0x0000100
#define E1701P_CSTATE_WAS_SILENTTRIGGER 0x0000200 // internal use only, do not check this flag
#define E1701P_CSTATE_STOPPING          0x0000400
#define E1701P_AXIS_STATE_MOVING        0x0000800
#define E1701P_AXIS_STATE_REFERENCING   0x0001000
#define E1701P_AXIS_STATE_CONDSTOP      0x0002000
#define E1701P_AXIS_STATE_SETPOS        0x0004000 

#define E1701P_FILEMODE_OFF   0xFFFFFFFF
#define E1701P_FILEMODE_LOCAL 0
#define E1701P_FILEMODE_SEND  1

#define E1701P_BSTATE_XY2_100_BB        0x0001
#define E1701P_BSTATE_ILDA_BB           0x0002
#define E1701P_BSTATE_LP8_EB            0x0100
#define E1701P_BSTATE_DIGI_EB           0x0200

#define E1701P_BSTATE_BB_MASK          (E1701P_BSTATE_XY2_100_BB|E1701P_BSTATE_ILDA_BB)

#define E1701P_TUNE_EXTTRIG_DIGIIN7     0x0001
#define E1701P_TUNE_reserved            0x0002 // used by E1701A/D firmware part
#define E1701P_TUNE_SAVE_SERIAL_STATES  0x0004 // when this option is set the current state of serial numbers is stored during marking and does not get los on power cycle
#define E1701P_TUNE_INVERT_LASERGATE    0x0008
#define E1701P_TUNE_INVERT_LASERA       0x0010
#define E1701P_TUNE_INVERT_LASERB       0x0020
#define E1701P_TUNE_POWERRAMP_ENABLE    0x2000

#define E1701P_COMMAND_FLAG_STREAM      0x0001 // command has to be enqueued in stream
#define E1701P_COMMAND_FLAG_DIRECT      0x0002 // command has to be executed directly and immediately

#define E1701P_COMMAND_FLAG_STREAM      0x0001 // command has to be enqueued in stream
#define E1701P_COMMAND_FLAG_DIRECT      0x0002 // command has to be executed directly and immediately

#define E1701P_COMMAND_FLAG_WRITE_MASK       0x0F00
#define E1701P_COMMAND_FLAG_WRITE_LP8MO      0x0100
#define E1701P_COMMAND_FLAG_WRITE_LP8LATCH   0x0200
#define E1701P_COMMAND_FLAG_WRITE_LASERA_GPO 0x0300
#define E1701P_COMMAND_FLAG_WRITE_LASERB_GPO 0x0400
#define E1701P_COMMAND_FLAG_WRITE_LASERGATE  0x0800

#define E1701P_PIXELMODE_NO_JUMPS     0x0001 // do not perform jumps also in case power is 0%; when this flag is set, power threshold is ignored
#define E1701P_PIXELMODE_JUMP_N_SHOOT 0x0002 // no continuous movement, jump to the marking position and shoot there for laseroff minus laseron time

#define E1701P_AXIS_X 0x01 // axis-flag for X-axis
#define E1701P_AXIS_Y 0x02 // axis-flag for Y-axis
#define E1701P_AXIS_Z 0x04 // axis-flag for Z-axis

#ifndef ENV_E1701P
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the E1701P_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// E1701P_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef ENV_LINUX
 #ifdef E1701P_EXPORTS
  #define E1701P_API __attribute ((visibility ("default")))
 #else
  #define E1701P_API
 #endif
#else
 #ifdef E1701P_EXPORTS
  #define E1701P_API __declspec(dllexport)
 #else
  #define E1701P_API __declspec(dllimport)
 #endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif
   typedef int(*E1701P_power_callback)(unsigned char n, double power, void *userData); /** type definition for the callback function that has to be provided by client for setting power */

   // ***** E1701 easy interface functions *************************************************************************************
   // base functions
   E1701P_API unsigned char E1701P_set_connection(const char *address); /**/
   E1701P_API void          E1701P_set_password(const unsigned char n,const char *ethPwd);/**/
   E1701P_API int           E1701P_set_filepath(unsigned char n,const char *fname,unsigned int mode);
   E1701P_API int           E1701P_set_logfile(unsigned char n,const char *path);/**/
   E1701P_API void          E1701P_close(unsigned char n);/**/
   E1701P_API int           E1701P_open_connection(unsigned char n);/**/
   E1701P_API int           E1701P_set_xy_correction(unsigned char n, double gainX, double gainY,double rot,int offsetX,int offsetY);/**/
   E1701P_API int           E1701P_tune(const unsigned char n,const unsigned int tuneFlags);
   E1701P_API int           E1701P_set_speeds(unsigned char n, double jumpspeed,double markspeed);/**/
   E1701P_API int           E1701P_set_stop_angle(unsigned char n, double angle);/**/
   E1701P_API int           E1701P_set_laser_mode(unsigned char n, unsigned int mode);/**/
   E1701P_API int           E1701P_jump_abs(unsigned char n,int x,int y);/* DEPRECATED: use E1701P_jump_abs2() instead! */
   E1701P_API int           E1701P_mark_abs(unsigned char n,int x,int y);/* DEPRECATED: use E1701P_mark_abs2() instead! */
   E1701P_API int           E1701P_jump_abs2(const unsigned char n,const int x,const int y,const int z);/**/
   E1701P_API int           E1701P_mark_abs2(const unsigned char n,const int x,const int y,const int z);/**/
   E1701P_API int           E1701P_set_pixelmode(const unsigned char n,const unsigned int mode,const double powerThres,const unsigned int res);
   E1701P_API int           E1701P_mark_pixelline(const unsigned char n,int x,int y,const int pixWidth,const int pixHeight,unsigned int pixNum,const double *pixels,E1701P_power_callback power_callback,void *userData);/* DEPRECATED, use E1701P_mark_pixelline2() instead! */
   E1701P_API int           E1701P_mark_pixelline2(const unsigned char n,int x,int y,int z,const int pixWidth,const int pixHeight,const int pixDepth,unsigned int pixNum,const double *pixels,E1701P_power_callback power_callback,void *userData);
   E1701P_API int           E1701P_set_matrix(unsigned char n, double m11, double m12, double m21, double m22);/**/
   E1701P_API int           E1701P_execute(unsigned char n);/**/
   E1701P_API int           E1701P_stop_execution(unsigned char n);/**/
   E1701P_API int           E1701P_delay(unsigned char n,double delay);/**/
   E1701P_API int           E1701P_dynamic_data(const unsigned char n, const struct oapc_bin_struct_dyn_data *dynData);
   E1701P_API unsigned int  E1701P_get_card_state(unsigned char n);/**/
   E1701P_API unsigned int  E1701P_get_card_info(unsigned char n);
   E1701P_API int           E1701P_set_laser_timing(unsigned char n,double frequency,double pulse);/**/
   E1701P_API int           E1701P_set_standby(unsigned char n,double frequency,double pulse);/**/
   E1701P_API int           E1701P_set_fpk(unsigned char n,double fpk,double qag3QTime);/**/
   E1701P_API int           E1701P_set_sky_params(unsigned char n,double angle, unsigned int fadeIn,unsigned int fadeOut);
   E1701P_API void          E1701P_get_version(unsigned char n,unsigned short *hwVersion,unsigned short *fwVersion);/**/
   E1701P_API const int     E1701P_get_library_version();
   E1701P_API int           E1701P_write(const unsigned char n,unsigned int flags,unsigned int value);

   E1701P_API int           E1701P_lp8_write2(const unsigned char n,unsigned int flags,unsigned char value);
   E1701P_API int           E1701P_lp8_write_latch(const unsigned char n,unsigned char on,double delay1,unsigned char value,double delay2,double delay3);/**/
   E1701P_API int           E1701P_lp8_a0_2(const unsigned char n,unsigned int flags,unsigned char value);
   E1701P_API int           E1701P_lp8_write_mo(const unsigned char n,unsigned char on);/**/

   E1701P_API int           E1701P_digi_write2(const unsigned char n,unsigned int flags,unsigned int value,unsigned int mask);
   E1701P_API unsigned int  E1701P_digi_read(const unsigned char n);/**/
   E1701P_API int           E1701P_digi_set_mip_output(const unsigned char n,const unsigned int value,const unsigned int flags);

   E1701P_API unsigned int  E1701P_send_data(const unsigned char n,const unsigned int flags,const char *sendData,const unsigned int length,unsigned int *sentLength);

   E1701P_API int           E1701P_set_accels(const unsigned char n,unsigned char axes,double accel,double decel,double stopDecel);
   E1701P_API int           E1701P_set_accel_modes(const unsigned char n,unsigned char axes,unsigned int accelMode,unsigned int decelMode,unsigned int res);
   E1701P_API int           E1701P_set_limits(const unsigned char n,unsigned char axes,int llimit,int hlimit,double slimit);
   E1701P_API int           E1701P_set_stopcond(const unsigned char n,unsigned char axes,unsigned char stopOnEnter,unsigned char stopOnLeave);
   E1701P_API int           E1701P_set_enc(const unsigned char n,char axis,unsigned char encoder,double resolution);
   E1701P_API int           E1701P_reference(const unsigned char n,unsigned char axes,unsigned int mode,unsigned char refSwitch,
                                             double speedStep1,double speedStep2,double speedStep3,double speedStep4);
   E1701P_API int           E1701P_set_pos(unsigned char n,unsigned char axes,int pos);

   // ***** end of E1701 easy interface functions ******************************************************************************
#ifdef __cplusplus
};
#endif
#endif

#endif //E1701P_H
