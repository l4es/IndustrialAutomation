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

#include "Application.hpp"

#include <QDir>
#include <QProgressDialog>
#include <QMessageBox>
#include <QFileDialog>

#include <cmath>
#include <iostream>
#include <ctime>
#include <cstdlib>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include "structured_light.hpp"

#include "cognex_util.hpp"


Application::Application(int & argc, char ** argv) : 
    QApplication(argc, argv),
#ifdef _MSC_VER
    config(APP_NAME ".ini", QSettings::IniFormat, this),
#else
    config(QSettings::IniFormat, QSettings::UserScope, APP_NAME, APP_NAME, this),
#endif
    model(this),
    calib(),
    corner_count(11, 7),
    corner_size(21.f, 21.f),
    corners_world(),
    corners_camera(),
    corners_projector(),
    pattern_list(),
    min_max_list(),
    projector_view_list(),
    pointcloud(),
    mainWin((QWidget*)(load_config(), NULL)),
    processingDialog(&mainWin, Qt::Window|Qt::CustomizeWindowHint|Qt::WindowTitleHint)
{
    connect(this, SIGNAL(aboutToQuit()), this, SLOT(deinit()));

    //setup the main window state
    mainWin.show();
    mainWin.restoreGeometry(config.value("main/window_geometry").toByteArray());
    QVariant window_state = config.value("main/window_state");
    if (window_state.isValid())
    {
        mainWin.setWindowState(static_cast<Qt::WindowStates>(window_state.toUInt()));
    }

    //set model
    set_root_dir(config.value("main/root_dir", QDir::currentPath()).toString());
    QModelIndex index = model.index(0, 0);
    mainWin._on_image_tree_currentChanged(index, index);
}

Application::~Application()
{
}

void Application::deinit(void)
{
    config.setValue("main/window_geometry", mainWin.saveGeometry());
    config.setValue("main/window_state", static_cast<unsigned>(mainWin.windowState()));
}

void Application::clear(void)
{
    //calib.clear();
    corners_world.clear();
    corners_camera.clear();
    corners_projector.clear();
    pattern_list.clear();
    min_max_list.clear();
    projector_view_list.clear();
    pointcloud.clear();
}

void Application::load_config(void)
{
    //decode
    if (!config.value(THRESHOLD_CONFIG).isValid())
    {
        config.setValue(THRESHOLD_CONFIG, THRESHOLD_DEFAULT);
    }
    if (!config.value(ROBUST_B_CONFIG).isValid())
    {
        config.setValue(ROBUST_B_CONFIG, ROBUST_B_DEFAULT);
    }
    if (!config.value(ROBUST_M_CONFIG).isValid())
    {
        config.setValue(ROBUST_M_CONFIG, ROBUST_M_DEFAULT);
    }

    //checkerboard size
    if (!config.value("main/corner_count_x").isValid())
    {
        config.setValue("main/corner_count_x", DEFAULT_CORNER_X);
    }
    if (!config.value("main/corner_count_y").isValid())
    {
        config.setValue("main/corner_count_y", DEFAULT_CORNER_Y);
    }
    if (!config.value("main/corners_width").isValid())
    {
        config.setValue("main/corners_width", DEFAULT_CORNER_WIDTH);
    }
    if (!config.value("main/corners_height").isValid())
    {
        config.setValue("main/corners_height", DEFAULT_CORNER_HEIGHT);
    }

    //reconstruction
    if (!config.value(MAX_DIST_CONFIG).isValid())
    {
        config.setValue(MAX_DIST_CONFIG, MAX_DIST_DEFAULT);
    }
    if (!config.value(SAVE_NORMALS_CONFIG).isValid())
    {
        config.setValue(SAVE_NORMALS_CONFIG, SAVE_NORMALS_DEFAULT);
    }
    if (!config.value(SAVE_COLORS_CONFIG).isValid())
    {
        config.setValue(SAVE_COLORS_CONFIG, SAVE_COLORS_DEFAULT);
    }
    if (!config.value(SAVE_BINARY_CONFIG).isValid())
    {
        config.setValue(SAVE_BINARY_CONFIG, SAVE_BINARY_DEFAULT);
    }
}

void Application::set_root_dir(const QString & dirname)
{
    QDir root_dir(dirname);

    //reset internal data
    model.clear();
    clear();

    QStringList dirlist = root_dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot, QDir::Name);
    foreach (const QString & item, dirlist)
    {
        QDir dir(root_dir.filePath(item));

        QStringList filters;
        filters << "*.jpg" << "*.bmp" << "*.png";

        QStringList filelist = dir.entryList(filters, QDir::Files, QDir::Name);
        QString path = dir.path();

        //setup the model
        int filecount = filelist.count();

        if (filecount<1)
        {   //no images, skip
            continue;
        }

        unsigned row = model.rowCount();
        if (!model.insertRow(row))
        {
            std::cout << "Failed model insert " << item.toStdString() << "("<< row << ")" << std::endl;
            continue;
        }

        //add the childrens
        QModelIndex parent = model.index(row, 0);
        model.setData(parent, item,  Qt::DisplayRole);
        model.setData(parent, item,  Qt::ToolTipRole);
        model.setData(parent, Qt::Checked, Qt::CheckStateRole);

        //read projector info
        int projector_width = 1024, projector_height = 768; //defaults compatible with old software 
        QString projector_filename = dirname + "/" + item + "/projector_info.txt";
        FILE * fp = fopen(qPrintable(projector_filename), "r");
        if (fp)
        {   //projector info file exists
            int width, height;
            if (fscanf(fp, "%u %u", &width, &height)==2 && width>0 && height)
            {   //ok
                projector_width = width;
                projector_height = height;
                std::cerr << "Projector info file loaded: " << projector_filename.toStdString() << std::endl;
            }
            else
            {
                std::cerr << "Projector info file has invalid values" << std::endl;
            }
            fclose(fp);
        }
        else
        {
            std::cerr << "Projector info file failed to open: " << projector_filename.toStdString() << std::endl;
        }
        std::cerr << "Projector info file: using width=" << projector_width << " height=" << projector_height << std::endl;
        model.setData(parent, projector_width,  ProjectorWidthRole);
        model.setData(parent, projector_height,  ProjectorHeightRole);

        for (int i=0; i<filecount; i++)
        {
            const QString & filename = filelist.at(i);
            if (!model.insertRow(i, parent))
            {
                std::cout << "Failed model insert " << filename.toStdString() << "("<< row << ")" << std::endl;
                break;
            }

            QModelIndex index = model.index(i, 0, parent);
            QString label = QString("#%1 %2").arg(i, 2, 10, QLatin1Char('0')).arg(filename);
            model.setData(index, label, Qt::DisplayRole);
            model.setData(index, label, Qt::ToolTipRole);

            //additional data
            model.setData(index, path + "/" + filename, ImageFilenameRole);
        }
    }

    config.setValue("main/root_dir", dirname);
    emit root_dir_changed(dirname);
}

