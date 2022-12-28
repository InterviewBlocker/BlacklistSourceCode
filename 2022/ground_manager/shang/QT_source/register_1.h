#ifndef REGISTER_H
#define REGISTER_H

#include <QMainWindow>
#include <QSqlQuery>

namespace Ui {
class Register;
}

class Register : public QMainWindow
{
    Q_OBJECT

public:
    explicit Register(QWidget *parent = 0);
    ~Register();
    bool IsValidPhoneNumber(const QString & phoneNum);

private slots:
    void on_z_pushButtonExit_clicked();
    void on_z_pushButtonOk_clicked();


private:
    Ui::Register *ui;
    QSqlQuery *z_sqlUser;

};

#endif // REGISTER_H
