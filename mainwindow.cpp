q#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QProcess>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setDroppedPaths(const QStringList &paths)
{
    if (paths.isEmpty())
    {
        setWindowTitle("No file dropped");
        return;
    }
    setWindowTitle(paths.first());

    QString exe = R"(S:\Notepad++\notepad++.exe)";
    QString file = paths.first();
    QProcess::startDetached(exe, QStringList() << file);
    qDebug()<<"ok";


}
