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

#include <QtCharts/QChartView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtCore/QDebug>
#include <QtWidgets/QFormLayout>
#include <QtCharts/QXYLegendMarker>
#include "iostream"
#include <QScreen>
#include <QtGui>
#include "XYChartWidget.h"

QT_CHARTS_USE_NAMESPACE

XYChartWidget::XYChartWidget(QMainWindow* parent/*=Q_NULLPTR*/, Qt::WindowFlags flags/*=0*/) :
	VppTabDockWidget(parent, flags) {

    setObjectName("XYPlotView");
    setWindowTitle("XYPlotView");

    // Instantiate the XY plot
    pChart_= new XYChart;

    // Instantiate a view for this chart. The view is to be visualized in the widget
    pChartView_.reset(new VppChartView(pChart_));

    // Improve the view of this plot using antialiasing
    pChartView_->setRenderHint(QPainter::Antialiasing);

    // Set the chart view to be visualized in this dockable widget
    setWidget(pChartView_.get());

    // Set a minimum size for this widget
    QScreen* pScreen= QGuiApplication::primaryScreen();
	setMinimumSize(QSize(pScreen->size().width()/ 3, pScreen->size().height() / 3));

}

// Dtor
XYChartWidget::~XYChartWidget() {
}

// Returns the chart view
QChartView* XYChartWidget::getChartView() const {
	return pChartView_.get();
}
