#include "zhelp.h"
#include "ui_zhelp.h"

zHelp::zHelp(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::zHelp)
{
    ui->setupUi(this);
}

zHelp::~zHelp()
{
    delete ui;
}
