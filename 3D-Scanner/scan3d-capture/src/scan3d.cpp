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

#include "scan3d.hpp"

#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <QApplication>
#include <QProgressDialog>
#include <QMap>

#include "structured_light.hpp"

void scan3d::Pointcloud::clear(void)
{
    points = cv::Mat();
    colors = cv::Mat();
    normals = cv::Mat();
}

void scan3d::Pointcloud::init_points(int rows, int cols)
{
    points = cv::Mat(rows, cols, CV_32FC3);
    size_t total = points.total()*points.channels();
    float * data = points.ptr<float>(0);
    for (size_t i=0; i<total; i++)
    {
        data[i] = std::numeric_limits<float>::quiet_NaN();
    }
}

void scan3d::Pointcloud::init_color(int rows, int cols)
{
    colors = cv::Mat::zeros(rows, cols, CV_8UC3);
    memset(colors.data, 255, colors.total()*colors.channels()); //white
}

void scan3d::Pointcloud::init_normals(int rows, int cols)
{
    normals = cv::Mat(rows, cols, CV_32FC3);
    size_t total = normals.total()*normals.channels();;
    float * data = normals.ptr<float>(0);
    for (size_t i=0; i<total; i++)
    {
        data[i] = std::numeric_limits<float>::quiet_NaN();
    }
}

void scan3d::reconstruct_model(Pointcloud & pointcloud, CalibrationData const& calib, 
                                cv::Mat const& pattern_image, cv::Mat const& min_max_image, cv::Mat const& color_image,
                                cv::Size const& projector_size, int threshold, double max_dist, QWidget * parent_widget)
{
    reconstruct_model_patch_center(pointcloud, calib, pattern_image, min_max_image, color_image, projector_size, 
                                    threshold, max_dist, parent_widget);
}

