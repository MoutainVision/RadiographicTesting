#ifndef RECHECKWDG_H
#define RECHECKWDG_H

#include <QWidget>

namespace Ui {
class ReCheckWdg;
}

class ReCheckWdg : public QWidget
{
    Q_OBJECT

public:
    explicit ReCheckWdg(QWidget *parent = nullptr);
    ~ReCheckWdg();

private:
    Ui::ReCheckWdg *ui;
};

#endif // RECHECKWDG_H
