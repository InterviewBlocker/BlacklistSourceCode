#ifndef QUN_H
#define QUN_H

#include <QMainWindow>

namespace Ui {
class qun;
}

class qun : public QMainWindow
{
    Q_OBJECT

public:
    explicit qun(QWidget *parent = nullptr);
    ~qun();

private:
    Ui::qun *ui;
};

#endif // QUN_H