void scan3d::reconstruct_model_simple(Pointcloud & pointcloud, CalibrationData const& calib, 
                                cv::Mat const& pattern_image, cv::Mat const& min_max_image, cv::Mat const& color_image,
                                cv::Size const& projector_size, int threshold, double max_dist, QWidget * parent_widget)
{
    if (!pattern_image.data || pattern_image.type()!=CV_32FC2)
    {   //pattern not correctly decoded
        std::cerr << "[reconstruct_model] ERROR invalid pattern_image\n";
        return;
    }
    if (!min_max_image.data || min_max_image.type()!=CV_8UC2)
    {   //pattern not correctly decoded
        std::cerr << "[reconstruct_model] ERROR invalid min_max_image\n";
        return;
    }
    if (color_image.data && color_image.type()!=CV_8UC3)
    {   //not standard RGB image
        std::cerr << "[reconstruct_model] ERROR invalid color_image\n";
        return;
    }
    if (!calib.is_valid())
    {   //invalid calibration
        return;
    }

    //parameters
    //const unsigned threshold = config.value("main/shadow_threshold", 70).toUInt();
    //const double   max_dist  = config.value("main/max_dist_threshold", 40).toDouble();
    //const bool     remove_background = config.value("main/remove_background", true).toBool();
    //const double   plane_dist = config.value("main/plane_dist", 100.0).toDouble();
    double plane_dist = 100.0;

    /* background removal
    cv::Point2i plane_coord[3];
    plane_coord[0] = cv::Point2i(config.value("background_plane/x1").toUInt(), config.value("background_plane/y1").toUInt());
    plane_coord[1] = cv::Point2i(config.value("background_plane/x2").toUInt(), config.value("background_plane/y2").toUInt());
    plane_coord[2] = cv::Point2i(config.value("background_plane/x3").toUInt(), config.value("background_plane/y3").toUInt());

    if (plane_coord[0].x<=0 || plane_coord[0].x>=pattern_local.cols
        || plane_coord[0].y<=0 || plane_coord[0].y>=pattern_local.rows)
    {
        plane_coord[0] = cv::Point2i(50, 50);
        config.setValue("background_plane/x1", plane_coord[0].x);
        config.setValue("background_plane/y1", plane_coord[0].y);
    }
    if (plane_coord[1].x<=0 || plane_coord[1].x>=pattern_local.cols
        || plane_coord[1].y<=0 || plane_coord[1].y>=pattern_local.rows)
    {
        plane_coord[1] = cv::Point2i(50, pattern_local.rows-50);
        config.setValue("background_plane/x2", plane_coord[1].x);
        config.setValue("background_plane/y2", plane_coord[1].y);
    }
    if (plane_coord[2].x<=0 || plane_coord[2].x>=pattern_local.cols
        || plane_coord[2].y<=0 || plane_coord[2].y>=pattern_local.rows)
    {
        plane_coord[2] = cv::Point2i(pattern_local.cols-50, 50);
        config.setValue("background_plane/x3", plane_coord[2].x);
        config.setValue("background_plane/y3", plane_coord[2].y);
    }
    */

    //init point cloud
    int scale_factor = 1;
    int out_cols = pattern_image.cols/scale_factor;
    int out_rows = pattern_image.rows/scale_factor;
    pointcloud.clear();
    pointcloud.init_points(out_rows, out_cols);
    pointcloud.init_color(out_rows, out_cols);

    //progress
    QProgressDialog * progress = NULL;
    if (parent_widget)
    {
        progress = new QProgressDialog("Reconstruction in progress.", "Abort", 0, pattern_image.rows, parent_widget, 
                                        Qt::Dialog|Qt::CustomizeWindowHint|Qt::WindowCloseButtonHint);
        progress->setWindowModality(Qt::WindowModal);
        progress->setWindowTitle("Processing");
        progress->setMinimumWidth(400);
    }

    //take 3 points in back plane
    /*cv::Mat plane;
    if (remove_background)
    {
        cv::Point3d p[3];
        for (unsigned i=0; i<3;i++)
        {
            for (unsigned j=0; 
                j<10 && (
                    INVALID(pattern_local.at<cv::Vec2f>(plane_coord[i].y, plane_coord[i].x)[0])
                    || INVALID(pattern_local.at<cv::Vec2f>(plane_coord[i].y, plane_coord[i].x)[1])); j++)
            {
                plane_coord[i].x += 1.f;
            }
            const cv::Vec2f & pattern = pattern_local.at<cv::Vec2f>(plane_coord[i].y, plane_coord[i].x);

            const float col = pattern[0];
            const float row = pattern[1];

            if (projector_size.width<=static_cast<int>(col) || projector_size.height<=static_cast<int>(row))
            {   //abort
                continue;
            }

            //shoot a ray through the image: u=\lambda*v + q
            cv::Point3d u1 = camera.to_world_coord(plane_coord[i].x, plane_coord[i].y);
            cv::Point3d v1 = camera.world_ray(plane_coord[i].x, plane_coord[i].y);

            //shoot a ray through the projector: u=\lambda*v + q
            cv::Point3d u2 = projector.to_world_coord(col, row);
            cv::Point3d v2 = projector.world_ray(col, row);

            //compute ray-ray approximate intersection
            double distance = 0.0;
            p[i] = geometry::approximate_ray_intersection(v1, u1, v2, u2, &distance);
            std::cout << "Plane point " << i << " distance " << distance << std::endl;
        }
        plane = geometry::get_plane(p[0], p[1], p[2]);
        if (cv::Mat(plane.rowRange(0,3).t()*cv::Mat(cv::Point3d(p[0].x, p[0].y, p[0].z-1.0)) + plane.at<double>(3,0)).at<double>(0,0)
                <0.0)
        {
            plane = -1.0*plane;
        }
        std::cout << "Background plane: " << plane << std::endl;
    }
    */

    cv::Mat Rt = calib.R.t();

    unsigned good = 0;
    unsigned bad  = 0;
    unsigned invalid = 0;
    unsigned repeated = 0;
    for (int h=0; h<pattern_image.rows; h+=scale_factor)
    {
        if (progress && h%4==0)
        {
            progress->setValue(h);
            progress->setLabelText(QString("Reconstruction in progress: %1 good points/%2 bad points").arg(good).arg(bad));
            QApplication::instance()->processEvents();
        }
        if (progress && progress->wasCanceled())
        {   //abort
            pointcloud.clear();
            return;
        }

        register const cv::Vec2f * curr_pattern_row = pattern_image.ptr<cv::Vec2f>(h);
        register const cv::Vec2b * min_max_row = min_max_image.ptr<cv::Vec2b>(h);
        for (register int w=0; w<pattern_image.cols; w+=scale_factor)
        {
            double distance = max_dist;  //quality meassure
            cv::Point3d p;               //reconstructed point
            //cv::Point3d normal(0.0, 0.0, 0.0);

            const cv::Vec2f & pattern = curr_pattern_row[w];
            const cv::Vec2b & min_max = min_max_row[w];

            if (sl::INVALID(pattern) || pattern[0]<0.f || pattern[1]<0.f
                || (min_max[1]-min_max[0])<static_cast<int>(threshold))
            {   //skip
                invalid++;
                continue;
            }

            const float col = pattern[0];
            const float row = pattern[1];

            if (projector_size.width<=static_cast<int>(col) || projector_size.height<=static_cast<int>(row))
            {   //abort
                continue;
            }

            cv::Vec3f & cloud_point = pointcloud.points.at<cv::Vec3f>(h/scale_factor, w/scale_factor);
            if (!sl::INVALID(cloud_point[0]))
            {   //point already reconstructed!
                repeated++;
                continue;
            }

            //standard
            cv::Point2d p1(w, h);
            cv::Point2d p2(col, row);
            triangulate_stereo(calib.cam_K, calib.cam_kc, calib.proj_K, calib.proj_kc, Rt, calib.T, p1, p2, p, &distance);

            //save texture coordinates
            /*
            normal.x = static_cast<float>(w)/static_cast<float>(color_image.cols);
            normal.y = static_cast<float>(h)/static_cast<float>(color_image.rows);
            normal.z = 0;
            */

            if (distance < max_dist)
            {   //good point

                //evaluate the plane
                double d = plane_dist+1;
                /*if (remove_background)
                {
                    d = cv::Mat(plane.rowRange(0,3).t()*cv::Mat(p) + plane.at<double>(3,0)).at<double>(0,0);
                }*/
                if (d>plane_dist)
                {   //object point, keep
                    good++;

                    cloud_point[0] = p.x;
                    cloud_point[1] = p.y;
                    cloud_point[2] = p.z;

                    //normal
                    /*cpoint.normal_x = normal.x;
                    cpoint.normal_y = normal.y;
                    cpoint.normal_z = normal.z;*/

                    if (color_image.data)
                    {
                        const cv::Vec3b & vec = color_image.at<cv::Vec3b>(h, w);
                        cv::Vec3b & cloud_color = pointcloud.colors.at<cv::Vec3b>(h/scale_factor, w/scale_factor);
                        cloud_color[0] = vec[0];
                        cloud_color[1] = vec[1];
                        cloud_color[2] = vec[2];
                    }
                }
            }
            else
            {   //skip
                bad++;
                //std::cout << " d = " << distance << std::endl;
            }
        }   //for each column
    }   //for each row

    if (progress)
    {
        progress->setValue(pattern_image.rows);
        progress->close();
        delete progress;
        progress = NULL;
    }

    std::cout << "Reconstructed points[simple]: " << good << " (" << bad << " skipped, " << invalid << " invalid) " << std::endl
                << " - repeated points: " << repeated << " (ignored) " << std::endl;
}

