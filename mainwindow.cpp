#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "mainwindow_child/backupfileindfo.h"
#include <QProcess>
#include <QStandardPaths>
#include <QFileInfo>
#include <QDir>
#include <QFileDialog>
#include <QSettings>

/*配置文件地址*/
static QString configPath()
{
    const QString docs = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    return QDir(docs).filePath("VersionBox/config.ini");
}

/*构造函数*/
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

/*解构函数*/
MainWindow::~MainWindow()
{
    delete ui;
}

/*创建备份*/
void MainWindow::setDroppedPaths(const QStringList &paths)
{
    if (paths.isEmpty()) return;

    /*获取一些初始内容*/
    QString file = paths.first();
    QFileInfo fileInfo(file);
    QString suffix = fileInfo.suffix();

    /*读取配置文件*/
    QSettings ini(configPath(), QSettings::IniFormat);
    //FileType组里是否指定了文件打开方式
    ini.beginGroup("FileType");
    //检查组里面有没有suffix
    if (!ini.contains(suffix))
    {
        //如果没有，让用户选择一个打开方式
        QString exe = QFileDialog::getOpenFileName(this, "Select Application to Open *." + suffix, "", "Applications (*.exe);;All Files (*)");
    }
    ini.endGroup();

    /*文件打开*/
    QString exe = ini.value("FileType/" + suffix).toString();
    QProcess::startDetached(exe, QStringList() << file);

    /*文件备份*/
    //把file复制到文档文件夹下的VersionBox文件夹
    QString docPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString targetDir = docPath + "/VersionBox/Backup/"+fileInfo.completeBaseName()+"_"+fileInfo.completeSuffix();
    QDir dir;
    //如果没有文件夹就创建
    if (!dir.exists(targetDir))
    {
        dir.mkpath(targetDir);
    }
    //创建和复制文件
    QString targetFilePath = targetDir + "/" +
                             QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + "." +
                             fileInfo.suffix();
    QFile::copy(file, targetFilePath);
    LoadFileBackup(paths);
}

/*手动选择文件*/
void MainWindow::on_pushButton_SelectFile_clicked()
{
    //文件选择对话框
    QString fileName = QFileDialog::getOpenFileName(this, "Select File", "", "All Files (*)");
    if (!fileName.isEmpty())
    {
        setDroppedPaths(QStringList() << fileName);
    }
}

/*加载文件备份*/
void MainWindow::LoadFileBackup(const QStringList &paths)
{
    QString file = paths.first();
    QFileInfo fileInfo(file);
    //获取当前文件的所有备份
    QString docPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString targetDir = docPath + "/VersionBox/Backup/"+fileInfo.completeBaseName()+"_"+fileInfo.completeSuffix();
    QDir dir(targetDir);
    QStringList backupFiles = dir.entryList(QDir::Files, QDir::Time);

    //循环获取备份文件信息
    for (const QString &backupFile : std::as_const(backupFiles))
    {
        QString backupFilePath = targetDir + "/" + backupFile;
        //创建backupfileinfo窗口
        BackupFileIndfo* backupFileIndfo = new BackupFileIndfo(backupFilePath, this);
        ui->verticalLayout_BackupFiles->addWidget(backupFileIndfo);
    }
}

