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
package simplemasterj;

import java.nio.charset.Charset;
import java.util.List;
import soem.*;

public class SimpleMasterJ 
{
    public static void main(String[] args)
    {       
        // Shows all Ethernet/Wifi Interface
        System.out.println("\r\nAvailable interfaces :");
        List<EtherCATInterface> it=SoemInterrop.GetEtherCATInterfaces();
        for (EtherCATInterface eth : it)
        {
            System.out.println("\t"+eth.Descr+"\r\n\t\t"+eth.Name);
        }
        
        // Interface here is the Name field of one of the previous displayed values
        if (SoemInterrop.StartActivity("\\Device\\NPF_{DB61FC71-8D09-444A-9187-CAD656E0D3FC}", 1000)<1)
        {
            System.out.println("No slave or interface error");
            return;
        }
        
        int SlaveNb=SoemInterrop.GetDevicesNumber();
        
        System.out.println("\r\nEtherCAT Slaves :");
        for (int i=1;i<SlaveNb+1;i++)
        {
            String name=SoemInterrop.GetDeviceInfo(i, DeviceInfoParam.Name.GetVal());
            System.out.println("\tName = "+name);
        }
        
        // Read PDO idx 0x1008 in device 1 : Manufacturer Device Name 
        byte[] buf=new byte[1024]; // large enought
        int lenght=SoemInterrop.ReadPDO(1, 0x1008, -1, 1024, buf);
        if (lenght!=-1)
        {
            String DevName=new String(buf,0,lenght,Charset.forName("US-ASCII"));                        
            System.out.println("\r\nManufacturer Device Name = "+DevName);
        }
        
        // Run for all devices
        SoemInterrop.Run(SlaveState.Operational.GetVal());
        
        byte[] bin1 = new byte[6]; // must be equal or more than InputSize in the corresponding slave
        byte[] bout2 = new byte[6]; // must be strikcly equal to OutputSize in the corresponding slave
                
        System.out.println("\r\n... running ...");
        for (; ; )
        {
            // Get Input from the first slave
            SoemInterrop.GetInput(1, bin1);

            // Some action ! 
            bout2[4] = bin1[4];

            // Set Output in the second slave
            SoemInterrop.SetOutput(2, bout2);
            
            // Quite tired with this work
            try
            {
                Thread.sleep(200);
            } 
            catch (Exception e) { }
        }
        
    }
    
}