void scan3d::reconstruct_model_patch_center(Pointcloud & pointcloud, CalibrationData const& calib, 
                                cv::Mat const& pattern_image, cv::Mat const& min_max_image, cv::Mat const& color_image,
                                cv::Size const& projector_size, int threshold, double max_dist, QWidget * parent_widget)
{
    if (!pattern_image.data || pattern_image.type()!=CV_32FC2)
    {   //pattern not correctly decoded
        std::cerr << "[reconstruct_model] ERROR invalid pattern_image\n";
        return;
    }
    if (!min_max_image.data || min_max_image.type()!=CV_8UC2)
    {   //pattern not correctly decoded
        std::cerr << "[reconstruct_model] ERROR invalid min_max_image\n";
        return;
    }
    if (color_image.data && color_image.type()!=CV_8UC3)
    {   //not standard RGB image
        std::cerr << "[reconstruct_model] ERROR invalid color_image\n";
        return;
    }
    if (!calib.is_valid())
    {   //invalid calibration
        return;
    }

    //parameters
    //const unsigned threshold = config.value("main/shadow_threshold", 70).toUInt();
    //const double   max_dist  = config.value("main/max_dist_threshold", 40).toDouble();
    //const bool     remove_background = config.value("main/remove_background", true).toBool();
    //const double   plane_dist = config.value("main/plane_dist", 100.0).toDouble();
    double plane_dist = 100.0;

    /* background removal
    cv::Point2i plane_coord[3];
    plane_coord[0] = cv::Point2i(config.value("background_plane/x1").toUInt(), config.value("background_plane/y1").toUInt());
    plane_coord[1] = cv::Point2i(config.value("background_plane/x2").toUInt(), config.value("background_plane/y2").toUInt());
    plane_coord[2] = cv::Point2i(config.value("background_plane/x3").toUInt(), config.value("background_plane/y3").toUInt());

    if (plane_coord[0].x<=0 || plane_coord[0].x>=pattern_local.cols
        || plane_coord[0].y<=0 || plane_coord[0].y>=pattern_local.rows)
    {
        plane_coord[0] = cv::Point2i(50, 50);
        config.setValue("background_plane/x1", plane_coord[0].x);
        config.setValue("background_plane/y1", plane_coord[0].y);
    }
    if (plane_coord[1].x<=0 || plane_coord[1].x>=pattern_local.cols
        || plane_coord[1].y<=0 || plane_coord[1].y>=pattern_local.rows)
    {
        plane_coord[1] = cv::Point2i(50, pattern_local.rows-50);
        config.setValue("background_plane/x2", plane_coord[1].x);
        config.setValue("background_plane/y2", plane_coord[1].y);
    }
    if (plane_coord[2].x<=0 || plane_coord[2].x>=pattern_local.cols
        || plane_coord[2].y<=0 || plane_coord[2].y>=pattern_local.rows)
    {
        plane_coord[2] = cv::Point2i(pattern_local.cols-50, 50);
        config.setValue("background_plane/x3", plane_coord[2].x);
        config.setValue("background_plane/y3", plane_coord[2].y);
    }
    */

    //init point cloud
    int scale_factor_x = 1;
    int scale_factor_y = (projector_size.width>projector_size.height ? 1 : 2); //XXX HACK: preserve regular aspect ratio XXX HACK
    int out_cols = projector_size.width/scale_factor_x;
    int out_rows = projector_size.height/scale_factor_y;
    pointcloud.clear();
    pointcloud.init_points(out_rows, out_cols);
    pointcloud.init_color(out_rows, out_cols);

    //progress
    QProgressDialog * progress = NULL;
    if (parent_widget)
    {
        progress = new QProgressDialog("Reconstruction in progress.", "Abort", 0, pattern_image.rows, parent_widget, 
                                        Qt::Dialog|Qt::CustomizeWindowHint|Qt::WindowCloseButtonHint);
        progress->setWindowModality(Qt::WindowModal);
        progress->setWindowTitle("Processing");
        progress->setMinimumWidth(400);
        progress->show();
    }

    //take 3 points in back plane
    /*cv::Mat plane;
    if (remove_background)
    {
        cv::Point3d p[3];
        for (unsigned i=0; i<3;i++)
        {
            for (unsigned j=0; 
                j<10 && (
                    INVALID(pattern_local.at<cv::Vec2f>(plane_coord[i].y, plane_coord[i].x)[0])
                    || INVALID(pattern_local.at<cv::Vec2f>(plane_coord[i].y, plane_coord[i].x)[1])); j++)
            {
                plane_coord[i].x += 1.f;
            }
            const cv::Vec2f & pattern = pattern_local.at<cv::Vec2f>(plane_coord[i].y, plane_coord[i].x);

            const float col = pattern[0];
            const float row = pattern[1];

            if (projector_size.width<=static_cast<int>(col) || projector_size.height<=static_cast<int>(row))
            {   //abort
                continue;
            }

            //shoot a ray through the image: u=\lambda*v + q
            cv::Point3d u1 = camera.to_world_coord(plane_coord[i].x, plane_coord[i].y);
            cv::Point3d v1 = camera.world_ray(plane_coord[i].x, plane_coord[i].y);

            //shoot a ray through the projector: u=\lambda*v + q
            cv::Point3d u2 = projector.to_world_coord(col, row);
            cv::Point3d v2 = projector.world_ray(col, row);

            //compute ray-ray approximate intersection
            double distance = 0.0;
            p[i] = geometry::approximate_ray_intersection(v1, u1, v2, u2, &distance);
            std::cout << "Plane point " << i << " distance " << distance << std::endl;
        }
        plane = geometry::get_plane(p[0], p[1], p[2]);
        if (cv::Mat(plane.rowRange(0,3).t()*cv::Mat(cv::Point3d(p[0].x, p[0].y, p[0].z-1.0)) + plane.at<double>(3,0)).at<double>(0,0)
                <0.0)
        {
            plane = -1.0*plane;
        }
        std::cout << "Background plane: " << plane << std::endl;
    }
    */

    //candidate points
    QMap<unsigned, cv::Point2f> proj_points;
    QMap<unsigned, std::vector<cv::Point2f> > cam_points;

    //cv::Mat proj_image = cv::Mat::zeros(out_rows, out_cols, CV_8UC3);

    unsigned good = 0;
    unsigned bad  = 0;
    unsigned invalid = 0;
    unsigned repeated = 0;
    for (int h=0; h<pattern_image.rows; h++)
    {
        if (progress && h%4==0)
        {
            progress->setValue(h);
            progress->setLabelText(QString("Reconstruction in progress: collecting points"));
            QApplication::instance()->processEvents();
        }
        if (progress && progress->wasCanceled())
        {   //abort
            pointcloud.clear();
            return;
        }

        register const cv::Vec2f * curr_pattern_row = pattern_image.ptr<cv::Vec2f>(h);
        register const cv::Vec2b * min_max_row = min_max_image.ptr<cv::Vec2b>(h);
        for (register int w=0; w<pattern_image.cols; w++)
        {
            const cv::Vec2f & pattern = curr_pattern_row[w];
            const cv::Vec2b & min_max = min_max_row[w];

            if (sl::INVALID(pattern) 
                || pattern[0]<0.f || pattern[0]>=projector_size.width || pattern[1]<0.f || pattern[1]>=projector_size.height
                || (min_max[1]-min_max[0])<static_cast<int>(threshold))
            {   //skip
                continue;
            }

            //ok
            cv::Point2f proj_point(pattern[0]/scale_factor_x, pattern[1]/scale_factor_y);
            unsigned index = static_cast<unsigned>(proj_point.y)*out_cols + static_cast<unsigned>(proj_point.x);
            proj_points.insert(index, proj_point);
            cam_points[index].push_back(cv::Point2f(w, h));

            //proj_image.at<cv::Vec3b>(static_cast<unsigned>(proj_point.y), static_cast<unsigned>(proj_point.x)) = color_image.at<cv::Vec3b>(h, w);
        }
    }

    //cv::imwrite("proj_image.png", proj_image);
    
    if (progress)
    {
        progress->setValue(pattern_image.rows);
    }

    cv::Mat Rt = calib.R.t();

    if (progress)
    {
        progress->setMaximum(proj_points.size());
    }

    QMapIterator<unsigned, cv::Point2f> iter1(proj_points);
    unsigned n = 0;
    while (iter1.hasNext()) 
    {
        n++;
        if (progress && n%1000==0)
        {
            progress->setValue(n);
            progress->setLabelText(QString("Reconstruction in progress: %1 good points/%2 bad points").arg(good).arg(bad));
            QApplication::instance()->processEvents();
        }
        if (progress && progress->wasCanceled())
        {   //abort
            pointcloud.clear();
            return;
        }

        iter1.next();
        unsigned index = iter1.key();
        const cv::Point2f & proj_point = iter1.value();
        const std::vector<cv::Point2f> & cam_point_list = cam_points.value(index);
        const unsigned count = static_cast<int>(cam_point_list.size());

        if (!count)
        {   //empty list
            continue;
        }

        //center average
        cv::Point2d sum(0.0, 0.0), sum2(0.0, 0.0);
        for (std::vector<cv::Point2f>::const_iterator iter2=cam_point_list.begin(); iter2!=cam_point_list.end(); iter2++)
        {
            sum.x += iter2->x;
            sum.y += iter2->y;
            sum2.x += (iter2->x)*(iter2->x);
            sum2.y += (iter2->y)*(iter2->y);
        }
        cv::Point2d cam(sum.x/count, sum.y/count);
        cv::Point2d proj(proj_point.x*scale_factor_x, proj_point.y*scale_factor_y);

        //triangulate
        double distance = max_dist;  //quality meassure
        cv::Point3d p;          //reconstructed point
        triangulate_stereo(calib.cam_K, calib.cam_kc, calib.proj_K, calib.proj_kc, Rt, calib.T, cam, proj, p, &distance);

        if (distance < max_dist)
        {   //good point

            //evaluate the plane
            double d = plane_dist+1;
            /*if (remove_background)
            {
                d = cv::Mat(plane.rowRange(0,3).t()*cv::Mat(p) + plane.at<double>(3,0)).at<double>(0,0);
            }*/
            if (d>plane_dist)
            {   //object point, keep
                good++;

                cv::Vec3f & cloud_point = pointcloud.points.at<cv::Vec3f>(proj_point.y, proj_point.x);
                cloud_point[0] = p.x;
                cloud_point[1] = p.y;
                cloud_point[2] = p.z;

                if (color_image.data)
                {
                    const cv::Vec3b & vec = color_image.at<cv::Vec3b>(static_cast<unsigned>(cam.y), static_cast<unsigned>(cam.x));
                    cv::Vec3b & cloud_color = pointcloud.colors.at<cv::Vec3b>(proj_point.y, proj_point.x);
                    cloud_color[0] = vec[0];
                    cloud_color[1] = vec[1];
                    cloud_color[2] = vec[2];
                }
            }
        }
        else
        {   //skip
            bad++;
            //std::cout << " d = " << distance << std::endl;
        }
    }   //while

    if (progress)
    {
        progress->setValue(proj_points.size());
        progress->close();
        delete progress;
        progress = NULL;
    }

    std::cout << "Reconstructed points [patch center]: " << good << " (" << bad << " skipped, " << invalid << " invalid) " << std::endl
                << " - repeated points: " << repeated << " (ignored) " << std::endl;
}