QString Application::get_root_dir(void) const 
{
    return config.value("main/root_dir").toString();
}

bool Application::change_root_dir(QWidget * parent_widget)
{
    QString dirname = QFileDialog::getExistingDirectory(parent_widget, "Select Image Directory",
                           config.value("main/root_dir", QString()).toString());

    if (dirname.isEmpty())
    {   //nothing selected
        return false;
    }

    set_root_dir(dirname);
    return true;
}

const cv::Mat Application::get_image(unsigned level, unsigned n, Role role) const
{
    if (role!=GrayImageRole && role!=ColorImageRole)
    {   //invalid args
        return cv::Mat();
    }

    //try to load
    if (model.rowCount()<static_cast<int>(level))
    {   //out of bounds
        return cv::Mat();
    }
    QModelIndex parent = model.index(level, 0);
    if (model.rowCount(parent)<static_cast<int>(n))
    {   //out of bounds
        return cv::Mat();
    }

    QModelIndex index = model.index(n, 0, parent);
    if (!index.isValid())
    {   //invalid index
        return cv::Mat();
    }

    QString filename = model.data(index, ImageFilenameRole).toString();
    std::cout << "[" << (role==GrayImageRole ? "gray" : "color") << "] Filename: " << filename.toStdString() << std::endl;

    //load image
    cv::Mat rgb_image = cv::imread(filename.toStdString());
    if (rgb_image.rows>0 && rgb_image.cols>0)
    {
        //color
        if (role==ColorImageRole)
        {
            return rgb_image;
        }
        
        //gray scale
        if (role==GrayImageRole)
        {
            cv::Mat gray_image;
            cvtColor(rgb_image, gray_image, CV_BGR2GRAY);
            return gray_image;
        }
    }

    return cv::Mat();
}

int Application::get_camera_width(unsigned level) const
{
  return get_image(level, 0, ColorImageRole).cols;
}

int Application::get_camera_height(unsigned level) const
{
  return get_image(level, 0, ColorImageRole).rows;
}

int Application::get_projector_width(unsigned level) const
{
    if (static_cast<int>(level)<model.rowCount())
    {   //ok
        QModelIndex parent = model.index(level, 0);
        return model.data(parent, ProjectorWidthRole).toInt();
    }
    return 0;
}

int Application::get_projector_height(unsigned level) const
{
    if (static_cast<int>(level)<model.rowCount())
    {   //ok
        QModelIndex parent = model.index(level, 0);
        return model.data(parent, ProjectorHeightRole).toInt();
    }
    return 0;
}

