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


/***********************************************************
 *                  Convert to GL ES 2.0                   *
 ***********************************************************/

#include "GLWidget.hpp"

#include <cmath>

#include "Application.hpp"
#include "structured_light.hpp"

GLWidget::GLWidget(QWidget * parent) : QGLWidget(parent)
{
}

GLWidget::~GLWidget()
{
}

void GLWidget::initializeGL()
{
    // Set up the rendering context, define display lists etc.

    /*

    glDisable(GL_LIGHTING);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_DITHER);
    glDisable(GL_TEXTURE_2D);

    glClearColor(0.0, 0.0, 0.0, 0.0);

    */
}

void GLWidget::resizeGL(int w, int h)
{
  // setup viewport, projection etc.:

  cv::Mat K = APP->calib.cam_K;

  if (!K.data)
  { //no calibration
    return;
  }

  double width = APP->get_camera_width();
  double height = APP->get_camera_height();
  if (!width || !height)
  { //error
    return;
  }

  /*
  double fx = K.at<double>(0,0);
  double fy = K.at<double>(1,1);
  double cx = K.at<double>(0,2);
  double cy = K.at<double>(1,2);

  double view_width = std::min(static_cast<double>(w), width*h/height);
  double view_height = height*view_width/width;

  glViewport(0, 0, (GLint)view_width, (GLint)view_height);

  //tmp
  double near_plane = 100;
  double far_plane = 10000;

  double M[16] = {    2*fx/width,               0,                                              0, 0,
                               0,    -2*fy/height,                                              0, 0,
                  1-(2*cx/width), 1-(2*cy/height),  (far_plane+near_plane)/(far_plane-near_plane), 1,
                               0,               0, -2*far_plane*near_plane/(far_plane-near_plane), 0};

  glViewport(0, 0, (GLint)view_width, (GLint)view_height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glLoadMatrixd(M);

  */
}

void GLWidget::paintGL()
{
  // draw the scene:
  glClear(GL_COLOR_BUFFER_BIT);

  scan3d::Pointcloud const& pointcloud = APP->pointcloud;

  if (!pointcloud.points.data || !pointcloud.colors.data)
  {   //empty pointcloud
      return;
  }

  //draw
  /*
  glBegin(GL_POINTS);
  for (int h=0; h<pointcloud.points.rows; ++h)
  {
    const cv::Vec3f * point_row = pointcloud.points.ptr<cv::Vec3f>(h);
    const cv::Vec3b * color_row = pointcloud.colors.ptr<cv::Vec3b>(h);
    for (int w=0; w<pointcloud.points.cols; ++w)
    {
      cv::Vec3f const& pt = point_row[w];
      cv::Vec3b const& color = color_row[w];
      if (sl::INVALID(pt))
      {
          continue;
      }

      glColor3f(color[2]/255.f, color[1]/255.f, color[0]/255.f);
      glVertex3f(pt[0], pt[1], pt[2]);
    }
  }
  glEnd();
  */
}
