/****************************************************************************
 ** Form interface generated from reading ui file 'dlggenerate.ui'
 **
 ** Created: mer nov 30 17:55:40 2005
 **      by: The User Interface Compiler ($Id: dlggenerate.h,v 1.1.1.1 2006/01/16 18:28:14 cirdan Exp $)
 **
 ** WARNING! All changes made in this file will be lost!
 ****************************************************************************/

#ifndef DLGGENERATE_H
#define DLGGENERATE_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qdialog.h>

#include "./viewer.h"
#include "./renderobject.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class MyCustomWidget;
class QGroupBox;
class QPushButton;
class QButtonGroup;
class QRadioButton;
class QSlider;

namespace display {
  class DlgGenerate : public QDialog
  {
    Q_OBJECT

  public:
    DlgGenerate( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~DlgGenerate();
    void  addObject (RenderObject* object);
    Viewer* glView;
    void  addWidget (QWidget* widget);
  private slots:
    virtual void btn_save_clicked();
    virtual void rdb_flat_toggled( bool );

  protected:
    QGridLayout* DlgGenerateLayout;
    QVBoxLayout* vbx_menu;
    QSpacerItem* spacer;
    QVBoxLayout* btg_displayLayout;
    QVBoxLayout* gbx_fileLayout;

  protected slots:
    virtual void languageChange();

  private:
    QButtonGroup* btg_display;
    QRadioButton* rdb_wire;
    QRadioButton* rdb_flat;
    QGroupBox* gbx_file;
    QPushButton* btn_exit;
    QPushButton* btn_save;
    std::list<RenderObject*> objects;
  };
}
#endif // DLGGENERATE_H
