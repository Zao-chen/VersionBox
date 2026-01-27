#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    /*传递路径到Mainwindow*/
    QStringList args = QCoreApplication::arguments();
    args.removeFirst(); // args[0] 是程序自身路径，去掉
    w.setDroppedPaths(args);

    w.show();
    return a.exec();
}
