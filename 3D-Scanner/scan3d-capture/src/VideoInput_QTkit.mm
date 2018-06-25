/*
Copyright (c) 2012, Daniel Moreno and Gabriel Taubin
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Brown University nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL DANIEL MORENO AND GABRIEL TAUBIN BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "VideoInput_QTkit.hpp"

#include <iostream>

#import <QTKit/QTKit.h>

QStringList list_devices_qtkit(bool silent)
{
    QStringList list;

    NSAutoreleasePool* localpool = [[NSAutoreleasePool alloc] init];

    NSArray* devices = [[[QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeVideo]
            arrayByAddingObjectsFromArray:[QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeMuxed]] retain];

    if ([devices count] == 0) {
        std::cout << "QTKit didn't find any attached Video Input Devices!" << std::endl;
        [localpool drain];
        return list;
    }

    int nCameras = [devices count];
    if (!silent) std::cerr << "QTkit: found " << nCameras << " cameras" << std::endl;

    for (int i=0; i<nCameras; i++)
    {
        QTCaptureDevice * device = [devices objectAtIndex:i];
        std::string name([[device localizedDisplayName] cStringUsingEncoding:NSUTF8StringEncoding]);
        list.append(QString::fromStdString(name));
 
        if (!silent) std::cerr << "QTkit: camera name=" << name << std::endl; 
    }

    [localpool drain];
    return list;
}
