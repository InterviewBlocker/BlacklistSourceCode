#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include"QThread"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void readPendingDatagrams();

public slots:
    void replayFinished(qint64, qint64);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::MainWindow *ui;
};
class my_thread:public QThread
{
    //添加信号与槽的宏定义
    Q_OBJECT
    public:
    //设计构造函数
    my_thread(QObject *parent =nullptr):QThread(parent){}

    //覆盖run 函数 ,调用 start 就会执行该函数
    void run();

};


class my_thread1:public QThread
{
    //添加信号与槽的宏定义
    Q_OBJECT
    public:
    //设计构造函数
    my_thread1(QObject *parent =nullptr):QThread(parent){}

    //覆盖run 函数 ,调用 start 就会执行该函数
    void run();

};





#endif // MAINWINDOW_H
