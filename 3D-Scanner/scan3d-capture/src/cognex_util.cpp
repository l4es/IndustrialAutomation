/*
Copyright (c) 2014, Daniel Moreno and Gabriel Taubin
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

#include "cognex_util.hpp"

#include <ch_cvl/calibftr.h>

bool cognex::extract_corners(cv::Mat const& image, 
                                std::vector<cv::Point2f> & cam_corners,
                                std::vector<cv::Point3f> & world_corners)
{
    ccPelBuffer<c_UInt8> srcImg(new ccPelRoot<c_UInt8>(image.cols, image.rows, image.step[0], image.data));

    //c_Int32 width = image.cols;
    //c_Int32 height = image.rows;
    //ccPelBuffer<c_UInt8> srcImg(width, height);
    //srcImg.copyFromBuffer(image.data);
    
    ccCalib2VertexFeatureParams params;
        //params.algorithm(ccCalib2VertexFeatureDefs::eExhaustive);
    ccCrspPairVector resultPairs;
    try
    {
        cfCalib2VertexFeatureExtract(srcImg, params, resultPairs);
    }
    catch (ccException &exc)
    {
        std::cout << "cfCalib2VertexFeatureExtract threw exception:" << std::endl;
        std::cout << exc.message() << std::endl;

        /*
            ccPel::UnboundWindow
            ccCalib2VertexFeatureDefs::FailedCorrespondence
            ccCalib2VertexFeatureDefs::FiducialNotFound
            ccCalib2VertexFeatureDefs::eUseFiducial
            ccCalib2VertexFeatureDefs::BadParam
        */

        srcImg.setUnbound();  //if removed, pixel data is deleted
        return false;
    }

    srcImg.setUnbound();  //if removed, pixel data is deleted

    //delete old points
    cam_corners.clear();
    world_corners.clear();

    for (auto it=resultPairs.cbegin(); it!=resultPairs.end(); ++it)
    {
        cam_corners.push_back(cv::Point2f(static_cast<float>(it->first.x()), static_cast<float>(it->first.y())));
        world_corners.push_back(cv::Point3f(static_cast<float>(it->second.x()), static_cast<float>(it->second.y()), 0.f));
    }

    return true;
}
