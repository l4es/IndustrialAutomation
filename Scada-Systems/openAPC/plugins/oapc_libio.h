/* This file is distributed in the hope that they will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE. All information and definitions are
   subject to change without notice. Errors and omissions excepted. 
   
   This file is Freeware
   */

#ifndef OAPC_LIBIO_H
#define OAPC_LIBIO_H

#ifndef ENV_EMBEDDED

#include <math.h> // for pow() in OAPC_ROUND

#ifdef OAPC_EXT_HMI_EXPORTS
 #include <wx/dcbuffer.h>
#endif

#pragma pack(8)

#define MAX_NUM_IOS 8

#include "liboapc.h" // for definition of OAPC_EXT_API

#define OAPC_SIGN(val)               ((val>=0)? 1 : -1)
#define OAPC_ROUND(real_val,num_dig) (((double)(int)(real_val*pow((double)10,num_dig)+OAPC_SIGN(real_val)*0.5))/pow((double)10,num_dig))
#define OAPC_PI                      3.141592653589793238462643383279502
#define OAPC_DEG_TO_RAD(deg)         (2.0*OAPC_PI*((deg)/360.0))
#define OAPC_RAD_TO_DEG(rad)         ((((rad)/2.0)/OAPC_PI)*360.0)

/** Following defines need to be set by the build environment/the Makefile depending on the
 * used operating system:
 * ENV_LINUX - for Linux operating systems
 * ENV_MACOS - for MacOS operating systems
 * ENV_QNX - for QNX operating systems
 * ENV_WINDOWS - for standard Windows operating systems including XPe
 * ENV_WINDOWS and ENV_WINDOWSCE - for Windows CE operating systems
 */

struct oapc_bin_head; // forward declaration

/** Capability flags that define what a plug-in is able to do/to handle/where it belongs
 *  to */

#define OAPC_HAS_INPUTS                      0x00000001 // the plug-in handles inputs
#define OAPC_HAS_OUTPUTS                     0x00000002 // the plug-in provides outputs
#define OAPC_HAS_XML_CONFIGURATION           0x00000004 // the plug-in has configuration possibilities and provides them using an XML structure, this flag can be used for both, the HMI and the Flow configuration dialogue
#define OAPC_HAS_STANDARD_FLOW_CONFIGURATION 0x00000008 // when this flag is set, no XML data for the Flow configuration are fetched, instead of it a standard configuration dialogue is opened by the main application
                                                        // automatically that depends on the used flags; PLEASE NOTE: this capability can be used only together with some very specific combinations of inputs and outputs!
#define OAPC_HAS_LOG_TYPE_DIGI               0x00000010 // the handled digital data can be configured for data logging
#define OAPC_HAS_LOG_TYPE_INTNUM             0x00000020 // the handled numerical data can be configured for floating number data logging
#define OAPC_HAS_LOG_TYPE_FLOATNUM           0x00000040 // the handled numerical data can be configured for integer number logging
#define OAPC_HAS_LOG_TYPE_CHAR               0x00000080 // the handled text data can be configured for data logging

#define OAPC_ACCEPTS_PLAIN_CONFIGURATION     0x00000100 // the library accepts data that are returned after configuration as plain strings in pairs of name/value
#define OAPC_ACCEPTS_IO_CALLBACK             0x00000200 // the Plug-In is able to handle the oapc_io_callback()-function to inform the main application about IO changes; in this case the outputs are NOT polled periodically,
                                                        // it could be necessary for the plug-in to use an own thread to watch for output data
#define OAPC_ACCEPTS_WRITE_DATA_MODE         0x00000400 // the plug-in can be created in write-data-mode where it writes data to disk
#define OAPC_ACCEPTS_SEND_DATA_MODE          0x00000800 // the plug-in can be created in send-data-mode where it sends data to the device (without operating it)
#define OAPC_ACCEPTS_MOUSECLICKS             0x00001000 // used in HMI plug-in only: the plug-in needs to receive mouse down and mouse up events from the main application
#define OAPC_ACCEPTS_MOUSEMOVES              0x00002000 // used in HMI plug-in only: the plug-in needs to receive mouse movement events
#define OAPC_ACCEPTS_MOUSEDRAGS              0x00004000 // used in HMI plug-in only: the plug-in needs to receive mouse movement events with left button held down
#define OAPC_ACCEPTS_KEYPRESS                0x00008000 // reserved, currently not supported
#define OAPC_ACCEPTS_SEND_NAMED_DATA_MODE    0x00010000 // the plug-in can be created in named send-data-mode where it sends data to the device using a given name (without operating it)

#define OAPC_USERPRIVI_DISABLE               0x00010000 // the HMI element supports user privileges and the option to disable the element
#define OAPC_USERPRIVI_HIDE                  0x00020000 // the HMI element supports user privileges and the option to hide the element
//#define OAPC_USERPRIVI_IGNORE - this flag does not exist, it exists implicitly and is done by the application by hiding the base canvas
//#define OAPC_USERPRIVI_ENABLE - this flag does not exist, this option is available in every case
#define OAPC_USERPRIVI_MASK                  (OAPC_USERPRIVI_DISABLE|OAPC_USERPRIVI_HIDE)

#define OAPC_iTYPE_1                         0x00100000 // type identifier 1, internally used only
#define OAPC_iTYPE_2                         0x00200000 // type identifier 1, internally used only
#define OAPC_iTYPE_3                         0x00300000 // type identifier 1, internally used only
#define OAPC_iTYPE_MASK                     (OAPC_iTYPE_1|OAPC_iTYPE_2|OAPC_iTYPE_3)
                                                        // mask for the type identifier information (they can be used exclusively only, therefore they are not flags)

#define OAPC_FLOWCAT_CONVERSION              0x01000000 // the plug-in belongs to Flow Editor, category conversion
#define OAPC_FLOWCAT_LOGIC                   0x02000000 // the plug-in belongs to Flow Editor, category logic operations
#define OAPC_FLOWCAT_CALC                    0x03000000 // the plug-in belongs to Flow Editor, category calculations
#define OAPC_FLOWCAT_FLOW                    0x04000000 // the plug-in belongs to Flow Editor, category program flow
#define OAPC_FLOWCAT_IO                      0x05000000 // the plug-in belongs to Flow Editor, category input/output (with access to hardware devices)
#define OAPC_FLOWCAT_MOTION                  0x06000000 // the plug-in belongs to Flow Editor, category motion (with access to motion devices)
#define OAPC_FLOWCAT_DATA                    0x07000000 // the plug-in belongs to Flow Editor, category data handling (but no access to external devices)
#define OAPC_FLOWCAT_LASER                   0x08000000 // the plug-in belongs to Flow Editor, category laser and handles all laser/laser marking related plug-ins
#define OAPC_FLOWCAT_MASK                   (OAPC_FLOWCAT_CONVERSION|OAPC_FLOWCAT_LOGIC|OAPC_FLOWCAT_CALC|OAPC_FLOWCAT_FLOW|OAPC_FLOWCAT_IO|OAPC_FLOWCAT_MOTION|OAPC_FLOWCAT_DATA|OAPC_FLOWCAT_LASER)
                                                        // mask for the Flow Editor category information (they can be used exclusively only, therefore they are not flags)

#define OAPC_HMICAT_CONTROL                  0x10000000 // the plug-in belongs to HMI Editor, category user-accessible control elements
#define OAPC_HMICAT_DISPLAY                  0x20000000 // the plug-in belongs to HMI Editor, category displaying varying information to the user
#define OAPC_HMICAT_STATIC                   0x30000000 // the plug-in belongs to HMI Editor, category static design elements
#define OAPC_HMICAT_MASK                    (OAPC_HMICAT_CONTROL|OAPC_HMICAT_DISPLAY|OAPC_HMICAT_STATIC)
                                                        // mask for the HMI Editor category information (they can be used exclusively only, therefore they are not flags)

#define OAPC_IS_DEPRECATED                   0x80000000 // this plug-in is marked as deprecated and therefore is not longer shown in Editor so that it is not used for new projects


/** flags that disable capabilities within the user interface definition dialoge; they
 *  apply to HMI plug-ins only and have to be returned by oapc_get_no_ui_flags() */

#define OAPC_HMI_NO_UI_DISABLED     0x0002 // no "disabled" state
#define OAPC_HMI_NO_UI_RO           0x0004 // no "read only" state; this has to be set for all HMI elements that are not of type control
#define OAPC_HMI_NO_UI_MINMAX       0x0008 // no minimum and maximum values
#define OAPC_HMI_NO_UI_TEXT         0x0010 // no text
#define OAPC_HMI_NO_UI_FG           0x0020 // no foreground colour
#define OAPC_HMI_NO_UI_FONT         0x0040 // no font definition
#define OAPC_HMI_NO_SIZE            0x0080 // no size
#define OAPC_HMI_NO_POS             0x0100 // no position
#define OAPC_HMI_NO_FREE_SIZE_RATIO 0x0200 // keep the ratio between width and height constant
#define OAPC_HMI_NO_UI_BG           0x0400 // no background colour
#define OAPC_HMI_NO_UI_LAYOUT       0x0800 // element can't be layouted; this does not apply to external HMI elements normally!


