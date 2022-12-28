#ifndef ZRECORD_H
#define ZRECORD_H

#include <QWidget>
#include <QFileDialog>
#include <QTextEdit>
#include <QStandardItemModel>
#include "database/sqlCarState.h"
#include "common/carstate.h"
#include <QMenu>
#include <Qmessagebox>

namespace Ui {
class ZRecord;
}

class ZRecord : public QWidget
{
    Q_OBJECT

public:
    explicit ZRecord(QWidget *parent = 0);
    ~ZRecord();
    void loadDefaultData();

private slots:
    void on_btnDelete_clicked();//删除选中项
    void on_btnClear_clicked();//清空数据库
    void on_btnExport_clicked();//导出数据
    void on_pbtnFirstPage_clicked();//首页
    void on_pbtnLastPage_clicked();//末页
    void on_pbtnPreviousPage_clicked();//前一页
    void on_pbtnNextPage_clicked();//后一页
    void on_pbtnGoto_clicked();//跳转

private:
    Ui::ZRecord *ui;
    QStandardItemModel *model;
    int pageRecordCount = 20;
    void loadLastPageData();
    void modelSetItem(int start, int end);
    int getDbItemNumber();
};

#endif // ZRECORD_H
