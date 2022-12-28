#include "zdata.h"
#include "ui_zdata.h"
#include <QAbstractSocket>
#include <QString>
#include <QDebug>

zData::zData(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::zData)
{
    ui->setupUi(this);

    model = new QStandardItemModel(this);
    ui->tableView->setModel(model);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->model->setHorizontalHeaderItem(0, new QStandardItem("姓名"));
    this->model->setHorizontalHeaderItem(1, new QStandardItem("车牌号"));
    this->model->setHorizontalHeaderItem(2, new QStandardItem("电话"));
    this->model->setHorizontalHeaderItem(3, new QStandardItem("年龄"));
    this->model->setHorizontalHeaderItem(4, new QStandardItem("性别"));
    this->model->setHorizontalHeaderItem(5, new QStandardItem("余额"));

    ui->tableView->setColumnWidth(0,200);
    ui->tableView->setColumnWidth(1,200);
    ui->tableView->setColumnWidth(2,250);
    ui->tableView->setColumnWidth(3,150);
    ui->tableView->setColumnWidth(4,150);
    ui->tableView->setColumnWidth(5,150);
}

zData::~zData()
{
    delete ui;
}

void zData::loadDefaultData()
{
    loadAllData(0);
}

void zData::initPark()
{
    sqlPark sp;
    park p = sp.GetParkInfo(1);
    ui->le_parkName->setText(p.parkName);
    ui->spinBox_price->setValue(p.charge);
    ui->spinBox_stallNumber->setValue(p.stallNumber);
}

void zData::on_pbtn_connect_clicked()
{
    tcpSocket = new QTcpSocket;
//    tcpClient::getInstance().connectTo(tcpSocket, QHostAddress("127.0.0.1"), 8888);
    if(!ui->lineEdit_ip->text().isEmpty() and !ui->lineEdit_port->text().isEmpty())
    {
        tcpClient::getInstance().connectTo(tcpSocket, QHostAddress(ui->lineEdit_ip->text()), ui->lineEdit_port->text().toInt());
//        qDebug()<<"address:" << ui->lineEdit_ip->text();
//        qDebug()<<"port:" << ui->lineEdit_port->text();
    }
    else
    {
        tcpClient::getInstance().connectTo(tcpSocket, QHostAddress("127.0.0.1"), 8888);
    }
    connect(this,SIGNAL(sigTcpStateUpdate(QAbstractSocket::SocketState)),this,SLOT(slotTcpStateDeal()));
    connect(tcpSocket,SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(slotStateChanged(QAbstractSocket::SocketState)));
    connect(tcpSocket, &QTcpSocket::readyRead, [this]()
    {
       emit tcpReadyRead(tcpSocket);
    });
}

void zData::slotTcpStateDeal()
{
    qDebug()<<isOpen;
//    qDebug()<<"addr:" << tcpSocket->peerAddress();
//    qDebug()<<"port:" << tcpSocket->peerPort();
    if(isOpen)
    {
        QString currentDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd::mm::ss ");
        QString te = currentDateTime +"\n连接成功!\nip:" + tcpSocket->peerAddress().toString()\
                +"\n端口：" + QString::number(tcpSocket->peerPort());
        loger::getInstance().write(QString("TCP连接%1成功，启动心跳协议").arg(tcpSocket->peerAddress().toString()));
        ui->textEdit->setText(te);
        ui->pbtn_connect->setIcon(QIcon(":/pic/netwark_blue.png"));
    }
    else
    {
        QString currentDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ");
        QString te = currentDateTime +"\n连接失败!\nip:" + tcpSocket->peerAddress().toString()\
                +"\n端口：" + QString::number(tcpSocket->peerPort());
        loger::getInstance().write("TCP连接异常");
        ui->textEdit->setText(te);
        ui->pbtn_connect->setIcon(QIcon(":/pic/netwark_blue.png"));
    }
}

void zData::slotStateChanged(QAbstractSocket::SocketState state)
{
    switch(state)
    {
        case QAbstractSocket::ConnectedState:
            loger::getInstance().write("TcpsocketStates:ConnetedStates");
            isOpen = true;
            emit sigTcpStateUpdate(state);
        break;
        case QAbstractSocket::ConnectingState:
            loger::getInstance().write("TcpSocketStates:ConnectingState");break;
        case QAbstractSocket::ClosingState:
            loger::getInstance().write("TcpSocketStates:ClosingState");break;
        case QAbstractSocket::UnconnectedState:
            loger::getInstance().write("TcpSocketStates:UnconnectedState");
            isOpen = false;
            emit sigTcpStateUpdate(state);
            break;
        default:
            break;
    }
}
void zData::loadAllData(int selectRow)
{
    sqlUser su;
    vector<carUser> users;
    users = su.GetAllUsers();
    qint32 i = 0;
    for(auto it = users.begin(); it!=users.end();it++){
        this->model->setItem(i, 0, new QStandardItem(it->Name));
        this->model->setItem(i, 1, new QStandardItem(it->CarNumber));
        this->model->setItem(i, 2, new QStandardItem(it->PhoneNumber));
        this->model->setItem(i, 3, new QStandardItem(it->Age));
        this->model->setItem(i, 4, new QStandardItem(it->Sex));
        this->model->setItem(i, 5, new QStandardItem(QString::number(it->Balance)));
        for(int j = 0; j < 6; j++){
            this->model->item(i, j)->setTextAlignment(Qt::AlignCenter);//居中
            this->model->item(i, j)->setForeground(Qt::white);
        }
        QVariant qvar;
        qvar.setValue(*it);
        this->model->item(i)->setData(qvar, Qt::UserRole);
        i++;
    }
    ui->tableView->setColumnWidth(0,200);
    ui->tableView->setColumnWidth(1,200);
    ui->tableView->setColumnWidth(2,250);
    ui->tableView->setColumnWidth(3,150);
    ui->tableView->setColumnWidth(4,150);
    ui->tableView->setColumnWidth(5,150);
    if(this->model->rowCount() > 0){
        ui->tableView->selectRow(selectRow);
        ui->btnModifyCarUser->setEnabled(true);
        ui->btnDeleteCarUser->setEnabled(true);
    }
    else {
        ui->btnModifyCarUser->setEnabled(false);
        ui->btnDeleteCarUser->setEnabled(false);
    }

}
void zData::on_btnAddCarUser_clicked()
{
    zcarUserManage *zcm = new zcarUserManage(this);
    zcm->setWindowModality(Qt::WindowModal);
    zcm->isAdd = true;
    zcm->setWindowTitle("新增用户");
    if(zcm->exec() == QDialog::Accepted){
        loadAllData(this->model->rowCount());
    }
    zcm->deleteLater();
}