void scan3d::triangulate_stereo(const cv::Mat & K1, const cv::Mat & kc1, const cv::Mat & K2, const cv::Mat & kc2, 
                                  const cv::Mat & Rt, const cv::Mat & T, const cv::Point2d & p1, const cv::Point2d & p2, 
                                  cv::Point3d & p3d, double * distance)
{
    //to image camera coordinates
    cv::Mat inp1(1, 1, CV_64FC2), inp2(1, 1, CV_64FC2);
    inp1.at<cv::Vec2d>(0, 0) = cv::Vec2d(p1.x, p1.y);
    inp2.at<cv::Vec2d>(0, 0) = cv::Vec2d(p2.x, p2.y);
    cv::Mat outp1, outp2;
    cv::undistortPoints(inp1, outp1, K1, kc1);
    cv::undistortPoints(inp2, outp2, K2, kc2);
    assert(outp1.type()==CV_64FC2 && outp1.rows==1 && outp1.cols==1);
    assert(outp2.type()==CV_64FC2 && outp2.rows==1 && outp2.cols==1);
    const cv::Vec2d & outvec1 = outp1.at<cv::Vec2d>(0,0);
    const cv::Vec2d & outvec2 = outp2.at<cv::Vec2d>(0,0);
    cv::Point3d u1(outvec1[0], outvec1[1], 1.0);
    cv::Point3d u2(outvec2[0], outvec2[1], 1.0);

    //to world coordinates
    cv::Point3d w1 = u1;
    cv::Point3d w2 = cv::Point3d(cv::Mat(Rt*(cv::Mat(u2) - T)));

    //world rays
    cv::Point3d v1 = w1;
    cv::Point3d v2 = cv::Point3d(cv::Mat(Rt*cv::Mat(u2)));

    //compute ray-ray approximate intersection
    p3d = approximate_ray_intersection(v1, w1, v2, w2, distance);
}

