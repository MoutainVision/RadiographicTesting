#include "aboutdlg.h"
#include "ui_aboutdlg.h"

AboutDlg::AboutDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDlg)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::WindowCloseButtonHint);
}

AboutDlg::~AboutDlg()
{
    delete ui;
}

void AboutDlg::setSoftName(QString softname)
{
    ui->label_softname->setText(softname);
}

void AboutDlg::setVersion(QString version)
{
    ui->label_version->setText(version);
}
