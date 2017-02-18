/****************************************************************************
 **
 ** Copyright (C) 2016 The Qt Company Ltd.
 ** Contact: https://www.qt.io/licensing/
 **
 ** This file is part of the demonstration applications of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:BSD$
 ** Commercial License Usage
 ** Licensees holding valid commercial Qt licenses may use this file in
 ** accordance with the commercial license agreement provided with the
 ** Software or, alternatively, in accordance with the terms contained in
 ** a written agreement between you and The Qt Company. For licensing terms
 ** and conditions see https://www.qt.io/terms-conditions. For further
 ** information use the contact form at https://www.qt.io/contact-us.
 **
 ** BSD License Usage
 ** Alternatively, you may use this file under the terms of the BSD license
 ** as follows:
 **
 ** "Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 **   * Redistributions of source code must retain the above copyright
 **     notice, this list of conditions and the following disclaimer.
 **   * Redistributions in binary form must reproduce the above copyright
 **     notice, this list of conditions and the following disclaimer in
 **     the documentation and/or other materials provided with the
 **     distribution.
 **   * Neither the name of The Qt Company Ltd nor the names of its
 **     contributors may be used to endorse or promote products derived
 **     from this software without specific prior written permission.
 **
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 **
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QToolbar>
#include "LogWindow.h"
#include <QtCharts/QChartGlobal>
#include <QtCharts/QChart>
#include <QtCharts/QLogValueAxis>
#include <QtCharts/QPolarChart>
#include <QtCharts/QLegend>
#include "VariablesDockWidget.h"
#include "LogDockWidget.h"
#include "PolarPlotWidget.h"
#include "MultiplePlotWidget.h"
#include "ThreeDPlotWidget.h"
#include "XYChartWidget.h"
#include "VppTableDockWidget.h"

class ToolBar;
QT_FORWARD_DECLARE_CLASS(QMenu)
QT_CHARTS_USE_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:

	typedef QMap<QString, QSize> CustomSizeHintMap;

	/// Ctor. Explicit specifies that the constructor
	/// doesn't allow implicit conversions or copy-initialization
	explicit MainWindow(
			QWidget *parent = Q_NULLPTR,
			Qt::WindowFlags flags = 0);

	/// Virtual destructor
	virtual ~MainWindow();

	/// Append a string to the log
	void appendToLog(const QString &text);

	/// Create a toolbar with some actions
	void setupToolBar();

public slots:

	/// Import the data from variableFile.txt
	void import();

	/// Run the VPP analysis
	void run();

	/// Save the VPP results to file
	bool saveResults();

	// if save was successiful, return true, otherwise false
	bool saveFile(const QString &fileName);

	/// Import the VPP results from file to the current session
	void importResults();

	/// Add a table widget with the results
	void tableResults();

	/// Plot the velocity polars
	void plotPolars();

	/// Plot the sail coefficients
	void plotSailCoeffs();

	/// Add a multiPlot widget
	void multiPlot();

	/// Add a 3dPlot widget
	void threedPlot();

	/// print out the software info
	void about();

	void actionTriggered(QAction *action);

	/// Slot called on deletion of a plotWidget. It assures
	/// the sync of the tabbedWidgets_ vector
	void updateTabbedWidgetsVector(const VppTabDockWidget*);

private:

	/// Set the menu bar
	void setupMenuBar();

	/// Add the widget to the tabbed ones
	void tabDockWidget(VppTabDockWidget*);

	// Remove a widget from the buffer vector. This should be called from the
	// as a slot from the delete of the QDockWidget right before the destruction
	void removeWidgetFromVector(VppTabDockWidget* pWidget);

	/// Log widget - This must be declared at the very beginning as we want to
	/// make sure it is the last item to be removed on destruction
	boost::shared_ptr<QTextEdit> pLogWidget_;

	/// Menus available to the menu bar
	QMenu* pVppActionMenu_;
	QMenu* pPlotMenu_;
	QMenu* pPreferencesMenu_;
	boost::shared_ptr<QMenu> pWidgetMenu_;

	/// Toolbar with some shortcuts to actions
	QToolBar* pToolBar_;

	/// Widget that contains the tabular view of the results
	boost::shared_ptr<VppTabDockWidget> pTableWidget_;

	/// Widget that contains the variable tree or the log view
	boost::shared_ptr<VppTabDockWidget> pPolarPlotWidget_;

	/// XY Plot widget
	boost::shared_ptr<VppTabDockWidget> pXYPlotWidget_;

	/// Multi-plot widget
	boost::shared_ptr<VppTabDockWidget> pMultiPlotWidget_;

	/// Three dimensional plot widget
	boost::shared_ptr<VppTabDockWidget> p3dPlotWidget_;

	/// Variable Widget
	boost::shared_ptr<VariablesDockWidget> pVariablesWidget_;

	/// This action generates the menu that drives the presence of the Variable
	/// tree widget or the Log widget
	QAction* VariableAction_, *LogAction_;

	std::vector<VppTabDockWidget*> tabbedWidgets_;

	/// Main window label
	const QString windowLabel_;

};

#endif // MAINWINDOW_H