/** IO flags that define which inputs/outputs of which type are available */
/** Digital IOs */
#define OAPC_DIGI_IO_MASK  0x000000FF
#define OAPC_DIGI_IO0      0x00000001
#define OAPC_DIGI_IO1      0x00000002
#define OAPC_DIGI_IO2      0x00000004
#define OAPC_DIGI_IO3      0x00000008
#define OAPC_DIGI_IO4      0x00000010
#define OAPC_DIGI_IO5      0x00000020
#define OAPC_DIGI_IO6      0x00000040
#define OAPC_DIGI_IO7      0x00000080

/** Numerical IOs */
#define OAPC_NUM_IO_MASK   0x0000FF00
#define OAPC_NUM_IO0       0x00000100
#define OAPC_NUM_IO1       0x00000200
#define OAPC_NUM_IO2       0x00000400
#define OAPC_NUM_IO3       0x00000800
#define OAPC_NUM_IO4       0x00001000
#define OAPC_NUM_IO5       0x00002000
#define OAPC_NUM_IO6       0x00004000
#define OAPC_NUM_IO7       0x00008000

/** Character IOs */
#define OAPC_CHAR_IO_MASK  0x00FF0000
#define OAPC_CHAR_IO0      0x00010000
#define OAPC_CHAR_IO1      0x00020000
#define OAPC_CHAR_IO2      0x00040000
#define OAPC_CHAR_IO3      0x00080000
#define OAPC_CHAR_IO4      0x00100000
#define OAPC_CHAR_IO5      0x00200000
#define OAPC_CHAR_IO6      0x00400000
#define OAPC_CHAR_IO7      0x00800000

/** Binary IOs */
#define OAPC_BIN_IO_MASK   0xFF000000
#define OAPC_BIN_IO0       0x01000000
#define OAPC_BIN_IO1       0x02000000
#define OAPC_BIN_IO2       0x04000000
#define OAPC_BIN_IO3       0x08000000
#define OAPC_BIN_IO4       0x10000000
#define OAPC_BIN_IO5       0x20000000
#define OAPC_BIN_IO6       0x40000000
#define OAPC_BIN_IO7       0x80000000

#define OAPC_OK                       0x00000001 /** everything is OK, an operation was performed successfully */
#define OAPC_ERROR_CONNECTION         0x00000002 /** a required connection could not be established */
#define OAPC_ERROR_DEVICE             0x00000003 /** a required device could not be opened/accessed */
#define OAPC_ERROR_NOT_SUPPORTEDi     0x00000004 /** NO LONGER REQUIRED: functions that are not required due to the capability definitions do not need to be provided by a plug-in */
#define OAPC_ERROR_NO_DATA_AVAILABLE  0x00000005 /** this value is returned when data are requested for an output where no new data are available at this moment */
#define OAPC_ERROR_NO_SUCH_IO         0x00000006 /** the main application tried to access an input or output that doesn't exists, that means the given input/output flags are wrong or it is a misbehaviour of the application  */
#define OAPC_ERROR_RESOURCE           0x00000007 /** a required resource could not be created/accessed */
#define OAPC_ERROR_AUTHENTICATION     0x00000008 /** authentication error, required authentication/username/password did not match */
#define OAPC_ERROR                    0x00000009 /** other error, has to be used as followup-error code of a more detailled one only/has to be used for cases that normally never happen */
#define OAPC_ERROR_CONVERSION_ERROR   0x0000000A /** there was a conversion error, input data have not been able to be converted */
#define OAPC_ERROR_NO_MEMORY          0x0000000B /** required memory could not be allocated */
#define OAPC_ERROR_STILL_IN_PROGRESS  0x0000000C /** an operation is still in progress so that the new data sent to an input can't be handled at the moment */
#define OAPC_ERROR_RECV_DATA          0x0000000D /** the reception of data failed */
#define OAPC_ERROR_SEND_DATA          0x0000000E /** the transmission of data failed */
#define OAPC_ERROR_PROTOCOL           0x0000000F /** an error with/in a used communication occurred */
#define OAPC_ERROR_INVALID_INPUT      0x00000010 /** the data given to the Plug-In are invalid/out of allowed range */
#define OAPC_ERROR_CUSTOM             0x00000011 /** a custom error occured, the related error message can be fetched via function oapc_get_error_message() from the plug-in */
#define OAPC_ERROR_CREATE_FILE_FAILED 0x00000012 /** creation of a new file/appending data to an existing file failed, file could not be opened */
#define OAPC_ERROR_OPEN_FILE_FAILED   0x00000013 /** opening an existing file for reading failed */
#define OAPC_ERROR_WRITE_FILE_FAILED  0x00000014 /** writing data into an already opened file failed */
#define OAPC_ERROR_READ_FILE_FAILED   0x00000015 /** reading data from an already opened file failed */
#define OAPC_ERROR_LICENSE            0x00000016 /** calling of a function failed due to missing/insufficient license */
#define OAPC_ERROR_LIBRARY_MISSING    0x00000017 /** a shared library that is required for this device could not be found */
#define OAPC_ERROR_INTERNAL           0x00000018 /** an internal error occurred which should never happen and can point e.g. to a software problem */
#define OAPC_ERROR_UNKNOWN_FILEFORMAT 0x00000019 /** the format of the file is unknown */
#define OAPC_ERROR_STOPPED            0x0000001A /** the operation was interrupted by an (external) stop-event */
#define OAPC_ERROR_OUT_OF_RANGE       0x0000001B /** a (measured) value is out of its allowed range */

#define OAPC_ERROR_MASK              0x000000FF /** mask for filtering the error code part */

#define OAPC_INT_FLOW_FORKEDi        0x00000100 /** internal use only, does not apply for external plug-ins */
#define OAPC_INT_BIN_NOT_UNLOCKEDi   0x00000200 /** internal use only, does not apply for external plug-ins */
#define OAPC_OK_ENTITY_FINISHEDi     0x00000300

/** mapped internal error codes - to be removed! ***************************************************/
#define CNCO_OK_ENTITY_FINISHED            OAPC_OK_ENTITY_FINISHEDi // the entity was finished, construction/drawing is complete now

#define CNCO_ERROR_IN_GEOMETRY                0x1003 // the geometry does not allow this operation
#define CNCO_ERROR_INIT_RESSOURCE             OAPC_ERROR_RESOURCE // initializing of an external resource failed
#define CNCO_CANCELLED                        0x100A // not really an error, the operation was cancelled
#define CNCO_ERROR_UNSUPP_FILEFORMAT_VARIANT  0x100B // the variant of this file format is unsupported
#define CNCO_ERROR_UNSUPPORTED_DATA_FORMAT    0x100C // the input data type is not supported
#define CNCO_ERROR_UNKNOWN_INPUT_DATA         0x100D // input data are unknown/can't be handled at this position
#define CNCO_ERROR_WRONG_HEAD                 0x100F // not really an error but function call is valid only for a different head
//#define CNCO_ERROR_UNKNOWN_FILEFORMAT         0x1010 // for LTS build only
/** mapped internal error codes - to be removed! **************************************************/

/** instance flags, to be used with oapc_create_instance2() */

#define OAPC_INSTANCE_OPERATION      0x00000000 /** default value */
#define OAPC_INSTANCE_SIMULATION     0x00000001 /** plug-in is called in simulation mode, do not try to initialize and access the hardware but simulate operation instead */
#if !defined ENV_PLAYER | defined ENV_BEAM
 #define OAPC_INSTANCE_MINIMUM_INIT  0x00000002 /** initialize as less as possible in order to read some default data fro ma device which would have been overwritten elsewhere; this flag corresponds to oapc_read_pvalue() */
#endif
#define OAPC_INSTANCE_WRITE_DATA      0x00000004 /** plug-in is called to not to access its original device but to write some data to disk that can be used later (e.g. in stand alone mode directly from within the device */
#define OAPC_INSTANCE_SEND_DATA       0x00000008 /** plug-in is called to not to access its original device but to write some data to the device that can be used later (e.g. for stand alone mode directly from within the device */
#define OAPC_INSTANCE_SEND_NAMED_DATA 0x00000010 /** plug-in is called to not to access its original device but to write some named data to the device that can be used later, in this mode a (file)name is required for these data */

/** capability flags to be used within laser controller plugins and with struct config_laser_controller.capabilites */
#define OAPC_LC_HAS_LASERON  0x00000001
#define OAPC_LC_HAS_LASEROFF 0x00000002
#define OAPC_LC_HAS_FREQ     0x00000004
#define OAPC_LC_HAS_POWER    0x00000008

/** predefined identifiers used as name parameter for oapc_set_config_data() */

#define OAPC_ID_CYCLETIME            "oapc_cycletime"
#define OAPC_ID_WRITE_DATA_PATH      "oapc_write_data_path"
#define OAPC_ID_CORRECTION_FILE_PATH "oapc_correction_file_path"
#define OAPC_ID_BRIGHTNESS           "oapc_brightness"
#define OAPC_ID_CONTRAST             "oapc_contrast"
#define OAPC_ID_GAMMA                "oapc_gamma"
#define OAPC_ID_RED                  "oapc_red"
#define OAPC_ID_GREEN                "oapc_green"
#define OAPC_ID_BLUE                 "oapc_blue"

typedef void (*lib_oapc_io_callback)(const unsigned long outputs,const unsigned long callbackID); /** type definition for the callback function that is provided by the main application in order to get informed about changes at the output */


