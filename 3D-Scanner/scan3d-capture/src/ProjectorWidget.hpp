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

#ifndef __PROJECTORWIDGET_HPP__
#define __PROJECTORWIDGET_HPP__

#include <QWidget>

class ProjectorWidget : public QWidget
{
    Q_OBJECT

public:
    ProjectorWidget(QWidget * parent = 0, Qt::WindowFlags flags = 0);
    ~ProjectorWidget();

    void reset(void);
    inline void set_screen(int screen) {_screen = screen;}
    inline void set_pattern_count(int count) {_pattern_count = count;}
    inline int get_current_pattern(void) const {return _current_pattern;}

    //projection cycle
    void start(void);
    void stop(void);
    void prev(void);
    void next(void);
    bool finished(void);

    void clear() { _pixmap = QPixmap(); update(); }
    const QPixmap * pixmap() const {return &_pixmap;}
    void setPixmap(const QPixmap & pixmap) { _pixmap = pixmap; update(); }

    inline bool is_updated(void) const {return _updated;}
    inline void clear_updated(void) {_updated = false;}

    bool save_info(QString const& filename, bool invert) const;

signals:
    void new_image(QPixmap image);

protected:
    virtual void paintEvent(QPaintEvent *);

    void make_pattern(void);
    void update_pattern_bit_count(void);
    static QPixmap make_pattern(int rows, int cols, int vmask, int voffset, int hmask, int hoffset, int inverted);

private:
    int _screen;
    QPixmap _pixmap;
    int _current_pattern;
    int _pattern_count;
    int _vbits;
    int _hbits;
    volatile bool _updated;

};


#endif  /* __PROJECTORWIDGET_HPP__ */
