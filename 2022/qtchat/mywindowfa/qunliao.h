#ifndef QUNLIAO_H
#define QUNLIAO_H

#include <QWidget>

namespace Ui {
class qunliao;
}

class qunliao : public QWidget
{
    Q_OBJECT

public:
    explicit qunliao(QWidget *parent = nullptr);
    ~qunliao();

private:
    Ui::qunliao *ui;
};

#endif // QUNLIAO_H