bool Application::extract_chessboard_corners(void)
{
    corner_count = cv::Size(config.value("main/corner_count_x").toUInt(), config.value("main/corner_count_y").toUInt()); //interior number of corners
    corner_size = cv::Size2f(config.value("main/corners_width").toDouble(), config.value("main/corners_height").toDouble());

    unsigned count = static_cast<unsigned>(model.rowCount());

    processing_set_progress_total(count);
    processing_set_progress_value(0);
    processing_set_current_message("Extracting corners...");

    corners_world.clear();
    corners_camera.clear();
    corners_world.resize(count);
    corners_camera.resize(count);

    cv::Size imageSize(0,0);
    int image_scale = 1;

    bool all_found = true;
    for (unsigned i=0; i<count; i++)
    {
        QModelIndex index = model.index(i, 0);
        QString set_name = model.data(index, Qt::DisplayRole).toString();
        bool checked = (model.data(index, Qt::CheckStateRole).toInt()==Qt::Checked);
        if (!checked)
        {   //skip
            processing_message(QString(" * %1: skip (not selected)").arg(set_name));
            processing_set_progress_value(i+1);
            continue;
        }
        processing_set_current_message(QString("Extracting corners... %1").arg(set_name));

        cv::Mat gray_image = get_image(i, 1, GrayImageRole);
        if (gray_image.rows<1)
        {
            processing_set_progress_value(i+1);
            continue;
        }

        if (imageSize.width==0)
        {   //init image size
            imageSize = gray_image.size();
            if (imageSize.width>1024)
            {
                image_scale = cvRound(imageSize.width/1024.0);
            }
        }
        else if (imageSize != gray_image.size())
        {   //error
            std::cout << "ERROR: image of different size: set " << i << std::endl;
            return false;
        }

        cv::Mat small_img;

        if (image_scale>1)
        {
            cv::resize(gray_image, small_img, cv::Size(gray_image.cols/image_scale, gray_image.rows/image_scale));
        }
        else
        {
            gray_image.copyTo(small_img);
        }

        if (processing_canceled())
        {
            processing_set_current_message("Extract corners canceled");
            processing_message("Extract corners canceled");
            return false;
        }

        //this will be filled by the detected corners
        bool cognex_chessboard = false;
        std::vector<cv::Point2f> & cam_corners = corners_camera[i];
        std::vector<cv::Point3f> & world_corners = corners_world[i];
        if (cv::findChessboardCorners(small_img, corner_count, cam_corners, 
                     cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE /*+ cv::CALIB_CB_FILTER_QUADS*/))
        {
            processing_message(QString(" * %1: found %2 corners").arg(set_name).arg(cam_corners.size()));
            std::cout << " - corners: " << cam_corners.size() << std::endl;

            get_chessboard_world_coords(world_corners, corner_count, corner_size);
        }
#ifdef USE_COGNEX
        else
        {
            //try cognex cal
            if (cognex::extract_corners(gray_image, cam_corners, world_corners))
            {   //cognex cal plate not found
                processing_message(QString(" * %1: Cognex chessboard found").arg(set_name));
                cognex_chessboard = true;
            }
            else
            {   //cognex cal plate not found
                all_found = false;
                processing_message(QString(" * %1: chessboard not found!").arg(set_name));
                std::cout << " - chessboard not found!" << std::endl;
            }
        }
#endif //USE_COGNEX

        if (!cognex_chessboard)
        {
            for (std::vector<cv::Point2f>::iterator iter=cam_corners.begin(); iter!=cam_corners.end(); iter++)
            {
                *iter = image_scale*(*iter);
            }
            if (cam_corners.size())
            {
                cv::cornerSubPix(gray_image, cam_corners, cv::Size(11, 11), cv::Size(-1, -1), 
                                    cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
            }
        }

        processing_set_progress_value(i+1);
    }

    processing_set_current_message("Extract corners finished");
    processing_set_progress_value(count);
    return all_found;
}

void Application::decode_all(void)
{
    unsigned count = static_cast<unsigned>(model.rowCount());
    cv::Size imageSize(0,0);

    processing_set_progress_total(count);
    processing_set_progress_value(0);
    processing_set_current_message("Decoding...");

    pattern_list.resize(count);
    min_max_list.resize(count);

    QString path = config.value("main/root_dir").toString();
 
    //decode gray patterns
    for (unsigned i=0; i<count; i++)
    {
        QModelIndex index = model.index(i, 0);
        QString set_name = model.data(index, Qt::DisplayRole).toString();
        bool checked = (model.data(index, Qt::CheckStateRole).toInt()==Qt::Checked);
        if (!checked)
        {   //skip
            processing_message(QString(" * %1: skipped [not selected]").arg(set_name));
            processing_set_progress_value(i+1);
            continue;
        }

        processing_set_current_message(QString("Decoding... %1").arg(set_name));

        cv::Mat & pattern_image = pattern_list[i];
        cv::Mat & min_max_image = min_max_list[i];
        if (!decode_gray_set(i, pattern_image, min_max_image))
        {   //error
            std::cout << "ERROR: Decode image set " << i << " failed. " << std::endl;
            return;
        }

        if (processing_canceled())
        {
            processing_set_current_message("Decode canceled");
            processing_message("Decode canceled");
            return;
        }

        if (imageSize.width==0)
        {
            imageSize = pattern_image.size();
        }
        else if (imageSize != pattern_image.size())
        {
            processing_message(QString("ERROR: pattern image of different size: set %1").arg(set_name));
            std::cout << "ERROR: pattern image of different size: set " << i << std::endl;
            return;
        }
        else if (get_projector_width(0)!=get_projector_width(i) || get_projector_height(0)!=get_projector_height(i))
        {
            QString error_message = QString("ERROR: projector resolution does not match: set %1 [expected %2x%3, got %4x%5").arg(set_name)
                        .arg(get_projector_width(0)).arg(get_projector_height(0)).arg(get_projector_width(i)).arg(get_projector_height(i));
            processing_message(error_message);
            std::cout << error_message.toStdString() << std::endl;
            return;
        }

        //save pattern image as PGM for debugging
        //QString filename = path + "/" + set_name;
        //io_util::write_pgm(pattern_image, qPrintable(filename));

        processing_message(QString(" * %1: decoded").arg(set_name));
        processing_set_progress_value(i+1);
    }

    processing_set_current_message("Decode finished");
    processing_set_progress_value(count);
}

void Application::decode(int level, QWidget * parent_widget)
{
    if (level<0 || level>=model.rowCount())
    {   //invalid row
        return;
    }
    if (pattern_list.size()<model.rowCount<size_t>())
    {
        pattern_list.resize(model.rowCount());
    }
    if (min_max_list.size()<model.rowCount<size_t>())
    {
        min_max_list.resize(model.rowCount());
    }

    cv::Mat & pattern_image = pattern_list[level];
    cv::Mat & min_max_image = min_max_list[level];

    if (!decode_gray_set(level, pattern_image, min_max_image, parent_widget))
    {   //error
        std::cout << "ERROR: Decode image set " << level << " failed. " << std::endl;
    }
}

bool Application::dump_decoded(const char* filename, int type, cv::Mat2f const& pattern_image, cv::Mat2b const& min_max_image, cv::Mat3b const& color_image) const
{
    if (!filename || !pattern_image.data || !min_max_image.data || !color_image.data)
    {
        return false;
    }

    FILE * fp = fopen(filename, "wb");
    if (!fp)
    {
        return false;
    }

    fwrite(&type, sizeof(int), 1, fp);
    fwrite(&pattern_image.cols, sizeof(int), 1, fp);
    fwrite(&pattern_image.rows, sizeof(int), 1, fp);

    //int index = type-1;

    //dump cols
    for (int h=0; h<pattern_image.rows; ++h)
    {
        cv::Vec2f const* row = pattern_image.ptr<cv::Vec2f>(h);
        for (int w=0; w<pattern_image.cols; ++w)
        {
            float value = row[w][0];
            fwrite(&value, sizeof(float), 1, fp);
        }
    }

    //dump rows
    for (int h=0; h<pattern_image.rows; ++h)
    {
        cv::Vec2f const* row = pattern_image.ptr<cv::Vec2f>(h);
        for (int w=0; w<pattern_image.cols; ++w)
        {
            float value = row[w][1];
            fwrite(&value, sizeof(float), 1, fp);
        }
    }

    //dump min
    for (int h=0; h<min_max_image.rows; ++h)
    {
        cv::Vec2b const* row = min_max_image.ptr<cv::Vec2b>(h);
        for (int w=0; w<min_max_image.cols; ++w)
        {
            unsigned char value = row[w][0];
            fwrite(&value, sizeof(unsigned char), 1, fp);
        }
    }

    //dump max
    for (int h=0; h<min_max_image.rows; ++h)
    {
        cv::Vec2b const* row = min_max_image.ptr<cv::Vec2b>(h);
        for (int w=0; w<min_max_image.cols; ++w)
        {
            unsigned char value = row[w][1];
            fwrite(&value, sizeof(unsigned char), 1, fp);
        }
    }

    //dump rgb
    for (int h=0; h<color_image.rows; ++h)
    {
        cv::Vec3b const* row = color_image.ptr<cv::Vec3b>(h);
        for (int w=0; w<color_image.cols; ++w)
        {
            fwrite(&(row[w]), sizeof(unsigned char), 3, fp);
        }
    }

    fclose(fp);

    printf("saved file %s\n", filename); 

    return true;
}

bool Application::load_dump(const char* filename, int type, cv::Mat2f & pattern_image, cv::Mat2b & min_max_image, cv::Mat3b & color_image) const
{
    if (!filename)
    {
        return false;
    }

    FILE * fp = fopen(filename, "rb");
    if (!fp)
    {
        return false;
    }

    int rows, cols;
    fread(&type, sizeof(int), 1, fp);
    fread(&cols, sizeof(int), 1, fp);
    fread(&rows, sizeof(int), 1, fp);

    pattern_image.create(rows, cols);
    min_max_image.create(rows, cols);
    color_image.create(rows, cols);

    //dump cols
    for (int h=0; h<pattern_image.rows; ++h)
    {
        cv::Vec2f * row = pattern_image.ptr<cv::Vec2f>(h);
        for (int w=0; w<pattern_image.cols; ++w)
        {
            float value;
            fread(&value, sizeof(float), 1, fp);
            row[w][0] = value;
        }
    }

    //dump rows
    for (int h=0; h<pattern_image.rows; ++h)
    {
        cv::Vec2f * row = pattern_image.ptr<cv::Vec2f>(h);
        for (int w=0; w<pattern_image.cols; ++w)
        {
            float value;
            fread(&value, sizeof(float), 1, fp);
            row[w][1] = value;
        }
    }

    //dump min
    for (int h=0; h<min_max_image.rows; ++h)
    {
        cv::Vec2b * row = min_max_image.ptr<cv::Vec2b>(h);
        for (int w=0; w<min_max_image.cols; ++w)
        {
            unsigned char value;
            fread(&value, sizeof(unsigned char), 1, fp);
            row[w][0] = value;
        }
    }

    //dump max
    for (int h=0; h<min_max_image.rows; ++h)
    {
        cv::Vec2b * row = min_max_image.ptr<cv::Vec2b>(h);
        for (int w=0; w<min_max_image.cols; ++w)
        {
            unsigned char value;
            fread(&value, sizeof(unsigned char), 1, fp);
            row[w][1] = value;
        }
    }

    //dump rgb
    for (int h=0; h<color_image.rows; ++h)
    {
        cv::Vec3b * row = color_image.ptr<cv::Vec3b>(h);
        fread(row, sizeof(unsigned char), 3*cols, fp);
    }

    fclose(fp);

    printf("loaded file %s\n", filename); 

    return true;
}

void Application::calibrate(void)
{   //try to calibrate the camera, projector, and stereo system

    unsigned count = static_cast<unsigned>(model.rowCount());
    const unsigned threshold = config.value("main/shadow_threshold", 0).toUInt();

    calib.clear();

    std::cout << " shadow_threshold = " << threshold << std::endl;

    cv::Size imageSize(0,0);

    //detect corners ////////////////////////////////////
    processing_message("Extracting corners:");
    if (!extract_chessboard_corners())
    {
        return;
    }
    processing_message("");

    //collect projector correspondences
    corners_projector.resize(count);
    pattern_list.resize(count);
    min_max_list.resize(count);

    processing_set_progress_total(count);
    processing_set_progress_value(0);
    processing_set_current_message("Decoding and computing homographies...");

    for (unsigned i=0; i<count; i++)
    {
        std::vector<cv::Point2f> const& cam_corners = corners_camera[i];
        std::vector<cv::Point2f> & proj_corners = corners_projector[i];

        QModelIndex index = model.index(i, 0);
        QString set_name = model.data(index, Qt::DisplayRole).toString();
        bool checked = (model.data(index, Qt::CheckStateRole).toInt()==Qt::Checked);
        if (!checked)
        {   //skip
            processing_message(QString(" * %1: skip (not selected)").arg(set_name));
            processing_set_progress_value(i+1);
            continue;
        }

        //checked: use this set
        proj_corners.clear(); //erase previous points

        processing_set_current_message(QString("Decoding... %1").arg(set_name));

        cv::Mat & pattern_image = pattern_list[i];
        cv::Mat & min_max_image = min_max_list[i];
        if (!decode_gray_set(i, pattern_image, min_max_image))
        {   //error
            std::cout << "ERROR: Decode image set " << i << " failed. " << std::endl;
            return;
        }

        if (imageSize.width==0)
        {
            imageSize = pattern_image.size();
        }
        else if (imageSize != pattern_image.size())
        {
            std::cout << "ERROR: pattern image of different size: set " << i << std::endl;
            return;
        }

        //cv::Mat out_pattern_image = sl::PIXEL_UNCERTAIN*cv::Mat::ones(pattern_image.size(), pattern_image.type());

        processing_set_current_message(QString("Computing homographies... %1").arg(set_name));

        for (std::vector<cv::Point2f>::const_iterator iter=cam_corners.cbegin(); iter!=cam_corners.cend(); iter++)
        {
            const cv::Point2f & p = *iter;
            cv::Point2f q;

            if (processing_canceled())
            {
                processing_set_current_message("Calibration canceled");
                processing_message("Calibration canceled");
                return;
            }
            processEvents();

            //find an homography around p
            unsigned WINDOW_SIZE = config.value(HOMOGRAPHY_WINDOW_CONFIG, HOMOGRAPHY_WINDOW_DEFAULT).toUInt()/2;
            std::vector<cv::Point2f> img_points, proj_points;
            if (p.x>WINDOW_SIZE && p.y>WINDOW_SIZE && p.x+WINDOW_SIZE<pattern_image.cols && p.y+WINDOW_SIZE<pattern_image.rows)
            {
                for (unsigned h=p.y-WINDOW_SIZE; h<p.y+WINDOW_SIZE; h++)
                {
                    register const cv::Vec2f * row = pattern_image.ptr<cv::Vec2f>(h);
                    register const cv::Vec2b * min_max_row = min_max_image.ptr<cv::Vec2b>(h);
                    //cv::Vec2f * out_row = out_pattern_image.ptr<cv::Vec2f>(h);
                    for (unsigned w=p.x-WINDOW_SIZE; w<p.x+WINDOW_SIZE; w++)
                    {
                        const cv::Vec2f & pattern = row[w];
                        const cv::Vec2b & min_max = min_max_row[w];
                        //cv::Vec2f & out_pattern = out_row[w];
                        if (sl::INVALID(pattern))
                        {
                            continue;
                        }
                        if ((min_max[1]-min_max[0])<static_cast<int>(threshold))
                        {   //apply threshold and skip
                            continue;
                        }

                        img_points.push_back(cv::Point2f(w, h));
                        proj_points.push_back(cv::Point2f(pattern));

                        //out_pattern = pattern;
                    }
                }
                cv::Mat H = cv::findHomography(img_points, proj_points, CV_RANSAC);
                //std::cout << " H:\n" << H << std::endl;
                cv::Point3d Q = cv::Point3d(cv::Mat(H*cv::Mat(cv::Point3d(p.x, p.y, 1.0))));
                q = cv::Point2f(Q.x/Q.z, Q.y/Q.z);
            }
            else
            {
                return;
            }

            //save
            proj_corners.push_back(q);
        }

        processing_message(QString(" * %1: finished").arg(set_name));
        processing_set_progress_value(i+1);
    }
    processing_message("");
    
    std::vector<std::vector<cv::Point3f> > world_corners_active;
    std::vector<std::vector<cv::Point2f> > camera_corners_active;
    std::vector<std::vector<cv::Point2f> > projector_corners_active;
    world_corners_active.reserve(count);
    camera_corners_active.reserve(count);
    projector_corners_active.reserve(count);
    for (unsigned i=0; i<count; i++)
    {
        std::vector<cv::Point3f> const& world_corners = corners_world.at(i);
        std::vector<cv::Point2f> const& cam_corners = corners_camera.at(i);
        std::vector<cv::Point2f> const& proj_corners = corners_projector.at(i);
        if (world_corners.size() && cam_corners.size() && proj_corners.size())
        {   //active set
            world_corners_active.push_back(world_corners);
            camera_corners_active.push_back(cam_corners);
            projector_corners_active.push_back(proj_corners);
        }
    }

    if (world_corners_active.size()<3)
    {
        processing_set_current_message("ERROR: use at least 3 sets");
        processing_message("ERROR: use at least 3 sets");
        return;
    }

    int cal_flags = 0
                  //+ cv::CALIB_FIX_K1
                  //+ cv::CALIB_FIX_K2
                  //+ cv::CALIB_ZERO_TANGENT_DIST
                  + cv::CALIB_FIX_K3
                  ;

    //calibrate the camera ////////////////////////////////////
    processing_message(QString(" * Calibrate camera [%1x%2]").arg(imageSize.width).arg(imageSize.height));
    std::vector<cv::Mat> cam_rvecs, cam_tvecs;
    int cam_flags = cal_flags;
    calib.cam_error = cv::calibrateCamera(world_corners_active, camera_corners_active, imageSize, calib.cam_K, calib.cam_kc, cam_rvecs, cam_tvecs, cam_flags, 
                                            cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 50, DBL_EPSILON));

    //calibrate the projector ////////////////////////////////////
    cv::Size projector_size(get_projector_width(), get_projector_height());
    processing_message(QString(" * Calibrate projector [%1x%2]").arg(projector_size.width).arg(projector_size.height));
    std::vector<cv::Mat> proj_rvecs, proj_tvecs;
    int proj_flags = cal_flags;
    calib.proj_error = cv::calibrateCamera(world_corners_active, projector_corners_active, projector_size, calib.proj_K, calib.proj_kc, proj_rvecs, proj_tvecs, proj_flags, 
                                             cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 50, DBL_EPSILON));
