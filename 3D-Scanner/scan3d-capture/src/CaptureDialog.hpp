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

#ifndef __CAPTURENDIALOG_HPP__
#define __CAPTURENDIALOG_HPP__

#include <QDialog>

#include <opencv2/core/core.hpp>

#include "ui_CaptureDialog.h"

#include "ProjectorWidget.hpp"
#include "VideoInput.hpp"

class CaptureDialog : public QDialog, public Ui::CaptureDialog
{
    Q_OBJECT

public:
    CaptureDialog(QWidget * parent = 0, Qt::WindowFlags flags = Qt::WindowMaximizeButtonHint);
    ~CaptureDialog();

    inline void set_current_message(const QString & text) {current_message_label->setText(text);}
    void reset(void);
    inline void set_progress_total(unsigned value) {_total = value; progress_bar->setMaximum(_total);}
    inline void set_progress_value(unsigned value) {progress_bar->setValue(value);}

    void finish(void);

    inline bool canceled(void) const {return _cancel;}

    int update_screen_combo(void);
    int update_camera_combo(void);

    bool start_camera(void);
    void stop_camera(void);

    static void wait(int msecs);

public slots:
    void on_close_cancel_button_clicked(bool checked = false);
    void _on_root_dir_changed(const QString & dirname);
    void _on_new_projector_image(QPixmap image);
    void _on_new_camera_image(cv::Mat image);
    void _on_resolution_change();
    void on_screen_combo_currentIndexChanged(int index);
    void on_camera_combo_currentIndexChanged(int index);
    void on_output_dir_line_textEdited(const QString & text);
    void on_capture_button_clicked(bool checked = false);
    void on_output_dir_button_clicked(bool checked = false);
    void on_test_check_stateChanged(int state);
    void on_test_prev_button_clicked(bool checked = false);
    void on_test_next_button_clicked(bool checked = false);

    void auto_next();

private:
    ProjectorWidget _projector;
    VideoInput _video_input;
    volatile bool _capture;
    QString _session;
    int _wait_time;
    unsigned _total;
    bool _cancel;
};

#endif  /* __CAPTURENDIALOG_HPP__ */