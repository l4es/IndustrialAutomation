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

#ifndef __STRUCTURED_LIGHT_HPP__
#define __STRUCTURED_LIGHT_HPP__

#include <opencv2/core/core.hpp>

#ifndef _MSC_VER
#  ifndef _isnan
#    include <math.h>
#    define _isnan std::isnan
#  endif
#endif

namespace sl
{
    enum DecodeFlags {SimpleDecode = 0x00, GrayPatternDecode = 0x01, RobustDecode = 0x02};

    extern const float PIXEL_UNCERTAIN;
    extern const unsigned short BIT_UNCERTAIN;

    bool decode_pattern(const std::vector<std::string> & images, cv::Mat & pattern_image, cv::Mat & min_max_image, cv::Size const& projector_size,
                        unsigned flags = SimpleDecode, const cv::Mat & direct_light = cv::Mat(), unsigned m = 5);
    unsigned short get_robust_bit(unsigned value1, unsigned value2, unsigned Ld, unsigned Lg, unsigned m);
    void convert_pattern(cv::Mat & pattern_image, cv::Size const& projector_size, const int offset[2], bool binary);
    cv::Mat estimate_direct_light(const std::vector<cv::Mat> & images, float b);

    cv::Mat get_gray_image(const std::string & filename);
    static inline bool INVALID(float value) {return _isnan(value)>0;}
    static inline bool INVALID(const cv::Vec2f & pt) {return _isnan(pt[0]) || _isnan(pt[1]);}
    static inline bool INVALID(const cv::Vec3f & pt) {return _isnan(pt[0]) || _isnan(pt[1]) || _isnan(pt[2]);}

    int binaryToGray(int value);
    inline int binaryToGray(int value, unsigned offset);
    inline int grayToBinary(int value, unsigned offset);

    cv::Mat colorize_pattern(const cv::Mat & pattern_image, unsigned set, float max_value);
};

#endif //__STRUCTURED_LIGHT_HPP__
