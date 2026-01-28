#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>

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
    /*创建备份*/
    void setDroppedPaths(const QStringList& paths); //获取传递的文件路径
    /*加载文件备份*/
    void LoadFileBackup(const QStringList& path, const QString& SourceFilePath); //加载文件备份

protected:
    void closeEvent(QCloseEvent *event) override; //重写主窗口关闭函数

private slots:
    void on_showMainAction(); //打开设置
    void on_exitAppAction(); //退出程序

private:
    Ui::MainWindow *ui;
    QSystemTrayIcon *m_sysTrayIcon; //系统托盘
    QMenu *m_menu; //菜单
    QAction *m_setting; //显示设置
    QAction *m_exitAppAction; //退出程序
};
#endif // MAINWINDOW_H
