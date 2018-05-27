/**************************************************************************
*                           MIT License
* 
* Copyright (C) 2016 Frederic Chaxel <fchaxel@free.fr>
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*********************************************************************/
package soem;

import java.util.ArrayList;
import java.util.List;

public class SoemInterrop 
{
    static 
    {
        try
        {
            System.loadLibrary("SoemWrapper");
        }
        catch (Exception e)
        {
            System.err.println("Native code library failed to load.\n" + e);
            System.exit(1);
        }
    } 
     public static native int StartActivity(String interfaceName, int DelayUpMs);
     public static native void StopActivity();
     public static native void Run(short State);
     public static native void RefreshSlavesState();
     public static native int GetDevicesNumber();
     public static native String GetDeviceInfo(int deviceNum, int ParamNum);
     public static native void WriteState(int deviceNum, short State);
     public static native void Reconfigure(int deviceNum);
     public static native int GetInput(int deviceNum, byte[] buf);
     public static native int SetOutput(int deviceNum, byte[] buf);
     public static native int ReadPDO(int deviceNum, int Index,
                                    int SubIndex, int size, byte[] buf);
     public static native int WritePDO(int deviceNum, int Index,
                                    int SubIndex, int size, byte[] buf);
     
     private static native String GetInterfaces();
     
     public static List<EtherCATInterface> GetEtherCATInterfaces()
     {         
         String it=GetInterfaces();
         if (it.length()!=0)
         {
            String[] sts=it.split("\t");
            
            List<EtherCATInterface> itface=new ArrayList<>();
            for (int i=0;i<sts.length/2;i++)
            {
                itface.add(new EtherCATInterface(sts[i*2], sts[i*2+1]));
            }     
            return itface;
         }
         else
             return null;
     }
}
   
