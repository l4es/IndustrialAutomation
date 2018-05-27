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

 public enum SlaveState 
    {
        Unknow (1),
        Init  (0x01),
        PreOperational (0x02),
        Boot (0x03),
        SafeOperational (0x04),
        Operational (0x08),
        Ack (0x10),                       // Not really a state
        Init_ErrorActive (0x11),
        PreOperational_ErrorActive (0x12),
        SafeOperational_ErrorActive (0x14),
        Operational_ErrorActive (0x18) ; // Don't think it can exist
        
        private short state;
        
        SlaveState(int state)
        { 
            this.state=(short)state; 
        }

        public short GetVal()
        {
            return state;
        }
    };
