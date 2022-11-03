#ifndef DUOLIAO_H
#define DUOLIAO_H

#include <QWidget>

namespace Ui {
class duoliao;
}

class duoliao : public QWidget
{
    Q_OBJECT

public:
    explicit duoliao(QWidget *parent = nullptr);
    ~duoliao();

 signals:
  void  show2_duolaio();
  void show4_xuanze();


private slots:
  void on_btn_tui1_clicked();

private:
    Ui::duoliao *ui;
    void paintEvent(QPaintEvent *e);//背景图
};

#endif // DUOLIAO_H