// (file) error constants that have to be used in case of file access errors; they are normally returned by
// using a separate numeric output

#define FILEERROR_NOT_OPENED   1
#define FILEERROR_NOT_READ     3
#define FILEERROR_EOF          4
#define FILEERROR_NO_FILENAME  5
#define FILEERROR_WRONG_FORMAT 6
#define FILEERROR_OTHER        7
#define FILEERROR_NO_MEMORY    8
#define FILEERROR_DAMAGED      9


// structures and definitions for function oapc_get_config_info_data ****************************************************
#define OAPC_CONFIG_TYPE_MOTIONCONTROLLER  0x0002
#define OAPC_CONFIG_TYPE_IMAGECAPTURE      0x0004
#define OAPC_CONFIG_TYPE_LASERCONTROLLER   0x0008
#define OAPC_CONFIG_TYPE_ZSHIFTER          0x0010
#define OAPC_CONFIG_TYPE_PCONTROL          0x0020

#define INT_MAX_DISABLED_IO_PORTS 12
#define INT_TOTAL_MAX_HEADNUM     25
#define INT_FLAGS_ROUTE_BIN_OUT7_TO_SCANNER_BIN_IN7     0x0001 // send data emitted at BIN_OUT7 to BIN_IN7 of the scanner controller
#define INT_FLAGS_ROUTE_BIN_IN7_FROM_SCANNER_BIN_OUT7   0x0002 // get data emitted at BIN_OUT7 of the scanner controller and input them at BIN_IN7
#define INT_FLAGS_ROUTE_DIGI_IN6_FROM_SCANNER_DIGI_OUT6 0x0004 // get data emitted at DIGI_OUT6 of the scanner controller and input them at DIGI_IN6

struct config_scanner_controller
{
   unsigned short version,length;
   unsigned char  numOfAxes;
   unsigned char  cNum;
   unsigned short res3;
   unsigned int   availableIoPorts;
   unsigned int   miscCapabilities;
   unsigned int   disabledIoPort[INT_MAX_DISABLED_IO_PORTS];
   unsigned int   disabledIoBits[INT_MAX_DISABLED_IO_PORTS];
   unsigned int   laserType;
   int            mFieldX_depr,mFieldY_depr,mFieldZ_depr,mFieldSize_depr; // deprecated, use INT_CAPABILITY_HAS_UM_FIELDSIZE-values below
   unsigned int   flags,scannerResolution;
   unsigned int   mFieldSizeHeight_depr;                                  // deprecated, use INT_CAPABILITY_HAS_UM_FIELDSIZE-values below
   int            uFieldX,uFieldY,uFieldZ;                                // provide field position in um, requires the flag INT_CAPABILITY_HAS_UM_FIELDSIZE to be set
   int            mFieldSizeDepth_depr;                                   // deprecated, use INT_CAPABILITY_HAS_UM_FIELDSIZE-values below
   int            res2;
   unsigned int   availableInputPorts;
   unsigned int   disabledInputPort[INT_MAX_DISABLED_IO_PORTS];
   unsigned int   disabledInputBits[INT_MAX_DISABLED_IO_PORTS];
   int            uFieldWidth,uFieldHeight,uFieldDepth;                   // provide field size in um, requires the flag INT_CAPABILITY_HAS_UM_FIELDSIZE to be set
};

#define OAPC_CONFIG_MOTION_CONTROLLER_VERSION 1

struct config_motion_controller
{
   unsigned short version,length;
   unsigned char  availableAxes;  // bitfield of available and configured axes
   unsigned char  useBinOutput;   // currently 2 or 3 similar to the additional motion binary output of the BEAMP/CNCO-plug-in
   unsigned char  rotationalAxes; // bitfield of axes that operate in rotational mode (and not planar)
   unsigned char  pad2;
   unsigned int   pad4;
   int            uMinPos[8],uMaxPos[8]; // minimum and maximum position in micrometers
   int            uMaxSpeed[8];          // maximum speed in micrometers per second
   unsigned int   flags;
};

#define OAPC_CONFIG_IMAGE_CAPTURE_VERSION 1

struct config_image_capture
{
    unsigned short version,length;
    unsigned short frameDelay; // delay in msec between two frames
    unsigned short reserved1;
    unsigned int   frameWidth,frameHeight,reserved4,reserved5;
};

#define OAPC_CONFIG_LASER_CONTROLLER_VERSION 1

struct config_laser_controller
{
    unsigned short version,length;
    unsigned short reserved0,reserved1;
    unsigned int   capabilities,flags,reserved4,reserved5;
};

#define OAPC_CONFIG_ZSHIFTER_VERSION 1

struct config_zshifter
{
    unsigned short version,length;
    unsigned short reserved0,reserved1;
    unsigned int   reserved2,reserved3,reserved4,reserved5;
};

#define OAPC_CONFIG_PCONTROL_VERSION 1

#define OAPC_CONFIG_PCONTROL_FLAG_INT_TYPE          0x00000001
#define OAPC_CONFIG_PCONTROL_FLAG_FLOAT_TYPE        0x00000002
#define OAPC_CONFIG_PCONTROL_FLAG_TYPE_MASK         0x000000FF

#define OAPC_CONFIG_PCONTROL_FLAG_TEMPERATURE_STYLE 0x00000100
#define OAPC_CONFIG_PCONTROL_FLAG_PRESSURE_STYLE    0x00000200
#define OAPC_CONFIG_PCONTROL_FLAG_BRIGHTNESS_STYLE  0x00000400
#define OAPC_CONFIG_PCONTROL_FLAG_STYLE_MASK        0x0000FF00

#define OAPC_PCONTROL_MAX_CUST_PARAMS 12

struct config_pcontrol
{
   unsigned short version,length;
   unsigned int   flags;
   // *** parameters to be used within pen settings ********************
   char           penPanelName[15];
   unsigned int   paramFlag[OAPC_PCONTROL_MAX_CUST_PARAMS];
   int            paramMin[OAPC_PCONTROL_MAX_CUST_PARAMS],paramMax[OAPC_PCONTROL_MAX_CUST_PARAMS],paramDef[OAPC_PCONTROL_MAX_CUST_PARAMS];
   unsigned int   paramFloatFactor;
   char           paramName[OAPC_PCONTROL_MAX_CUST_PARAMS][40];
   char           paramUnit[OAPC_PCONTROL_MAX_CUST_PARAMS][20];
   // *** measured/evaluated values to be shown during processing ******
   unsigned int   dispFlag[OAPC_PCONTROL_MAX_CUST_PARAMS];
   char           dispName[OAPC_PCONTROL_MAX_CUST_PARAMS][40];

   int            unused1[OAPC_PCONTROL_MAX_CUST_PARAMS],unused2[OAPC_PCONTROL_MAX_CUST_PARAMS];

   char           dispUnit[OAPC_PCONTROL_MAX_CUST_PARAMS][20];
   // ******************************************************************
   unsigned short reserved0,reserved1;
   unsigned int   reserved2,reserved3,reserved4,reserved5;
};

#define OAPC_CONFIG_INFO_VERSION 3

struct config_info
{
   unsigned short version,length;
   unsigned int   configType;
   struct config_scanner_controller scannerController;
   struct config_motion_controller  motionController;
   struct config_image_capture      imageCapture;
   struct config_laser_controller   laserController;
   struct config_zshifter           zShifter;
   struct config_pcontrol           pControl;
};

#ifdef __cplusplus
extern "C"
{
#endif
   OAPC_EXT_API unsigned long oapc_get_config_info_data(void *instanceData,struct config_info *fillStruct);
   OAPC_EXT_API unsigned long oapc_get_beam_id();
#ifdef __cplusplus
}
#endif
// end of structures and definitions for function oapc_get_config_info_data ****************************************************

#ifndef ENV_EDITOR   // internal definition
#ifndef ENV_PLAYER   // internal definition
#ifndef ENV_DEBUGGER // internal definition

