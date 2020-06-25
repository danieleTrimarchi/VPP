#include "surfacegraph.h"

#include <QtDataVisualization/QValue3DAxis>
#include <QtDataVisualization/Q3DTheme>
#include <QtGui/QImage>
#include <QtCore/qmath.h>

using namespace QtDataVisualization;

SurfaceGraph::SurfaceGraph(Q3DSurface *surface):
		pGraph_(surface),
		rangeMinX_(0),
		rangeMinZ_(0),
		stepX_(0),
		stepZ_(0),
		pAxisMinSliderX_(0),
		pAxisMaxSliderX_(0),
		pAxisMinSliderZ_(0),
		pAxisMaxSliderZ_(0) {

	pGraph_->setAxisX(new QValue3DAxis);
	pGraph_->setAxisY(new QValue3DAxis);
	pGraph_->setAxisZ(new QValue3DAxis);

	pGraph_->axisX()->setLabelFormat("%.3f");
	pGraph_->axisY()->setLabelFormat("%.4f");
	pGraph_->axisZ()->setLabelFormat("%.3f");

	pGraph_->axisX()->setTitleVisible(true);
	pGraph_->axisY()->setTitleVisible(true);
	pGraph_->axisZ()->setTitleVisible(true);

	pGraph_->axisX()->setLabelAutoRotation(30);
	pGraph_->axisY()->setLabelAutoRotation(90);
	pGraph_->axisZ()->setLabelAutoRotation(30);

}

SurfaceGraph::~SurfaceGraph() {
	delete pGraph_;
}

// Show the i-th data (and provide all operations requested
// for the 3d plot to be consistent : labels, rescaling...
void SurfaceGraph::show( int iDataSet ) {

	if(iDataSet>=vDataSeries_.size())
		return;

	// Remove any previous series (if any) and plot the current series
	// WARNING : can I remove a non-active series from mGraph?
	for(size_t iSeries=0; iSeries<pGraph_->seriesList().size(); iSeries++){
		pGraph_->removeSeries(pGraph_->seriesList().at(iSeries));
	}

	// Add the selected series to the 3d plot
	pGraph_->addSeries(vDataSeries_[iDataSet]);

	ThreeDDataContainer* pData= &(vThreeDDataContainer_[iDataSet]);

	pGraph_->axisX()->setTitle(pData->xAxisLabel_);
	pGraph_->axisY()->setTitle(pData->yAxisLabel_);
	pGraph_->axisZ()->setTitle(pData->zAxisLabel_);

	pGraph_->axisX()->setRange(pData->getXRange()(0), pData->getXRange()(1));
	pGraph_->axisZ()->setRange(pData->getZRange()(0), pData->getZRange()(1));
	pGraph_->axisY()->setRange(pData->getYRange()(0), pData->getYRange()(1));

	// Reset range sliders for this series
	rangeMinX_ = pData->getXRange()(0);
	rangeMinZ_ = pData->getZRange()(0);
	stepX_ = pData->getDx();
	stepZ_ = pData->getDz();

	int sampleCountX = pData->get()->size();
	int sampleCountZ = pData->get()->at(0)->size();

	pAxisMinSliderX_->setMaximum( sampleCountX - 2);
	pAxisMinSliderX_->setValue(0);
	pAxisMaxSliderX_->setMaximum(sampleCountX - 1);
	pAxisMaxSliderX_->setValue(sampleCountX - 1);
	pAxisMinSliderZ_->setMaximum(sampleCountZ - 2);
	pAxisMinSliderZ_->setValue(0);
	pAxisMaxSliderZ_->setMaximum(sampleCountZ - 1);
	pAxisMaxSliderZ_->setValue(sampleCountZ - 1);

	// Update the colormap to span the values of this chart
	setGreenToRedGradient();
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
	pGraph_->setSelectionMode(QAbstract3DGraph::SelectionNone);
}

void SurfaceGraph::toggleModeItem() {
	pGraph_->setSelectionMode(QAbstract3DGraph::SelectionItem);
}

