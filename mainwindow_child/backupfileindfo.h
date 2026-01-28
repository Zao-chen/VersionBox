#ifndef BACKUPFILEINDFO_H
#define BACKUPFILEINDFO_H

#include <QWidget>

namespace Ui {
class BackupFileIndfo;
}

class BackupFileIndfo : public QWidget
{
    Q_OBJECT

public:
    explicit BackupFileIndfo(QString backupFilePath, QWidget *parent = nullptr);
    ~BackupFileIndfo();

private slots:
    void on_pushButton_Open_clicked();

private:
    Ui::BackupFileIndfo *ui;
};

#endif // BACKUPFILEINDFO_H
