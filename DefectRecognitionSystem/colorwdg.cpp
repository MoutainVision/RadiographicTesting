#include "colorwdg.h"
#include "ui_colorwdg.h"

ColorWdg::ColorWdg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ColorWdg)
{
    ui->setupUi(this);

	this->setAttribute(Qt::WA_TranslucentBackground);
	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    mCurColor = QColor("#ff0000");

    connect(ui->buttonGroup_color, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(slot_buttonGroupColorClick(QAbstractButton*)));
}

ColorWdg::~ColorWdg()
{
    delete ui;
}

void ColorWdg::slot_buttonGroupColorClick(QAbstractButton *btn)
{
    if (btn == ui->pushButton_black)
    {
        mCurColor = QColor("#000000");
    }
    else if (btn == ui->pushButton_blue)
    {
        mCurColor = QColor("#0000ff");
    }
    else if (btn == ui->pushButton_green)
    {
        mCurColor = QColor("#42ff00");
    }
    else if (btn == ui->pushButton_pink)
    {
        mCurColor = QColor("#ea00ff");
    }
    else if (btn == ui->pushButton_red)
    {
        mCurColor = QColor("#ff0000");
    }
    else if (btn == ui->pushButton_white)
    {
        mCurColor = QColor("#ffffff");
    }

    emit sig_btnColorClick();
}