/*
    //TMP: estimate an initial stereo R and T
    double errStereo = 0.0;
    std::vector<cv::Point3f> wpts;
    std::vector<cv::Point2f> cipts, pipts;
    for (size_t i=0; i<world_corners_active.size(); ++i)
    {
      auto const& world_curr = world_corners_active.at(i);
      auto const& cam_curr = camera_corners_active.at(i);
      auto const& proj_curr = projector_corners_active.at(i);
      cv::Matx33d Rc; cv::Rodrigues(cam_rvecs.at(i), Rc); cv::Point3d Tc = cam_tvecs.at(i);
      cv::Matx33d Rp; cv::Rodrigues(proj_rvecs.at(i), Rp); cv::Point3d Tp = proj_tvecs.at(i);
      for (size_t j=0; j<world_curr.size(); ++j)
      {
        auto const& wpt = world_curr.at(j);

        cv::Matx31d wptc = Rc*cv::Point3d(wpt) + Tc;
        wpts.push_back( cv::Point3f(wptc(0,0),wptc(1,0),wptc(2,0)) );

        cipts.push_back(cam_curr.at(j));
        pipts.push_back(proj_curr.at(j));
      }
    }

    //cam ransac
    cv::Vec3d cam_rvec, cam_tvec;
    cv::solvePnPRansac(wpts, cipts, calib.cam_K, calib.cam_kc, cam_rvec, cam_tvec);
    std::cerr << "Cam (ransac) R,T: R " << cam_rvec << " T " << cam_tvec << std::endl;

    { //reproj error
      double errCam = 0.0;
      std::vector<cv::Point2f> imgPts;
      cv::projectPoints(wpts, cam_rvec, cam_tvec, calib.cam_K, calib.cam_kc, imgPts);
      for (size_t i=0; i<imgPts.size(); ++i)
      {
        auto const& p1 = imgPts.at(i);
        auto const& p2 = cipts.at(i);
        double err = cv::norm(p1-p2);
        errCam += err;
      }
      errCam /= imgPts.size();
      std::cerr << " errCam (ransac):  " << errCam << std::endl;
    }

    //cam LM
    bool rv1 = cv::solvePnP(wpts, cipts, calib.cam_K, calib.cam_kc, cam_rvec, cam_tvec, true);
    std::cerr << "Cam (LM) R,T: R " << cam_rvec << " T " << cam_tvec << std::endl;

    { //reproj error
      double errCam = 0.0;
      std::vector<cv::Point2f> imgPts;
      cv::projectPoints(wpts, cam_rvec, cam_tvec, calib.cam_K, calib.cam_kc, imgPts);
      for (size_t i=0; i<imgPts.size(); ++i)
      {
        auto const& p1 = imgPts.at(i);
        auto const& p2 = cipts.at(i);
        double err = cv::norm(p1-p2);
        errCam += err;
        errStereo += err;
      }
      errCam /= imgPts.size();
      std::cerr << " errCam (LM):  " << errCam << std::endl;
    }

    //proj ransac
    cv::Vec3d proj_rvec, proj_tvec;
    cv::solvePnPRansac(wpts, pipts, calib.proj_K, calib.proj_kc, proj_rvec, proj_tvec);
    std::cerr << "Proj (ransac) R,T: R " << proj_rvec << " T " << proj_tvec << std::endl;

    { //reproj error
      double errPrj = 0.0;
      std::vector<cv::Point2f> projPts;
      cv::projectPoints(wpts, proj_rvec, proj_tvec, calib.proj_K, calib.proj_kc, projPts);
      for (size_t i=0; i<projPts.size(); ++i)
      {
        auto const& p1 = projPts.at(i);
        auto const& p2 = pipts.at(i);
        double err = cv::norm(p1-p2);
        errPrj += err;
      }
      errPrj /= projPts.size();
      std::cerr << " errPrj (ransac):  " << errPrj << std::endl;
    }

    //proj LM
    bool rv2 = cv::solvePnP(wpts, pipts, calib.proj_K, calib.proj_kc, proj_rvec, proj_tvec, true);
    std::cerr << "Proj (LM) R,T: R " << proj_rvec << " T " << proj_tvec << std::endl;

    { //reproj error
      double errPrj = 0.0;
      std::vector<cv::Point2f> projPts;
      cv::projectPoints(wpts, proj_rvec, proj_tvec, calib.proj_K, calib.proj_kc, projPts);
      for (size_t i=0; i<projPts.size(); ++i)
      {
        auto const& p1 = projPts.at(i);
        auto const& p2 = pipts.at(i);
        double err = cv::norm(p1-p2);
        errPrj += err;
        errStereo += err;
      }
      errPrj /= projPts.size();
      std::cerr << " errPrj (LM):  " << errPrj << std::endl;
    }

    errStereo /= 2*wpts.size();;
    std::cerr << " errStereo:  " << errStereo << std::endl;
*/

    //stereo calibration
    processing_message(" * Calibrate stereo");
    cv::Mat E, F;
    calib.stereo_error = cv::stereoCalibrate(world_corners_active, camera_corners_active, projector_corners_active, calib.cam_K, calib.cam_kc, calib.proj_K, calib.proj_kc, 
                                                imageSize /*ignored*/, calib.R, calib.T, E, F, 
                                                cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 150, DBL_EPSILON), 
                                                cv::CALIB_FIX_INTRINSIC /*cv::CALIB_USE_INTRINSIC_GUESS*/ + cal_flags);
    //print to console
    calib.display();

    //print to GUI
    std::stringstream stream;
    calib.display(stream);
    processing_message("\n **** Calibration results ****\n");
    processing_message(QString::fromStdString(stream.str()));

    //save to file
    QString path = config.value("main/root_dir").toString();
    QString filename = path + "/calibration.yml";
    if (calib.save_calibration(filename))
    {
        processing_message(QString("Calibration saved: %1").arg(filename));
    }
    else
    {
        processing_message(QString("[ERROR] Saving %1 failed").arg(filename));
    }

    //save to MATLAB format
    filename = path + "/calibration.m";
    if (calib.save_calibration(filename))
    {
        processing_message(QString("Calibration saved [MATLAB]: %1").arg(filename));
    }
    else
    {
        processing_message(QString("[ERROR] Saving %1 failed").arg(filename));
    }


    //save corners
    for (unsigned i=0; i<count; i++)
    {
        std::vector<cv::Point3f> const& world_corners = corners_world.at(i);
        std::vector<cv::Point2f> const& cam_corners = corners_camera.at(i);
        std::vector<cv::Point2f> const& proj_corners = corners_projector.at(i);

        QString filename0 = QString("%1/world_%2.txt").arg(path).arg(i, 2, 10, QLatin1Char('0'));
        FILE * fp0 = fopen(qPrintable(filename0), "w");
        if (!fp0)
        {
            std::cout << "ERROR: could no open " << filename0.toStdString() << std::endl;
            return;
        }
        QString filename1 = QString("%1/cam_%2.txt").arg(path).arg(i, 2, 10, QLatin1Char('0'));
        FILE * fp1 = fopen(qPrintable(filename1), "w");
        if (!fp1)
        {
            std::cout << "ERROR: could no open " << filename1.toStdString() << std::endl;
            return;
        }
        QString filename2 = QString("%1/proj_%2.txt").arg(path).arg(i, 2, 10, QLatin1Char('0'));
        FILE * fp2 = fopen(qPrintable(filename2), "w");
        if (!fp2)
        {
            fclose(fp1);
            std::cout << "ERROR: could no open " << filename2.toStdString() << std::endl;
            return;
        }

        std::cout << "Saved " << filename0.toStdString() << std::endl;
        std::cout << "Saved " << filename1.toStdString() << std::endl;
        std::cout << "Saved " << filename2.toStdString() << std::endl;

        std::vector<cv::Point3f>::const_iterator iter0 = world_corners.begin();
        std::vector<cv::Point2f>::const_iterator iter1 = cam_corners.begin();
        std::vector<cv::Point2f>::const_iterator iter2 = proj_corners.begin();
        for (unsigned j=0; j<world_corners.size(); j++, ++iter0, ++iter1, ++iter2)
        {
            fprintf(fp0, "%lf %lf %lf\n", iter0->x, iter0->y, iter0->z);
            fprintf(fp1, "%lf %lf\n", iter1->x, iter1->y);
            fprintf(fp2, "%lf %lf\n", iter2->x, iter2->y);
        }
        fclose(fp0);
        fclose(fp1);
        fclose(fp2);
    }

    processing_message("Calibration finished");
}

