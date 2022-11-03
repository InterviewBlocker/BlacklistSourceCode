#include "say.h"
#include "ui_say.h"

say::say(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::say)
{
    ui->setupUi(this);


}

say::~say()
{
    delete ui;
}
