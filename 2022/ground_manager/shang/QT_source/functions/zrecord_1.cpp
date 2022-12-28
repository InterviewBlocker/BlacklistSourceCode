#include "zrecord.h"
#include "ui_zrecord.h"

ZRecord::ZRecord(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ZRecord)
{
    ui->setupUi(this);
    model = new QStandardItemModel(this);
    ui->tableView->setModel(model);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->model->setHorizontalHeaderItem(0, new QStandardItem("车牌号"));
    this->model->setHorizontalHeaderItem(1, new QStandardItem("出/入"));
    this->model->setHorizontalHeaderItem(2, new QStandardItem("入库时间"));
    this->model->setHorizontalHeaderItem(3, new QStandardItem("出库时间"));
    this->model->setHorizontalHeaderItem(4, new QStandardItem("停车时长(s)"));
    ui->tableView->setColumnWidth(0,250);
    ui->tableView->setColumnWidth(1,150);
    ui->tableView->setColumnWidth(2,350);
    ui->tableView->setColumnWidth(3,350);
    ui->tableView->setColumnWidth(4,200);

    int curRow = ui->tableView->currentIndex().row();
    ui->tableView->model()->removeRow(curRow);
}

ZRecord::~ZRecord()
{
    delete ui;
}

void ZRecord::loadDefaultData()
{
    loadLastPageData();
}
void ZRecord::loadLastPageData()
{
    int num = getDbItemNumber();
    if( num == 0){
        ui->label_CurrentPage->setText("1");
        ui->label_allPage->setText("1");
        ui->label_allNum->setText("0");
        return;
    }
    else {
        int p;
        if(num%pageRecordCount!=0){
            p = num/pageRecordCount;
        }
        else {
            p = num/pageRecordCount-1;
        }
        ui->label_CurrentPage->setText(QString::number(p + 1));
        ui->label_allPage->setText(QString::number(p + 1));
        ui->tableView->model()->removeRows(0,pageRecordCount);
        modelSetItem(pageRecordCount*p,num);
        ui->label_allNum->setText(QString::number(num));
    }
}

void ZRecord::modelSetItem(int start, int end)
{
    ui->label_warning->clear();
    sqlCarState sc;
    vector<carState> states;
    states = sc.GetAllCarState();
    qint32 i = 0;
    for(int k = start; k != end; k++){
        auto it = states.at(k);
        this->model->setItem(i,0,new QStandardItem(it.CarNumber));
        this->model->setItem(i,1,new QStandardItem(it.State));
        this->model->setItem(i,2,new QStandardItem(it.InTime));
        this->model->setItem(i,3,new QStandardItem(it.OutTime));
        this->model->setItem(i,4,new QStandardItem(QString::number(it.StopTime)));
        for(int j = 0; j < 5; j++){
            this->model->item(i,j)->setTextAlignment(Qt::AlignCenter);
            this->model->item(i,j)->setForeground(Qt::white);
        }
        QVariant qvar;
        qvar.setValue(states.at(k));
        this->model->item(i)->setData(qvar,Qt::UserRole);
        i++;
    }
    ui->tableView->setColumnWidth(0,250);
    ui->tableView->setColumnWidth(1,150);
    ui->tableView->setColumnWidth(2,350);
    ui->tableView->setColumnWidth(3,350);
    ui->tableView->setColumnWidth(4,200);
    if(this->model->rowCount() > 0){
        ui->tableView->selectRow(0);
        ui->btnDelete->setEnabled(true);
        ui->btnClear->setEnabled(true);
    }
    else {
        ui->btnDelete->setEnabled(false);
        ui->btnClear->setEnabled(false);
    }
}
int ZRecord::getDbItemNumber()
{
    sqlCarState sc;
    vector<carState> states;
    states = sc.GetAllCarState();

    int num = 0;
    for(auto it = states.begin(); it!=states.end(); ++it){
        num++;
    }
    return num;
}
void ZRecord::on_btnDelete_clicked()
{
    if(getDbItemNumber() == 0){
        return;
    }
    QVariant qvar = this->model->item(ui->tableView->currentIndex().row())->data(Qt::UserRole);
    if(!qvar.isValid()){
        return;
    }
    auto ret = QMessageBox::question(this,tr("删除"),tr("确定要删除此项吗？"),\
                                     tr("确定"),tr("取消"),0);
    if(ret == 0){
        sqlCarState sc;
        carState selectUser = qvar.value<carState>();
        sc.removeCarState(selectUser.ID);

        int curRow = ui->tableView->currentIndex().row();
        ui->tableView->model()->removeRow(curRow);
        int selectRow = 0;
        if(curRow < this->model->rowCount()){
            selectRow = curRow;
        }
        else {
            selectRow = this->model->rowCount() - 1;
        }
        ui->spinBox_goToPage->setValue(ui->label_CurrentPage->text().toInt());
        on_pbtnGoto_clicked();
    }
}

void ZRecord::on_btnClear_clicked()
{
    if(getDbItemNumber() == 0){
        return;
    }
    auto ret = QMessageBox::question(this,tr("清空"),tr("清空后将无法恢复,确定要清空数据库吗？"),\
                                     tr("确定"),tr("取消"),0);
    if(ret == 0){
        sqlCarState sc;
        vector<carState> users;
        users = sc.GetAllCarState();
        int i = 0;
        for(auto it = users.begin(); it!=users.end();++it){
            sc.removeCarState(it->ID);
            i++;
        }
        ui->tableView->model()->removeRows(0,i-1);
        ui->label_CurrentPage->setText("1");
        ui->label_allPage->setText("1");
        ui->label_allNum->setText("0");
    }
}