bool Application::decode_gray_set(unsigned level, cv::Mat & pattern_image, cv::Mat & min_max_image, QWidget * parent_widget) const
{
    if (model.rowCount()<static_cast<int>(level))
    {   //out of bounds
        return false;
    }

    pattern_image = cv::Mat();
    min_max_image = cv::Mat();

    //progress
    QProgressDialog * progress = NULL;
    if (parent_widget)
    {
        progress = new QProgressDialog("Decoding...", "Abort", 0, 100, parent_widget, 
                                        Qt::Dialog|Qt::CustomizeWindowHint|Qt::WindowCloseButtonHint);
        progress->setWindowModality(Qt::WindowModal);
        progress->setWindowTitle("Processing");
        progress->setMinimumWidth(400);
        progress->show();
    }

    if (processing_canceled() || (progress && progress->wasCanceled()))
    {   //abort
        processing_set_current_message("Decode canceled");
        processing_message("Decode canceled");
        if (progress)
        {
            progress->close();
            delete progress;
            progress = NULL;
        }
        return false;
    }
    processEvents();

    //parameters
    const float b = config.value(ROBUST_B_CONFIG, ROBUST_B_DEFAULT).toFloat();
    const unsigned m = config.value(ROBUST_M_CONFIG, ROBUST_M_DEFAULT).toUInt();

    //estimate direct component
    std::vector<cv::Mat> images;
    int total_images = model.rowCount(model.index(level, 0));
    int total_patterns = total_images/2 - 1;
    const int direct_light_count = 4;
    const int direct_light_offset = 4;
    if (total_patterns<direct_light_count+direct_light_offset)
    {   //too few images
        processing_set_current_message("ERROR: too few pattern images");
        processing_message("ERROR: too few pattern images");
        return false;
    }
    if (progress)
    {
        progress->setLabelText("Decoding: estimating direct and global light components...");
        processEvents();
    }

    QList<unsigned> direct_component_images;
    for (unsigned i=0; i<direct_light_count; i++)
    {
        int index = total_images - total_patterns - direct_light_count - direct_light_offset + i + 1;
        direct_component_images.append(index);
        direct_component_images.append(index + total_patterns);
    }
    //QList<unsigned> direct_component_images(QList<unsigned>() << 15 << 16 << 17 << 18 << 35 << 36 << 37 << 38);
    foreach (unsigned i, direct_component_images)
    {
        images.push_back(get_image(level, i-1));
    }
    cv::Mat direct_light = sl::estimate_direct_light(images, b);
    processing_message("Estimate direct and global light components... done.");

    if (progress)
    {
        progress->setValue(50);
        progress->setLabelText("Decoding: projector column and row values...");
        processEvents();
    }

    std::vector<std::string> image_names;

    QModelIndex parent = model.index(level, 0);
    unsigned level_count = static_cast<unsigned>(model.rowCount(parent));
    for (unsigned i=0; i<level_count; i++)
    {
        QModelIndex index = model.index(i, 0, parent);
        std::string filename = model.data(index, ImageFilenameRole).toString().toStdString();
        std::cout << "[decode_set " << level << "] Filename: " << filename << std::endl;

        image_names.push_back(filename);
    }

    if (processing_canceled() || (progress && progress->wasCanceled()))
    {   //abort
        processing_set_current_message("Decode canceled");
        processing_message("Decode canceled");
        if (progress)
        {
            progress->close();
            delete progress;
            progress = NULL;
        }
        return false;
    }
    processEvents();

    processing_message("Decoding, please wait...");
    cv::Size projector_size(get_projector_width(), get_projector_height());
    bool rv = sl::decode_pattern(image_names, pattern_image, min_max_image, projector_size, sl::RobustDecode|sl::GrayPatternDecode, direct_light, m);

    if (progress)
    {
        progress->setValue(100);
        progress->setLabelText(QString("Decoding: %1").arg((rv?"finished":"failed")));
        processEvents();

        progress->close();
        delete progress;
        progress = NULL;
        processEvents();
    }

    return rv;
}

