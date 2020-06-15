#include "brightclbwdg.h"
#include "ui_brightclbwdg.h"

BrightClbWdg::BrightClbWdg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BrightClbWdg)
{
    ui->setupUi(this);

    ui->widget_histogram->installEventFilter(this);
}

BrightClbWdg::~BrightClbWdg()
{
    delete ui;
}

void BrightClbWdg::setHist(unsigned aF[], int size)
{
    for (int i=0; i<size; i++)
    {
        m_g[i] = aF[i];
    }

    update();
}

int BrightClbWdg::getYValue(int x, QPoint point1, QPoint point2)
{
    if ((point2.x() - point1.x()) == 0)
        return 0;

    if ((point2.y() - point1.y()) == 0)
        return point2.y();

    double param1 = (x - point1.x()) / (point2.x() - point1.x());
    double param2 = param1 / (point2.y() - point1.y());
    double param3 = param2 + point1.y();

    return param3;
}

bool BrightClbWdg::eventFilter(QObject *obj, QEvent *e)
{
    if (obj == ui->widget_histogram)
    {
        if (e->type() == QEvent::Paint)
        {
            QPainter painter(ui->widget_histogram);

            QPen gPen;
            gPen.setColor(QColor(0, 255 , 0));

            int h = ui->widget_histogram->height()-2;
            int w = ui->widget_histogram->width()-2;

            unsigned gmin =  10000000;
            unsigned gmax = 0;
            for (int i=0; i<65536; i++)
            {
               if (m_g[i] < gmin)
                   gmin = m_g[i];

               if (m_g[i] > gmax)
                   gmax = m_g[i];
            }

            if (gmax - gmin > 0)
            {
               float sx = (float)w / 255;
               float sy = (float)h / (double)(gmax-gmin);

               for (int i=0; i<65536; i++)
               {
                    int x = (int)(i * sx);

                   //
                   int yg = (int)((m_g[i] - gmin) * sy);

                   painter.setPen(gPen);
                   painter.drawLine(x, h, x, h-yg);

                   if (i>1)
                   {
                       int ygPre = (int)((m_g[i-1] - gmin) * sy);
                       int xPre = (int)((i-1) * sx);
                       QPoint point1 = QPoint(xPre,ygPre);
                       QPoint point2 = QPoint(x,yg);

                       for (int j=xPre+1; j<x; j++)
                       {
                           int newYg = getYValue(j, point1, point2);
                           painter.drawLine(j, h, j, h-newYg);
                       }
                   }
               }
            }

            //draw grid
            QPen pen;
            pen.setColor(QColor("#b8cae7"));
            painter.setPen(pen);

            float widthPer = (float)w / 4.0;
            float heightPer = (float)h / 4.0;


            for (int i=0; i<5; i++)
            {
               //ver
               painter.drawLine(QPointF(i*widthPer, 0), QPointF(i*widthPer, h));

               //horizon
               painter.drawLine(QPointF(0, i*heightPer), QPointF(w, i*heightPer));
            }


            return true;
        }
    }

    return QWidget::eventFilter(obj, e);
}
