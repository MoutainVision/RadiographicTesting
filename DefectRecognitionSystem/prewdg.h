#ifndef PREWDG_H
#define PREWDG_H

#include <QWidget>
#include <QPaintEvent>
#include <QPainter>

#include <thread>


#include "appconfig.h"
#include "readdcmfile.h"


#include "Base/FunctionTransfer.h"


namespace Ui {
class PreWdg;
}

class PreWdg : public QWidget
{
    Q_OBJECT

public:
    explicit PreWdg(QWidget *parent = nullptr);

    ~PreWdg();

    void setSelectStatus(bool status);


    void setDCMFileInfo(DcmFileNode info);

    DcmFileNode getDCMFileNode(){return mInfo;}

    void reLoad();

signals:
    void sig_clickItem();

    void sig_doubleClickItem();


protected:
    bool eventFilter(QObject *obj, QEvent *e);



private:
    Ui::PreWdg *ui;

    DcmFileNode mInfo;

    QPixmap mPreviewPixImg;

    bool    mBHover;

    bool    mBSelect;
};

#endif // PREWDG_H
