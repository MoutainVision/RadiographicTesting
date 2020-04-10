#ifndef RECOGNIZEWDG_H
#define RECOGNIZEWDG_H

#include <QWidget>

namespace Ui {
class RecognizeWdg;
}

class RecognizeWdg : public QWidget
{
    Q_OBJECT

public:
    explicit RecognizeWdg(QWidget *parent = nullptr);
    ~RecognizeWdg();

private:
    Ui::RecognizeWdg *ui;
};

#endif // RECOGNIZEWDG_H