void SurfaceGraph::toggleModeSliceRow() {
	pGraph_->setSelectionMode(QAbstract3DGraph::SelectionItemAndRow
			| QAbstract3DGraph::SelectionSlice);
}

void SurfaceGraph::toggleModeSliceColumn() {
	pGraph_->setSelectionMode(QAbstract3DGraph::SelectionItemAndColumn
			| QAbstract3DGraph::SelectionSlice);
}

void SurfaceGraph::adjustXMin(int min)
{
	float minX = stepX_ * float(min) + rangeMinX_;

	int max = pAxisMaxSliderX_->value();
	if (min >= max) {
		max = min + 1;
		pAxisMaxSliderX_->setValue(max);
	}
	float maxX = stepX_ * max + rangeMinX_;

	setAxisXRange(minX, maxX);
}

void SurfaceGraph::adjustXMax(int max)
{
	float maxX = stepX_ * float(max) + rangeMinX_;

	int min = pAxisMinSliderX_->value();
	if (max <= min) {
		min = max - 1;
		pAxisMinSliderX_->setValue(min);
	}
	float minX = stepX_ * min + rangeMinX_;

	setAxisXRange(minX, maxX);
}

void SurfaceGraph::adjustZMin(int min)
{
	float minZ = stepZ_ * float(min) + rangeMinZ_;

	int max = pAxisMaxSliderZ_->value();
	if (min >= max) {
		max = min + 1;
		pAxisMaxSliderZ_->setValue(max);
	}
	float maxZ = stepZ_ * max + rangeMinZ_;

	setAxisZRange(minZ, maxZ);
}

void SurfaceGraph::adjustZMax(int max)
{
	float maxZ = stepZ_ * float(max) + rangeMinZ_;

	int min = pAxisMinSliderZ_->value();
	if (max <= min) {
		min = max - 1;
		pAxisMinSliderZ_->setValue(min);
	}
	float minZ = stepZ_ * min + rangeMinZ_;

	setAxisZRange(minZ, maxZ);
}

void SurfaceGraph::setAxisXRange(float min, float max)
{
	pGraph_->axisX()->setRange(min, max);
}

void SurfaceGraph::setAxisZRange(float min, float max)
{
	pGraph_->axisZ()->setRange(min, max);
}

void SurfaceGraph::changeTheme(int theme)
{
	pGraph_->activeTheme()->setType(Q3DTheme::Theme(theme));
}

void SurfaceGraph::setBlackToYellowGradient()
{
	QLinearGradient gr;
	gr.setColorAt(0.0, Qt::black);
	gr.setColorAt(0.33, Qt::blue);
	gr.setColorAt(0.67, Qt::red);
	gr.setColorAt(1.0, Qt::yellow);

	for(size_t i=0; i<pGraph_->seriesList().size(); i++) {
		pGraph_->seriesList().at(i)->setBaseGradient(gr);
		pGraph_->seriesList().at(i)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
	}
}

void SurfaceGraph::setGreenToRedGradient()
{
	QLinearGradient gr;
	gr.setColorAt(0.0, Qt::darkGreen);
	gr.setColorAt(0.5, Qt::yellow);
	gr.setColorAt(0.8, Qt::red);
	gr.setColorAt(1.0, Qt::darkRed);

	for(size_t i=0; i<pGraph_->seriesList().size(); i++) {
		pGraph_->seriesList().at(i)->setBaseGradient(gr);
		pGraph_->seriesList().at(i)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
	}
}

void SurfaceGraph::setAxisMinSliderX(QSlider *slider) {
	pAxisMinSliderX_ = slider;
}

void SurfaceGraph::setAxisMaxSliderX(QSlider *slider) {
	pAxisMaxSliderX_ = slider;
}

void SurfaceGraph::setAxisMinSliderZ(QSlider *slider) {
	pAxisMinSliderZ_ = slider;
}

void SurfaceGraph::setAxisMaxSliderZ(QSlider *slider) {
	pAxisMaxSliderZ_ = slider;
}
