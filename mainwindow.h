#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setDroppedPaths(const QStringList& paths); //获取传递的文件路径
    /*加载文件备份*/
    void LoadFileBackup(const QStringList& paths); //加载文件备份

private slots:
    void on_pushButton_SelectFile_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
