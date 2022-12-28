#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <QMainWindow>
#include <QAction>
#include <QLabel>
#include <QTimer>
#include <QFile>
#include <QCloseEvent>
#include <QMessageBox>
#include <QTcpSocket>
#include <QHostAddress>

#include "functions/zvideo.h"
#include "functions/zdata.h"
#include "functions/zfindresult.h"
#include "functions/zhelp.h"
#include "functions/zrecord.h"
#include "common/adminInfo.h"
#include "register.h"

namespace Ui {
class MainFrame;
}

class MainFrame : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainFrame(QMainWindow *M, QWidget *parent = 0);
    ~MainFrame();
    void initFrame();
    void initStatus();

private slots:
    void slotFuncSelectResponse(int btnNameIndex);//功能选择
    void slotVideoStateChange(int videoNum, bool state);//摄像头状态
    void slotTcpConnectState(QAbstractSocket::SocketState state);//网络连接状态
    void slotShowTime();//显示时间

    void on_tbtnUser_triggered(QAction *arg1);//用户选择下拉选项

    void slotChangeParkName(const QString parkName);
    void slotChangeStallNumberValue(const QString stallNumber);
    void slotUpdateStallNumber(int usedStall, int restStall);
    void slotPass();
    void slotNotPass();

protected:
    void closeEvent(QCloseEvent *event);


private:
    Ui::MainFrame *ui;

    QFile *qssFile;
    //定义主框架上面的四个按钮对应的四个widget
    zVideo *z_video;
    zData *z_data;
    ZRecord *z_record;
    zHelp *z_help;

    QLabel *zLabelTime, *zLabelNet;
    QAction *zSwitchUserAct, *zPersonalInfoAct, *zCloseAct;
    QTimer *zTimer;
//    Register *register;
    QMainWindow *mainwindow;

};

#endif // MAINFRAME_H