cv::Point3d scan3d::approximate_ray_intersection(const cv::Point3d & v1, const cv::Point3d & q1,
                                                    const cv::Point3d & v2, const cv::Point3d & q2,
                                                    double * distance, double * out_lambda1, double * out_lambda2)
{
    cv::Mat v1mat = cv::Mat(v1);
    cv::Mat v2mat = cv::Mat(v2);
    
    double v1tv1 = cv::Mat(v1mat.t()*v1mat).at<double>(0,0);
    double v2tv2 = cv::Mat(v2mat.t()*v2mat).at<double>(0,0);
    double v1tv2 = cv::Mat(v1mat.t()*v2mat).at<double>(0,0);
    double v2tv1 = cv::Mat(v2mat.t()*v1mat).at<double>(0,0);

    //cv::Mat V(2, 2, CV_64FC1);
    //V.at<double>(0,0) = v1tv1;  V.at<double>(0,1) = -v1tv2;
    //V.at<double>(1,0) = -v2tv1; V.at<double>(1,1) = v2tv2;
    //std::cout << " V: "<< V << std::endl;

    cv::Mat Vinv(2, 2, CV_64FC1);
    double detV = v1tv1*v2tv2 - v1tv2*v2tv1;
    Vinv.at<double>(0,0) = v2tv2/detV;  Vinv.at<double>(0,1) = v1tv2/detV;
    Vinv.at<double>(1,0) = v2tv1/detV; Vinv.at<double>(1,1) = v1tv1/detV;
    //std::cout << " V.inv(): "<< V.inv() << std::endl << " Vinv: " << Vinv << std::endl;

    //cv::Mat Q(2, 1, CV_64FC1);
    //Q.at<double>(0,0) = cv::Mat(v1mat.t()*(cv::Mat(q2-q1))).at<double>(0,0);
    //Q.at<double>(1,0) = cv::Mat(v2mat.t()*(cv::Mat(q1-q2))).at<double>(0,0);
    //std::cout << " Q: "<< Q << std::endl;

    cv::Point3d q2_q1 = q2 - q1;
    double Q1 = v1.x*q2_q1.x + v1.y*q2_q1.y + v1.z*q2_q1.z;
    double Q2 = -(v2.x*q2_q1.x + v2.y*q2_q1.y + v2.z*q2_q1.z);

    //cv::Mat L = V.inv()*Q;
    //cv::Mat L = Vinv*Q;
    //std::cout << " L: "<< L << std::endl;
    
    double lambda1 = (v2tv2 * Q1 + v1tv2 * Q2) /detV;
    double lambda2 = (v2tv1 * Q1 + v1tv1 * Q2) /detV;
    //std::cout << "lambda1: " << lambda1 << " lambda2: " << lambda2 << std::endl;

    //cv::Mat p1 = L.at<double>(0,0)*v1mat + cv::Mat(q1); //ray1
    //cv::Mat p2 = L.at<double>(1,0)*v2mat + cv::Mat(q2); //ray2
    //cv::Point3d p1 = L.at<double>(0,0)*v1 + q1; //ray1
    //cv::Point3d p2 = L.at<double>(1,0)*v2 + q2; //ray2
    cv::Point3d p1 = lambda1*v1 + q1; //ray1
    cv::Point3d p2 = lambda2*v2 + q2; //ray2

    //cv::Point3d p = cv::Point3d(cv::Mat((p1+p2)/2.0));
    cv::Point3d p = 0.5*(p1+p2);

    if (distance!=NULL)
    {
        *distance = cv::norm(p2-p1);
    }
    if (out_lambda1)
    {
        *out_lambda1 = lambda1;
    }
    if (out_lambda2)
    {
        *out_lambda2 = lambda2;
    }

    return p;
}

