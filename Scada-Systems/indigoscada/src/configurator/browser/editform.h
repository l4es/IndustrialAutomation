/****************************************************************************
** Form interface generated from reading ui file 'editform.ui'
**
** Created: Sat May 5 20:10:28 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef EDITFORM_H
#define EDITFORM_H

#include <qvariant.h>
#include <qdialog.h>
#include <qimage.h>
#include <qapplication.h>
#include <qfile.h>
#include <stdlib.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QToolButton;
class QWidgetStack;
class QWidget;
class QTextEdit;
class QLabel;
class QFrame;
class QPushButton;

class editForm : public QDialog
{
    Q_OBJECT

public:
    editForm( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~editForm();

    QToolButton* importButton;
    QToolButton* exportButton;
    QToolButton* clearButton;
    QWidgetStack* editWidgetStack;
    QWidget* WStackPage;
    QTextEdit* textEditor;
    QWidget* WStackPage_2;
    QLabel* editPixmap;
    QFrame* frame9;
    QLabel* currentTypeLabel;
    QLabel* currentDataInfo;
    QPushButton* closeButton;
    QPushButton* saveChangesButton;

    int curCol;
    int curRow;

public slots:
    virtual void reset();
    virtual void setModified( bool modifiedstate );
    virtual void enableExport( bool enabled );
    virtual void enableTextEditor( bool enabled );
    virtual void setDataType( int type, int size );
    virtual void closeEvent( QCloseEvent * );
    virtual void loadText( QString text, int row, int col );
    virtual void importData();
    virtual void exportData();
    virtual void clearData();
    virtual void saveChanges();
    virtual void editTextChanged();
    virtual void editorClicked();

signals:
    void goingAway();
    void updateRecordText(int, int, QString);
    void updateRecordBinary(int, int, unsigned char *);

protected:
    bool modified;
    int dataType;
    int dataSize;
    int dataDepth;

    QVBoxLayout* editFormLayout;
    QHBoxLayout* layout11;
    QGridLayout* WStackPageLayout;
    QGridLayout* WStackPageLayout_2;
    QVBoxLayout* frame9Layout;
    QHBoxLayout* layout3;

protected slots:
    virtual void languageChange();

private:
    void init();
    void destroy();

};

#endif // EDITFORM_H
