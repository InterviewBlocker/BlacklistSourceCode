#ifndef SAY_H
#define SAY_H

#include <QWidget>

namespace Ui {
class say;
}

class say : public QWidget
{
    Q_OBJECT

public:
    explicit say(QWidget *parent = nullptr);
    ~say();

private:
    Ui::say *ui;
};

#endif // SAY_H
