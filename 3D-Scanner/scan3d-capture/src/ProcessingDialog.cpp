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


#include "ProcessingDialog.hpp"


ProcessingDialog::ProcessingDialog(QWidget * parent, Qt::WindowFlags flags): 
    QDialog(parent, flags),
    _total(0),
    _cancel(false)
{
    setupUi(this);
}

ProcessingDialog::~ProcessingDialog()
{
}

void ProcessingDialog::reset(void)
{
    progress_bar->setMaximum(0);
    current_message_label->clear();
    message_text->clear();
    close_cancel_button->setText("Cancel");
    _cancel = false;
}

void ProcessingDialog::finish(void)
{
    //progress_bar->setValue(_total);
    close_cancel_button->setText("Close");
}

void ProcessingDialog::on_close_cancel_button_clicked(bool checked)
{
    if (close_cancel_button->text()=="Close")
    {
        accept();
    }
    else if (!_cancel)
    {
        _cancel = true;
        message("CANCEL: waiting the current operation to finish (might take a little)");
    }
}

void ProcessingDialog::message(const QString & text)
{
    message_text->append(text);
}
