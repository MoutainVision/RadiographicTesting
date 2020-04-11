#include "recognizewdg.h"
#include "ui_recognizewdg.h"

#include <QFile>

RecognizeWdg::RecognizeWdg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecognizeWdg)
{
    ui->setupUi(this);

    ui->widget_measure->hide();
    ui->widget_tool->hide();


    connect(ui->pushButton_measure, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_invert, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
}

RecognizeWdg::~RecognizeWdg()
{
    delete ui;
}

void RecognizeWdg::slotBtnClick(bool bClick)
{
    if (QObject::sender() == ui->pushButton_measure)
    {
        ui->widget_tool->setVisible(ui->pushButton_measure->isChecked());
    }
}