bool Application::load_calibration(QWidget * parent_widget)
{
    QString name = config.value("main/calibration_file", config.value("main/root_dir")).toString();
    QString filename = QFileDialog::getOpenFileName(parent_widget, "Open calibration", name, "Calibration (*.yml)");
    if (!filename.isEmpty() && calib.load_calibration(filename))
    {   //ok
        config.setValue("main/calibration_file", filename);
        mainWin.show_message(QString("Calibration loaded from %1").arg(filename));
        calib.display();
        return true;
    }
    if (!filename.isEmpty())
    {   //error
        QMessageBox::critical(parent_widget, "Error", QString("Calibration not loaded from %1").arg(filename));
    }
    return false;
}

bool Application::save_calibration(QWidget * parent_widget)
{
    if (!calib.is_valid())
    {   //invalid calibration
        QMessageBox::critical(parent_widget, "Error", "No valid calibration found.");
        return false;
    }
    QString name = config.value("main/calibration_file", config.value("main/root_dir")).toString();
    QString filename = QFileDialog::getSaveFileName(parent_widget, "Save calibration", name, "Calibration (*.yml *.m)");
    if (!filename.isEmpty() && calib.save_calibration(filename))
    {   //ok
        config.setValue("main/calibration_file", filename);
        mainWin.show_message(QString("Calibration saved to %1").arg(filename));
        calib.display();
        return true;
    }
    if (!filename.isEmpty())
    {   //error
        QMessageBox::critical(parent_widget, "Error", QString("Calibration not saved to %1").arg(filename));
    }
    return false;
}

