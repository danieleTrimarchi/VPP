#include "MultiplePlotChartComponent.h"

#include "qEvent.h"

// Ctor
MultiplePlotChartComponent::MultiplePlotChartComponent(QWidget* parent /*= Q_NULLPTR*/) :
QChartView(parent) {

}

// Ctor taking a chart
MultiplePlotChartComponent::MultiplePlotChartComponent(QChart *chart, QWidget *parent /*= Q_NULLPTR*/) :
	QChartView(chart, parent) {

	// Reduce the margins and maximize the plot region
	setContentsMargins(0,0,0,0);

	// Set a rubberband used to zoom the plot
	setRubberBand(QChartView::RectangleRubberBand);

	// Improve the readeability of the chart with anti-aliasing
	setRenderHint(QPainter::Antialiasing);
}

// Dtor
MultiplePlotChartComponent::~MultiplePlotChartComponent() {

}


// Override the parent class method called on double click
void MultiplePlotChartComponent::mouseDoubleClickEvent(QMouseEvent* pMouseEvent) {

	// Launch a signal that will be caught by the MultiplePlotWidget
	requestFullScreen(this);

	// This method decorates the parent method
	QChartView::mouseDoubleClickEvent(pMouseEvent);
}

void MultiplePlotChartComponent::keyPressEvent(QKeyEvent *event) {

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
