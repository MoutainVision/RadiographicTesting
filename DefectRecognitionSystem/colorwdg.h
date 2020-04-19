#ifndef COLORWDG_H
#define COLORWDG_H

#include <QWidget>
#include <QAbstractButton>

namespace Ui {
class ColorWdg;
}

class ColorWdg : public QWidget
{
    Q_OBJECT

public:
    explicit ColorWdg(QWidget *parent = 0);
    ~ColorWdg();

    QColor getCurColor(){return mCurColor;}

signals:
    void sig_btnColorClick();

private slots:
    void slot_buttonGroupColorClick(QAbstractButton *btn);

private:
    Ui::ColorWdg *ui;

    QColor     mCurColor;

};

#endif // COLORWDG_H