#ifdef __cplusplus
extern "C" 
{
#endif
   OAPC_EXT_API unsigned long  oapc_get_capabilities(void);
   OAPC_EXT_API char          *oapc_get_name(void);
   OAPC_EXT_API unsigned long  oapc_get_input_flags(void);
   OAPC_EXT_API unsigned long  oapc_get_output_flags(void);

   OAPC_EXT_API void*          oapc_create_instance2(unsigned long flags); // to be used with OAPC_INSTANCE_-flags, please refer above
   OAPC_EXT_API void*          oapc_create_instance(void); // deprecated, do not use it with version 1.2 and newer
   OAPC_EXT_API void           oapc_delete_instance(void* instanceData);
   OAPC_EXT_API unsigned long  oapc_init(void* instanceData);
   OAPC_EXT_API unsigned long  oapc_exit(void* instanceData);
   OAPC_EXT_API unsigned long  oapc_read_pvalue(void* instanceData,double param,double *value); // read parameter value
   OAPC_EXT_API unsigned long  oapc_get_error_message(void *instanceData,unsigned long length,char* value);
   OAPC_EXT_API char          *oapc_get_config_data(void* instanceData);
   OAPC_EXT_API void           oapc_set_config_data(void* instanceData,const char *name,const char *value);
   OAPC_EXT_API unsigned long  oapc_get_config_value(const void* instanceData,const char *name,char *value,const int valLength);
   OAPC_EXT_API char          *oapc_get_save_data(void* instanceData,unsigned long *length);
   OAPC_EXT_API void           oapc_set_loaded_data(void* instanceData,unsigned long length,char *loadedData);
   OAPC_EXT_API unsigned long  oapc_set_digi_value(void* instanceData,unsigned long input,unsigned char value);
   OAPC_EXT_API unsigned long  oapc_get_digi_value(void* instanceData,unsigned long output,unsigned char *value);
   OAPC_EXT_API unsigned long  oapc_set_num_value(void* instanceData,unsigned long input,double value);
   OAPC_EXT_API unsigned long  oapc_get_num_value(void* instanceData,unsigned long output,double *value);
   OAPC_EXT_API unsigned long  oapc_set_char_value(void* instanceData,unsigned long input,char *value);
   OAPC_EXT_API unsigned long  oapc_get_char_value(void* instanceData,unsigned long output,unsigned long length,char *value);
   OAPC_EXT_API unsigned long  oapc_set_bin_value(void* instanceData,unsigned long input,struct oapc_bin_head *value);
   OAPC_EXT_API unsigned long  oapc_get_bin_value(void* instanceData,unsigned long outputNum,struct oapc_bin_head **value);
   OAPC_EXT_API void           oapc_release_bin_data(void* instanceData,unsigned long outputNum);
   OAPC_EXT_API void           oapc_set_io_callback(void* instanceData,lib_oapc_io_callback oapc_io_callback,unsigned long callbackID);

#ifdef OAPC_EXT_HMI_EXPORTS
   OAPC_EXT_API unsigned long  oapc_get_no_ui_flags(void);
   OAPC_EXT_API void           oapc_get_colours(wxUint32 *background,wxUint32 *foreground);

   OAPC_EXT_API void           oapc_paint(void* instanceData,wxAutoBufferedPaintDC *dc,wxPanel *canvas);
   OAPC_EXT_API void           oapc_mouseevent(void* instanceData,wxMouseEvent* event);
   OAPC_EXT_API void           oapc_get_defsize(wxFloat32 *x,wxFloat32 *y);
   OAPC_EXT_API void           oapc_get_minsize(void* instanceData,wxFloat32 *x,wxFloat32 *y);
   OAPC_EXT_API void           oapc_get_maxsize(void* instanceData,wxFloat32 *x,wxFloat32 *y);
   OAPC_EXT_API void           oapc_get_numminmax(void* instanceData,wxFloat32 *minValue,wxFloat32 *maxValue);
   OAPC_EXT_API void           oapc_set_numminmax(void* instanceData,wxFloat32 minValue,wxFloat32 maxValue);
   OAPC_EXT_API char          *oapc_get_hmi_config_data(void* instanceData);
#endif
#ifdef __cplusplus
}
#endif

#endif
#endif
#endif

// Following some definitions are made that belong to the binary datatype

#define OAPC_BIN_HEAD_VERSION_1    1
#define OAPC_BIN_HEAD_CURR_VERSION OAPC_BIN_HEAD_VERSION_1 // current version number of struct oapc_bin_head

struct oapc_bin_head
{
   int           version;           // the structure version number, this field will always stay at this position
   int           sizeHead_donotuse; // the length of the structure, use sizeof(struct oapc_bin_Head) instead!
   unsigned char type,subType;      // type and subtype definitions for the appended data, please refer below
   unsigned char compression;       // the data may be comressed additionally, the type of compression method is specified by this element using a OAPC_COMPRESS_xxx value
   unsigned char unit;              // measurement unit, mainly applies to type OAPC_BIN_TYPE_DATA
   short         unitExponent;      // exponent of the unit (as examples: 3 means "kilo", 6 "mega", -3 "milli", -12 "nano")
   short         int1;              // for internal use only, do not change!            
   int           param1,param2,     // data type dependent parameters, they may be used for additional parameters describing the binary data
                 param3;     
   int           sizeData;          // the length of the payload data that are appended directly after this structure
   char          data;              // start here with adding data
   // following the binary data have to be appended
};

// compression method definitions
#define OAPC_COMPRESS_NONE      0 // data are not packed additionally
#define OAPC_COMPRESS_GZ        1 // GZip compression
#define OAPC_COMPRESS_BZ2       2 // BZip2 compression
#define OAPC_COMPRESS_LZMA      3 // LZMA compression
#define OAPC_COMPRESS_ZIP       4 // Zip compression
#define OAPC_COMPRESS_RLE       5 // run length encoding
#define OAPC_COMPRESS_CUSTOM  255 // to be used temporarily as long as no appropriate type exists
                                  // for a specific compression method, NOT TO BE USED IN RELEASED VERSIONS!

#endif //ENV_EMBEDDED

// type definitions, the related subtypes can be found below
#define OAPC_BIN_TYPE_UNKNOWN   0 // the type of the binary data is currently unknown
#define OAPC_BIN_TYPE_TEXT      1 // text data
#define OAPC_BIN_TYPE_IMAGE     2 // images
#define OAPC_BIN_TYPE_VIDEO     3 // video data
#define OAPC_BIN_TYPE_AUDIO     4 // audio data
#define OAPC_BIN_TYPE_MODEL     5 // 2D and 3D data
#define OAPC_BIN_TYPE_DATA      6 // other data, e.g. streams of measurement values
#define OAPC_BIN_TYPE_STRUCT    7 // structured data, used for internal functionality
#define OAPC_BIN_TYPE_CUSTOM  255 // to be used temporarily as long as no appropriate type exists
                                  // for a specific application, can be combined with
                                  // OAPC_BIN_SUBTYPE_CUSTOM only, NOT TO BE USED IN RELEASED VERSIONS!

// text subtypes
#define OAPC_BIN_SUBTYPE_TEXT_CSV      1 // [RFC4180]
#define OAPC_BIN_SUBTYPE_TEXT_HTML     2 // [RFC2854]
#define OAPC_BIN_SUBTYPE_TEXT_PLAIN    3 // [RFC2046][RFC3676][RFC5147]
#define OAPC_BIN_SUBTYPE_TEXT_UTF8     4
#define OAPC_BIN_SUBTYPE_TEXT_UTF16    5
#define OAPC_BIN_SUBTYPE_TEXT_UTF32    6
#define OAPC_BIN_SUBTYPE_TEXT_RICHTEXT 7 // [RFC2045][RFC2046]
#define OAPC_BIN_SUBTYPE_TEXT_XML      8 // [RFC3023]
#define OAPC_BIN_SUBTYPE_TEXT_CUSTOM 255 // to be used temporarily as long as no appropriate subtype exists for a specific application, NOT TO BE USED IN RELEASED VERSIONS!
#define OAPC_BIN_UNIT_TEXT_NONE        0 // no measurement unit used for the type/subtype

// image subtypes
#define OAPC_BIN_SUBTYPE_IMAGE_RGB24    1 // raw, uncompressed image data in RGB-triplets
#define OAPC_BIN_SUBTYPE_IMAGE_GIF      2 // [RFC2045,RFC2046]
#define OAPC_BIN_SUBTYPE_IMAGE_JPEG     3 // [RFC2045,RFC2046]
#define OAPC_BIN_SUBTYPE_IMAGE_PNG      4
#define OAPC_BIN_SUBTYPE_IMAGE_TIFF     5 // [RFC3302]
#define OAPC_BIN_SUBTYPE_IMAGE_PCX      6
#define OAPC_BIN_SUBTYPE_IMAGE_PNM      7
#define OAPC_BIN_SUBTYPE_IMAGE_BMP      8
#define OAPC_BIN_SUBTYPE_IMAGE_IFF      9
#define OAPC_BIN_SUBTYPE_IMAGE_XPM     10
#define OAPC_BIN_SUBTYPE_IMAGE_ICO     11
#define OAPC_BIN_SUBTYPE_IMAGE_CUR     12
#define OAPC_BIN_SUBTYPE_IMAGE_ANI     13
#define OAPC_BIN_SUBTYPE_IMAGE_YUV     14 // raw, uncompressed image data in YUV format
#define OAPC_BIN_SUBTYPE_IMAGE_GRGB    15 // raw, uncompressed image data in GRGB format
#define OAPC_BIN_SUBTYPE_IMAGE_YUV422  16 // raw, uncompressed image data in YUV422 format
#define OAPC_BIN_SUBTYPE_IMAGE_GREY8   17 // raw, uncompressed greyscale image data
#define OAPC_BIN_SUBTYPE_IMAGE_BW1     18 // raw, uncompressed black/white image data
#define OAPC_BIN_SUBTYPE_IMAGE_CUSTOM 255 // to be used temporarily as long as no appropriate subtype exists for a specific application, NOT TO BE USED IN RELEASED VERSIONS!
#define OAPC_BIN_UNIT_IMAGE_NONE        0 // no measurement unit used for the type/subtype

