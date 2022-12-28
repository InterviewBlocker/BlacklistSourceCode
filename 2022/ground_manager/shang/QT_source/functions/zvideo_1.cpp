#include "zvideo.h"
#include "ui_zvideo.h"

zVideo::zVideo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::zVideo)
{
    ui->setupUi(this);
    api = new BaiduAPI(this);
    connect(ui->pb_lpr, SIGNAL(clicked(bool)), this, SLOT(sloInNetworkPost()));
    connect(ui->pb_lpr2, SIGNAL(clicked(bool)), this, SLOT(sloOutNetworkPost()));
    ui->pb_lpr->setEnabled(false);
    ui->pb_lpr2->setEnabled(false);
}

zVideo::~zVideo()
{
    delete ui;
}

void zVideo::getUsedStalNumber()
{
    sqlPark sp;
    park p = sp.GetParkInfo(1);
    ui->label_usedStallNumber->setText(QString::number(p.usedStallNumber));
    ui->label_residueStallNumber->setText(QString::number(p.stallNumber - p.usedStallNumber));
    emit updateStallNumber(p.usedStallNumber,p.stallNumber - p.usedStallNumber);
}

void zVideo::sloShowReadPic(QTcpSocket *tcpSocket)
{
    char buf[20] = {0};
    switch (state)
    {
    case 1:
        if(tcpSocket->bytesAvailable() < 20)
            return ;
        tcpSocket->read(buf, sizeof(buf));
        pic_len = atoi(buf);
        state = 2;
        break;
    case 2:
        if(tcpSocket->bytesAvailable() < pic_len)
            return ;
        int ret = tcpSocket->read(pic_buf, pic_len);

        QPixmap *pix = new QPixmap;
        pix->loadFromData((uchar *)pic_buf,pic_len, "JPG");
        pix->save("car1.jpg");          //将图片保存到当前路径

        if(ui->rbtn_on1->isChecked()){
            //            emit sigVideoState(1,true);
            ui->label_showPicIn->setPixmap(*pix);
        }
        else{
            //            emit sigVideoState(1,false);
            ui->label_showPicIn->setText(tr("监控未打开"));
        }

        if(ui->rbtn_on2->isChecked()){
            //            emit sigVideoState(2,true);
            ui->label_showPicOut->setPixmap(*pix);
        }
        else{
            //           emit sigVideoState(2,false);
            ui->label_showPicOut->setText(tr("监控未打开"));
        }
        state = 1;
        break;
    }
}

void zVideo::sloInNetworkPost()
{
    api->networkPost();

    QString datetime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    if(ui->rbtn_on1->isChecked() and api->getCarNumber() != ""){
        ui->le_carNumber->setText(api->getCarNumber());
        ui->le_carNumber_2->setText(api->getCarNumber());
        carUser user;
        sqlUser su;
        user = su.GetUserInfoByCarNum(api->getCarNumber());
        if(user.CarNumber.isEmpty()){
            QMessageBox::warning(this,tr("查询结果"),tr("该用户尚未注册!\n请联系车库管理员进行注册充值方可入内"),tr("确定"));
        }
        else {
            ui->label_inTime2->setText(datetime);
            ui->label_inTime1->setText(datetime);
            ui->label_outTime2->clear();
            ui->label_charge->clear();
            ui->label_parkingTimes->clear();
            //车位计算
            sqlPark sp;
            park p = sp.GetParkInfo(1);
            p.usedStallNumber = p.usedStallNumber + 1;
            sp.modifyPark(p);

            ui->label_usedStallNumber->setText(QString::number(p.usedStallNumber));
            int stallNumber = p.stallNumber;
            int usedStallNumber = ui->label_usedStallNumber->text().toInt();
            ui->label_residueStallNumber->setText(QString::number(stallNumber - usedStallNumber));
//            qDebug()<< "车位总数：" << stallNumber;
//            qDebug()<< "已用车位：" << usedStallNumber;
            emit updateStallNumber(usedStallNumber,stallNumber-usedStallNumber);
            //添加数据库记录
            sqlCarState sc;
            carState car;
            car.CarNumber = api->getCarNumber();
            car.State = tr("入库");
            car.InTime = datetime;
            car.OutTime = " ";
            car.StopTime = 0;
            car.ID = sc.getNewId();
            sc.addCarState(car);
        }
    }
    else {
        ui->le_carNumber_2->clear();
    }
}

