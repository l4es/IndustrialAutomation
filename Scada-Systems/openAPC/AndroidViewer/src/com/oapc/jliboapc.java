package com.oapc;

import java.io.*;

class oapc_num_value_block
{
   int   numValue;     // the numerical value
   short numDivider; // the power of ten to divide by
   short reserved;

   int load(DataInputStream in)
   {
      int loaded=0;

      try
      {
    	   numValue=IO.readInt(in);     loaded+=4;
    	   numDivider=IO.readShort(in); loaded+=2;
         reserved=IO.readShort(in);   loaded+=2;
      }
      catch (IOException ioe)
      {
    	  
      }
      return loaded;
   }
   
   void set(oapc_num_value_block data)
   {
      numValue=data.numValue;
      numDivider=data.numDivider;
      reserved=data.reserved;
   }
   
   double toDouble()
   {
      if ((numDivider==0) || (numDivider==1)) return (double)numValue;
      return numValue/Math.pow(10.0,numDivider);
   }
   
};



class jliboapc 
{
   static final int OAPC_OK                      =0x00000001; /** everything is OK, an operation was performed successfully */
   static final int OAPC_ERROR_CONNECTION        =0x00000002; /** a required connection could not be established */
   static final int OAPC_ERROR_DEVICE            =0x00000003; /** a required device could not be opened/accessed */
   static final int OAPC_ERROR_NOT_SUPPORTEDi    =0x00000004; /** NO LONGER REQUIRED: functions that are not required due to the capability definitions do not need to be provided by a plug-in */
   static final int OAPC_ERROR_NO_DATA_AVAILABLE =0x00000005; /** this value is returned when data are requested for an output where no new data are available at this moment */
   static final int OAPC_ERROR_NO_SUCH_IO        =0x00000006; /** the main application tried to access an input or output that doesn't exists, that means the given input/output flags are wrong or it is a misbehaviour of the application  */
   static final int OAPC_ERROR_RESOURCE          =0x00000007; /** a required resource could not be created/accessed */
   static final int OAPC_ERROR_AUTHENTICATION    =0x00000008; /** authentication error, required authentication/username/password did not match */
   static final int OAPC_ERROR                   =0x00000009; /** other error, has to be used as followup-error code of a more detailled one only/has to be used for cases that normally never happen */
   static final int OAPC_ERROR_CONVERSION_ERROR  =0x0000000A; /** there was a conversion error, input data have not been able to be converted */
   static final int OAPC_ERROR_NO_MEMORY         =0x0000000B; /** required memory could not be allocated */
   static final int OAPC_ERROR_STILL_IN_PROGRESS =0x0000000C; /** an operation is still in progress so that the new data sent to an input can't be handled at the moment */
   static final int OAPC_ERROR_RECV_DATA         =0x0000000D; /** the reception of data failed */
   static final int OAPC_ERROR_SEND_DATA         =0x0000000E; /** the transmission of data failed */
   static final int OAPC_ERROR_PROTOCOL          =0x0000000F; /** an error with/in a used communication occured */
   static final int OAPC_ERROR_INVALID_INPUT     =0x00000010; /** the data given to the Plug-In are invalid/out of allowed range */
   static final int OAPC_ERROR_CUSTOM            =0x00000011; /** a custom error occured, the related error message can be fetched via function oapc_get_error_message() from the plug-in */
   static final int OAPC_ERROR_CREATE_FILE_FAILED=0x00000012; /** creation of a new file/appending data to an existing file failed, file could not be opened */
   static final int OAPC_ERROR_OPEN_FILE_FAILED  =0x00000013; /** opening an existing file for reading failed */
   static final int OAPC_ERROR_WRITE_FILE_FAILED =0x00000014; /** writing data into an already opened file failed */
   static final int OAPC_ERROR_READ_FILE_FAILED  =0x00000015; /** reading data from an already opened file failed */

   static final int OAPC_ERROR_MASK             =0x000000FF; /** mask for filtering the error code part */

   static final int OAPC_INT_FLOW_FORKEDi       =0x00000100; /** internal use only, does not apply for externel plug-ins */
   static final int OAPC_INT_BIN_NOT_UNLOCKEDi  =0x00000200; /** internal use only, does not apply for externel plug-ins */
   
   
}
