#include "mainframe.h"
#include "ui_mainframe.h"

#include <QLineEdit>
#include <QSpinBox>
#include <QTimer>
#include <QTime>

MainFrame::MainFrame(QMainWindow *M, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainFrame)
{
    mainwindow = M;
    ui->setupUi(this);

    qssFile = new QFile(":/qss/style.qss",this);
    qssFile->open(QFile::ReadOnly);
    QString styleSheel = tr(qssFile->readAll());
    qApp->setStyleSheet(styleSheel);
    qssFile->close();

    this->setWindowTitle("基于QT的停车收费系统");
    this->setWindowIcon(QIcon(":/pic/logo.ico"));

    QStringList buttonNames={"tbtnVideo","tbtnData","tbtnRecord","tbtnHelper","tbtnUser"};
    for(int i=0;i<buttonNames.size() ;i++)
    {
        QToolButton *button= this->findChild<QToolButton*>(buttonNames[i]);
        connect(button, &QToolButton::clicked,this,[=]{
            slotFuncSelectResponse(i);
        });
    }
    initFrame();
    initStatus();
}

MainFrame::~MainFrame()
{
    delete ui;
}

void MainFrame::initFrame()
{
    z_video = new zVideo;
    z_data = new zData;
    z_record = new ZRecord;
    z_help = new zHelp;

    ui->stackedWidget->insertWidget(0,z_video);
    ui->stackedWidget->insertWidget(1,z_data);
    ui->stackedWidget->insertWidget(2,z_record);
    ui->stackedWidget->insertWidget(3,z_help);

    ui->tbtnVideo->clicked();

    connect(z_data, SIGNAL(sigParkNameTextChanged(const QString)),this, SLOT(slotChangeParkName(const QString)));
    connect(z_data, SIGNAL(sigParkNameTextChanged(const QString)),z_video,SLOT(sloChangeParkName(const QString)));
    connect(z_data, SIGNAL(sigStallNumberValueChanged(const QString)),this,SLOT(slotChangeStallNumberValue(const QString)));
    connect(z_data, SIGNAL(sigStallNumberValueChanged(const QString)),z_video,SLOT(sloChangeStallNumberValue(const QString)));
    connect(z_data, SIGNAL(sigPriceValueChanged(const QString)),z_video,SLOT(sloChangePriceValue(const QString)));

    connect(z_data,SIGNAL(tcpReadyRead(QTcpSocket *)),z_video, SLOT(sloShowReadPic(QTcpSocket *)));
    connect(z_data,SIGNAL(sigTcpStateUpdate(QAbstractSocket::SocketState)),this,SLOT(slotTcpConnectState(QAbstractSocket::SocketState)));
    connect(z_video,SIGNAL(sigVideoState(int,bool)),this,SLOT(slotVideoStateChange(int,bool)));
    connect(z_video,SIGNAL(updateStallNumber(int,int)),this,SLOT(slotUpdateStallNumber(int,int)));
    connect(z_video, SIGNAL(sigPass()),this,SLOT(slotPass()));
    connect(z_video, SIGNAL(sigNotPass()),this,SLOT(slotNotPass()));

    zSwitchUserAct = new QAction(QIcon(":/pic/switch.png"),"切换用户",this);
    zPersonalInfoAct = new QAction(QIcon(":/pic/personalInfo.png"),"个人信息",this);
    zCloseAct = new QAction(QIcon(":/pic/exit.png"),"关闭系统",this);
    QList <QAction*> userActs = {zSwitchUserAct,zPersonalInfoAct,zCloseAct};
    ui->tbtnUser->addActions(userActs);
}

void MainFrame::initStatus()
{
    zLabelTime = new QLabel;
    zLabelNet = new QLabel;
    //底部的时间
    QLabel *zLabelNetText = new QLabel;
    QFont z_font1("Times", 13, QFont::Bold);
    zLabelNet->setFont(z_font1);
    zLabelNet->setScaledContents(true);
    zLabelNet->setFixedSize(23,23);
    zLabelNet->setPixmap(QPixmap(":/pic/warn.png"));
    zLabelNet->setToolTip("TCP未连接");

    zLabelNetText->setFont(z_font1);
    zLabelTime->setFont(z_font1);
    zLabelNetText->setText("网络");

    ui->statusBar->addPermanentWidget(zLabelNetText);
    ui->statusBar->addPermanentWidget(zLabelNet);
    ui->statusBar->addPermanentWidget(zLabelTime);

    QTimer *t = new QTimer;
    t->start(1000);
    connect(t, SIGNAL(timeout()), this, SLOT(slotShowTime()));
}

