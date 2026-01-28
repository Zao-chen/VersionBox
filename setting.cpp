#include "setting.h"
#include "ui_setting.h"

#include <QSettings>
#include <QDir>
#include <QStandardPaths>

static QString configPath()
{
    const QString docs = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    return QDir(docs).filePath("VersionBox/config.ini");
}

setting::setting(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::setting)
{
    ui->setupUi(this);
    this->setWindowTitle("设置");
    qInfo()<<"打开设置";
    //读取配置文件中的备份数量设置
    QSettings ini(configPath(), QSettings::IniFormat);
    ini.beginGroup("General");
    int backupQuantity = ini.value("BackupQuantity", 100).toInt(); //默认10个备份
    ui->spinBox_BackupQuantity->setValue(backupQuantity);
    ini.endGroup();
    ui->spinBox_BackupQuantity->setValue(backupQuantity);
}

setting::~setting()
{
    delete ui;
}

/*修改最大备份数*/
void setting::on_spinBox_BackupQuantity_valueChanged(int arg1)
{
    qInfo()<<"修改最大备份数为"<<arg1;
    //写入配置文件
    QSettings ini(configPath(), QSettings::IniFormat);
    ini.beginGroup("General");
    ini.setValue("BackupQuantity", arg1);
    ini.endGroup();
}

