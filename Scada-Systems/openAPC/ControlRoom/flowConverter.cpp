/******************************************************************************

This file is part of ControlRoom process control/HMI software.

ControlRoom is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation, either version 3 of the License, or (at your option) any
later version.

ControlRoom is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
details.

You should have received a copy of the GNU General Public License along with
ControlRoom. If not, see http://www.gnu.org/licenses/

*******************************************************************************

For different licensing and/or usage of the sources apart from GPL or any other
open source license, please contact us at https://openapc.com/contact.php

*******************************************************************************/

#include <wx/wx.h>
#include <wx/mstream.h>

#ifndef __WXMSW__
#include <arpa/inet.h>
#endif

#include "iff.h"
#include "globals.h"
#include "flowConverter.h"
#include "oapc_libio.h"


wxUint32 flowConverter::CONVERTER_FLAGS_OUTPUT_LOW         =0x00000001;
wxUint32 flowConverter::CONVERTER_FLAGS_OUTPUT_HIGH        =0x00000002;
wxUint32 flowConverter::CONVERTER_FLAGS_OUTPUT_PULSE_LOHI  =0x00000003;
wxUint32 flowConverter::CONVERTER_FLAGS_OUTPUT_PULSE_HILO  =0x00000004;
wxUint32 flowConverter::CONVERTER_FLAGS_BINARY_MODE        =0x00000005; // perform a conversion
wxUint32 flowConverter::CONVERTER_FLAGS_DIRECT_MODE        =0x00000006; // perform an assignment from input value to assigned output value
wxUint32 flowConverter::CONVERTER_FLAGS_FORMAT_MODE        =0x00000007; // use a format string for conversion
wxUint32 flowConverter::CONVERTER_FLAGS_OUTPUT_MASK        =0x0000000F;

wxUint8 flowConverter::CONVERTER_FLAGS_CMP_EQUAL           =0x01;
wxUint8 flowConverter::CONVERTER_FLAGS_CMP_NOTEQUAL        =0x02;
wxUint8 flowConverter::CONVERTER_FLAGS_CMP_GREATER         =0x03;
wxUint8 flowConverter::CONVERTER_FLAGS_CMP_SMALLER         =0x04;
wxUint8 flowConverter::CONVERTER_FLAGS_CMP_MASK            =0x0F;

wxUint32 flowConverter::CONVERTER_FLAGS_USE_CLOCK          =0x00000010;
wxUint32 flowConverter::CONVERTER_FLAGS_FORCE_TO_DOT       =0x00000020;


flowConverter::flowConverter():flowObject(NULL)
{
   this->data.type=0; // this is a base-class that should not be used directly
}



flowConverter::~flowConverter()
{
}



wxString flowConverter::getDefaultName()
{
   wxASSERT(0);
   return _T("##invalid###");
}



