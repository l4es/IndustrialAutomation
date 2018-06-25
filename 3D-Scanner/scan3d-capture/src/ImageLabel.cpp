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


#include "ImageLabel.hpp"

#include <QPainter>

#include "io_util.hpp"

ImageLabel::ImageLabel(QWidget * parent, Qt::WindowFlags flags): 
    QWidget(parent, flags),
    _pixmap(),
    _image(),
    _mutex()
{
}

ImageLabel::~ImageLabel()
{
}

void ImageLabel::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    if (_image.data && _image.type()==CV_8UC3)
    {   //copy cv::Mat image
        _mutex.lock();
        _pixmap = QPixmap::fromImage(io_util::qImage(_image));
        _image = cv::Mat();
        _mutex.unlock();
    }

    if (!_pixmap.isNull())
    {
        QPixmap scale_pixmap = _pixmap.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QRectF rect = QRectF(QPointF(0,0), QPointF(scale_pixmap.width(),scale_pixmap.height()));
        painter.drawPixmap(rect, scale_pixmap, rect);
    }
    else 
    {
        QRectF rect = QRectF(QPointF(0,0), QPointF(width(),height()));
        painter.drawText(rect, Qt::AlignCenter, "No image");
    }
}

void ImageLabel::setImage(cv::Mat const& image)
{
    //save image as cv::Mat because QPixmap's cannot be used in threads
    _mutex.lock();
    image.copyTo(_image);
    _mutex.unlock();
    update();
}