// video subtypes
#define OAPC_BIN_SUBTYPE_VIDEO_3GPP       1 // [RFC4281][RFC3839]
#define OAPC_BIN_SUBTYPE_VIDEO_3GPP2      2 // [RFC4393][RFC4281]
#define OAPC_BIN_SUBTYPE_VIDEO_3GPPTT     3 // [RFC4396]
#define OAPC_BIN_SUBTYPE_VIDEO_H261       4 // [RFC4587]
#define OAPC_BIN_SUBTYPE_VIDEO_H263       5 // [RFC3555]
#define OAPC_BIN_SUBTYPE_VIDEO_H264       6 // [RFC3984]
#define OAPC_BIN_SUBTYPE_VIDEO_MP4        7 // [RFC4337]
#define OAPC_BIN_SUBTYPE_VIDEO_MPEG       8 // [RFC2045,RFC2046]
#define OAPC_BIN_SUBTYPE_VIDEO_OGG        9 // [RFC5334]
#define OAPC_BIN_SUBTYPE_VIDEO_RAW       10 // [RFC4175]
#define OAPC_BIN_SUBTYPE_VIDEO_CUSTOM   255 // to be used temporarily as long as no appropriate subtype exists for a specific application, NOT TO BE USED IN RELEASED VERSIONS!
#define OAPC_BIN_UNIT_VIDEO_NONE          0 // no measurement unit used for the type/subtype

// audio subtypes
#define OAPC_BIN_SUBTYPE_AUDIO_3GPP       1 // [RFC4281][RFC3839]
#define OAPC_BIN_SUBTYPE_AUDIO_3GPP2      2 // [RFC4393][RFC4281]
#define OAPC_BIN_SUBTYPE_AUDIO_MP4        3 // [RFC4337]
#define OAPC_BIN_SUBTYPE_AUDIO_MPEG       4 // [RFC3003]
#define OAPC_BIN_SUBTYPE_AUDIO_OGG        5 // [RFC5334]
#define OAPC_BIN_SUBTYPE_AUDIO_SPEEX      6 // [RFC5574]
#define OAPC_BIN_SUBTYPE_AUDIO_TONE       7 // [RFC4733]
#define OAPC_BIN_SUBTYPE_AUDIO_VORBIS     8 // [RFC5215]
#define OAPC_BIN_SUBTYPE_AUDIO_RAW8U      9 // 8 bit unsigned data, mono
#define OAPC_BIN_SUBTYPE_AUDIO_RAW8S     10 // 8 bit signed data, mono
#define OAPC_BIN_SUBTYPE_AUDIO_RAW8UI    11 // 8 bit unsigned data, interleaved stereo
#define OAPC_BIN_SUBTYPE_AUDIO_RAW8SI    12 // 8 bit signed data, interleaved stereo
#define OAPC_BIN_SUBTYPE_AUDIO_RAW16U    13 // 16 bit unsigned data, big-endian, mono
#define OAPC_BIN_SUBTYPE_AUDIO_RAW16S    14 // 16 bit signed data, big-endian, mono
#define OAPC_BIN_SUBTYPE_AUDIO_RAW16UI   15 // 16 bit unsigned data, big-endian, interleaved stereo
#define OAPC_BIN_SUBTYPE_AUDIO_RAW16SI   16 // 16 bit signed data, big-endian, interleaved stereo
#define OAPC_BIN_SUBTYPE_AUDIO_RAW16ULE  17 // 16 bit unsigned data, little-endian, mono
#define OAPC_BIN_SUBTYPE_AUDIO_RAW16SLE  18 // 16 bit signed data, little-endian, mono
#define OAPC_BIN_SUBTYPE_AUDIO_RAW16UILE 19 // 16 bit unsigned data, little-endian, interleaved stereo
#define OAPC_BIN_SUBTYPE_AUDIO_RAW16SILE 20 // 16 bit signed data, little-endian, interleaved stereo
#define OAPC_BIN_SUBTYPE_AUDIO_CUSTOM   255 // to be used temporarily as long as no appropriate subtype exists for a specific application, NOT TO BE USED IN RELEASED VERSIONS!
#define OAPC_BIN_UNIT_AUDIO_NONE          0 // no measurement unit used for the type/subtype

// model subtypes
#define OAPC_BIN_SUBTYPE_MODEL_IGES     1
#define OAPC_BIN_SUBTYPE_MODEL_MESH     2 // [RFC2077]
#define OAPC_BIN_SUBTYPE_MODEL_VRML     3 // [RFC2077]
#define OAPC_BIN_SUBTYPE_MODEL_CLI      4 // Common Layer Interface
#define OAPC_BIN_SUBTYPE_MODEL_STL      5 // Surface Tesselation Language
#define OAPC_BIN_SUBTYPE_MODEL_CUSTOM 255 // to be used temporarily as long as no appropriate subtype exists for a specific application, NOT TO BE USED IN RELEASED VERSIONS!
#define OAPC_BIN_UNIT_MODEL_NONE        0 // no measurement unit used for the type/subtype

