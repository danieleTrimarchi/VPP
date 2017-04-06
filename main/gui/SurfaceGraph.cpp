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

	m_graph->axisX()->setLabelFormat("%.3f");
	m_graph->axisY()->setLabelFormat("%.4f");
	m_graph->axisZ()->setLabelFormat("%.3f");

	m_graph->axisX()->setTitleVisible(true);
	m_graph->axisY()->setTitleVisible(true);
	m_graph->axisZ()->setTitleVisible(true);

	m_graph->axisX()->setLabelAutoRotation(30);
	m_graph->axisY()->setLabelAutoRotation(90);
	m_graph->axisZ()->setLabelAutoRotation(30);

}

SurfaceGraph::~SurfaceGraph() {
	delete m_graph;
}

// Show the i-th data (and provide all operations requested
// for the 3d plot to be consistent : labels, rescaling...
void SurfaceGraph::show( int iDataSet ) {

	if(iDataSet>=vDataSeries_.size())
		return;

	// Remove any previous series (if any) and plot the current series
	// WARNING : can I remove a non-active series from mGraph?
	for(size_t iSeries=0; iSeries<m_graph->seriesList().size(); iSeries++){
		m_graph->removeSeries(m_graph->seriesList().at(iSeries));
	}

	// Add the selected series to the 3d plot
	m_graph->addSeries(vDataSeries_[iDataSet]);

	ThreeDDataContainer* pData= &(vThreeDDataContainer_[iDataSet]);

	m_graph->axisX()->setTitle(pData->xAxisLabel_);
	m_graph->axisY()->setTitle(pData->yAxisLabel_);
	m_graph->axisZ()->setTitle(pData->zAxisLabel_);

	m_graph->axisX()->setRange(pData->getXRange()(0), pData->getXRange()(1));
	m_graph->axisZ()->setRange(pData->getZRange()(0), pData->getZRange()(1));
	m_graph->axisY()->setRange(pData->getYRange()(0), pData->getYRange()(1));

	// Reset range sliders for this series
	m_rangeMinX = pData->getXRange()(0);
	m_rangeMinZ = pData->getZRange()(0);
	m_stepX = pData->getDx();
	m_stepZ = pData->getDz();

	int sampleCountX = pData->get()->size();
	int sampleCountZ = pData->get()->at(0)->size();

	m_axisMinSliderX->setMaximum( sampleCountX - 2);
	m_axisMinSliderX->setValue(0);
	m_axisMaxSliderX->setMaximum(sampleCountX - 1);
	m_axisMaxSliderX->setValue(sampleCountX - 1);
	m_axisMinSliderZ->setMaximum(sampleCountZ - 2);
	m_axisMinSliderZ->setValue(0);
	m_axisMaxSliderZ->setMaximum(sampleCountZ - 1);
	m_axisMaxSliderZ->setValue(sampleCountZ - 1);

}

void SurfaceGraph::fillData( ThreeDDataContainer& data ) {

	// Make some space for these data
	vDataProxy_.push_back( new QSurfaceDataProxy() );
	vDataSeries_.push_back( new QSurface3DSeries(vDataProxy_.back()));
	vThreeDDataContainer_.push_back(data);

	// Assign the data to the proxy
	vDataProxy_.back()->resetArray(data.get());

	vDataSeries_.back()->setDrawMode(QSurface3DSeries::DrawSurfaceAndWireframe);
	vDataSeries_.back()->setFlatShadingEnabled(true);

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
