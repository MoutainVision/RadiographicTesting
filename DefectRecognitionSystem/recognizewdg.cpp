#include "recognizewdg.h"
#include "ui_recognizewdg.h"

RecognizeWdg::RecognizeWdg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecognizeWdg)
{
    ui->setupUi(this);
}

RecognizeWdg::~RecognizeWdg()
{
    delete ui;
}
