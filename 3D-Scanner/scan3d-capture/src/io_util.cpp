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

#include "io_util.hpp"

#include <QPainter>

#include <iostream>
#include <fstream>
#include <float.h>

#if defined(_MSC_VER) && !defined(isnan)
# include <float.h>
# define isnan _isnan
#endif

#include "structured_light.hpp"


QImage io_util::qImage(const cv::Mat & image)
{
    switch (image.type())
    {
    case CV_8UC3: return io_util::qImageFromRGB(image); break;
    case CV_8UC1: return io_util::qImageFromGray(image); break;
    }
    return QImage();
}

QImage io_util::qImageFromRGB(const cv::Mat & image)
{
    if (image.type()!=CV_8UC3)
    {   //unsupported type
        return QImage();
    }

    QImage qimg(image.cols, image.rows, QImage::Format_RGB32);
    for (int h=0; h<image.rows; h++)
    {
        const cv::Vec3b * row = image.ptr<cv::Vec3b>(h);
        unsigned * qrow = reinterpret_cast<unsigned *>(qimg.scanLine(h));
        for (register int w=0; w<image.cols; w++)
        {
            const cv::Vec3b & vec = row[w];
            qrow[w] = qRgb(vec[2], vec[1], vec[0]);
        }
    }
    return qimg;
}

QImage io_util::qImageFromGray(const cv::Mat & image)
{
    if (image.type()!=CV_8UC1)
    {   //unsupported type
        return QImage();
    }

    QImage qimg(image.cols, image.rows, QImage::Format_RGB32);
    for (int h=0; h<image.rows; h++)
    {
        const unsigned char * row = image.ptr<unsigned char>(h);
        unsigned * qrow = reinterpret_cast<unsigned *>(qimg.scanLine(h));
        for (register int w=0; w<image.cols; w++)
        {
            qrow[w] = qRgb(row[w], row[w], row[w]);
        }
    }
    return qimg;
}

bool io_util::write_pgm(const cv::Mat & image, const char * basename)
{
    if (!image.data || image.type()!=CV_32FC2 || !basename)
    {
        return false;
    }

    //open
    QString filename1 = QString("%1_col.pgm").arg(basename);
    QString filename2 = QString("%1_row.pgm").arg(basename);
    FILE * fp1 = fopen(qPrintable(filename1), "w");
    FILE * fp2 = fopen(qPrintable(filename2), "w");

    if (!fp1 || !fp2)
    {
        if (fp1) { fclose(fp1); }
        if (fp2) { fclose(fp2); }
        return false;
    }

    //write header
    /*
        P5      //binary grayscale   P2: ASCII grayscale
        24 7    //width height
        15      //maximum value
    */
    fprintf(fp1, "P2\n%d %d\n1024\n", image.cols, image.rows);
    fprintf(fp2, "P2\n%d %d\n1024\n", image.cols, image.rows);

    //write contents
    for (int h=0; h<image.rows; h++)
    {
        const cv::Vec2f * row = image.ptr<cv::Vec2f>(h);
        for (int w=0; w<image.cols; w++)
        {
            cv::Vec2f const& pattern = row[w];
            char c = (w+1<image.cols ? ' ' : '\n');
            fprintf(fp1, "%u%c", static_cast<unsigned>(pattern[0]), c);
            fprintf(fp2, "%u%c", static_cast<unsigned>(pattern[1]), c);
        }
    }

    //close
    if (fp1)
    {
        fclose(fp1);
    }
    if (fp2)
    {
        fclose(fp2);
    }

    return true;
}

