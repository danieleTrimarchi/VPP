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

#ifndef MULTIPLEPLOTWIDGET_H
#define MULTIPLEPLOTWIDGET_H

#include <QtCharts/QChartGlobal>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtWidgets/QWidget>
#include <QtWidgets/QGraphicsWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGraphicsGridLayout>
#include <QtCharts/QLineSeries>
#include <QMainWindow>
#include "boost/shared_ptr.hpp"
#include "VppTabDockWidget.h"

QT_CHARTS_USE_NAMESPACE

typedef QPair<QPointF, QString> Data;
typedef QList<Data> DataList;
typedef QList<DataList> DataTable;

// forward declarations
class MultiplePlotChartComponent;
class QCustomPlot;

class MultiplePlotWidget : public VppTabDockWidget {
    Q_OBJECT

public:

	/// Explicit Constructor
	explicit MultiplePlotWidget(QMainWindow* parent= Q_NULLPTR, Qt::WindowFlags flags =0);

	/// Dtor
	virtual ~MultiplePlotWidget();

	/// Add a chart in a given position
	void addChart(QChart&, size_t px, size_t py);

	/// Add a chart in a given position
	void addChart(QCustomPlot*, size_t px, size_t py);

public slots:

	/// What to do when one of the plots is clicked?
	void toggleFullScreen(const MultiplePlotChartComponent*);

private:

	/// Connect all the charts in the multiplot to the toggleFullScreen
	/// signal that is used to expand one chart to full screen
	void connectFullScreenSignals();

	/// List with all the QChart-style plots that will be visualized
	QList<MultiplePlotChartComponent*> chartList_;

	/// List with all the qCustomPlot-style plots that will be visualized
	QList<QCustomPlot*> customPlotList_;

	/// Grid layout of the multiple plot. Raw ptr, the ownership is taken by the widget
	QGridLayout* pGridLayout_;
};

#endif
