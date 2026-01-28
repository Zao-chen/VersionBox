#include "backupfileindfo.h"
#include "ui_backupfileindfo.h"

#include <QFileInfo>

BackupFileIndfo::BackupFileIndfo(QString backupFilePath, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BackupFileIndfo)
{
    ui->setupUi(this);
    //获取文件名放到label
    QFileInfo fileInfo(backupFilePath);
    ui->label_Name->setText(fileInfo.fileName());
}

BackupFileIndfo::~BackupFileIndfo()
{
    delete ui;
}

/*打开文件*/
void BackupFileIndfo::on_pushButton_Open_clicked()
{

}