// data subtypes
#define OAPC_BIN_SUBTYPE_DATA_PRESSURE_8U     1 // pressure data in 8 bit unsigned format
#define OAPC_BIN_SUBTYPE_DATA_PRESSURE_8S     2 // pressure data in 8 bit signed format
#define OAPC_BIN_SUBTYPE_DATA_PRESSURE_16U    3 // pressure data in 16 bit unsigned format, big-endian
#define OAPC_BIN_SUBTYPE_DATA_PRESSURE_16S    4 // pressure data in 16 bit signed format, big-endian
#define OAPC_BIN_SUBTYPE_DATA_PRESSURE_16ULE  5 // pressure data in 16 bit unsigned format, little-endian
#define OAPC_BIN_SUBTYPE_DATA_PRESSURE_16SLE  6 // pressure data in 16 bit signed format, little-endian
#define OAPC_BIN_SUBTYPE_DATA_PRESSURE_32U    7 // pressure data in 32 bit unsigned format, big-endian
#define OAPC_BIN_SUBTYPE_DATA_PRESSURE_32S    8 // pressure data in 32 bit signed format, big-endian
#define OAPC_BIN_SUBTYPE_DATA_PRESSURE_32ULE  9 // pressure data in 32 bit unsigned format, little-endian
#define OAPC_BIN_SUBTYPE_DATA_PRESSURE_32SLE 10 // pressure data in 32 bit signed format, little-endian
#define OAPC_BIN_SUBTYPE_DATA_LEVEL_8U       11 // level data in 8 bit unsigned format
#define OAPC_BIN_SUBTYPE_DATA_LEVEL_8S       12 // level data in 8 bit signed format
#define OAPC_BIN_SUBTYPE_DATA_LEVEL_16U      13 // level data in 16 bit unsigned format, big-endian
#define OAPC_BIN_SUBTYPE_DATA_LEVEL_16S      14 // level data in 16 bit signed format, big-endian
#define OAPC_BIN_SUBTYPE_DATA_LEVEL_16ULE    15 // level data in 16 bit unsigned format, little-endian
#define OAPC_BIN_SUBTYPE_DATA_LEVEL_16SLE    16 // level data in 16 bit signed format, little-endian
#define OAPC_BIN_SUBTYPE_DATA_LEVEL_32U      17 // level data in 32 bit unsigned format, big-endian
#define OAPC_BIN_SUBTYPE_DATA_LEVEL_32S      18 // level data in 32 bit signed format, big-endian
#define OAPC_BIN_SUBTYPE_DATA_LEVEL_32ULE    19 // level data in 32 bit unsigned format, little-endian
#define OAPC_BIN_SUBTYPE_DATA_LEVEL_32SLE    20 // level data in 32 bit signed format, little-endian
#define OAPC_BIN_SUBTYPE_DATA_SPEED_8U       21 // speed data in 8 bit unsigned format
#define OAPC_BIN_SUBTYPE_DATA_SPEED_8S       22 // speed data in 8 bit signed format
#define OAPC_BIN_SUBTYPE_DATA_SPEED_16U      23 // speed data in 16 bit unsigned format, big-endian
#define OAPC_BIN_SUBTYPE_DATA_SPEED_16S      24 // speed data in 16 bit signed format, big-endian
#define OAPC_BIN_SUBTYPE_DATA_SPEED_16ULE    25 // speed data in 16 bit unsigned format, little-endian
#define OAPC_BIN_SUBTYPE_DATA_SPEED_16SLE    26 // speed data in 16 bit signed format, little-endian
#define OAPC_BIN_SUBTYPE_DATA_SPEED_32U      27 // speed data in 32 bit unsigned format, big-endian
#define OAPC_BIN_SUBTYPE_DATA_SPEED_32S      28 // speed data in 32 bit signed format, big-endian
#define OAPC_BIN_SUBTYPE_DATA_SPEED_32ULE    29 // speed data in 32 bit unsigned format, little-endian
#define OAPC_BIN_SUBTYPE_DATA_SPEED_32SLE    30 // speed data in 32 bit signed format, little-endian
#define OAPC_BIN_SUBTYPE_DATA_VOLTAGE_8U     31 // voltage data in 8 bit unsigned format
#define OAPC_BIN_SUBTYPE_DATA_VOLTAGE_8S     32 // voltage data in 8 bit signed format
#define OAPC_BIN_SUBTYPE_DATA_VOLTAGE_16U    33 // voltage data in 16 bit unsigned format, big-endian
#define OAPC_BIN_SUBTYPE_DATA_VOLTAGE_16S    34 // voltage data in 16 bit signed format, big-endian
#define OAPC_BIN_SUBTYPE_DATA_VOLTAGE_16ULE  35 // voltage data in 16 bit unsigned format, little-endian
#define OAPC_BIN_SUBTYPE_DATA_VOLTAGE_16SLE  36 // voltage data in 16 bit signed format, little-endian
#define OAPC_BIN_SUBTYPE_DATA_VOLTAGE_32U    37 // voltage data in 32 bit unsigned format, big-endian
#define OAPC_BIN_SUBTYPE_DATA_VOLTAGE_32S    38 // voltage data in 32 bit signed format, big-endian
#define OAPC_BIN_SUBTYPE_DATA_VOLTAGE_32ULE  39 // voltage data in 32 bit unsigned format, little-endian
#define OAPC_BIN_SUBTYPE_DATA_VOLTAGE_32SLE  40 // voltage data in 32 bit signed format, little-endian
#define OAPC_BIN_SUBTYPE_DATA_CURRENT_8U     41 // current data in 8 bit unsigned format
#define OAPC_BIN_SUBTYPE_DATA_CURRENT_8S     42 // current data in 8 bit signed format
#define OAPC_BIN_SUBTYPE_DATA_CURRENT_16U    43 // current data in 16 bit unsigned format, big-endian
#define OAPC_BIN_SUBTYPE_DATA_CURRENT_16S    44 // current data in 16 bit signed format, big-endian
#define OAPC_BIN_SUBTYPE_DATA_CURRENT_16ULE  45 // current data in 16 bit unsigned format, little-endian
#define OAPC_BIN_SUBTYPE_DATA_CURRENT_16SLE  46 // current data in 16 bit signed format, little-endian
#define OAPC_BIN_SUBTYPE_DATA_CURRENT_32U    47 // current data in 32 bit unsigned format, big-endian
#define OAPC_BIN_SUBTYPE_DATA_CURRENT_32S    48 // current data in 32 bit signed format, big-endian
#define OAPC_BIN_SUBTYPE_DATA_CURRENT_32ULE  49 // current data in 32 bit unsigned format, little-endian
#define OAPC_BIN_SUBTYPE_DATA_CURRENT_32SLE  50 // current data in 32 bit signed format, little-endian
#define OAPC_BIN_SUBTYPE_DATA_POWER_8U       51 // power data in 8 bit unsigned format
#define OAPC_BIN_SUBTYPE_DATA_POWER_8S       52 // power data in 8 bit signed format
#define OAPC_BIN_SUBTYPE_DATA_POWER_16U      53 // power data in 16 bit unsigned format, big-endian
#define OAPC_BIN_SUBTYPE_DATA_POWER_16S      54 // power data in 16 bit signed format, big-endian
#define OAPC_BIN_SUBTYPE_DATA_POWER_16ULE    55 // power data in 16 bit unsigned format, little-endian
#define OAPC_BIN_SUBTYPE_DATA_POWER_16SLE    56 // power data in 16 bit signed format, little-endian
#define OAPC_BIN_SUBTYPE_DATA_POWER_32U      57 // power data in 32 bit unsigned format, big-endian
#define OAPC_BIN_SUBTYPE_DATA_POWER_32S      58 // power data in 32 bit signed format, big-endian
#define OAPC_BIN_SUBTYPE_DATA_POWER_32ULE    59 // power data in 32 bit unsigned format, little-endian
#define OAPC_BIN_SUBTYPE_DATA_POWER_32SLE    60 // power data in 32 bit signed format, little-endian
#define OAPC_BIN_SUBTYPE_DATA_GENERIC_8U     70 // other, numerical data in 8 bit unsigned format
#define OAPC_BIN_SUBTYPE_DATA_GENERIC_8S     62 // other, numerical data in 8 bit signed format
#define OAPC_BIN_SUBTYPE_DATA_GENERIC_16U    63 // other, numerical data in 16 bit unsigned format, big-endian
#define OAPC_BIN_SUBTYPE_DATA_GENERIC_16S    64 // other, numerical data in 16 bit signed format, big-endian
#define OAPC_BIN_SUBTYPE_DATA_GENERIC_16ULE  65 // other, numerical data in 16 bit unsigned format, little-endian
#define OAPC_BIN_SUBTYPE_DATA_GENERIC_16SLE  66 // other, numerical data in 16 bit signed format, little-endian
#define OAPC_BIN_SUBTYPE_DATA_GENERIC_32U    67 // other, numerical data in 32 bit unsigned format, big-endian
#define OAPC_BIN_SUBTYPE_DATA_GENERIC_32S    68 // other, numerical data in 32 bit signed format, big-endian
#define OAPC_BIN_SUBTYPE_DATA_GENERIC_32ULE  69 // other, numerical data in 32 bit unsigned format, little-endian
#define OAPC_BIN_SUBTYPE_DATA_GENERIC_32SLE  61 // other, numerical data in 32 bit signed format, little-endian
#define OAPC_BIN_SUBTYPE_DATA_CUSTOM        255 // to be used temporarily as long as no appropriate subtype exists for a specific application, NOT TO BE USED IN RELEASED VERSIONS!

#define OAPC_BIN_UNIT_DATA_NONE               0 // no measurement unit used for the type/subtype

#define OAPC_BIN_UNIT_DATA_PRESSURE_PA        1 // Pascal
#define OAPC_BIN_UNIT_DATA_PRESSURE_NM_2      2 // Nm^-2
#define OAPC_BIN_UNIT_DATA_PRESSURE_PSI       3 // psi
#define OAPC_BIN_UNIT_DATA_PRESSURE_IN_HG     4 // in Hg
#define OAPC_BIN_UNIT_DATA_PRESSURE_BAR       5 // bar

#define OAPC_BIN_UNIT_DATA_LEVEL_M            1 // meter
#define OAPC_BIN_UNIT_DATA_LEVEL_IN           2 // inch
#define OAPC_BIN_UNIT_DATA_LEVEL_FT           3 // foot
#define OAPC_BIN_UNIT_DATA_LEVEL_LITRE        4 // l

#define OAPC_BIN_UNIT_DATA_SPEED_M_SEC        1 // meter per second
#define OAPC_BIN_UNIT_DATA_SPEED_M_MIN        2 // meter per minute
#define OAPC_BIN_UNIT_DATA_SPEED_KM_SEC       3 // kilometers per second
#define OAPC_BIN_UNIT_DATA_SPEED_KM_H         4 // kilometers per hour
#define OAPC_BIN_UNIT_DATA_SPEED_MPH          5 // mile per hour

#define OAPC_BIN_UNIT_DATA_VOLTAGE_V          1 // volts

#define OAPC_BIN_UNIT_DATA_CURRENT_A          1 // amperes

#define OAPC_BIN_UNIT_DATA_POWER_W            1 // watts

// structured subtype
#define OAPC_BIN_SUBTYPE_STRUCT_LOG             1 // log data structure
#define OAPC_BIN_SUBTYPE_STRUCT_CTRL            2 // 2D/3D machinery control data (motion and tool data)
#define OAPC_BIN_SUBTYPE_STRUCT_CTRLEND         3 // end of control data stream
#define OAPC_BIN_SUBTYPE_STRUCT_LASERCTRL       4 // laser control data (additional tool data, to be sent before the next motion data for which it has to become valid)
#define OAPC_BIN_SUBTYPE_STRUCT_MOTIONCTRL      5 // motion control data (motion information with independent axes)
#define OAPC_BIN_SUBTYPE_STRUCT_OUTPUTCTRL      6 // I/O data for output (analogue and digital)
#define OAPC_BIN_SUBTYPE_STRUCT_INPUTCTRL       7 // I/O data for input (analogue and digital)
#define OAPC_BIN_SUBTYPE_STRUCT_WAITTRIG        8 // wait for an external trigger
#define OAPC_BIN_SUBTYPE_STRUCT_BITMAP          9 // special definitions for processing a bitmap; this structure should be followed by a OAPC_BIN_SUBTYPE_IMAGE_RGB24 or OAPC_BIN_SUBTYPE_IMAGE_GREY8
#define OAPC_BIN_SUBTYPE_STRUCT_DELAY          10 // wait for the number of microseconds given in param1
#define OAPC_BIN_SUBTYPE_STRUCT_FASTOUTPUTCTRL 11 // I/O data for output (analogue and digital), comparing to OAPC_BIN_SUBTYPE_STRUCT_OUTPUTCTRL this value has to be set immediately and not enqueued
#define OAPC_BIN_SUBTYPE_STRUCT_POS_CORR       12 // position correction data
#define OAPC_BIN_SUBTYPE_STRUCT_JOBSTART       13 // currently used by laser controller only, marks the beginning of a job
#define OAPC_BIN_SUBTYPE_STRUCT_JOBEND         14 // currently used by laser controller only, marks the end of a job
#define OAPC_BIN_SUBTYPE_STRUCT_MARKREADY      15 // currently used by laser controller only, signals when user is about to start marking (issued when mark dialogue is opened)
#define OAPC_BIN_SUBTYPE_STRUCT_ENDMARKREADY   16 // currently used by laser controller only, signals when user has finished marking (issued when mark dialogue is closed)
#define OAPC_BIN_SUBTYPE_STRUCT_STOPOUTPUT     17 // signals to stop all output as fast as possible and to reject possibly pending data
#define OAPC_BIN_SUBTYPE_STRUCT_SCANHEADINFO   18 // contains extended information about scanhead
#define OAPC_BIN_SUBTYPE_STRUCT_BRAKEOUTPUT    19 // signals to stop all output by flushing the pending data but not interrupting immediately
#define OAPC_BIN_SUBTYPE_STRUCT_STARTOUTPUT    20 // signals the main application to start output of data; this can be used when a hardware has to tirgger the main application
#define OAPC_BIN_SUBTYPE_STRUCT_OUTPUTRESP     21 // response to submitted I/O data for output (analogue and digital)
#define OAPC_BIN_SUBTYPE_STRUCT_AXISSTATE      22 // position and speed information for axis at given index
#define OAPC_BIN_SUBTYPE_STRUCT_WAITINPUTCTRL  23 // stop execution until these input data are found
#define OAPC_BIN_SUBTYPE_STRUCT_DYNGEOMSTART   24 // currently used by scanner controller only, marks the beginning of geometry data containing dynamic text/barcode/... information
#define OAPC_BIN_SUBTYPE_STRUCT_DYNGEOMEND     25 // currently used by scanner controller only, marks the end of geometry data that belong to previously sent dynamic data
#define OAPC_BIN_SUBTYPE_STRUCT_RESETTIMER     26 // signals the main application to reset the process timer
#define OAPC_BIN_SUBTYPE_STRUCT_SLICESTART     27 // currently used by laser controller only, marks the beginning of a slice
#define OAPC_BIN_SUBTYPE_STRUCT_SLICEEND       28 // currently used by laser controller only, marks the end of a slice
#define OAPC_BIN_SUBTYPE_STRUCT_ENABLE         29 // enable/disable a process by value in param1
#define OAPC_BIN_SUBTYPE_STRUCT_STOPAXIS       30 // stop the axis that is specified by the number in param1
#define OAPC_BIN_SUBTYPE_STRUCT_HALT           31 // halt current operation (param1=1) or continue a halted operation (param1=2)
#define OAPC_BIN_SUBTYPE_STRUCT_POWERDOWN      32 // turn off power for used hardware since it has not been used for a longer time
#define OAPC_BIN_SUBTYPE_STRUCT_DYNGEOMMARK    33 // currently used by scanner controller only, marks an intermediate position within a stream of vector data used for dynamic content
#define OAPC_BIN_SUBTYPE_STRUCT_SYNC           34 // provide synchronisation information with unique ID in param1
//#define OAPC_BIN_SUBTYPE_STRUCT_CUSTOM      255 // not used here, there can't be a undefined internal structure

