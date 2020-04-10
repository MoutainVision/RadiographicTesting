#include "recheckwdg.h"
#include "ui_recheckwdg.h"

ReCheckWdg::ReCheckWdg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReCheckWdg)
{
    ui->setupUi(this);
}

ReCheckWdg::~ReCheckWdg()
{
    delete ui;
}
