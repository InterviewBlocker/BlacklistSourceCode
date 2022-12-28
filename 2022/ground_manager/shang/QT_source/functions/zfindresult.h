#ifndef ZFINDRESULT_H
#define ZFINDRESULT_H

#include <QDialog>
#include "common/caruser.h"

namespace Ui {
class zFindResult;
}

class zFindResult : public QDialog
{
    Q_OBJECT

public:
    explicit zFindResult(QWidget *parent = 0);
    ~zFindResult();
    void InitUI();
    void setFindHintLabel(QString arg);
    carUser car;

private slots:
    void accept();


private:
    Ui::zFindResult *ui;
};

#endif // ZFINDRESULT_H
