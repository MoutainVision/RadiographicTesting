#ifndef PREWDG_H
#define PREWDG_H

#include <QWidget>
#include <QPaintEvent>
#include <QPainter>

#include "appconfig.h"

namespace Ui {
class PreWdg;
}

class PreWdg : public QWidget
{
    Q_OBJECT

public:
    explicit PreWdg(QWidget *parent = nullptr);

    ~PreWdg();


    void setDCMFileInfo(DcmFileNode info);

    DcmFileNode getDCMFileNode(){return mInfo;}

    void reLoad();

signals:
    void sig_clickItem();


protected:
    bool eventFilter(QObject *obj, QEvent *e);



private:
    Ui::PreWdg *ui;

    DcmFileNode mInfo;

    QPixmap mPreviewPixImg;
};

#endif // PREWDG_H
