#ifndef ZHELP_H
#define ZHELP_H

#include <QWidget>

namespace Ui {
class zHelp;
}

class zHelp : public QWidget
{
    Q_OBJECT

public:
    explicit zHelp(QWidget *parent = 0);
    ~zHelp();

private:
    Ui::zHelp *ui;
};

#endif // ZHELP_H
