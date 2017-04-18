#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //prevent multiple instance
    QSharedMemory sharedMemory;
    sharedMemory.setKey("DAQ_GUI-Key");
    if( sharedMemory.create(1) == false){
        QMessageBox::warning(NULL, "Warning!", "Another instance alredy running!");
        a.exit();
        return 0;
    }

    MainWindow w;

    //w.setWindowFlags(Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint);

    w.show();
    return a.exec();
}