bool io_util::write_ply(const std::string & filename, scan3d::Pointcloud const& pointcloud, unsigned flags)
{
    if (!pointcloud.points.data
        || (pointcloud.colors.data && pointcloud.colors.rows!=pointcloud.points.rows && pointcloud.colors.cols!=pointcloud.points.cols)
        || (pointcloud.normals.data && pointcloud.normals.rows!=pointcloud.points.rows && pointcloud.normals.cols!=pointcloud.points.cols))
    {
        return false;
    }

    bool binary  = (flags&PlyBinary);
    bool colors = (flags&PlyColors) && pointcloud.colors.data;
    bool normals = (flags&PlyNormals) && pointcloud.normals.data;
    std::vector<int> points_index;
    points_index.reserve(pointcloud.points.total());

    const cv::Vec3f * points_data = pointcloud.points.ptr<cv::Vec3f>(0);
    const cv::Vec3b * colors_data = (colors ? pointcloud.colors.ptr<cv::Vec3b>(0) : NULL);
    const cv::Vec3f * normals_data = (normals ? pointcloud.normals.ptr<cv::Vec3f>(0) : NULL);

    int total = static_cast<int>(pointcloud.points.total());
    for (int i=0; i<total; i++)
    {
        if (!sl::INVALID(points_data[i]) && (!normals_data || !sl::INVALID(normals_data[i])))
        {
            points_index.push_back(i);
        }
    }

    std::ofstream outfile;
    std::ios::openmode mode = std::ios::out|std::ios::trunc|(binary?std::ios::binary:static_cast<std::ios::openmode>(0));
    outfile.open(filename.c_str(), mode);
    if (!outfile.is_open())
    {
        return false;
    }

    const char * format_header = (binary? "binary_little_endian 1.0" : "ascii 1.0");
    outfile << "ply" << std::endl 
            << "format " << format_header << std::endl 
            << "comment scan3d-capture generated" << std::endl 
            << "element vertex " << points_index.size() << std::endl 
            << "property float x" << std::endl 
            << "property float y" << std::endl 
            << "property float z" << std::endl;
    if (normals)
    {
        outfile << "property float nx" << std::endl 
                << "property float ny" << std::endl 
                << "property float nz" << std::endl;
    }
    if (colors)
    {
        outfile << "property uchar red" << std::endl 
                << "property uchar green" << std::endl 
                << "property uchar blue" << std::endl 
                << "property uchar alpha" << std::endl;
    }
    outfile << "element face 0" << std::endl 
            << "property list uchar int vertex_indices" << std::endl 
            << "end_header" << std::endl ;

    for(std::vector<int>::const_iterator iter=points_index.begin(); iter!=points_index.end(); iter++)
    {
        cv::Vec3f const& p = points_data[*iter];
        if (binary)
        {
            outfile.write(reinterpret_cast<const char *>(&(p[0])), sizeof(float));
            outfile.write(reinterpret_cast<const char *>(&(p[1])), sizeof(float));
            outfile.write(reinterpret_cast<const char *>(&(p[2])), sizeof(float));
            if (normals)
            {
                cv::Vec3f const& n = normals_data[*iter];
                outfile.write(reinterpret_cast<const char *>(&(n[0])), sizeof(float));
                outfile.write(reinterpret_cast<const char *>(&(n[1])), sizeof(float));
                outfile.write(reinterpret_cast<const char *>(&(n[2])), sizeof(float));
            }
            if (colors)
            {
                cv::Vec3b const& c = colors_data[*iter];
                const unsigned char a = 255U;
                outfile.write(reinterpret_cast<const char *>(&(c[2])), sizeof(unsigned char));
                outfile.write(reinterpret_cast<const char *>(&(c[1])), sizeof(unsigned char));
                outfile.write(reinterpret_cast<const char *>(&(c[0])), sizeof(unsigned char));
                outfile.write(reinterpret_cast<const char *>(&a), sizeof(unsigned char));
            }
        }
        else
        {
            outfile << p[0] << " " << p[1] << " "  << p[2];
            if (normals)
            {
                cv::Vec3f const& n = normals_data[*iter];
                outfile << " " << n[0] << " " << n[1] << " " << n[2];
            }
            if (colors)
            {
                cv::Vec3b const& c = colors_data[*iter];
                outfile << " " << static_cast<int>(c[2]) << " " << static_cast<int>(c[1]) << " " << static_cast<int>(c[0]) << " 255";
            }
            outfile << std::endl;
        }
    }

    outfile.close();
    std::cerr << "[write_ply] Saved " << points_index.size() << " points (" << filename << ")" << std::endl;
    return true;
}
