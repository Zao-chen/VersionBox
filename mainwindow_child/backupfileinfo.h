#ifndef BACKUPFILEINFO_H
#define BACKUPFILEINFO_H

#include <QWidget>

namespace Ui {
class BackupFileInfo;
}

class BackupFileInfo : public QWidget
{
    Q_OBJECT

public:
    explicit BackupFileInfo(QString backupFilePath, QString SourceFilePath, QWidget *parent = nullptr);
    ~BackupFileInfo();

private slots:
    void on_pushButton_Open_clicked();
    void on_pushButton_Restore_clicked();
    void on_pushButton_Delete_clicked();

private:
    Ui::BackupFileInfo *ui;
    QString m_backupFilePath;
    QString m_sourceFilePath;
};

#endif // BACKUPFILEINFO_H
