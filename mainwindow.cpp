#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "mainwindow_child/backupfileinfo.h"
#include "setting.h"

#include <QProcess>
#include <QStandardPaths>
#include <QFileInfo>
#include <QDir>
#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>
#include <QRegularExpression>
#include <QCryptographicHash>
#include <QMenu>
#include <QCloseEvent>

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

    /*创建托盘*/
    m_sysTrayIcon = new QSystemTrayIcon(this); //新建QSystemTrayIcon对象
    //创建菜单
    m_menu = new QMenu(this);
    m_setting = new QAction("设置", this);
    m_exitAppAction = new QAction("退出", this);
    m_menu->addAction(m_setting); //新增菜单项---设置
    m_menu->addAction(m_exitAppAction); //新增菜单项---退出程序
    m_sysTrayIcon->setContextMenu(m_menu); //把QMenu赋给QSystemTrayIcon对象
    connect(m_setting,SIGNAL(triggered()),this,SLOT(on_showMainAction()));
    connect(m_exitAppAction,SIGNAL(triggered()),this,SLOT(on_exitAppAction()));
    //设置图标
    QIcon icon = QIcon(":/img/ico/img/logo.png"); //资源文件添加的图标
    m_sysTrayIcon->setIcon(icon);
    m_sysTrayIcon->show();

    /*托盘事件*/
    connect(m_sysTrayIcon, &QSystemTrayIcon::activated, //给QSystemTrayIcon添加槽函数
        [=](QSystemTrayIcon::ActivationReason reason)
        {
            switch(reason)
            {
            case QSystemTrayIcon::Trigger: //单击托盘图标
                //显示主窗口
                this->show();
                break;
            case QSystemTrayIcon::DoubleClick: //双击托盘图标
                //退出程序
                qApp->quit();
                break;
            default:
                break;
            }
        });
}

/*解构函数*/
MainWindow::~MainWindow()
{
    delete ui;
}

/*创建备份*/
void MainWindow::setDroppedPaths(const QStringList &paths)
{
    qInfo()<<"打开文件路径："<<paths;
    //检查是否打开文件
    if (paths.isEmpty())
    {
        //如果没有就选择
        //文件选择对话框
        QString fileName = QFileDialog::getOpenFileName(this, "Select File", "", "All Files (*)");
        if (fileName.isEmpty())
        {
            qInfo()<<"未选择文件，退出程序";
            QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection); //这里因为在构造窗口前，所以说需要这种方式
        }
        else
        {
            setDroppedPaths(QStringList() << fileName);
            return;
        }
    }
    else
    {
        //如果有，静默启动
        this->hide();
    }
    /*获取一些初始内容*/
    QString file = paths.first();
    QFileInfo fileInfo(file);
    QString suffix = fileInfo.suffix();
    this->setWindowTitle("当前文件："+fileInfo.fileName());
    /*读取配置文件*/
    QSettings ini(configPath(), QSettings::IniFormat);
    //FileType组里是否指定了文件打开方式
    ini.beginGroup("FileType");
    //检查组里面有没有suffix
    QString exe;
    if (!ini.contains(suffix))
    {
        //让用户选择一个打开方式
        QMessageBox::information(this, "初始化", "请选择该文件的打开方式");
        exe = QFileDialog::getOpenFileName(this, "选择一个应用来打开 *." + suffix, "", "Applications (*.exe);;All Files (*)");
        //同时写入配置文件
        ini.setValue(suffix, exe);
    }
    else //如果找到了就用ini里的值
    {
        exe = ini.value(suffix).toString();
    }
    ini.endGroup();

    /*文件打开*/
    QProcess::startDetached(exe, QStringList() << file);

    /*文件备份*/
    //把file复制到文档文件夹下的VersionBox文件夹
    QString docPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString targetDir = docPath + "/VersionBox/Backup/"+QUrl::toPercentEncoding(file);
    QDir dir;
    //如果没有文件夹就创建
    if (!dir.exists(targetDir))
    {
        dir.mkpath(targetDir);
    }

    /*检查和最新的文件是否相同*/
    //从文件名判断targetDir中的所有文件哪个最新
    QDir scanDir(targetDir);
    QFileInfoList fileList = scanDir.entryInfoList(QDir::Files | QDir::NoSymLinks);
    QFileInfo newestFile;
    QDateTime newestTime;
    //匹配文件名中的时间戳部分
    QRegularExpression re(R"((\d{8}_\d{6})\..+$)");
    //搜索最新文件
    for (const QFileInfo &file : std::as_const(fileList))
    {
        QRegularExpressionMatch match = re.match(file.fileName());
        if (match.hasMatch())
        {
            QString timestampStr = match.captured(1); // yyyyMMdd_HHmmss
            QDateTime fileTime = QDateTime::fromString(timestampStr, "yyyyMMdd_HHmmss");
            if (fileTime.isValid() && (!newestTime.isValid() || fileTime > newestTime))
            {
                newestTime = fileTime;
                newestFile = file;
            }
        }
    }

    /*对比哈希值*/
    if (newestFile.exists())
    {
        QFile currentFile(file);
        QFile backupFile(newestFile.filePath());
        if (currentFile.open(QIODevice::ReadOnly) && backupFile.open(QIODevice::ReadOnly))
        {
            QByteArray currentHash = QCryptographicHash::hash(currentFile.readAll(), QCryptographicHash::Sha256);
            qInfo()<<"当前文件哈希值："<<currentHash.toHex();
            QByteArray backupHash = QCryptographicHash::hash(backupFile.readAll(), QCryptographicHash::Sha256);
            currentFile.close();
            backupFile.close();
            //如果相同就不备份
            if (currentHash != backupHash)
            {
                qInfo()<<"文件不同，正在备份";
                /*创建和复制文件*/
                QString targetFilePath = targetDir + "/" +
                                         QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + "." +
                                         fileInfo.suffix();
                QFile::copy(file, targetFilePath);
            }
            else
            {
                qInfo()<<"文件相同，无需备份";
            }
        }
    }
    else
    {
        qInfo()<<"文件无备份，正在备份";
        /*创建和复制文件*/
        QString targetFilePath = targetDir + "/" +
                                 QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + "." +
                                 fileInfo.suffix();
        QFile::copy(file, targetFilePath);
    }
    LoadFileBackup(paths,file);
}

