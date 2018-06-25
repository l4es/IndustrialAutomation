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


#include "MainWindow.hpp"

#include <QMessageBox>
#include <QFileDialog>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <QPainter>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QDesktopWidget>

#include <iostream>

#include "structured_light.hpp"

#include "Application.hpp"
#include "io_util.hpp"

#include "AboutDialog.hpp"
#include "CaptureDialog.hpp"
#include "CalibrationDialog.hpp"

MainWindow::MainWindow(QWidget * parent, Qt::WindowFlags flags): 
    QMainWindow(parent, flags)
{
    setupUi(this);
    
#ifdef Q_OS_OSX
    image_tree->setFocusPolicy(Qt::ClickFocus);
#endif //Q_OS_OSX

    image_tree->setModel(&(APP->model));
    connect(APP, &Application::root_dir_changed, this, &MainWindow::_on_root_dir_changed);
    connect(image_tree->selectionModel(), &QItemSelectionModel::currentChanged,
                this, &MainWindow::_on_image_tree_currentChanged);

    QSettings & config = APP->config;

    setWindowTitle(WINDOW_TITLE);

    threshold_spin->blockSignals(true);
    threshold_spin->setRange(0, 255);
    threshold_spin->setValue(config.value(THRESHOLD_CONFIG, THRESHOLD_DEFAULT).toUInt());
    threshold_spin->blockSignals(false);

    b_line->blockSignals(true);
    b_line->setValidator(new QDoubleValidator(0.0, 1.0, 6, this));
    b_line->setText(config.value(ROBUST_B_CONFIG, ROBUST_B_DEFAULT).toString());
    b_line->blockSignals(false);

    m_spin->blockSignals(true);
    m_spin->setRange(0, 255);
    m_spin->blockSignals(false);
    m_spin->setValue(config.value(ROBUST_M_CONFIG, ROBUST_M_DEFAULT).toUInt());

    corner_count_x_spin->blockSignals(true);
    corner_count_x_spin->setRange(1, 255);
    corner_count_x_spin->blockSignals(false);
    corner_count_x_spin->setValue(config.value("main/corner_count_x", DEFAULT_CORNER_X).toUInt());

    corner_count_y_spin->blockSignals(true);
    corner_count_y_spin->setRange(1, 255);
    corner_count_y_spin->blockSignals(false);
    corner_count_y_spin->setValue(config.value("main/corner_count_y", DEFAULT_CORNER_Y).toUInt());

    corners_width_line->blockSignals(true);
    corners_width_line->setValidator(new QDoubleValidator(this));
    corners_width_line->setText(config.value("main/corners_width", DEFAULT_CORNER_WIDTH).toString());
    corners_width_line->blockSignals(false);

    corners_height_line->blockSignals(true);
    corners_height_line->setValidator(new QDoubleValidator(this));
    corners_height_line->setText(config.value("main/corners_height", DEFAULT_CORNER_HEIGHT).toString());
    corners_height_line->blockSignals(false);

    homography_window_spin->blockSignals(true);
    homography_window_spin->setRange(0, 1024);
    homography_window_spin->setValue(config.value(HOMOGRAPHY_WINDOW_CONFIG, HOMOGRAPHY_WINDOW_DEFAULT).toUInt());
    homography_window_spin->blockSignals(false);

    display_original_radio->blockSignals(true);
    display_original_radio->setChecked(true);
    display_original_radio->blockSignals(false);

    max_dist_line->blockSignals(true);
    max_dist_line->setValidator(new QDoubleValidator(this));
    max_dist_line->setText(config.value(MAX_DIST_CONFIG, MAX_DIST_DEFAULT).toString());
    max_dist_line->blockSignals(false);

    normals_check->blockSignals(true);
    normals_check->setChecked(config.value(SAVE_NORMALS_CONFIG, SAVE_NORMALS_DEFAULT).toBool());
    normals_check->blockSignals(false);

    colors_check->blockSignals(true);
    colors_check->setChecked(config.value(SAVE_COLORS_CONFIG, SAVE_COLORS_DEFAULT).toBool());
    colors_check->blockSignals(false);

    binary_file_check->blockSignals(true);
    binary_file_check->setChecked(config.value(SAVE_BINARY_CONFIG, SAVE_BINARY_DEFAULT).toBool());
    binary_file_check->blockSignals(false);

    image2_label->setVisible(false);
    glwidget->setVisible(false);
    display_3dview_radio->setEnabled(false);

    //set contextual menu for layout changing
    QAction * horizontal_action = new QAction("Horizontal", current_image_group);
    QAction * vertical_action = new QAction("Vertical", current_image_group);
    QActionGroup * layout_action_group = new QActionGroup(current_image_group);
    layout_action_group->addAction(horizontal_action);
    layout_action_group->addAction(vertical_action);
    horizontal_action->setCheckable(true);
    vertical_action->setCheckable(true);
    horizontal_action->setChecked(true);
    vertical_action->setChecked(false);
    connect(horizontal_action, SIGNAL(triggered(bool)), this, SLOT(_on_horizontal_layout_action_triggered(bool)));
    connect(vertical_action, SIGNAL(triggered(bool)), this, SLOT(_on_vertical_layout_action_triggered(bool)));
    current_image_group->addActions(layout_action_group->actions());
    current_image_group->setContextMenuPolicy(Qt::ActionsContextMenu);

    show_message("Ready");
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_change_dir_action_triggered(bool checked)
{
    APP->change_root_dir(this);
}

void MainWindow::on_load_calibration_action_triggered(bool checked)
{
    APP->load_calibration(this);
    glwidget->update_camera();
}

void MainWindow::on_save_calibration_action_triggered(bool checked)
{
    APP->save_calibration(this);
}

void MainWindow::on_display_calibration_action_triggered(bool checked)
{
    CalibrationDialog dialog(this, Qt::WindowCloseButtonHint);
    dialog.exec();
}

void MainWindow::_on_image_tree_currentChanged(const QModelIndex & current, const QModelIndex & previous)
{
    //delete current image
    image1_label->clear();
    image2_label->clear();

    if (!current.isValid())
    {
        return;
    }

    Application * app = APP;

    unsigned level = 0, row = 0;
    QModelIndex parent = app->model.parent(current);

    if (parent.parent().isValid())
    {   //child
        level = parent.row();
        row   = current.row();
    }
    else
    {   //top level, select first child
        level = current.row();
        row   = 0;
    }
   
    cv::Mat image1, image2;

    //busy cursor
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QApplication::processEvents();
    
    if (display_original_radio->isChecked())
    {
        image1 = app->get_image(level, row, ColorImageRole);
    }

    if (display_decoded_radio->isChecked() && app->pattern_list.size()>level)
    {
       app->make_pattern_images(level, image1, image2);
    }

    if (display_projector_radio->isChecked())
    {
        image1 = app->get_projector_view(level);
    }

    //draw chessboard
    std::vector<cv::Point2f> corners;
    if (!display_projector_radio->isChecked() && level<APP->corners_camera.size())
    {
        corners = APP->corners_camera.at(level);
    }
    else if (display_projector_radio->isChecked() && level<APP->corners_projector.size())
    {
        corners = APP->corners_projector.at(level);
        const float scale_factor_x = image1.cols*1.f/APP->get_projector_width();
        const float scale_factor_y = image1.rows*1.f/APP->get_projector_height();
        for (auto it=corners.begin(); it!=corners.end(); ++it)
        {
            it->x *= scale_factor_x;
            it->y *= scale_factor_y;
        }
    }
    if (corners.size()>0)
    {
        if (image1.rows)
        {
            image1 = image1.clone();
            //cv::drawChessboardCorners(image1, APP->chessboard_size, corners, true);

            for (auto it=corners.cbegin(); it!=corners.cend(); ++it)
            {
                cv::circle(image1, *it, 6, CV_RGB(255,0,0), 2);
            }
        }
        if (image2.rows)
        {
            image2 = image2.clone();
            //cv::drawChessboardCorners(image2, APP->chessboard_size, corners, true);

            for (auto it=corners.cbegin(); it!=corners.cend(); ++it)
            {
                cv::circle(image2, *it, 6, CV_RGB(255,0,0), 2);
            }
        }
    }

    //update the viewer
    image1_label->setPixmap(QPixmap::fromImage(io_util::qImage(image1)));
    image2_label->setPixmap(QPixmap::fromImage(io_util::qImage(image2)));

    //restore regular cursor
    QApplication::restoreOverrideCursor();
    QApplication::processEvents();
}

void MainWindow::show_message(const QString & message)
{
    if (!message.isEmpty())
    {
        statusBar()->showMessage(message);
    }
    else
    {
        statusBar()->clearMessage();
    }
    QApplication::processEvents();
}

void MainWindow::on_corner_count_x_spin_valueChanged(int i)
{
    APP->config.setValue("main/corner_count_x", i);
}

void MainWindow::on_corner_count_y_spin_valueChanged(int i)
{
    APP->config.setValue("main/corner_count_y", i);
}

void MainWindow::on_corners_width_line_editingFinished()
{
    APP->config.setValue("main/corners_width", corners_width_line->text().toDouble());
}

void MainWindow::on_corners_height_line_editingFinished()
{
    APP->config.setValue("main/corners_height", corners_height_line->text().toDouble());
}

void MainWindow::on_threshold_button_clicked(bool checked)
{
    int row = get_current_set();
    if (row<0)
    {   //nothing selected
        return;
    }

    APP->get_projector_view(row, true);
    update_current_image();
}

void  MainWindow::on_threshold_spin_valueChanged(int i)
{
    APP->config.setValue(THRESHOLD_CONFIG, i);
}

void MainWindow::on_b_line_editingFinished()
{
    APP->config.setValue(ROBUST_B_CONFIG, b_line->text().toDouble());
}

void MainWindow::on_m_spin_valueChanged(int i)
{
    APP->config.setValue(ROBUST_M_CONFIG, i);
}

void MainWindow::on_homography_window_spin_valueChanged(int i)
{
  APP->config.setValue(HOMOGRAPHY_WINDOW_CONFIG, i);
}

void  MainWindow::on_max_dist_line_editingFinished()
{
    APP->config.setValue(MAX_DIST_CONFIG, max_dist_line->text().toDouble());
}

void MainWindow::on_normals_check_stateChanged(int state)
{
    APP->config.setValue(SAVE_NORMALS_CONFIG, (state==Qt::Checked));
}

void MainWindow::on_colors_check_stateChanged(int state)
{
    APP->config.setValue(SAVE_COLORS_CONFIG, (state==Qt::Checked));
}

void MainWindow::on_binary_file_check_stateChanged(int state)
{
    APP->config.setValue(SAVE_BINARY_CONFIG, (state==Qt::Checked));
}

void MainWindow::on_quit_action_triggered(bool checked)
{
    close();
    APP->quit();
}

void MainWindow::on_save_vertical_image_action_triggered(bool checked)
{
    if (image1_label->pixmap()->isNull())
    {
        QMessageBox::critical(this, "Error", "Vertical image is empy.");
        return;
    }
    QString filename = QFileDialog::getSaveFileName(this, "Save vertical image", "saved_image_vertical.png", "Images (*.png)");
    if (!filename.isEmpty())
    {
        image1_label->pixmap()->save(filename);
        show_message(QString("Vertical Image saved: %1").arg(filename));
    }
}

void MainWindow::on_save_horizontal_image_action_triggered(bool checked)
{
    if (image2_label->pixmap()->isNull())
    {
        QMessageBox::critical(this, "Error", "Horizontal image is empy.");
        return;
    }
    QString filename = QFileDialog::getSaveFileName(this, "Save horizontal image", "saved_image_horizontal.png", "Images (*.png)");
    if (!filename.isEmpty())
    {
        image2_label->pixmap()->save(filename);
        show_message(QString("Horizontal Image saved: %1").arg(filename));
    }
}

void MainWindow::_on_root_dir_changed(const QString & dirname)
{
    //update user interface
    image1_label->clear();
    image2_label->clear();

    display_original_radio->blockSignals(true);
    display_original_radio->setChecked(true);
    display_original_radio->blockSignals(false);

    image1_label->setVisible(true);
    image2_label->setVisible(false);
    glwidget->setVisible(false);

    setWindowTitle(QString("%1 - %2").arg(WINDOW_TITLE, dirname));

    QModelIndex index = APP->model.index(0, 0);
    image_tree->selectionModel()->clearSelection();
    if (APP->model.rowCount()>0)
    {
        image_tree->blockSignals(true);
        image_tree->selectionModel()->select(index, QItemSelectionModel::SelectCurrent);
        _on_image_tree_currentChanged(index, index);
        image_tree->blockSignals(false);
        show_message(QString("%1 set read").arg(APP->model.rowCount()));
    }
}

void MainWindow::on_display_original_radio_clicked(bool checked)
{
    if (checked)
    {
        image1_label->setVisible(true);
        image2_label->setVisible(false);
        glwidget->setVisible(false);
        update_current_image();
    }
}

void MainWindow::on_display_decoded_radio_clicked(bool checked)
{
    if (checked)
    {
        image1_label->setVisible(true);
        image2_label->setVisible(true);
        glwidget->setVisible(false);
        update_current_image();
    }
}

void MainWindow::on_display_projector_radio_clicked(bool checked)
{
    if (checked)
    {
        image1_label->setVisible(true);
        image2_label->setVisible(false);
        glwidget->setVisible(false);
        update_current_image();
    }
}

void MainWindow::on_display_3dview_radio_clicked(bool checked)
{
    if (checked)
    {
        image1_label->setVisible(false);
        image2_label->setVisible(false);
        glwidget->setVisible(true);
    }
}

void MainWindow::on_about_action_triggered(bool checked)
{
    AboutDialog dialog(this, Qt::WindowCloseButtonHint);
    dialog.exec();
}

void MainWindow::update_current_image(QModelIndex current)
{
    QModelIndex index = current;
    if (!index.isValid())
    {
        index = image_tree->selectionModel()->currentIndex();
    }
    if (!index.isValid())
    {
        index = APP->model.index(0, 0);
    }
    _on_image_tree_currentChanged(index, index);
}

void MainWindow::on_extract_corners_button_clicked(bool checked)
{
    show_message("Searching chessboard corners...");

    APP->processing_reset();
    APP->processingDialog.setWindowTitle("Corner detection");
    APP->processingDialog.show();
    QApplication::processEvents();

    APP->extract_chessboard_corners();
    update_current_image();

    APP->processingDialog.finish();
    APP->processingDialog.exec();
    APP->processingDialog.hide();
    QApplication::processEvents();

    show_message("Ready");
}

void MainWindow::on_calibrate_button_clicked(bool checked)
{
    show_message("Running calibration...");

    APP->processing_reset();
    APP->processingDialog.setWindowTitle("Calibration");
    APP->processingDialog.show();
    QApplication::processEvents();

    APP->calibrate();
    update_current_image();
    glwidget->update_camera();

    APP->processingDialog.finish();
    APP->processingDialog.exec();
    APP->processingDialog.hide();
    QApplication::processEvents();

    show_message("Ready");
}

void MainWindow::on_decode_button_clicked(bool checked)
{
    show_message("Decoding...");

    APP->processing_reset();
    APP->processingDialog.setWindowTitle("Decode");
    APP->processingDialog.show();
    QApplication::processEvents();

    APP->decode_all();
    update_current_image();

    APP->processingDialog.finish();
    APP->processingDialog.exec();
    APP->processingDialog.hide();
    QApplication::processEvents();

    show_message("Ready");
}

void MainWindow::on_capture_button_clicked(bool checked)
{
    CaptureDialog dialog(this);
    dialog.setModal(true);
    dialog.exec();
}

void MainWindow::_on_horizontal_layout_action_triggered(bool checked)
{
    QLayout *old_layout = current_image_group->layout();    
    if (old_layout)
    {
        delete old_layout;
    }
    QHBoxLayout *new_layout = new QHBoxLayout(current_image_group);
    new_layout->addWidget(image1_label);
    new_layout->addWidget(image2_label);
    new_layout->addWidget(glwidget);
    current_image_group->setLayout(new_layout);
}

void MainWindow::_on_vertical_layout_action_triggered(bool checked)
{
    QLayout *old_layout = current_image_group->layout();
    if (old_layout)
    {
        delete old_layout;
    }
    QVBoxLayout *new_layout = new QVBoxLayout(current_image_group);
    new_layout->addWidget(image1_label);
    new_layout->addWidget(image2_label);
    new_layout->addWidget(glwidget);
    current_image_group->setLayout(new_layout);
}

void MainWindow::on_reconstruct_button_clicked(bool checked)
{
    int row = get_current_set();
    if (row<0)
    {   //nothing selected
        return;
    }

    show_message("Reconstruction...");

    //parameters
    bool normals = APP->config.value(SAVE_NORMALS_CONFIG, SAVE_NORMALS_DEFAULT).toBool();
    bool colors = APP->config.value(SAVE_COLORS_CONFIG, SAVE_COLORS_DEFAULT).toBool();
    bool binary = APP->config.value(SAVE_BINARY_CONFIG, SAVE_BINARY_DEFAULT).toBool();

    scan3d::Pointcloud & pointcloud = APP->pointcloud;
    APP->reconstruct_model(row, pointcloud, this);
    if (!pointcloud.points.data)
    {   //no points: reconstruction canceled or failed
        show_message("Reconstruction failed");
        return;
    }

    //compute normals
    if (normals)
    {
        //busy cursor
        show_message("Computing normals...");
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        QApplication::processEvents();

        APP->compute_normals(pointcloud);

        //restore regular cursor
        QApplication::restoreOverrideCursor();
        QApplication::processEvents();
    }

    //save the points
    QString name = APP->get_root_dir()+"/"+APP->model.data(APP->model.index(row, 0), Qt::DisplayRole).toString();
    QString filename = QFileDialog::getSaveFileName(this, "Save pointcloud", name+".ply", "Pointclouds (*.ply)");
    if (!filename.isEmpty())
    {
        //busy cursor
        show_message(QString("Saving to %1...").arg(filename));
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        QApplication::processEvents();

        unsigned ply_flags = io_util::PlyPoints
                            | (colors?io_util::PlyColors:0)
                            | (normals?io_util::PlyNormals:0)
                            | (binary?io_util::PlyBinary:0);

        io_util::write_ply(filename.toStdString(), pointcloud, ply_flags);

        //restore regular cursor
        QApplication::restoreOverrideCursor();
        QApplication::processEvents();
        show_message(QString("Pointcloud saved: %1").arg(filename));
        std::cout << QString("Pointcloud saved: %1").arg(filename).toStdString() << std::endl;
    }
}

void MainWindow::on_reconstruct_dump_action_triggered(bool checked)
{
    QString root_dir = APP->get_root_dir();
    QString filename = QFileDialog::getOpenFileName(this, "Open decoded files", root_dir, "Decoded dump (*.sl)");
    if (filename.isEmpty())
    {
        return;
    }

    int type = 0;
    cv::Mat2f pattern_image;
    cv::Mat2b min_max_image; 
    cv::Mat3b color_image;
    APP->load_dump(qPrintable(filename), type, pattern_image, min_max_image, color_image);

    show_message("Reconstruction...");

    //parameters
    bool normals = APP->config.value(SAVE_NORMALS_CONFIG, SAVE_NORMALS_DEFAULT).toBool();
    bool colors = APP->config.value(SAVE_COLORS_CONFIG, SAVE_COLORS_DEFAULT).toBool();
    bool binary = APP->config.value(SAVE_BINARY_CONFIG, SAVE_BINARY_DEFAULT).toBool();

    scan3d::Pointcloud & pointcloud = APP->pointcloud;
    APP->reconstruct_model_dump(pattern_image, min_max_image, color_image, pointcloud, this);

    if (!pointcloud.points.data)
    {   //no points: reconstruction canceled or failed
        show_message("Reconstruction failed");
        return;
    }

    //compute normals
    if (normals)
    {
        //busy cursor
        show_message("Computing normals...");
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        QApplication::processEvents();

        APP->compute_normals(pointcloud);

        //restore regular cursor
        QApplication::restoreOverrideCursor();
        QApplication::processEvents();
    }

    //save the points
    QString name = root_dir+"/pointcloud";
    filename = QFileDialog::getSaveFileName(this, "Save pointcloud", name+".ply", "Pointclouds (*.ply)");
    if (!filename.isEmpty())
    {
        //busy cursor
        show_message(QString("Saving to %1...").arg(filename));
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        QApplication::processEvents();

        unsigned ply_flags = io_util::PlyPoints
                            | (colors?io_util::PlyColors:0)
                            | (normals?io_util::PlyNormals:0)
                            | (binary?io_util::PlyBinary:0);

        io_util::write_ply(filename.toStdString(), pointcloud, ply_flags);

        //restore regular cursor
        QApplication::restoreOverrideCursor();
        QApplication::processEvents();
        show_message(QString("Pointcloud saved: %1").arg(filename));
        std::cout << QString("Pointcloud saved: %1").arg(filename).toStdString() << std::endl;
    }

}

int MainWindow::get_current_set(void)
{
    QModelIndex index = image_tree->selectionModel()->currentIndex();
    if (!index.isValid())
    {
        index = APP->model.index(0, 0);
    }
    while (index.parent().parent().isValid())
    {
        index = index.parent();
    }
    if (!index.isValid())
    {
        return -1;
    }
    return index.row();
}

void MainWindow::on_select_all_button_clicked(bool checked)
{
    APP->select_all();
}

void MainWindow::on_select_none_button_clicked(bool checked)
{
    APP->select_none();
}
