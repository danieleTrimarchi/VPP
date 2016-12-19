#include "Notepad.h"
#include <QApplication>

// Minimal example found at :
// http://doc.qt.io/qt-5/gettingstartedqt.html
//
int main(int argc, char *argv[]) {

	QApplication a(argc, argv);
    Notepad w;
    w.show();

    return a.exec();
}