// custom subtype
#define OAPC_BIN_SUBTYPE_CUSTOM_CUSTOM 255 // to be used temporarily as long as no appropriate subtype exists for a specific application, NOT TO BE USED IN RELEASED VERSIONS!

#define TOOLPARAM_VALIDFLAG_NO_X              0x00000001
#define TOOLPARAM_VALIDFLAG_NO_Y              0x00000002
#define TOOLPARAM_VALIDFLAG_Z                 0x00000004
#define TOOLPARAM_VALIDFLAG_ON                0x00000008
#define TOOLPARAM_VALIDFLAG_POWER             0x00000010
#define TOOLPARAM_VALIDFLAG_FREQ              0x00000020
#define TOOLPARAM_VALIDFLAG_OFFSPEED          0x00000040
#define TOOLPARAM_VALIDFLAG_ONSPEED           0x00000080
#define TOOLPARAM_VALIDFLAG_OFFDELAY          0x00000100
#define TOOLPARAM_VALIDFLAG_ONDELAY           0x00000200
#define TOOLPARAM_VALIDFLAG_PARAM1            0x00000400
#define TOOLPARAM_VALIDFLAG_PARAM2            0x00000800
#define TOOLPARAM_VALIDFLAG_PARAM3            0x00001000
#define TOOLPARAM_VALIDFLAG_PARAM4            0x00002000
#define TOOLPARAM_VALIDFLAG_PARAM5            0x00004000
#define TOOLPARAM_VALIDFLAG_PARAM6            0x00008000
#define TOOLPARAM_DESCFLAG_IS_SLICE           0x00010000
#define TOOLPARAM_VALIDFLAG_VARIABLE_PARAM3   0x00020000

struct oapc_bin_struct_vec3d // structure for vector data
{
   int x,y,z;            // x, y and optional z coordinate in micrometers
};

struct oapc_bin_struct_ctrl // structure for OAPC_BIN_TYPE_STRUCT->OAPC_BIN_SUBTYPE_STRUCT_CTRL
{
   unsigned int                 validityFlags;    // specifies which of the below fields contain valid data
   unsigned char                on;               // turn the tool on (1) or not (0) or drill a single hole at the given position (2)
   unsigned char                reserved1;        // unused
   unsigned short               numCoords;        // number of coordinates stored in coord;
   int                          power,frequency;  // power in (1/1000 %) and frequency (in Hz) value
   unsigned int                 offSpeed,onSpeed; // motion speed values in unit micrometers per second
   int                          offDelay,onDelay; // time between start of movement and turning the tool/laser on, value in usec, can be negative
   int                          toolParam[6];     // additional tool parameters (e.g. 0 - jump delay, 1 - mark delay, 2 - polygon delay, 3 - spotsize...)
   struct oapc_bin_struct_vec3d coord[1];         // the vector data have a size of at least 1, numCoords defines the real size of the array
};

struct oapc_bin_struct_laserctrl // structure for OAPC_BIN_TYPE_STRUCT->OAPC_BIN_SUBTYPE_STRUCT_LASERCTRL
{
   unsigned short version,res;
   unsigned int   uPolyDelayBreakAngle;          // udeg
   unsigned int   nPulseLength;                  // YAG, IPG, nsec
   unsigned int   uWobbleAmp,mWobbleFreq;        // wobbel amplitude [micrometers] and frequency (milli-Hz)
   unsigned int   uPulseLength_deprec;           // YAG, usec (deprecated, use nPulseLength instead!)
   unsigned int   nFirstPulse;                   // YAG, nsec
   unsigned int   mSimmer;                       // SPI, simmer current in 1/1000%
   unsigned int   nStandByPulseWidth;            // CO2, nsec
   unsigned int   standByFreq;                   // CO2, Hz
   unsigned int   waveformNum;                   // SPI
   unsigned int   edgeLevel;
   unsigned int   skyTimeLag,skyOnShift,skyPrev,skyPost; // sky writing parameters
   unsigned int   rampMode;                      // ramp mode, power/speed ramping, ramp in/out
   unsigned int   uRampStartLen,uRampEndLen;     // ramping phase length
   unsigned int   mRampStartVal,mTampEndVal;     // ramping values in 1/1000%
};

struct oapc_bin_struct_motionctrl // structure for OAPC_BIN_TYPE_STRUCT->OAPC_BIN_SUBTYPE_STRUCT_MOTIONCTRL
{
   unsigned char enableAxes;       // bitpattern that defines which axes to move/stop/move to home
   unsigned char relativeMovement; // bitpattern that defines which of the moving axes have to perform relative movement
   unsigned char stopAxes;         // bitpattern that defines which of the axes has to be stopped, in case this flag is used for an axis, the position and speed is ignored
   unsigned char moveAxesToHome;   // bitpattern that defines which of the axes has to be moved to the home position, in case this flag is used for an axis, the position and speed is ignored
   unsigned int  res;              // reserved
   int           position[8];      // new position in unit um
   unsigned int  speed[8];         // motion speed in unit um/sec
   unsigned int  reserved1[8];
   unsigned int  reserved2[8];
};

//      IOCTRL_type_bits_number
#define IOCTRL_LASERPORT_8_1       0x00000001
#define IOCTRL_LASERPORT_8_2       0x00000002
#define IOCTRL_ANALOGUE_8_1        0x00000004
#define IOCTRL_ANALOGUE_8_2        0x00000008
#define IOCTRL_ANALOGUE_10_1       0x00000010
#define IOCTRL_ANALOGUE_10_2       0x00000020
#define IOCTRL_ANALOGUE_10_3       0x00000040
#define IOCTRL_ANALOGUE_10_4       0x00000080
#define IOCTRL_ANALOGUE_10_5       0x00000100
#define IOCTRL_ANALOGUE_10_6       0x00000200
#define IOCTRL_ANALOGUE_12_1       0x00000400
#define IOCTRL_ANALOGUE_12_2       0x00000800
#define IOCTRL_ANALOGUE_12_3       0x00001000
#define IOCTRL_DIGITAL_8_1         0x00002000
#define IOCTRL_DIGITAL_8_2         0x00004000
#define IOCTRL_DIGITAL_16_1        0x00008000
#define IOCTRL_DIGITAL_16_2        0x00010000
#define IOCTRL_DIGITAL_32          0x00020000
#define IOCTRL_ANALOGUE_16_0       0x00040000
#define IOCTRL_ANALOGUE_12_4       0x00080000
#define IOCTRL_ANALOGUE_16_1       0x00100000
#define IOCTRL_ANALOGUE_16_2       0x00200000
#define IOCTRL_SERIAL_DATA_1       0x00400000
#define IOCTRL_LASERPORT_8_1_BITS  0x00800000 // same as IOCTRL_LASERPORT_8_1 but bits can be set separately (with bitmask)
#define IOCTRL_LASERPORT_8_2_BITS  0x01000000 // same as IOCTRL_LASERPORT_8_2 but bits can be set separately (with bitmask)
#define IOCTRL_PILOTLASER          0x02000000

