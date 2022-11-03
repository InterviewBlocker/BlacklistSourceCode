#ifndef ZHUCE_H
#define ZHUCE_H

#include <QWidget>

namespace Ui {
class zhuce;
}

class zhuce : public QWidget
{
    Q_OBJECT

public:
    explicit zhuce(QWidget *parent = nullptr);
    ~zhuce();

private slots:
    void on_pushButton_2_clicked();

    void on_tn_ture_clicked();

signals:
     void show3_zhu();
     void MainWindow();


private:
    Ui::zhuce *ui;
    void paintEvent(QPaintEvent *e);//
};

#endif // ZHUCE_H
