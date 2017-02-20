#include "VppPolarChart.h"
#include <QtWidgets/QGesture>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGraphicsLayout>
#include "math.h"
#include <QDebug>

// Ctor
VppPolarChart::VppPolarChart():
	QPolarChart() {

	// Set the plot axes
    pAngularAxis_.reset(new QValueAxis());
    pAngularAxis_->setTickCount(9); // First and last ticks are co-located on 0/360 angle.
    pAngularAxis_->setLabelFormat("%.1f");
    pAngularAxis_->setShadesVisible(true);
    pAngularAxis_->setShadesBrush(QBrush(QColor(249, 249, 255)));
    pAngularAxis_->setRange(0, 360);
    addAxis(pAngularAxis_.get(), QPolarChart::PolarOrientationAngular);

    pRadialAxis_.reset(new QValueAxis());
    pRadialAxis_->setTickCount(9);
    pRadialAxis_->setLabelFormat("%.1f");
    addAxis(pRadialAxis_.get(), QPolarChart::PolarOrientationRadial);

	// show the legend
	legend()->show();

    // Seems that QGraphicsView (QChartView) does not grab gestures.
    // They can only be grabbed here in the QGraphicsWidget (QChart).
    //grabGesture(Qt::PanGesture);
    grabGesture(Qt::PinchGesture);

    // Attempt maximizing the plotting area
    layout()->setContentsMargins(0,0,0,0);
    setBackgroundRoundness(0);

}

// Dtor
VppPolarChart::~VppPolarChart() {

}

/// Add curves to the plot
void VppPolarChart::plotPolars() {

    const qreal angularMin = 30;
    const qreal angularMax = 180;

    const qreal radialMin = 0;
    const qreal radialMax = 1.5;

	QSplineSeries *series = new QSplineSeries();
    series->setName("boat speed");
    for (int i = angularMin; i <= angularMax; i += 5)
        series->append(i, cos(float(i)/angularMax) );

    // Set the color of this curve
    series->setColor(QColor("Red"));

    addSeries(series);

    series->attachAxis(pRadialAxis_.get());
    series->attachAxis(pAngularAxis_.get());

    pRadialAxis_->setRange(radialMin,radialMax);

    // Allow for switching on-off the curves on click
    connectMarkers();

    legend()->setVisible(true);
}

// Allow for switching on-off the curves on click
void VppPolarChart::connectMarkers() {

    // Connect all markers to handler
    foreach (QLegendMarker* marker, legend()->markers()) {
        // Disconnect possible existing connection to avoid multiple connections
        QObject::disconnect(marker, SIGNAL(clicked()), this, SLOT(handleMarkerClicked()));
        QObject::connect(marker, SIGNAL(clicked()), this, SLOT(handleMarkerClicked()));
    }
}

void VppPolarChart::disconnectMarkers() {

    foreach (QLegendMarker* marker, legend()->markers()) {
        QObject::disconnect(marker, SIGNAL(clicked()), this, SLOT(handleMarkerClicked()));
    }
}


// What to do on legend marker click?
void VppPolarChart::handleMarkerClicked() {

    QLegendMarker* marker = qobject_cast<QLegendMarker*>(sender());
    Q_ASSERT(marker);

    switch (marker->type())
    {
        case QLegendMarker::LegendMarkerTypeXY:
        {
        // Toggle visibility of series
        marker->series()->setVisible(!marker->series()->isVisible());

        // Turn legend marker back to visible, since hiding series also hides the marker
        // and we don't want it to happen now.
        marker->setVisible(true);

        // Dim the marker, if series is not visible
        qreal alpha = 1.0;

        if (!marker->series()->isVisible()) {
            alpha = 0.5;
        }

        QColor color;
        QBrush brush = marker->labelBrush();
        color = brush.color();
        color.setAlphaF(alpha);
        brush.setColor(color);
        marker->setLabelBrush(brush);

        brush = marker->brush();
        color = brush.color();
        color.setAlphaF(alpha);
        brush.setColor(color);
        marker->setBrush(brush);

        QPen pen = marker->pen();
        color = pen.color();
        color.setAlphaF(alpha);
        pen.setColor(color);
        marker->setPen(pen);

        break;
        }
    default:
        {
        qDebug() << "Unknown marker type";
        break;
        }
    }
}

bool VppPolarChart::sceneEvent(QEvent *event)
{
    if (event->type() == QEvent::Gesture)
        return gestureEvent(static_cast<QGestureEvent *>(event));
    return QChart::event(event);
}

bool VppPolarChart::gestureEvent(QGestureEvent *event)
{
//    if (QGesture* gesture = event->gesture(Qt::PanGesture)) {
//        QPanGesture* pan = static_cast<QPanGesture*>(gesture);
//        QChart::scroll(-(pan->delta().x()), pan->delta().y());
//    }

    if (QGesture* gesture = event->gesture(Qt::PinchGesture)) {
        QPinchGesture* pinch = static_cast<QPinchGesture*>(gesture);
        if (pinch->changeFlags() & QPinchGesture::ScaleFactorChanged) {
            if(pinch->scaleFactor()>1) {
            	QChart::scroll(0, -pinch->scaleFactor());
            } else {
            	QChart::scroll(0, pinch->scaleFactor());
            }
        }
    }


    return true;
}