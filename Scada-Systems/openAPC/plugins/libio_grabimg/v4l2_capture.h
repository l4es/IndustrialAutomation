/********************************************************************************************

These sources are distributed in the hope that they will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. All
the information given here, within the interface descriptions and within the specification
are subject to change without notice. Errors and omissions excepted.

These sources demonstrate the usage of the OpenAPC Plug-In programming interface. They can be
used freely according to the OpenAPC Dual License: As long as the sources and the resulting
applications/libraries/Plug-Ins are used together with the OpenAPC software, they are
licensed as freeware. When you use them outside the OpenAPC software they are licensed under
the terms of the GNU General Public License.

For more information please refer to http://www.openapc.com/oapc_license.php

*********************************************************************************************/


#ifndef V4L2_CAPTURE_H
#define V4L2_CAPTURE_H

#ifdef ENV_LINUX

extern int  openVideoDevice(struct instData *data,int device_number);
extern int  captureImage(struct instData *data);
extern void closeVideoDevice(struct instData *data);

/*int read_frame(struct instData *data);
int start_capturing(struct instData *data);
int init_device(struct instData *data);
int open_device(struct instData *data);
void uninit_device(struct instData *data);
void stop_capturing(struct instData *data);
int grab_frame(struct instData *data);*/

#endif //ENV_LINUX

#endif