void MainFrame::slotFuncSelectResponse(int btnNameIndex)
{
    switch (btnNameIndex)
    {
    case 0:
    {
        ui->stackedWidget->setCurrentIndex(0);
        z_video->getUsedStalNumber();
        break;
    }

    case 1:
    {
        ui->stackedWidget->setCurrentIndex(1);
        z_data->loadDefaultData();
        break;
    }
    case 2:
    {
        ui->stackedWidget->setCurrentIndex(2);
        z_record->loadDefaultData();
        break;
    }
    case 3:
    {
        ui->stackedWidget->setCurrentIndex(3);
        break;
    }
    case 4:
    {
        ui->tbtnUser->showMenu();
        break;
    }
        break;

    default:
    {

        break;
    }
    }
}

void MainFrame::slotVideoStateChange(int videoNum, bool state)
{
    switch(videoNum)
    {
    case 1:
        if(state)
        {
            ui->label_video1_state->setText(tr("已打开"));
            ui->label_video1_state->setStyleSheet("color: rgb(0, 255, 0);");
        }
        else
        {
            ui->label_video1_state->setText(tr("已关闭"));
            ui->label_video1_state->setStyleSheet("color: rgb(255, 0, 0);");
        }
        break;
    case 2:
        if(state)
        {
            ui->label_video2_state->setText(tr("已打开"));
            ui->label_video2_state->setStyleSheet("color: rgb(0, 255, 0);");
        }
        else
        {
            ui->label_video2_state->setText(tr("已关闭"));
            ui->label_video2_state->setStyleSheet("color: rgb(255, 0, 0);");
        }
        break;
    default:
        break;
    }
}

void MainFrame::slotTcpConnectState(QAbstractSocket::SocketState state)
{
    switch(state)
    {
    case QAbstractSocket::ConnectedState:
        zLabelNet->setPixmap(QPixmap(":/pic/netwark.png"));
        zLabelNet->setToolTip("TCP已连接");
        break;

    case QAbstractSocket::UnconnectedState:
        zLabelNet->setPixmap(QPixmap(":/pic/warn.png"));
        zLabelNet->setToolTip("TCP未连接");
        break;
    default:
        break;
    }
}

void MainFrame::slotShowTime()
{
    zLabelTime->setText(QTime::currentTime().toString());
    ui->label_time->setText(zLabelTime->text());
    QString date =QString::number(QDate::currentDate().year())+"年"+\
            QString::number(QDate::currentDate().month())+"月"+\
            QString::number(QDate::currentDate().day())+"日\n"+
            QDate::currentDate().toString("dddd");
    ui->label_date->setText(date);
    zLabelTime->setToolTip(date);
}

void MainFrame::on_tbtnUser_triggered(QAction *arg1)
{
    if(arg1 == zPersonalInfoAct){
        QString adminInfo("\n用户名:admin\n姓名:张三丰\n电话号码：1234\n职位：操作员\n年龄：48\n");
        QMessageBox::about(this,"个人信息",adminInfo);
    }
    else if(arg1 == zCloseAct){
        this->close();
    }
    else if(arg1 == zSwitchUserAct){
        this->close();
        mainwindow->show();
    }
}

void MainFrame::slotChangeParkName(const QString parkName)
{
    ui->label_parkName->setText(parkName);
}

void MainFrame::slotChangeStallNumberValue(const QString stallNumber)
{
    ui->label_allStallNumber->setText(stallNumber);
    sqlPark sp;
    park p = sp.GetParkInfo(1);
    slotUpdateStallNumber(p.usedStallNumber, stallNumber.toInt() - p.usedStallNumber);
}

void MainFrame::slotUpdateStallNumber(int usedStall, int restStall)
{
    ui->label_hasCarNumber->setText(QString::number(usedStall));
    ui->label_residueStallNumber->setText(QString::number(restStall));
}

void MainFrame::slotPass()
{
    ui->label_gate_state->setText("已打开");
    ui->label_gate_state->setStyleSheet("color: rgb(0, 255, 0);");
    zTimer = new QTimer(this);
    zTimer->start(5000);
    connect(zTimer,SIGNAL(timeout()),this,SLOT(slotNotPass()));
}

void MainFrame::slotNotPass()
{
    ui->label_gate_state->setText("已关闭");
    ui->label_gate_state->setStyleSheet("color: rgb(255, 0, 0);");
    zTimer->stop();
}

void MainFrame::closeEvent(QCloseEvent *event)
{
    auto ret = QMessageBox::question(this,tr("退出"),tr("确认退出？"),
                                     tr("Yes"),tr("No"),0);
    if(ret == 0){
        event->accept();
    }
    else {
        event->ignore();
    }
}
