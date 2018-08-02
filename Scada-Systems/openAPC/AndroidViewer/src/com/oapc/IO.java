package com.oapc;

import java.io.*;

public class IO 
{
   static long readLong(DataInputStream in)
   throws IOException
   {
      byte[] d=new byte[8];
      
      in.read(d);

      return ((d[0] % 0xFF)<<56 | (d[1] &0xFF)<<48 | (d[2] &0xFF)<<40 | (d[3] & 0xFF)<<32 |
              (d[4] % 0xFF)<<24 | (d[5] &0xFF)<<16 | (d[6] &0xFF)<<8 | (d[7] & 0xFF)); 
   }

   
   
   static int readInt(DataInputStream in)
   throws IOException
   {
      byte[] d=new byte[4];
      
      in.read(d);

      return ((d[0] % 0xFF)<<24 | (d[1] &0xFF)<<16 | (d[2] &0xFF)<<8 | (d[3] & 0xFF)); 
   }

   
   
   static short readShort(DataInputStream in)
   throws IOException
   {
      byte[] d=new byte[2];
      
      in.read(d);
      return (short)((d[0] & 0xFF)<<8 | (d[1] & 0xFF)); 
   }

   
   
   /**
    * 
    * @param in input stream to read from
    * @param length raw length in bytes, the number of resulting characters can be length/2 at maximum
    * @return loaded string
    * @throws IOException
    */
   static String readUTF16BE(DataInputStream in,int length)
   throws IOException
   {
      byte[] d=new byte[length];
      int    i;
      String s;
      
      in.read(d);
      //cut data
      for (i=0; i<length; i+=2)
      {
         if ((d[i]==0) && (d[i+1]==0))
         {
            if (i==0) return "";
            break;
         }
      }
      s=new String(d,"UTF-16BE");
      return s.substring(0,i/2);
   }

}