void ZRecord::on_btnExport_clicked()
{
    if(getDbItemNumber() == 0){
        return;
    }
    sqlCarState sc;
    vector<carState> states;
    states = sc.GetAllCarState();
    QStandardItemModel *ExportModel = new QStandardItemModel();
    qint32 i = 0;
    for(auto it = states.begin(); it!=states.end();++it){
        ExportModel->setItem(i,0,new QStandardItem(it->CarNumber));
        ExportModel->setItem(i,1,new QStandardItem(it->State));
        ExportModel->setItem(i,2,new QStandardItem(it->InTime));
        ExportModel->setItem(i,3,new QStandardItem(it->OutTime));
        ExportModel->setItem(i,4,new QStandardItem(QString::number(it->StopTime)));
        i++;
    }
    if(ExportModel->rowCount() <= 0){
        QMessageBox::warning(this,tr("导出"),tr("当前数据库记录为空,无法导出"),tr("确定"));
    }
    else {

        QString filepath = QFileDialog::getSaveFileName(this, tr("Save as..."),
                                                        QString("出入库车辆记录表"), tr("EXCEL files (*.xls *.xlsx)"));

        int row = ExportModel->rowCount();
        int col = ExportModel->columnCount();
        QList<QString> list;
        //添加列标题
        QString HeaderRow;
        for (int i = 0; i<col; i++)
        {
            HeaderRow.append(model->horizontalHeaderItem(i)->text() + "\t");
        }
        list.push_back(HeaderRow);
        for (int i = 0; i<row; i++)
        {
            QString rowStr = "";
            for (int j = 0; j<col; j++)
            {
                rowStr += ExportModel->item(i, j)->text() + "\t";
            }
            list.push_back(rowStr);
        }
        //将表格内容添加到富文本中
        QTextEdit textEdit;
        for (int i = 0; i<list.size(); i++)
        {
            textEdit.append(list.at(i));
        }
        //写入文件中
        QFile file(filepath);
        if (file.open(QFile::WriteOnly | QIODevice::Text))
        {
            QTextStream ts(&file);
            //            ts.setCodec("UTF-8");
            ts << textEdit.document()->toPlainText();
            file.close();
        }

    }
}

void ZRecord::on_pbtnFirstPage_clicked()
{
    int num = getDbItemNumber();
    if( num == 0){
        ui->label_CurrentPage->setText("1");
        ui->label_allPage->setText("1");
        ui->label_allNum->setText("0");
        return;
    }
    else if(num < pageRecordCount){
        ui->label_CurrentPage->setText("1");
        ui->label_allPage->setText(QString::number(num/pageRecordCount + 1));
        modelSetItem(0,num);
    }
    else {
        ui->label_CurrentPage->setText("1");
        ui->label_allPage->setText(QString::number(num/pageRecordCount + 1));
        modelSetItem(0,pageRecordCount);
    }
}

void ZRecord::on_pbtnLastPage_clicked()
{
    loadLastPageData();
}

void ZRecord::on_pbtnPreviousPage_clicked()
{
    if(getDbItemNumber() == 0){
        ui->label_CurrentPage->setText("1");
        ui->label_allPage->setText("1");
        ui->label_allNum->setText("0");
        return;
    }
    else {
        int currentPage = ui->label_CurrentPage->text().toInt();
        int PreviousPage = currentPage - 1;
        if(currentPage > 2 )
        {
            modelSetItem(pageRecordCount*(PreviousPage-1),pageRecordCount*PreviousPage);
            ui->label_CurrentPage->setText(QString::number(PreviousPage));
        }
        else {
            on_pbtnFirstPage_clicked();
        }
    }
}

void ZRecord::on_pbtnNextPage_clicked()
{
    int num = getDbItemNumber();
    if( num == 0){
        ui->label_CurrentPage->setText("1");
        ui->label_allPage->setText("1");
        ui->label_allNum->setText("0");
        return;
    }
    else {
        int currentPage = ui->label_CurrentPage->text().toInt();
        if(currentPage <  num/pageRecordCount )
        {
            ui->label_CurrentPage->setText(QString::number(currentPage+1));
            modelSetItem(pageRecordCount*currentPage,pageRecordCount*(currentPage+1));
        }
        else {
            loadLastPageData();
        }
    }
}

void ZRecord::on_pbtnGoto_clicked()
{
    if(getDbItemNumber() == 0){
        ui->label_CurrentPage->setText("1");
        ui->label_allPage->setText("1");
        ui->label_allNum->setText("0");
        return;
    }
    else {
        int goToPage = ui->spinBox_goToPage->value();
        int allPage = ui->label_allPage->text().toInt();
        if(goToPage < 1 or goToPage > allPage) {
            ui->label_warning->setText(tr("请输入1-%1有效页码!").arg(allPage));
        }
        else if (goToPage == 1){
            on_pbtnFirstPage_clicked();
        }
        else if (goToPage == allPage) {
            loadLastPageData();
        }
        else{
            ui->label_CurrentPage->setText(QString::number(goToPage));
            modelSetItem(pageRecordCount*(goToPage-1),pageRecordCount*goToPage);
        }
    }
}



