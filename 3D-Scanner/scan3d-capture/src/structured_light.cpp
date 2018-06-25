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

#include "structured_light.hpp"

#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace sl
{
    const float PIXEL_UNCERTAIN = std::numeric_limits<float>::quiet_NaN();
    const unsigned short BIT_UNCERTAIN = 0xffff;
};

bool sl::decode_pattern(const std::vector<std::string> & images, cv::Mat & pattern_image, cv::Mat & min_max_image, cv::Size const& projector_size, unsigned flags, const cv::Mat & direct_light, unsigned m)
{
    bool binary   = (flags & GrayPatternDecode)!=GrayPatternDecode;
    bool robust   = (flags & RobustDecode)==RobustDecode;

    std::cout << " --- decode_pattern START ---\n";

    //delete previous data
    pattern_image = cv::Mat();
    min_max_image = cv::Mat();
    bool init = true;

    std::cout << "Decode: " << (binary?"Binary ":"Gray ")
                            << (robust?"Robust ":"") 
                            << std::endl;

    int total_images = static_cast<int>(images.size());
    int total_patterns = total_images/2 - 1;
    int total_bits = total_patterns/2;
    if (2+4*total_bits!=total_images)
    {   //error
        std::cout << "[sl::decode_pattern] ERROR: cannot detect pattern and bit count from image set.\n";
        return false;
    }

    const unsigned bit_count[] = {0, static_cast<unsigned>(total_bits), static_cast<unsigned>(total_bits)};  //pattern bits
    const unsigned set_size[]  = {1, static_cast<unsigned>(total_bits), static_cast<unsigned>(total_bits)};  //number of image pairs
    const unsigned COUNT = 2*(set_size[0]+set_size[1]+set_size[2]); //total image count
    const int pattern_offset[2] = {((1<<total_bits)-projector_size.width)/2, ((1<<total_bits)-projector_size.height)/2};

    if (images.size()<COUNT)
    {   //error
        std::cout << "Image list size does not match set size, please supply exactly " << COUNT << " image names.\n";
        return false;
    }

    //load every image pair and compute the maximum, minimum, and bit code
    unsigned set = 0;
    unsigned current = 0;
    for (unsigned t=0; t<COUNT; t+=2, current++)
    {
        if (current==set_size[set])
        {
            set++;
            current = 0;
        }

        if (set==0)
        {   //skip
            continue;
        }

        unsigned bit = bit_count[set] - current - 1; //current bit: from 0 to (bit_count[set]-1)
        unsigned channel = set - 1;

        //load images
        const cv::Mat & gray_image1 = get_gray_image(images.at(t+0));
        if (gray_image1.rows<1)
        {
            std::cout << "Failed to load " << images.at(t+0) << std::endl;
            return false;
        }
        const cv::Mat & gray_image2 = get_gray_image(images.at(t+1));
        if (gray_image2.rows<1)
        {
            std::cout << "Failed to load " << images.at(t+1) << std::endl;
            return false;
        }

        //initialize data structures
        if (init)
        {
            //sanity check
            if (gray_image1.size()!=gray_image2.size())
            {   //different size
                std::cout << " --> Initial images have different size: \n";
                return false;
            }
            if (robust && gray_image1.size()!=direct_light.size())
            {   //different size
                std::cout << " --> Direct Component image has different size: \n";
                return false;
            }
            pattern_image = cv::Mat(gray_image1.size(), CV_32FC2);
            min_max_image = cv::Mat(gray_image1.size(), CV_8UC2);
        }

        //sanity check
        if (gray_image1.size()!=pattern_image.size())
        {   //different size
            std::cout << " --> Image 1 has different size, image pair " << t << " (skipped!)\n";
            continue;
        }
        if (gray_image2.size()!=pattern_image.size())
        {   //different size
            std::cout << " --> Image 2 has different size, image pair " << t << " (skipped!)\n";
            continue;
        }

        //compare
        for (int h=0; h<pattern_image.rows; h++)
        {
            const unsigned char * row1 = gray_image1.ptr<unsigned char>(h);
            const unsigned char * row2 = gray_image2.ptr<unsigned char>(h);
            const cv::Vec2b * row_light = (robust ? direct_light.ptr<cv::Vec2b>(h) : NULL);
            cv::Vec2f * pattern_row = pattern_image.ptr<cv::Vec2f>(h);
            cv::Vec2b * min_max_row = min_max_image.ptr<cv::Vec2b>(h);

            for (int w=0; w<pattern_image.cols; w++)
            {
                cv::Vec2f & pattern = pattern_row[w];
                cv::Vec2b & min_max = min_max_row[w];
                unsigned char value1 = row1[w];
                unsigned char value2 = row2[w];

                if (init)
                {
                    pattern[0] = 0.f; //vertical
                    pattern[1] = 0.f; //horizontal
                }

                //min/max
                if (init || value1<min_max[0] || value2<min_max[0])
                {
                    min_max[0] = (value1<value2?value1:value2);
                }
                if (init || value1>min_max[1] || value2>min_max[1])
                {
                    min_max[1] = (value1>value2?value1:value2);
                }
                
                if (!robust)
                {   // [simple] pattern bit assignment
                    if (value1>value2)
                    {   //set bit n to 1
                        pattern[channel] += (1<<bit);
                    }
                }
                else
                {   // [robust] pattern bit assignment
                    if (row_light && (init || pattern[channel]!=PIXEL_UNCERTAIN))
                    {
                        const cv::Vec2b & L = row_light[w];
                        unsigned short p = get_robust_bit(value1, value2, L[0], L[1], m);
                        if (p==BIT_UNCERTAIN)
                        {
                            pattern[channel] = PIXEL_UNCERTAIN;
                        }
                        else
                        {
                            pattern[channel] += (p<<bit);
                        }
                    }
                }

            }   //for each column
        }   //for each row

        init = false;
    }   //for all image pairs

    if (!binary)
    {   //not binary... it must be gray code
        convert_pattern(pattern_image, projector_size, pattern_offset, binary);
    }

    std::cout << " --- decode_pattern END ---\n";

    return true;
}

