#include "VppChartView.h"
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include "qEvent.h"

// Ctor
VppChartView::VppChartView(QChart* chart, QWidget* parent/*= Q_NULLPTR*/) :
QChartView(chart,parent) {

	// Reduce the margins and maximize the plot region
	setContentsMargins(0,0,0,0);

	setRubberBand(QChartView::RectangleRubberBand);

	// Improve the readability of the chart with anti-aliasing
	setRenderHint(QPainter::Antialiasing);

}

// Dtor
VppChartView::~VppChartView() {

}

// Override the parent class method called on double click
void VppChartView::mouseDoubleClickEvent(QMouseEvent* pMouseEvent) {

	// Launch a signal that will be caught by the MultiplePlotWidget
	requestFullScreen(this);

	// This method decorates the parent method
	QChartView::mouseDoubleClickEvent(pMouseEvent);
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
