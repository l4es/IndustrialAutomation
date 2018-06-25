/****************************************************************************
 ** Form interface generated from reading ui file 'mainwindow.ui'
 **
 ** Created: lun nov 28 17:53:21 2005
 **      by: The User Interface Compiler ($Id: mainwindow.h,v 1.1.1.1 2006/01/16 18:28:15 cirdan Exp $)
 **
 ** WARNING! All changes made in this file will be lost!
 ****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qmainwindow.h>

#include "./octreeview.h"
#include "./alphaview.h"
#include "./crustview.h"
#include "./viewer.h"
#include "./dlggenerate.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QAction;
class QActionGroup;
class QToolBar;
class QPopupMenu;
class MyCustomWidget;
class QSlider;

/**
 * Namespace
 */
namespace display {
  class MainWindow : public QMainWindow
  {
    Q_OBJECT

  public:
    MainWindow( QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel );
    ~MainWindow();
    
  protected slots:
    virtual void fileOpen();
    virtual void helpAbout();
    virtual void sld_granularity_valueChanged( int );
    virtual void fileSave();
    virtual void fileSave_as();
    virtual void editUndo();
    virtual void editDelete();
    virtual void generateAlphaShape();
    virtual void generateCrust();
    virtual void languageChange();

  private:
    QGridLayout* MainWindowLayout;
    QSlider* sld_granularity;
    QMenuBar *MenuBar;
    QPopupMenu *fileMenu;
    QPopupMenu *Edit;
    QPopupMenu *Generate;
    QPopupMenu *helpMenu;
    QToolBar *toolBar;
    QAction* fileOpenAction;
    QAction* fileExitAction;
    QAction* helpAboutAction;
    QAction* editUndoAction;
    QAction* fileSaveAction;
    QAction* editDeleteAction;
    QAction* generateAlpha_ShapeAction;
    QAction* generateCrustAction;
    QAction* fileSave_asAction;
    Viewer* glView;
    OctreeView* octreeview;
    QString filename;
  };
}
#endif // MAINWINDOW_H