void zData::on_btnDeleteCarUser_clicked()
{
    QVariant qvar = this->model->item(ui->tableView->currentIndex().row())->data(Qt::UserRole);
    if(!qvar.isValid()){
        return;
    }
    auto ret = QMessageBox::question(this,tr("删除"),tr("确定要删除此项吗？"),\
                                     tr("确定"),tr("取消"),0);
    if(ret == 0){
        sqlUser su;
        carUser selectUser = qvar.value<carUser>();
        su.removeCarUser(selectUser.ID);

        int curRow = ui->tableView->currentIndex().row();
        ui->tableView->model()->removeRow(curRow);
        int selectRow = 0;
        if(curRow < this->model->rowCount()){
            selectRow = curRow;
        }
        else {
            selectRow = this->model->rowCount() - 1;
        }
        loadAllData(selectRow);
    }
}

void zData::on_btnModifyCarUser_clicked()
{
    zcarUserManage *zcm = new zcarUserManage(this);
    zcm->setWindowModality(Qt::WindowModal);
    QVariant qvar = this->model->item(ui->tableView->currentIndex().row())->data(Qt::UserRole);
    carUser user = qvar.value<carUser>();
    zcm->isAdd = false;
    zcm->car = user;
    zcm->InitUI();
    zcm->setWindowTitle("修改用户信息");

    if(zcm->exec() == QDialog::Accepted){
        loadAllData(ui->tableView->currentIndex().row());
    }
    zcm->deleteLater();
}

void zData::on_btnFindCarUser_clicked()
{
    if(ui->lineEdit_FindText->text() == ""){
        QMessageBox::information(this,tr("查询结果"),tr("查询内容不能为空"),QMessageBox::Ok);
    }
    else {
        sqlUser su;
        if(ui->comboBox->currentText() == tr("所有")) {
            loadAllData(ui->tableView->currentIndex().row());
        }
        else if (ui->comboBox->currentText() == tr("车牌号")) {
            carUser user;
            user = su.GetUserInfoByCarNum(ui->lineEdit_FindText->text());
            if(!user.CarNumber.isEmpty()){
                zFindResult *z_fr = new zFindResult(this);
                z_fr->car = user;
                z_fr->InitUI();
                z_fr->setFindHintLabel(QString("查询到车牌号为%1的车辆用户信息").arg(user.CarNumber));
                z_fr->exec();
                z_fr->deleteLater();
            }
            else {
                QMessageBox::warning(this,tr("查询结果"),tr("未查询到相关车辆用户信息!"),tr("确定"));
            }

        }
        else if (ui->comboBox->currentText() == tr("电话号码")) {
            qDebug()<<"查询电话号码";
            carUser user;
            user = su.GetUserInfoByPhoneNumber(ui->lineEdit_FindText->text());
            qDebug()<<ui->lineEdit_FindText->text();
            qDebug()<<user.PhoneNumber;
            if(!user.PhoneNumber.isEmpty()){
                zFindResult *z_fr = new zFindResult(this);
                z_fr->car = user;
                z_fr->InitUI();
                z_fr->setFindHintLabel(QString("查询到电话号码为%1的车辆用户信息").arg(user.PhoneNumber));
                z_fr->exec();
                z_fr->deleteLater();
            }
            else {
                QMessageBox::warning(this,tr("查询结果"),tr("未查询到相关车辆用户信息!"),QMessageBox::Ok);
            }
            qDebug()<<user.PhoneNumber;
        }
    }


}

void zData::on_comboBox_currentTextChanged(const QString &arg1)
{
    if(arg1 == tr("所有")) {
        loadAllData(ui->tableView->currentIndex().row());
    }
}

void zData::on_le_parkName_textChanged(const QString &arg1)
{
    emit sigParkNameTextChanged(arg1);
    sqlPark sp;
    park p = sp.GetParkInfo(1);
    p.parkName = arg1;
    sp.modifyPark(p);
}

void zData::on_spinBox_stallNumber_valueChanged(const QString &arg1)
{
    emit sigStallNumberValueChanged(arg1);
    sqlPark sp;
    park p = sp.GetParkInfo(1);
    p.stallNumber = arg1.toInt();
    sp.modifyPark(p);
}

void zData::on_spinBox_price_valueChanged(const QString &arg1)
{
    emit sigPriceValueChanged(arg1);
    sqlPark sp;
    park p = sp.GetParkInfo(1);
    p.charge = arg1.toInt();
    sp.modifyPark(p);
}