unsigned short sl::get_robust_bit(unsigned value1, unsigned value2, unsigned Ld, unsigned Lg, unsigned m)
{
    if (Ld < m)
    {
        return BIT_UNCERTAIN;
    }
    if (Ld>Lg)
    {
        return (value1>value2 ? 1 : 0);
    }
    if (value1<=Ld && value2>=Lg)
    {
        return 0;
    }
    if (value1>=Lg && value2<=Ld)
    {
        return 1;
    }
    return BIT_UNCERTAIN;
}

void sl::convert_pattern(cv::Mat & pattern_image, cv::Size const& projector_size, const int offset[2], bool binary)
{
    if (pattern_image.rows==0)
    {   //no pattern image
        return;
    }
    if (pattern_image.type()!=CV_32FC2)
    {
        return;
    }

    if (binary)
    {
       std::cout << "Converting binary code to gray\n";
    }
    else
    {
        std::cout << "Converting gray code to binary\n";
    }

    for (int h=0; h<pattern_image.rows; h++)
    {
        cv::Vec2f * pattern_row = pattern_image.ptr<cv::Vec2f>(h);
        for (int w=0; w<pattern_image.cols; w++)
        {
            cv::Vec2f & pattern = pattern_row[w];
            if (binary)
            {
                if (!INVALID(pattern[0]))
                {
                    int p = static_cast<int>(pattern[0]);
                    pattern[0] = binaryToGray(p, offset[0]) + (pattern[0] - p);
                }
                if (!INVALID(pattern[1]))
                {
                    int p = static_cast<int>(pattern[1]);
                    pattern[1] = binaryToGray(p, offset[1]) + (pattern[1] - p);
                }
            }
            else
            {
                if (!INVALID(pattern[0]))
                {
                    int p = static_cast<int>(pattern[0]);
                    int code = grayToBinary(p, offset[0]);

                    if (code<0) {code = 0;}
                    else if (code>=projector_size.width) {code = projector_size.width - 1;}

                    pattern[0] = code + (pattern[0] - p);
                }
                if (!INVALID(pattern[1]))
                {
                    int p = static_cast<int>(pattern[1]);
                    int code = grayToBinary(p, offset[1]);

                    if (code<0) {code = 0;}
                    else if (code>=projector_size.height) {code = projector_size.height - 1;}

                    pattern[1] = code + (pattern[1] - p);
                }
            }
        }
    }
}

