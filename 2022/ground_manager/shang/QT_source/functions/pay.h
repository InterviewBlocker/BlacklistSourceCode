#ifndef PAY_H
#define PAY_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include "common/adminInfo.h"

namespace Ui {
class Pay;
}

class Pay : public QDialog
{
    Q_OBJECT

public:
    explicit Pay(QWidget *parent = 0);
    ~Pay();

private:
    Ui::Pay *ui;

private slots:
    void accept();
    void reject();
};

#endif // PAY_H
