#ifndef E1701M_H
#define E1701M_H

#define E1701M_DLL_VERSION 1

#define E1701M_OK                    0 // operation could be finished successfully
#define E1701M_ERROR_INVALID_CARD  101 // wrong/illegal card number specified
#define E1701M_ERROR_NO_CONNECTION 102 // could not connect to card
#define E1701M_ERROR_NO_MEMORY     103 // not enough memory available
#define E1701M_ERROR_UNKNOWN_FW    104 // unknown/incompatible firmware version
#define E1701M_ERROR               105 // unknown/unspecified error
#define E1701M_ERROR_TRANSMISSION  106 // transmission of data failed
#define E1701M_ERROR_FILEOPEN      107 // opening a file failed
#define E1701M_ERROR_FILEWRITE     108 // writing data to a file failed
// unused                          109 // a base- or extension board that would be required for a function is not available
#define E1701M_ERROR_INVALID_DATA  110 // a function was called with invalid data or by using an operation mode where this function is not used/allowed
#define E1701M_ERROR_UNKNOWN_BOARD 111 // trying to access a board that is not a motion controller

#define E1701M_MAX_CARD_NUM         16 // maximum number of cards that can be controlled

#define E1701M_AXIS_0    0x01
#define E1701M_AXIS_1    0x02
#define E1701M_AXIS_2    0x04
#define E1701M_AXIS_3    0x08

#define E1701_AXES_MASK (E1701M_AXIS_0|E1701M_AXIS_1|E1701M_AXIS_2|E1701M_AXIS_3)

#define E1701M_AXIS_STATE_MOVING      0x0001
#define E1701M_AXIS_STATE_REFERENCING 0x0002
#define E1701M_AXIS_STATE_CONDSTOP    0x0004
#define E1701M_AXIS_STATE_SETPOS      0x0008

#define E1701M_REFSTEP_1_ENTER_N 0x0001
#define E1701M_REFSTEP_1_ENTER_P 0x0002

#define E1701M_REFSTEP_2_ENTER_N 0x0004
#define E1701M_REFSTEP_2_ENTER_P 0x0008
#define E1701M_REFSTEP_2_LEAVE_N 0x0010
#define E1701M_REFSTEP_2_LEAVE_P 0x0020

#define E1701M_REFSTEP_3_LEAVE_N 0x0040
#define E1701M_REFSTEP_3_LEAVE_P 0x0080
#define E1701M_REFSTEP_3_ENTER_N 0x0100
#define E1701M_REFSTEP_3_ENTER_P 0x0200

#define E1701M_REFSTEP_4_ENTER_N 0x0400
#define E1701M_REFSTEP_4_ENTER_P 0x0800
#define E1701M_REFSTEP_4_LEAVE_N 0x1000
#define E1701M_REFSTEP_4_LEAVE_P 0x2000

#define E1701M_REFSTEP_INV_SWITCH 0x8000 // invert the logic of reference switch

#define E1701M_ACCEL_MODE_LIN    1
#define E1701M_ACCEL_MODE_EXP    2
#define E1701M_ACCEL_MODE_SSHAPE 3

#define E1701M_DECEL_MODE_LIN    E1701M_ACCEL_MODE_LIN
#define E1701M_DECEL_MODE_EXP    E1701M_ACCEL_MODE_EXP
#define E1701M_DECEL_MODE_SSHAPE E1701M_ACCEL_MODE_SSHAPE

#ifndef ENV_E1701
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the E1701M_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// E1701M_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef ENV_LINUX
 #ifdef E1701M_EXPORTS
  #define E1701M_API __attribute ((visibility ("default")))
 #else
  #define E1701M_API
 #endif
#else
 #ifdef ENV_WINDOWS
  #ifdef E1701M_EXPORTS
   #define E1701M_API __declspec(dllexport)
  #else
   #define E1701M_API __declspec(dllimport)
  #endif
 #else
  #error None of the environment switches ENV_LINUX or ENV_WINDOWS is defined!
 #endif
#endif

#define E1701M_ENABLE_LOW  0
#define E1701M_ENABLE_HIGH 1
#define E1701M_ENABLE_OFF  2

#ifdef __cplusplus
extern "C"
{
#endif
   // ***** E1701 easy interface functions *************************************************************************************
   // base functions
   E1701M_API unsigned char E1701M_set_connection(const char *address);
   E1701M_API void          E1701M_set_password(const unsigned char n,const char *ethPwd);
   E1701M_API int           E1701M_set_logfile(unsigned char n,const char *path);
   E1701M_API int           E1701M_open_connection(unsigned char n);
   E1701M_API void          E1701M_close(unsigned char n);

   E1701M_API int           E1701M_set_accels(unsigned char n,unsigned char axes,double accel,double decel,double stopDecel);
   E1701M_API int           E1701M_set_accel_modes(unsigned char n,unsigned char axes,unsigned int accelMode,unsigned int decelMode,unsigned int res);
   E1701M_API int           E1701M_set_limits(unsigned char n,unsigned char axes,int llimit,int hlimit,double slimit);
   E1701M_API int           E1701M_set_speed(unsigned char n,unsigned char axes,double speed);
   E1701M_API int           E1701M_reference(unsigned char n,unsigned char axes,unsigned int mode,unsigned char refSwitch,double speedStep1,double speedStep2,double speedStep3,double speedStep4);
   E1701M_API int           E1701M_enable(const unsigned char n,const unsigned char dout3,const unsigned char dout7);
   E1701M_API int           E1701M_move_abs(unsigned char n,unsigned char axes,int pos);
   E1701M_API int           E1701M_move_rel(unsigned char n,unsigned char axes,int pos);
   E1701M_API int           E1701M_set_stopcond(unsigned char n,unsigned char axes,unsigned char stopOnEnter,unsigned char stopOnLeave);
   E1701M_API int           E1701M_stop(unsigned char n,unsigned char axes);
   E1701M_API int           E1701M_set_pos(unsigned char n,unsigned char axes,int pos);
   E1701M_API int           E1701M_set_trigger_point(unsigned char n,unsigned char axes,unsigned int input);
   E1701M_API int           E1701M_release_trigger_point(unsigned char n,unsigned char axes);
   E1701M_API int           E1701M_delay(unsigned char n,unsigned char axes,double delay);
   E1701M_API int           E1701M_set_enc(unsigned char n,char axis,unsigned char encoder,double resolution);

   E1701M_API unsigned int  E1701M_get_axis_state(unsigned char n,unsigned char axis);
   E1701M_API double        E1701M_get_axis_speed(unsigned char n,unsigned char axis);
   E1701M_API int           E1701M_get_axis_pos(unsigned char n,unsigned char axis); // deprecated, use E1701M_get_axis_pos2() instead!
   E1701M_API int           E1701M_get_axis_pos2(const unsigned char n,const unsigned char axis,long *position);
   E1701M_API unsigned int  E1701M_get_inputs(unsigned char n);
   E1701M_API int           E1701M_set_outputs(const unsigned char n,const unsigned int flags,const unsigned int value, const unsigned int mask);
   E1701M_API void          E1701M_get_version(unsigned char n,unsigned short *hwVersion,unsigned short *fwVersion);
   E1701M_API const int     E1701M_get_library_version();
#ifdef __cplusplus
}
#endif // __cplusplus
#endif

#endif //E1701M_H