void scan3d::compute_normals(scan3d::Pointcloud & pointcloud)
{
    if (!pointcloud.points.data)
    {
        return;
    }

    pointcloud.init_normals(pointcloud.points.rows, pointcloud.points.cols);

    for (int h=1; h+1<pointcloud.points.rows; h++)
    {
        const cv::Vec3f * points_row0 = pointcloud.points.ptr<cv::Vec3f>(h-1);
        const cv::Vec3f * points_row1 = pointcloud.points.ptr<cv::Vec3f>(h);
        const cv::Vec3f * points_row2 = pointcloud.points.ptr<cv::Vec3f>(h+1);

        cv::Vec3f * normals_row = pointcloud.normals.ptr<cv::Vec3f>(h);

        for (int w=1; w+1<pointcloud.points.cols; w++)
        {
            cv::Vec3f const& w1 = points_row1[w-1];
            cv::Vec3f const& w2 = points_row1[w+1];

            cv::Vec3f const& h1 = points_row0[w];
            cv::Vec3f const& h2 = points_row2[w];

            if (sl::INVALID(w1[0]) || sl::INVALID(w2[0]) || sl::INVALID(h1[0]) || sl::INVALID(h2[0]))
            {
                continue;
            }

            cv::Point3d n1(w2[0]-w1[0], w2[1]-w1[1], w2[2]-w1[2]);
            cv::Point3d n2(h2[0]-h1[0], h2[1]-h1[1], h2[2]-h1[2]);

            cv::Point3d normal = cv::Point3d(-n2.z*n1.y+n2.y*n1.z, n2.z*n1.x-n2.x*n1.z, -n2.y*n1.x+n2.x*n1.y);
            double norm = std::sqrt(normal.x*normal.x+normal.y*normal.y+normal.z*normal.z);
            if (norm>0.0)
            {
                cv::Vec3f & cloud_normal = normals_row[w];
                cloud_normal[0] = normal.x/norm;
                cloud_normal[1] = normal.y/norm;
                cloud_normal[2] = normal.z/norm;
            }
        }
    }
}

