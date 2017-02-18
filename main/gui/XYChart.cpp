#include "XYChart.h"
#include <QtCharts/QLegend>
#include <QtCharts/QLegendMarker>
#include <QtWidgets/QGraphicsGridLayout>
#include <QtCore/QtMath>
#include <QtCore/QDebug>

// Explicit Constructor
XYChart::XYChart(
		QGraphicsItem* parent /*=Q_NULLPTR*/,
		Qt::WindowFlags wFlags/*=Qt::WindowFlags()*/) :
		QChart(parent,wFlags) {

    // Add few series
    addSeries();
    addSeries();
    addSeries();
    addSeries();

    // Connect the signal issued when the legend markers are clicked -> allow for showing/hiding the curves
    connectMarkers();

    // Set the title and show legend
    setTitle("Legendmarker example (click on legend)");
    legend()->setVisible(true);
    legend()->setAlignment(Qt::AlignBottom);

    // Attempt maximizing the plotting area
    layout()->setContentsMargins(0,0,0,0);
    setBackgroundRoundness(0);

}

// Dtor
XYChart::~XYChart() {

}

void XYChart::addSeries() {

    QLineSeries* series = new QLineSeries();
    series_.append(series);

    series->setName(QString("line " + QString::number(series_.count())));

    // Make some sine wave for data
    QList<QPointF> data;
    int offset = series_.count();
    for (int i = 0; i < 360; i++) {
        qreal x = offset * 20 + i;
        data.append(QPointF(i, qSin(2.0 * 3.141592 * x / 360.0)));
    }

    series->append(data);
    QChart::addSeries(series);

    if (series_.count() == 1) {
        createDefaultAxes();
    }
}

void XYChart::connectMarkers()
{
    // Connect all markers to handler
    foreach (QLegendMarker* marker, legend()->markers()) {
        // Disconnect possible existing connection to avoid multiple connections
        QObject::disconnect(marker, SIGNAL(clicked()), this, SLOT(handleMarkerClicked()));
        QObject::connect(marker, SIGNAL(clicked()), this, SLOT(handleMarkerClicked()));
    }
}


void XYChart::disconnectMarkers()
{
    foreach (QLegendMarker* marker, legend()->markers()) {
        QObject::disconnect(marker, SIGNAL(marker->clicked()), this, SLOT(handleMarkerClicked()));
    }
}


void XYChart::handleMarkerClicked()
{
    QLegendMarker* marker = qobject_cast<QLegendMarker*> (sender());
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

void XYChart::removeSeries()
{
    // Remove last series from chart
    if (series_.count() > 0) {
        QLineSeries *series = series_.last();
        QChart::removeSeries(series);
        series_.removeLast();
        delete series;
    }
}


