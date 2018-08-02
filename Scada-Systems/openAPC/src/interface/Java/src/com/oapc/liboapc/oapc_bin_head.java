package com.oapc.liboapc;

import java.io.*;

public class oapc_bin_head
{
   int           version;           // the structure version number, this field will always stay at this position
   int           sizeHead;          // the length of the structure, this field will always stay at this position
   byte          type,subType;      // type and subtype definitions for the appended data, please refer below
   byte          compression;       // the data may be comressed additionally, the type of compression method is specified by this element using a OAPC_COMPRESS_xxx value
   byte          unit;              // measurement unit, mainly applies to type OAPC_BIN_TYPE_DATA
   short         unitExponent;      // exponent of the unit (as examples: 3 means "kilo", 6 "mega", -3 "milli", -12 "nano")
   short         int1;              // for internal use only, do not change!            
   int           param1,param2,     // data type dependent parameters, they may be used for additional parameters describing the binary data
                 param3;     
   int           sizeData;          // the length of the payload data that are appended directly after this structure
   byte[]        data;              // data   

   
   
   public void write(DataOutputStream out)
   throws IOException
   {
      out.writeInt(version);      out.writeInt(sizeHead);
      out.writeByte(type);        out.writeByte(subType);
      out.writeByte(compression);
      out.writeByte(unit);        out.writeShort(unitExponent);
      out.writeShort(int1);
      out.writeInt(param1);       out.writeInt(param2);         out.writeInt(param3);
      out.writeInt(sizeData);
      if (data.length<sizeData) throw new IOException("Data size error");
      out.write(data);
   }
   
   
   
   public void read(DataInputStream in)
   throws IOException
   {
	  data=null;
      version=in.readInt();      sizeHead=in.readInt();
      type=in.readByte();        subType=in.readByte();
      compression=in.readByte();
      unit=in.readByte();        unitExponent=in.readShort();
      int1=in.readShort();
      param1=in.readInt();       param2=in.readInt();         param3=in.readInt();
      sizeData=in.readInt();
      data=new byte[sizeData];
      in.read(data);
   }
   
   
   
};
