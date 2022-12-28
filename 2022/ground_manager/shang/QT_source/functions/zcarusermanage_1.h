#ifndef ZCARUSERMANAGE_H
#define ZCARUSERMANAGE_H

#include <QDialog>
#include "common/caruser.h"
#include "database/sqlUser.h"

namespace Ui {
class zcarUserManage;
}

class zcarUserManage : public QDialog
{
    Q_OBJECT

public:
    explicit zcarUserManage(QWidget *parent = 0);
    ~zcarUserManage();

    void InitUI();
    carUser car;
    bool isAdd;

private slots:
    void accept();
    void reject();
    void getValue();
    bool checkValid();

private:
    Ui::zcarUserManage *ui;
};

#endif // ZCARUSERMANAGE_H