cv::Mat scan3d::make_projector_view(cv::Mat const& pattern_image, cv::Mat const& min_max_image, cv::Mat const& color_image,
                                        cv::Size const& projector_size, int threshold)
{
    if (!pattern_image.data || pattern_image.type()!=CV_32FC2)
    {   //pattern not correctly decoded
        std::cerr << "[reconstruct_model] ERROR invalid pattern_image\n";
        return cv::Mat();
    }
    if (!min_max_image.data || min_max_image.type()!=CV_8UC2)
    {   //pattern not correctly decoded
        std::cerr << "[reconstruct_model] ERROR invalid min_max_image\n";
        return cv::Mat();
    }
    if (color_image.data && color_image.type()!=CV_8UC3)
    {   //not standard RGB image
        std::cerr << "[reconstruct_model] ERROR invalid color_image\n";
        return cv::Mat();
    }

    //init
    int scale_factor_x = 1;
    int scale_factor_y = (projector_size.width>projector_size.height ? 1 : 2); //XXX HACK: preserve regular aspect ratio XXX HACK
    int out_cols = projector_size.width/scale_factor_x;
    int out_rows = projector_size.height/scale_factor_y;
    cv::Mat projector_image = cv::Mat::zeros(out_rows, out_cols, CV_8UC3);
    memset(projector_image.data, 255, projector_image.total()*projector_image.channels()); //white

    for (int h=0; h<pattern_image.rows; h++)
    {
        register const cv::Vec2f * curr_pattern_row = pattern_image.ptr<cv::Vec2f>(h);
        register const cv::Vec2b * min_max_row = min_max_image.ptr<cv::Vec2b>(h);
        for (register int w=0; w<pattern_image.cols; w++)
        {
            const cv::Vec2f & pattern = curr_pattern_row[w];
            const cv::Vec2b & min_max = min_max_row[w];

            if (sl::INVALID(pattern) 
                || pattern[0]<0.f || pattern[0]>=projector_size.width || pattern[1]<0.f || pattern[1]>=projector_size.height
                || (min_max[1]-min_max[0])<static_cast<int>(threshold))
            {   //skip
                continue;
            }

            //ok
            cv::Point2f proj_point(pattern[0]/scale_factor_x, pattern[1]/scale_factor_y);
            projector_image.at<cv::Vec3b>(static_cast<unsigned>(proj_point.y), static_cast<unsigned>(proj_point.x)) = color_image.at<cv::Vec3b>(h, w);
        }
    }

    return projector_image;
}