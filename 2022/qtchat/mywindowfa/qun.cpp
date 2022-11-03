#include "qun.h"
#include "ui_qun.h"

qun::qun(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::qun)
{
    ui->setupUi(this);
}

qun::~qun()
{
    delete ui;
}
