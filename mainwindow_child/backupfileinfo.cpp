#include "backupfileinfo.h"
#include "ui_backupfileinfo.h"

#include <QFileInfo>
#include <QDir>
#include <QSettings>
#include <QStandardPaths>
#include <QProcess>
#include <QMessageBox>

/*配置文件地址*/
static QString configPath()
{
    const QString docs = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    return QDir(docs).filePath("VersionBox/config.ini");
}

BackupFileInfo::BackupFileInfo(QString backupFilePath, QString SourceFilePath, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BackupFileInfo)
{
    ui->setupUi(this);
    qDebug()<<"备份文件"<<backupFilePath;
    qDebug()<<"源文件"<<SourceFilePath;
    m_backupFilePath = backupFilePath; //备份文件路径
    m_sourceFilePath = SourceFilePath; //源文件路径
    //获取文件名放到label
    QFileInfo fileInfo(backupFilePath);
    ui->label_Name->setText(fileInfo.fileName());
}

BackupFileInfo::~BackupFileInfo()
{
    delete ui;
}

/*打开文件*/
void BackupFileInfo::on_pushButton_Open_clicked()
{

    /*获取一些初始内容*/
    QFileInfo fileInfo(m_backupFilePath);
    QString suffix = fileInfo.suffix();

    /*读取配置文件*/
    QSettings ini(configPath(), QSettings::IniFormat);
    //FileType组里是否指定了文件打开方式
    ini.beginGroup("FileType");
    //获取打开方式
    QString exe = ini.value(suffix).toString();
    ini.endGroup();

    /*文件打开*/
    QProcess::startDetached(exe, QStringList() << m_backupFilePath);

}

/*还原备份*/
void BackupFileInfo::on_pushButton_Restore_clicked()
{
    //警告是否确认覆盖
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认还原", "确定要还原备份吗？这将覆盖当前文件！",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        //用备份文件覆盖原文件
        QFile::remove(m_sourceFilePath);
        QFile::copy(m_backupFilePath, m_sourceFilePath);
    }
}

/*删除备份*/
void BackupFileInfo::on_pushButton_Delete_clicked()
{
    //警告是否确认删除
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认删除", "确定要删除该备份吗？",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        //删除备份文件
        QFile::remove(m_backupFilePath);
        //从界面上移除
        this->deleteLater();
    }
}

