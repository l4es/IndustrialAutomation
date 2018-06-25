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

#ifndef __IO_UTIL_HPP__
#define __IO_UTIL_HPP__

#include <QImage>
#include <opencv2/core/core.hpp>
#include "scan3d.hpp"

namespace io_util
{
    enum PlyFlags {PlyPoints = 0x00, PlyColors = 0x01, PlyNormals = 0x02, PlyBinary = 0x04, PlyPlane = 0x08, PlyFaces = 0x10, PlyTexture = 0x20};
    
    bool write_ply(const std::string & filename, scan3d::Pointcloud const& pointcloud, unsigned flags = PlyPoints);

    QImage qImage(const cv::Mat & image);
    QImage qImageFromRGB(const cv::Mat & image);
    QImage qImageFromGray(const cv::Mat & image);

    bool write_pgm(const cv::Mat & image, const char * basename);
};

#endif  /* __IO_UTIL_HPP__ */