/*加载文件备份*/
void MainWindow::LoadFileBackup(const QStringList &paths,const QString& SourceFilePath)
{
    QString file = paths.first();
    QFileInfo fileInfo(file);
    //获取当前文件的所有备份
    QString docPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString targetDir = docPath + "/VersionBox/Backup/"+QUrl::toPercentEncoding(file);
    QDir dir(targetDir);
    QStringList backupFiles = dir.entryList(QDir::Files, QDir::Time);
    //按文件名排序
    QStringList sortedBackupFiles = backupFiles;
    sortedBackupFiles.sort(Qt::CaseInsensitive);
    //从配置文件读取最大备份数量
    QSettings ini(configPath(), QSettings::IniFormat);
    ini.beginGroup("General");
    int maxBackupQuantity = ini.value("BackupQuantity", 100).toInt(); //默认100个备份
    ini.endGroup();
    int i = 0;
    //循环获取备份文件信息
    for (const QString &backupFile : std::as_const(sortedBackupFiles))
    {
        QString backupFilePath = targetDir + "/" + backupFile;
        if(backupFiles.length() - i > maxBackupQuantity)
        {
            //删除多余的备份文件
            QString filePathToDelete = targetDir + "/" + backupFile;
            QFile::remove(filePathToDelete);
        }
        else
        {
            //创建 BackupFileInfo窗口
            BackupFileInfo *backupInfo = new BackupFileInfo(backupFilePath, SourceFilePath, this);
            ui->verticalLayout_BackupFiles->addWidget(backupInfo);
        }
        i++;
    }
    //最后再添加一个verticalSpacer
    auto *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    ui->verticalLayout_BackupFiles->addItem(spacer);
}

/*托盘动作*/
//托盘主界面
void MainWindow::on_showMainAction()
{
    //打开setting窗口
    setting *setWindow = new setting();
    setWindow->show();
}
//托盘推出
void MainWindow::on_exitAppAction()
{
    qApp->exit();
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    this->hide();
    e->ignore(); // 阻止真正 close
}