cv::Mat sl::estimate_direct_light(const std::vector<cv::Mat> & images, float b)
{
    static const unsigned COUNT = 10; // max number of images

    unsigned count = static_cast<int>(images.size());
    if (count<1)
    {   //no images
        return cv::Mat();
    }
    
    std::cout << " --- estimate_direct_light START ---\n";

    if (count>COUNT)
    {
        count = COUNT;
        std::cout << "WARNING: Using only " << COUNT << " of " << count << std::endl;
    }

    for (unsigned i=0; i<count; i++)
    {
        if (images.at(i).type()!=CV_8UC1)
        {   //error
            std::cout << "Gray images required\n";
            return cv::Mat();
        }
    }

    cv::Size size = images.at(0).size();

    //initialize direct light image
    cv::Mat direct_light(size, CV_8UC2);

    double b1 = 1.0/(1.0 - b);
    double b2 = 2.0/(1.0 - b*1.0*b);

    for (unsigned h=0; static_cast<int>(h)<size.height; h++)
    {
        unsigned char const* row[COUNT];
        for (unsigned i=0; i<count; i++)
        {
            row[i] = images.at(i).ptr<unsigned char>(h);
        }
        cv::Vec2b * row_light = direct_light.ptr<cv::Vec2b>(h);

        for (unsigned w=0; static_cast<int>(w)<size.width; w++)
        {
            unsigned Lmax = row[0][w];
            unsigned Lmin = row[0][w];
            for (unsigned i=0; i<count; i++)
            {
                if (Lmax<row[i][w]) Lmax = row[i][w];
                if (Lmin>row[i][w]) Lmin = row[i][w];
            }

            int Ld = static_cast<int>(b1*(Lmax - Lmin) + 0.5);
            int Lg = static_cast<int>(b2*(Lmin - b*Lmax) + 0.5);
            row_light[w][0] = (Lg>0 ? static_cast<unsigned>(Ld) : Lmax);
            row_light[w][1] = (Lg>0 ? static_cast<unsigned>(Lg) : 0);

            //std::cout << "Ld=" << (int)row_light[w][0] << " iTotal=" <<(int) row_light[w][1] << std::endl;
        }
    }

    std::cout << " --- estimate_direct_light END ---\n";

    return direct_light;
}

cv::Mat sl::get_gray_image(const std::string & filename)
{
    //load image
    cv::Mat rgb_image = cv::imread(filename);
    if (rgb_image.rows>0 && rgb_image.cols>0)
    {
        //gray scale
        cv::Mat gray_image;
        cvtColor(rgb_image, gray_image, CV_BGR2GRAY);
        return gray_image;
    }
    return cv::Mat();
}

/*      From Wikipedia: http://en.wikipedia.org/wiki/Gray_code
        The purpose of this function is to convert an unsigned
        binary number to reflected binary Gray code.
*/
static unsigned util_binaryToGray(unsigned num)
{
        return (num>>1) ^ num;
}
 
/*      From Wikipedia: http://en.wikipedia.org/wiki/Gray_code
        The purpose of this function is to convert a reflected binary
        Gray code number to a binary number.
*/
static unsigned util_grayToBinary(unsigned num, unsigned numBits)
{
    for (unsigned shift = 1; shift < numBits; shift <<= 1)
    {
        num ^= num >> shift;
    }
    return num;
}

int sl::binaryToGray(int value) {return util_binaryToGray(value);}

inline int sl::binaryToGray(int value, unsigned offset) {return util_binaryToGray(value + offset);}
inline int sl::grayToBinary(int value, unsigned offset) {return (util_grayToBinary(value, 32) - offset);}

cv::Mat sl::colorize_pattern(const cv::Mat & pattern_image, unsigned set, float max_value)
{
    if (pattern_image.rows==0)
    {   //empty image
        return cv::Mat();
    }
    if (pattern_image.type()!=CV_32FC2)
    {   //invalid image type
        return cv::Mat();
    }
    if (set!=0 && set!=1)
    {
        return cv::Mat();
    }

    cv::Mat image(pattern_image.size(), CV_8UC3);

    float max_t = max_value;
    float n = 4.f;
    float dt = 255.f/n;
    for (int h=0; h<pattern_image.rows; h++)
    {
        const cv::Vec2f * row1 = pattern_image.ptr<cv::Vec2f>(h);
        cv::Vec3b * row2 = image.ptr<cv::Vec3b>(h);
        for (int w=0; w<pattern_image.cols; w++)
        {
            if (row1[w][set]>max_value || INVALID(row1[w][set]))
            {   //invalid value: use grey
                row2[w] = cv::Vec3b(128, 128, 128);
                continue;
            }
            //display
            float t = row1[w][set]*255.f/max_t;
            float c1 = 0.f, c2 = 0.f, c3 = 0.f;
            if (t<=1.f*dt)
            {   //black -> red
                float c = n*(t-0.f*dt);
                c1 = c;     //0-255
                c2 = 0.f;   //0
                c3 = 0.f;   //0
            }
            else if (t<=2.f*dt)
            {   //red -> red,green
                float c = n*(t-1.f*dt);
                c1 = 255.f; //255
                c2 = c;     //0-255
                c3 = 0.f;   //0
            }
            else if (t<=3.f*dt)
            {   //red,green -> green
                float c = n*(t-2.f*dt);
                c1 = 255.f-c;   //255-0
                c2 = 255.f;     //255
                c3 = 0.f;       //0
            }
            else if (t<=4.f*dt)
            {   //green -> blue
                float c = n*(t-3.f*dt);
                c1 = 0.f;       //0
                c2 = 255.f-c;   //255-0
                c3 = c;         //0-255
            }
            row2[w] = cv::Vec3b(static_cast<uchar>(c3), static_cast<uchar>(c2), static_cast<uchar>(c1));
        }
    }
    return image;
}
