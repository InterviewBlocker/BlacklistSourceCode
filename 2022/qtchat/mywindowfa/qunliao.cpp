#include "qunliao.h"
#include "ui_qunliao.h"

qunliao::qunliao(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::qunliao)
{
    ui->setupUi(this);
}

qunliao::~qunliao()
{
    delete ui;
}
