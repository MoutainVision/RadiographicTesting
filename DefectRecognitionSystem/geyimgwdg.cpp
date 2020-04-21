#include "geyimgwdg.h"
#include "ui_geyimgwdg.h"

GeyImgWdg::GeyImgWdg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GeyImgWdg)
{
    ui->setupUi(this);

    setWindowFlags(Qt::WindowStaysOnTopHint);

    ui->widget_paint->installEventFilter(this);
}

void GeyImgWdg::SetIntensityCurveValues(std::vector<unsigned short> aIntensity, int pixWidth)
{
    m_aIntensity.clear();

    m_aIntensity = aIntensity;

    m_pixWidth = pixWidth;

    ui->label_max_pix->setText(QString("%1").arg(m_pixWidth));

    update();
}

void GeyImgWdg::resizeEvent(QResizeEvent *e)
{
    update();

    return QWidget::resizeEvent(e);
}

bool GeyImgWdg::eventFilter(QObject *obj, QEvent *e)
{
    if (obj == ui->widget_paint)
    {
        if (e->type() == QEvent::Paint)
        {
            QPainter  p(ui->widget_paint);

            int chartRectHeight = ui->widget_paint->height();
            int chartRectWidth = ui->widget_paint->width();

            float perHeight = (float)chartRectHeight / 65535;
            float perWidth = (float)chartRectWidth / m_pixWidth;

            QPainterPath path;
            for (int i=0; i<m_aIntensity.size(); i++)
            {
                unsigned short perValue =  m_aIntensity.at(i);

                float ax = i * perWidth;
                float ay = chartRectHeight - (perValue * perHeight);

                if (i == 0)
                    path.moveTo(ax, ay);
                else
                    path.lineTo(ax, ay);
            }

            p.drawPath(path);

            p.drawLine(QPoint(m_curPoint.x(),0), QPoint(m_curPoint.x(), ui->widget_paint->height()));
        }
        else if (e->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(e);
            QPoint pt = mouseEvent->pos();

            m_curPoint = pt;

            int chartRectWidth = ui->widget_paint->width();
            float perWidth = (float)chartRectWidth / m_pixWidth;

            int index = m_curPoint.x() / perWidth;
            if (index > 0 && index < m_aIntensity.size())
            {
                ui->label_grey->setText(QString("%1").arg(m_aIntensity.at(index)));
                ui->label_pixel->setText(QString("%1").arg(index));

                mCurIndex = index;
                emit sig_curGreayPos();
            }

            update();
        }
    }

    return QWidget::eventFilter(obj, e);
}

GeyImgWdg::~GeyImgWdg()
{
    delete ui;
}
