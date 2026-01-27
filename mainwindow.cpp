#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QProcess>
#include <QStandardPaths>
#include <QFileInfo>
#include <QDir>

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

    QString file = paths.first();
    QString exe = R"(S:\Notepad++\notepad++.exe)";
    QProcess::startDetached(exe, QStringList() << file);

    //把file复制到文档文件夹下的VersionBox文件夹
    QString docPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString targetDir = docPath + "/VersionBox";
    QDir dir;
    if (!dir.exists(targetDir)) //如果没有文件夹就创建
    {
        dir.mkpath(targetDir);
    }
    QFileInfo fileInfo(file);
    //名称改为原名+时间
    QString targetFilePath = targetDir + "/" + fileInfo.completeBaseName() + "_" +
                             QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + "." +
                             fileInfo.suffix();
    QFile::copy(file, targetFilePath);

}
