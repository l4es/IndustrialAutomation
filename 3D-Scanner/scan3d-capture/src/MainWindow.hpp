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

#ifndef __MAINWINDOW_HPP__
#define __MAINWINDOW_HPP__

#include <QMainWindow>

#include "ui_MainWindow.h"

class MainWindow : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget * parent = 0, Qt::WindowFlags flags = 0);
    ~MainWindow();

    void update_current_image(QModelIndex current = QModelIndex());

public slots:
    //menu actions
    void on_change_dir_action_triggered(bool checked = false);
    void on_save_vertical_image_action_triggered(bool checked = false);
    void on_save_horizontal_image_action_triggered(bool checked = false);
    void on_quit_action_triggered(bool checked = false);
    void on_load_calibration_action_triggered(bool checked = false);
    void on_save_calibration_action_triggered(bool checked = false);
    void on_display_calibration_action_triggered(bool checked = false);
    void on_reconstruct_dump_action_triggered(bool checked = false);
    void on_about_action_triggered(bool checked = false);

    //buttons
    void on_select_all_button_clicked(bool checked = false);
    void on_select_none_button_clicked(bool checked = false);
    void on_change_dir_button_clicked(bool checked = false) {on_change_dir_action_triggered(checked);}
    void on_extract_corners_button_clicked(bool checked = false);
    void on_decode_button_clicked(bool checked = false);
    void on_calibrate_button_clicked(bool checked = false);
    void on_capture_button_clicked(bool checked = false);
    void on_reconstruct_button_clicked(bool checked = false);

    //other
    void _on_image_tree_currentChanged(const QModelIndex & current, const QModelIndex & previous);
    void _on_root_dir_changed(const QString & dirname);

    //display group
    void on_display_original_radio_clicked(bool checked);
    void on_display_decoded_radio_clicked(bool checked);
    void on_display_projector_radio_clicked(bool checked);
    void on_display_3dview_radio_clicked(bool checked);

    //checkerboard
    void on_corner_count_x_spin_valueChanged(int i);
    void on_corner_count_y_spin_valueChanged(int i);
    void on_corners_width_line_editingFinished();
    void on_corners_height_line_editingFinished();

    //decode group
    void on_threshold_button_clicked(bool checked);
    void on_threshold_spin_valueChanged(int i);
    void on_b_line_editingFinished();
    void on_m_spin_valueChanged(int i);

    //calibration group
    void on_homography_window_spin_valueChanged(int i);

    //reconstruction group
    void on_max_dist_line_editingFinished();
    void on_normals_check_stateChanged(int state);
    void on_colors_check_stateChanged(int state);
    void on_binary_file_check_stateChanged(int state);

    //switch horizontal/vertical image display
    void _on_horizontal_layout_action_triggered(bool checked);
    void _on_vertical_layout_action_triggered(bool checked);

    void show_message(const QString & message = QString());

private:
    int get_current_set(void);
};

#endif  /* __MAINWINDOW_HPP__ */