void zVideo::sloOutNetworkPost()
{
    api->networkPost();

    QString datetime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    if(ui->rbtn_on2->isChecked() and api->getCarNumber() != ""){
        ui->le_carNumber_3->setText(api->getCarNumber());
        ui->le_carNumber->setText(api->getCarNumber());
        ui->label_outTime2->setText(datetime);
        ui->label_outTime1->setText(datetime);
        //车位计算
        sqlPark sp;
        park p = sp.GetParkInfo(1);
        p.usedStallNumber = p.usedStallNumber - 1;
        sp.modifyPark(p);

        ui->label_usedStallNumber->setText(QString::number(p.usedStallNumber));
        int stallNumber = p.stallNumber;
        int usedStallNuber = ui->label_usedStallNumber->text().toInt();
        ui->label_residueStallNumber->setText(QString::number(stallNumber - usedStallNuber));
        emit updateStallNumber(usedStallNuber,stallNumber-usedStallNuber);
        //添加数据库记录
        sqlCarState sc;
        carState car = sc.GetUserInfo(api->getCarNumber());
        car.State = tr("出库");
        car.OutTime = datetime;
        QDateTime inTime = QDateTime::fromString(car.InTime,"yyyy-MM-dd hh:mm:ss");
        int stopTime = QDateTime::currentDateTime().toTime_t() - inTime.toTime_t();
        car.StopTime = stopTime;
        car.ID = sc.getNewId();
        sc.addCarState(car);

        ui->label_inTime2->setText(car.InTime);
        ui->label_parkingTimes->setText(QString::number(stopTime/3600.00));
        //计费
        int price = ui->label_price->text().toInt();
        float charge = (stopTime/3600.00) * price;
        ui->label_charge->setText(QString::number(charge,'f',2));
        sqlUser cu;
        carUser carU;
        carU = cu.GetUserInfoByCarNum(car.CarNumber);
        carU.Balance = carU.Balance - ui->label_charge->text().toFloat();
        cu.modifyCarUser(carU);
        float todayAllCharge = ui->label_todayAllCharge->text().toFloat() + ui->label_charge->text().toFloat();
        ui->label_todayAllCharge->setText(QString::number(todayAllCharge));
        on_pb_pass_clicked();
    }
    else {
        ui->le_carNumber_3->clear();
    }
}

void zVideo::sloChangeParkName(const QString parkName)
{
    ui->label_parkName->setText(parkName);
}

void zVideo::sloChangeStallNumberValue(const QString stallNumber)
{
    ui->label_stallNumber->setText(stallNumber);
    sqlPark sp;
    park p = sp.GetParkInfo(1);
    ui->label_usedStallNumber->setText(QString::number(p.usedStallNumber));
    ui->label_residueStallNumber->setText(QString::number(stallNumber.toInt()-p.usedStallNumber));
}

void zVideo::sloChangePriceValue(const QString price)
{
     ui->label_price->setText(price);
}

void zVideo::on_pb_pass_clicked()
{
    emit sigPass();
}

void zVideo::on_pb_nopass_clicked()
{
    emit sigNotPass();
}

void zVideo::on_rbtn_on1_clicked()
{
    ui->pb_lpr->setEnabled(true);
    emit sigVideoState(1,true);
}

void zVideo::on_rbtn_off1_clicked()
{
    ui->pb_lpr->setEnabled(false);
    emit sigVideoState(1,false);
}

void zVideo::on_rbtn_on2_clicked()
{
    ui->pb_lpr2->setEnabled(true);
    emit sigVideoState(2,true);
}

void zVideo::on_rbtn_off2_clicked()
{
    ui->pb_lpr2->setEnabled(false);
    emit sigVideoState(2,false);
}
