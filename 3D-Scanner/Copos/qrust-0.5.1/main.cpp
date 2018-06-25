#include <qapplication.h>
#include "display/mainwindow.h"

using namespace std;
using namespace display;

extern void qInitImages_display();
extern void qCleanupImages_display();

/*
  The main program is here. 
*/

int main( int argc, char ** argv )
{
    QApplication a( argc, argv );
    qInitImages_display();
    MainWindow w;
    w.show();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    qCleanupImages_display();

    return a.exec();
}
