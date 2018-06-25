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

#ifndef __SCAN3D_HPP__
#define __SCAN3D_HPP__

#include <QWidget>
#include <QString>
#include <opencv2/core/core.hpp>

#ifndef _MSC_VER
#  ifndef _isnan
#    include <math.h>
#    define _isnan std::isnan
#  endif
#endif

#include "CalibrationData.hpp"

namespace scan3d
{
    class Pointcloud
    {
    public:
        void clear(void);
        void init_points(int rows, int cols);
        void init_color(int rows, int cols);
        void init_normals(int rows, int cols);

        //data
        cv::Mat points;
        cv::Mat colors;
        cv::Mat normals;
    };

    void reconstruct_model(Pointcloud & pointcloud, CalibrationData const& calib, 
            cv::Mat const& pattern_image, cv::Mat const& min_max_image, cv::Mat const& color_image,
            cv::Size const& projector_size, int threshold, double max_dist, QWidget * parent_widget = NULL);

    void reconstruct_model_simple(Pointcloud & pointcloud, CalibrationData const& calib, 
            cv::Mat const& pattern_image, cv::Mat const& min_max_image, cv::Mat const& color_image,
            cv::Size const& projector_size, int threshold, double max_dist, QWidget * parent_widget = NULL);

    void reconstruct_model_patch_center(Pointcloud & pointcloud, CalibrationData const& calib, 
            cv::Mat const& pattern_image, cv::Mat const& min_max_image, cv::Mat const& color_image,
            cv::Size const& projector_size, int threshold, double max_dist, QWidget * parent_widget = NULL);

    void triangulate_stereo(const cv::Mat & K1, const cv::Mat & kc1, const cv::Mat & K2, const cv::Mat & kc2, 
                            const cv::Mat & Rt, const cv::Mat & T, const cv::Point2d & p1, const cv::Point2d & p2, 
                            cv::Point3d & p3d, double * distance = NULL);

    cv::Point3d approximate_ray_intersection(const cv::Point3d & v1, const cv::Point3d & q1,
                                        const cv::Point3d & v2, const cv::Point3d & q2,
                                        double * distance = NULL, double * out_lambda1 = NULL, double * out_lambda2 = NULL);

    void compute_normals(scan3d::Pointcloud & pointcloud);

    cv::Mat make_projector_view(cv::Mat const& pattern_image, cv::Mat const& min_max_image, cv::Mat const& color_image, 
                                        cv::Size const& projector_size, int threshold);
};

#endif //__SCAN3D_HPP__