void Application::reconstruct_model(int level, scan3d::Pointcloud & pointcloud, QWidget * parent_widget)
{
    if (level<0 || level>=model.rowCount())
    {   //invalid row
        return;
    }
    if (!calib.is_valid())
    {   //invalid calibration
        QMessageBox::critical(parent_widget, "Error", "No valid calibration found.");
        return;
    }

    //decode first
    decode(level, parent_widget);
    if (pattern_list.size()<=static_cast<size_t>(level) || min_max_list.size()<=static_cast<size_t>(level))
    {   //error: decode failed
        return;
    }

    cv::Mat pattern_image = pattern_list.at(level);
    cv::Mat min_max_image = min_max_list.at(level);;
    cv::Mat color_image = get_image(level, 0, ColorImageRole);

    if (!pattern_image.data || !min_max_image.data)
    {   //error: decode failed
        return;
    }

    cv::Size projector_size(get_projector_width(), get_projector_height());
    int threshold = config.value(THRESHOLD_CONFIG, THRESHOLD_DEFAULT).toInt();;
    double max_dist = config.value(MAX_DIST_CONFIG, MAX_DIST_DEFAULT).toDouble();;
    
    scan3d::reconstruct_model(pointcloud, calib, pattern_image, min_max_image, color_image, projector_size, threshold, max_dist, parent_widget);

    //debug: dump code to file
    /*
    QString path = config.value("main/root_dir").toString();
    QModelIndex index = model.index(level, 0);
    QString set_name = model.data(index, Qt::DisplayRole).toString();
    dump_decoded(qPrintable(QString("%1/%2/decode_dump.sl").arg(path).arg(set_name)), 0, pattern_image, min_max_image, color_image);
    */

    //save the projector view
    if (projector_view_list.size()<model.rowCount<size_t>())
    {
        projector_view_list.resize(model.rowCount());
    }
    pointcloud.colors.copyTo(projector_view_list[level]);
}

