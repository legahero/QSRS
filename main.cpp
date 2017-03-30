//#include "qsrsmainwindow.h"
//#include <QApplication>

#include <QCoreApplication>
#include <QDebug>
#include "server_win.h"

int main(int argc, char *argv[])
{
    /*
    QApplication a(argc, argv);
    QSRSMainWindow w;
    w.show();
    */
    QCoreApplication a(argc, argv);

    //这个调用SRS服务
    Win32Main(argc, argv);

    int iret= a.exec();

    //st_thread_exit(NULL);
    return iret;
}