#define IOCTRL_SERIAL_DATA_LENGTH  200

struct oapc_bin_struct_ioctrl // structure for OAPC_BIN_TYPE_STRUCT->OAPC_BIN_SUBTYPE_STRUCT_OUTPUTCTRL and OAPC_BIN_TYPE_STRUCT->OAPC_BIN_SUBTYPE_STRUCT_INPUTCTRL
{
   unsigned int   enableFlags;      // enable flags that specify which of the following fields contain usable values
   unsigned char  laserport8[2];    // laserport digital data
   unsigned char  digital8[2];      // digital data
   unsigned char  digital8mask[2];  // mask for digital data is used for output and specifies which bits have to be changed/untouched
   unsigned short digital16[2];     // digital data
   unsigned short digital16mask[2]; // mask for digital data is used for output and specifies which bits have to be changed/untouched
   unsigned char  analogue8[2];
   unsigned short analogue16_0;
   unsigned short analogue10[6];
   unsigned short analogue12[4];
   unsigned short analogue16_1,
                  analogue16_2;     // two additional 16 bit analogue outputs
   unsigned long  digital32;        // digital data
   unsigned long  digital32mask;    // mask for digital data is used for output and specifies which bits have to be changed/untouched   
   char           serialData[IOCTRL_SERIAL_DATA_LENGTH+1];
   char           serialDataLength;
   unsigned char  laserport8mask[2]; // mask for laserport digital data when IOCTRL_LASERPORT_8_x_BITS is used
   char           pilotLaser;
};


#define BITMAP_FLAG_TYPE_DITHERFS       0x0001
#define BITMAP_FLAG_TYPE_BLACKWHITE     0x0002
#define BITMAP_FLAG_TYPE_GREYSCALE      0x0003
#define BITMAP_FLAG_TYPE_MASK           0x000F
#define BITMAP_FLAG_MARK_BIDIRECTIONAL  0x0010
#define BITMAP_FLAG_MARK_FROM_LAST_LINE 0x0020
#define BITMAP_FLAG_MARK_WO_LINE_INCR   0x0040
#define BITMAP_FLAG_MARK_INVERTED       0x0080 // this flag is handled internally, when it is set, BeamConstruct always generates inverted GREY8 images before sending them to the plug-in
#define BITMAP_FLAG_KEEP_ASPECT_FIXED_X 0x0100 // keep the x size of the image and the aspect ration in y direction when the image is replaced
#define BITMAP_FLAG_KEEP_ASPECT_FIXED_Y 0x0200 // keep the y size of the image and the aspect ration in x direction when the image is replaced
                                               // when none of these flags is set, x and y size is kept also whe naspect ratio is damaged

struct oapc_bin_struct_bitmap // structure for OAPC_BIN_TYPE_STRUCT->OAPC_BIN_SUBTYPE_STRUCT_BITMAP
{
   int uPosOffsetX,uPosOffsetY,uPosOffsetZ;    // top left coordinates relative to the top left corner of the working area [micrometers]
   int udPixelSizeX,udPixelSizeY,udPixelSizeZ; // width and height of one pixel [1/10 micrometers]; the width and height in pixels is defined by the image structure
   int flags;                                  // bitmap flags that specify the type and processing information
};


#define DYN_DATA_MAX_STRING_LENGTH 255

#define DYN_DATA_TYPE_FONT_RECT_SINGLE       0x01000000
#define DYN_DATA_TYPE_FONT_RECT_DOUBLE       0x02000000
#define DYN_DATA_TYPE_FONT_ROMAN_SIMPLE      0x03000000
#define DYN_DATA_TYPE_FONT_ROMAN_DOUBLE      0x04000000
#define DYN_DATA_TYPE_FONT_SCRIPT_SIMPLE     0x05000000
#define DYN_DATA_TYPE_FONT_SCRIPT_DOUBLE     0x06000000
#define DYN_DATA_TYPE_FONT_SCRIPT_COMPLEX    0x07000000
#define DYN_DATA_TYPE_FONT_TIMES             0x08000000
#define DYN_DATA_TYPE_FONT_TIMES_BOLD        0x09000000
#define DYN_DATA_TYPE_FONT_TIMES_ITALIC      0x0A000000
#define DYN_DATA_TYPE_FONT_TIMES_ITALIC_BOLD 0x0B000000
#define DYN_DATA_TYPE_FONT_CHAR_TABLE_BASED  0xFF000000
#define DYN_DATA_TYPE_BARCODE_DATAMATRIX             71
#define DYN_DATA_TYPE_BARCODE_QR                    104

#define DYN_DATA_MISCFLAG_FLIPFLAG_XY        0x01 // exchange x and y coodrinate during drawing

/** Dynamic text information, used to provide data regarding a dynamically changeable and automatically
    updated element instead of raw, fixed vector data */
struct oapc_bin_struct_dyn_data // structure for OAPC_BIN_TYPE_STRUCT->OAPC_BIN_SUBTYPE_STRUCT_DYN_DATA
{
   unsigned int   UID;                                     // unique identifier, used to modify it from outside e.g. on a stand-alone device
   // -------------------------------------------------------------------------------------------------------------------------------
   char           fmtString[DYN_DATA_MAX_STRING_LENGTH+1]; // string that contains the changeable text / the format string with some placeholders for
                                                           // automatically changeable parts of the string according to BeamConstructs format definition
   unsigned int   type;                                    // specifies the type of element to be used (which font or which barcode type)
   unsigned int   flags,param1,param2,param3;              // additional parameters belonging to creation of base element
   unsigned int   uScaleX,uScaleY;                         // scaling factors in X and Y direction (out of element matrix)
   // -------------------------------------------------------------------------------------------------------------------------------
   int            snStartValue;                            // value to start serial number counting at
   int            snResetAt;                               // value to reset serial number countint to start value, does not apply when it is smaller than snStartValue;
   unsigned short snIncrement,snBeatCount,snBeatOffset;    // counting defintions increment and beat
   unsigned char  snNumericBase,snMinDigits;               // numeric base for displayed serial number
   int            timeOffset;                              // time value offset (in unit seconds)
   unsigned int   snFlags;                                 // time at what the serial number has to be reset
   // -------------------------------------------------------------------------------------------------------------------------------
   unsigned char  quietZone;                               // size of quiet zone around inverted barcode in unit "multiple of cell size"
   // -------------------------------------------------------------------------------------------------------------------------------
   unsigned char  miscFlags;                               // additional flags specifying method of creating
   unsigned short res1b;
   int            res2,res3,res4,res5,res6,res7;           // reserved for later usage, has to be set to 0
};

/** Position correction data */
struct oapc_bin_struct_pos_corr // structure for OAPC_BIN_TYPE_STRUCT->OAPC_BIN_SUBTYPE_STRUCT_POS_CORR
{
   int uPosX,uPosY,uPosZ;       // position offset in X, Y and Z direction in unit micrometers
   int mXAngle,mYAngle,mZAngle; // rotational correction for X, Y and Z axes in milli-degrees
};


#define SCANHEAD_ERRORFLAG_VOLTAGE_X   0x00000001 // voltage error X-axis
#define SCANHEAD_ERRORFLAG_VOLTAGE_Y   0x00000002 // voltage error Y-Axis
#define SCANHEAD_ERRORFLAG_VOLTAGE_Z   0x00000004 // voltage error Z-axis
#define SCANHEAD_ERRORFLAG_GALVOTEMP_X 0x00000008 // X galvo temperature error
#define SCANHEAD_ERRORFLAG_GALVOTEMP_Y 0x00000010 // Y galvo temperature error
#define SCANHEAD_ERRORFLAG_GALVOTEMP_Z 0x00000020 // Z galvo temperature error
#define SCANHEAD_ERRORFLAG_POSITION_X  0x00000040 // X positioning error
#define SCANHEAD_ERRORFLAG_POSITION_Y  0x00000080 // Y positioning error
#define SCANHEAD_ERRORFLAG_POSITION_Z  0x00000100 // Z positioning error

struct oapc_bin_struct_scanheadinfo // structure for OAPC_BIN_TYPE_STRUCT->OAPC_BIN_SUBTYPE_STRUCT_SCANHEADINFO
{
   unsigned char  ndHead;                           // head number, this value is related to the primary/secondary head only and has nothing to do with the number of independent heads within a multihead environment
   char           res1,res2,res3;
   unsigned int   errorFlags;
   unsigned short galvoTemp[3];                     // temperature of galvos/axis drivers in 0.1 deg Celsius, 0 means no information available
   unsigned short boardTemp[3];                     // temperature of control board in 0.1 deg Celsius, 0 means no information available
   unsigned int   serialNumber[3],articleNumber[3];
   unsigned int   firmwareVersion[3];
   unsigned int   uAperture;                        // aperture in micrometers
   unsigned int   pWavelength;                      // wavelength in picometers
   unsigned int   operatingTime[3];                 // operating hours counter in unit seconds
   char           cardType[128],headType[128];      // name of card and head
   char           boardVersion[128];                // hard/firmware version of controller board
   unsigned int   xy2Status;                        // XY2-100 status word sent back from head
};

#endif
