#include "VppChartView.h"
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>

// Ctor
VppChartView::VppChartView(QChart* chart, QWidget* parent/*= Q_NULLPTR*/) :
QChartView(chart,parent) {

	setRubberBand(QChartView::RectangleRubberBand);
}

// Dtor
VppChartView::~VppChartView() {

}

void VppChartView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Plus:
        chart()->zoomIn();
        break;
    case Qt::Key_Minus:
        chart()->zoomOut();
        break;
    case Qt::Key_Left:
        chart()->scroll(-1.0, 0);
        break;
    case Qt::Key_Right:
        chart()->scroll(1.0, 0);
        break;
    case Qt::Key_Up:
        chart()->scroll(0, 1.0);
        break;
    case Qt::Key_Down:
        chart()->scroll(0, -1.0);
        break;
//    case Qt::Key_Space:
//        switchChartType();
//        break;
    default:
        QGraphicsView::keyPressEvent(event);
        break;
    }
}


/////////////


// Ctor
VppPolarChartView::VppPolarChartView(QChart* chart, QWidget* parent/*= Q_NULLPTR*/) :
		VppChartView(chart,parent) {
}

// Dtor
VppPolarChartView::~VppPolarChartView() {

}

void VppPolarChartView::keyPressEvent(QKeyEvent *event)
{

    QGraphicsView::keyPressEvent(event);

    switch (event->key()) {
    case Qt::Key_Plus:
        chart()->scroll(0, 1.0);
        break;
    case Qt::Key_Minus:
        chart()->scroll(0, -1.0);
        break;
    default:
        QGraphicsView::keyPressEvent(event);
        break;
    }
}
