#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QUdpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QUdpSocket *cli;
    QHostAddress addr;
    quint16 port;
    QString getName();//获取名字
private slots:
    void recv_data();


    void on_btn_Send_clicked();

    void on_btn_tui_clicked();
    void on_pushButton_clicked();

signals:
    void show4_xuanze();
    void show1_main();
     void show2_duoliao();
     void show_lianxi();
private:
    Ui::MainWindow *ui;
    void paintEvent(QPaintEvent *e);
    QString uName;//名字
    QUdpSocket *udpSocket;//udp套接字
    QString myname;

};
#endif // MAINWINDOW_H
