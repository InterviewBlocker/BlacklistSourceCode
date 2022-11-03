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
    QUdpSocket *ser;
    QHostAddress addr;
    quint16 port;
    quint16 p[2];
    QMap<quint16,QHostAddress> map;
    int i=0;

private slots:
    void recv_data();

    void on_btn_Send_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
