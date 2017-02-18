/****************************************************************************
 **
 ** Copyright (C) 2016 The Qt Company Ltd.
 ** Contact: https://www.qt.io/licensing/
 **
 ** This file is part of the Qt Charts module of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:GPL$
 ** Commercial License Usage
 ** Licensees holding valid commercial Qt licenses may use this file in
 ** accordance with the commercial license agreement provided with the
 ** Software or, alternatively, in accordance with the terms contained in
 ** a written agreement between you and The Qt Company. For licensing terms
 ** and conditions see https://www.qt.io/terms-conditions. For further
 ** information use the contact form at https://www.qt.io/contact-us.
 **
 ** GNU General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU
 ** General Public License version 3 or (at your option) any later version
 ** approved by the KDE Free Qt Foundation. The licenses are as published by
 ** the Free Software Foundation and appearing in the file LICENSE.GPL3
 ** included in the packaging of this file. Please review the following
 ** information to ensure the GNU General Public License requirements will
 ** be met: https://www.gnu.org/licenses/gpl-3.0.html.
 **
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

#include "MultiplePlotWidget.h"
#include <QtCharts/QLegendMarker>
#include <QtCharts/QXYLegendMarker>
#include <QtCore/QDebug>
#include <QtCore/QTime>
#include <QScreen>
#include <QtGui>
#include "MultiplePlotChartComponent.h"
#include "XYChart.h"
#include "VppPolarChart.h"

MultiplePlotWidget::MultiplePlotWidget(QMainWindow* parent/*=Q_NULLPTR*/, Qt::WindowFlags flags/*=0*/) :
	VppTabDockWidget(parent, flags) {

	setObjectName("MultiplePlotView");
	setWindowTitle("MultiplePlotView");

	// Create an empty widget that will hold the multiplot
	QWidget* widget = new QWidget;

	// Set this widget to the dockable widget
	setWidget(widget);

	// create layout as a child of the widget
	pGridLayout_= new QGridLayout(widget);

	// Attempt maximizing the plotting area
	pGridLayout_->setContentsMargins(0,0,0,0);
	pGridLayout_->setHorizontalSpacing(0);
	pGridLayout_->setVerticalSpacing(0);

	// Instantiate a ptr to temporarily store the adress of a chart to be assigned
	// to the grid layout, but also to be assigned to the chartList_. We iterate on
	// the chart list to define the actions used to expand one chart on double click.
	MultiplePlotChartComponent* pChart;

	pChart = new MultiplePlotChartComponent( new XYChart );
	pGridLayout_->addWidget(pChart, 0, 0);
	chartList_ << pChart;

	pChart = new MultiplePlotChartComponent( new VppPolarChart );
	pGridLayout_->addWidget(pChart, 0, 1);
	chartList_ << pChart;

	pChart = new MultiplePlotChartComponent( new VppPolarChart );
	pGridLayout_->addWidget(pChart, 1, 0);
	chartList_ << pChart;

	pChart = new MultiplePlotChartComponent( new XYChart );
	pGridLayout_->addWidget(pChart, 1, 1);
	chartList_ << pChart;

	setLayout(pGridLayout_);

	// Connect the widget to the function that handles the widget visibility in the layout
	foreach (MultiplePlotChartComponent* view, chartList_) {
		QObject::disconnect(view, SIGNAL(requestFullScreen(const MultiplePlotChartComponent*)), this, SLOT(toggleFullScreen(const MultiplePlotChartComponent*)));
		QObject::connect(view, SIGNAL(requestFullScreen(const MultiplePlotChartComponent*)), this, SLOT(toggleFullScreen(const MultiplePlotChartComponent*)));
	}

    // Set a minimum size for this widget
    QScreen* pScreen= QGuiApplication::primaryScreen();
	setMinimumSize(QSize(pScreen->size().width()/ 3, pScreen->size().height() / 3));

}

// What to do when one of the plots is clicked?
void MultiplePlotWidget::toggleFullScreen(const MultiplePlotChartComponent* pClickedView) {

	// Loop on the list of chart view and reverse the visibility of all,
	// except the one that was clicked
	foreach (MultiplePlotChartComponent* view, chartList_) {

		if(view!=pClickedView)
			view->setVisible( !view->isVisible() );
	}
}

// Dtor
MultiplePlotWidget::~MultiplePlotWidget() {
}



