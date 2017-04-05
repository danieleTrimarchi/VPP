#include "surfacegraph.h"

#include <QtDataVisualization/QValue3DAxis>
#include <QtDataVisualization/Q3DTheme>
#include <QtGui/QImage>
#include <QtCore/qmath.h>

using namespace QtDataVisualization;

SurfaceGraph::SurfaceGraph(Q3DSurface *surface):
		m_graph(surface),
		m_rangeMinX(0),
		m_rangeMinZ(0),
		m_stepX(0),
		m_stepZ(0),
		m_axisMinSliderX(0),
		m_axisMaxSliderX(0),
		m_axisMinSliderZ(0),
		m_axisMaxSliderZ(0) {

	m_graph->setAxisX(new QValue3DAxis);
	m_graph->setAxisY(new QValue3DAxis);
	m_graph->setAxisZ(new QValue3DAxis);
}

SurfaceGraph::~SurfaceGraph()
{
	delete m_graph;
}

void SurfaceGraph::fillData( ThreeDDataContainer& data ) {

	// Make some space for these data
	vDataProxy_.push_back( new QSurfaceDataProxy() );
	vDataSeries_.push_back( new QSurface3DSeries(vDataProxy_.back()));

	// Assign the data to the proxy
	vDataProxy_.back()->resetArray(data.get());

	vDataSeries_.back()->setDrawMode(QSurface3DSeries::DrawSurfaceAndWireframe);
	vDataSeries_.back()->setFlatShadingEnabled(true);

	m_graph->axisX()->setLabelFormat("%.3f");
	m_graph->axisY()->setLabelFormat("%.4f");
	m_graph->axisZ()->setLabelFormat("%.3f");

	m_graph->axisX()->setTitle(data.xAxisLabel_);
	m_graph->axisX()->setTitleVisible(true);
	m_graph->axisY()->setTitle(data.yAxisLabel_);
	m_graph->axisY()->setTitleVisible(true);
	m_graph->axisZ()->setTitle(data.zAxisLabel_);
	m_graph->axisZ()->setTitleVisible(true);

	m_graph->axisX()->setRange(data.getXRange()(0), data.getXRange()(1));
	m_graph->axisZ()->setRange(data.getZRange()(0), data.getZRange()(1));
	m_graph->axisY()->setRange(data.getYRange()(0), data.getYRange()(1));
	m_graph->axisX()->setLabelAutoRotation(30);
	m_graph->axisY()->setLabelAutoRotation(90);
	m_graph->axisZ()->setLabelAutoRotation(30);

	m_graph->addSeries(vDataSeries_.back());

	// Reset range sliders for Sqrt&Sin
	m_rangeMinX = data.getXRange()(0);
	m_rangeMinZ = data.getZRange()(0);
	m_stepX = data.getDx();
	m_stepZ = data.getDz();

	int sampleCountX = data.get()->size();
	int sampleCountZ = data.get()->at(0)->size();

	m_axisMinSliderX->setMaximum( sampleCountX - 2);
	m_axisMinSliderX->setValue(0);
	m_axisMaxSliderX->setMaximum(sampleCountX - 1);
	m_axisMaxSliderX->setValue(sampleCountX - 1);
	m_axisMinSliderZ->setMaximum(sampleCountZ - 2);
	m_axisMinSliderZ->setValue(0);
	m_axisMaxSliderZ->setMaximum(sampleCountZ - 1);
	m_axisMaxSliderZ->setValue(sampleCountZ - 1);
}

void SurfaceGraph::toggleModeNone() {
	m_graph->setSelectionMode(QAbstract3DGraph::SelectionNone);
}

void SurfaceGraph::toggleModeItem() {
	m_graph->setSelectionMode(QAbstract3DGraph::SelectionItem);
}

void SurfaceGraph::toggleModeSliceRow() {
	m_graph->setSelectionMode(QAbstract3DGraph::SelectionItemAndRow
			| QAbstract3DGraph::SelectionSlice);
}

void SurfaceGraph::toggleModeSliceColumn() {
	m_graph->setSelectionMode(QAbstract3DGraph::SelectionItemAndColumn
			| QAbstract3DGraph::SelectionSlice);
}

void SurfaceGraph::adjustXMin(int min)
{
	float minX = m_stepX * float(min) + m_rangeMinX;

	int max = m_axisMaxSliderX->value();
	if (min >= max) {
		max = min + 1;
		m_axisMaxSliderX->setValue(max);
	}
	float maxX = m_stepX * max + m_rangeMinX;

	setAxisXRange(minX, maxX);
}

void SurfaceGraph::adjustXMax(int max)
{
	float maxX = m_stepX * float(max) + m_rangeMinX;

	int min = m_axisMinSliderX->value();
	if (max <= min) {
		min = max - 1;
		m_axisMinSliderX->setValue(min);
	}
	float minX = m_stepX * min + m_rangeMinX;

	setAxisXRange(minX, maxX);
}

void SurfaceGraph::adjustZMin(int min)
{
	float minZ = m_stepZ * float(min) + m_rangeMinZ;

	int max = m_axisMaxSliderZ->value();
	if (min >= max) {
		max = min + 1;
		m_axisMaxSliderZ->setValue(max);
	}
	float maxZ = m_stepZ * max + m_rangeMinZ;

	setAxisZRange(minZ, maxZ);
}

void SurfaceGraph::adjustZMax(int max)
{
	float maxZ = m_stepZ * float(max) + m_rangeMinZ;

	int min = m_axisMinSliderZ->value();
	if (max <= min) {
		min = max - 1;
		m_axisMinSliderZ->setValue(min);
	}
	float minZ = m_stepZ * min + m_rangeMinZ;

	setAxisZRange(minZ, maxZ);
}

void SurfaceGraph::setAxisXRange(float min, float max)
{
	m_graph->axisX()->setRange(min, max);
}

void SurfaceGraph::setAxisZRange(float min, float max)
{
	m_graph->axisZ()->setRange(min, max);
}

void SurfaceGraph::changeTheme(int theme)
{
	m_graph->activeTheme()->setType(Q3DTheme::Theme(theme));
}

void SurfaceGraph::setBlackToYellowGradient()
{
	QLinearGradient gr;
	gr.setColorAt(0.0, Qt::black);
	gr.setColorAt(0.33, Qt::blue);
	gr.setColorAt(0.67, Qt::red);
	gr.setColorAt(1.0, Qt::yellow);

	for(size_t i=0; i<m_graph->seriesList().size(); i++) {
		m_graph->seriesList().at(i)->setBaseGradient(gr);
		m_graph->seriesList().at(i)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
	}
}

void SurfaceGraph::setGreenToRedGradient()
{
	QLinearGradient gr;
	gr.setColorAt(0.0, Qt::darkGreen);
	gr.setColorAt(0.5, Qt::yellow);
	gr.setColorAt(0.8, Qt::red);
	gr.setColorAt(1.0, Qt::darkRed);

	for(size_t i=0; i<m_graph->seriesList().size(); i++) {
		m_graph->seriesList().at(i)->setBaseGradient(gr);
		m_graph->seriesList().at(i)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
	}
}

void SurfaceGraph::setAxisMinSliderX(QSlider *slider) {
	m_axisMinSliderX = slider;
}

void SurfaceGraph::setAxisMaxSliderX(QSlider *slider) {
	m_axisMaxSliderX = slider;
}

void SurfaceGraph::setAxisMinSliderZ(QSlider *slider) {
	m_axisMinSliderZ = slider;
}

void SurfaceGraph::setAxisMaxSliderZ(QSlider *slider) {
	m_axisMaxSliderZ = slider;
}
