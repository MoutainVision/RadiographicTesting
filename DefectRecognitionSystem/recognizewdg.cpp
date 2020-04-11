#include "recognizewdg.h"
#include "ui_recognizewdg.h"

#include <QFile>

RecognizeWdg::RecognizeWdg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecognizeWdg)
{
    ui->setupUi(this);

//    QString fileName = ":/qss/main_view.qss";

//    QFile file(fileName);

//    if(file.open(QIODevice::ReadOnly))
//    {
//        ui->widget_img->setStyleSheet(file.readAll());
//        file.close();
//    }

    ui->widget_measure->hide();
}

RecognizeWdg::~RecognizeWdg()
{
    delete ui;
}