void Application::reconstruct_model_dump(cv::Mat2f const& pattern_image, cv::Mat2b const& min_max_image, cv::Mat3b const& color_image, scan3d::Pointcloud & pointcloud, QWidget * parent_widget)
{
    if (!pattern_image.data || !min_max_image.data || !color_image.data)
    {   //invalid dump
        return;
    }
    if (!calib.is_valid())
    {   //invalid calibration
        QMessageBox::critical(parent_widget, "Error", "No valid calibration found.");
        return;
    }

    cv::Size projector_size(get_projector_width(), get_projector_height());
    int threshold = config.value(THRESHOLD_CONFIG, THRESHOLD_DEFAULT).toInt();;
    double max_dist = config.value(MAX_DIST_CONFIG, MAX_DIST_DEFAULT).toDouble();;
    
    scan3d::reconstruct_model(pointcloud, calib, pattern_image, min_max_image, color_image, projector_size, threshold, max_dist, parent_widget);
}

void Application::compute_normals(scan3d::Pointcloud & pointcloud)
{
    scan3d::compute_normals(pointcloud);
}

void Application::get_chessboard_world_coords(std::vector<cv::Point3f> & world_corners, cv::Size corner_count, cv::Size corner_size)
{
    //generate world object coordinates
    for (int h=0; h<corner_count.height; h++)
    {
        for (int w=0; w<corner_count.width; w++)
        {
            world_corners.push_back(cv::Point3f(corner_size.width * w, corner_size.height * h, 0.f));
        }
    }
}

void Application::make_pattern_images(int level, cv::Mat & col_image, cv::Mat & row_image)
{
    col_image = cv::Mat();
    row_image = cv::Mat();

    if (level<0 ||level>=model.rowCount())
    {   //invalid level
        return;
    }
    if (pattern_list.size()<static_cast<size_t>(level) || min_max_list.size()<static_cast<size_t>(level))
    {   //no decoded
        return;
    }

    cv::Mat const& pattern_image = pattern_list.at(level);
    cv::Mat const& min_max_image = min_max_list.at(level);

    if (!pattern_image.data || !min_max_image.data)
    {   //no decoded
        return;
    }

    //apply threshold
    int threshold = config.value(THRESHOLD_CONFIG, THRESHOLD_DEFAULT).toInt();
    cv::Mat pattern_image_new = cv::Mat(pattern_image.size(), pattern_image.type());
    for (int h=0; h<pattern_image.rows; h++)
    {
        const cv::Vec2f * pattern_row = pattern_image.ptr<cv::Vec2f>(h);
        const cv::Vec2b * min_max_row = min_max_image.ptr<cv::Vec2b>(h);
        cv::Vec2f * pattern_new_row = pattern_image_new.ptr<cv::Vec2f>(h);
        for (int w=0; w<pattern_image.cols; w++)
        {
            cv::Vec2f const& pattern = pattern_row[w];
            cv::Vec2b const& min_max = min_max_row[w];
            cv::Vec2f & pattern_new = pattern_new_row[w];

            if (sl::INVALID(pattern) || (min_max[1]-min_max[0])<static_cast<int>(threshold))
            {   //invalid
                pattern_new = cv::Vec2f(sl::PIXEL_UNCERTAIN, sl::PIXEL_UNCERTAIN);
            }
            else
            {   //ok
                pattern_new = pattern;
            }
        }   //for each column
    }   //for each row

    col_image = sl::colorize_pattern(pattern_image_new, 0, get_projector_width(level));
    row_image = sl::colorize_pattern(pattern_image_new, 1, get_projector_height(level));
}

cv::Mat Application::get_projector_view(int level, bool force_update)
{
    if (level<0 || level>=model.rowCount())
    {   //invalid row
        return cv::Mat();
    }
    if (pattern_list.size()<=static_cast<size_t>(level))
    {   //not decoded
        return cv::Mat();
    }
    if (projector_view_list.size()<model.rowCount<size_t>())
    {
        projector_view_list.resize(model.rowCount());
    }

    cv::Mat & projector_image = projector_view_list[level];

    if (!projector_image.data || force_update)
    {   //make projector view with the current configuration
        int threshold = config.value(THRESHOLD_CONFIG, THRESHOLD_DEFAULT).toInt();

        cv::Mat pattern_image = pattern_list.at(level);
        cv::Mat min_max_image = min_max_list.at(level);;
        cv::Mat color_image = get_image(level, 0, ColorImageRole);
        cv::Size projector_size(get_projector_width(), get_projector_height());
    
        projector_image = scan3d::make_projector_view(pattern_image, min_max_image, color_image, projector_size, threshold);
    }

    return projector_image;
}

void Application::select_none(void)
{
    int count = model.rowCount();
    for (int i=0; i<count; i++)
    {
        model.setData(model.index(i, 0), Qt::Unchecked, Qt::CheckStateRole);
    }
}

void Application::select_all(void)
{
    int count = model.rowCount();
    for (int i=0; i<count; i++)
    {
        model.setData(model.index(i, 0), Qt::Checked, Qt::CheckStateRole);
    }
}
