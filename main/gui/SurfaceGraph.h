#ifndef SURFACEGRAPH_H
#define SURFACEGRAPH_H

#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QSurfaceDataProxy>
#include <QtDataVisualization/QHeightMapSurfaceDataProxy>
#include <QtDataVisualization/QSurface3DSeries>
#include <QtWidgets/QSlider>
#include "VPPItemFactory.h"

using namespace QtDataVisualization;

class SurfaceGraph : public QObject {

		Q_OBJECT

	public:

		explicit SurfaceGraph(Q3DSurface *surface);
		~SurfaceGraph();

		void toggleModeNone();
		void toggleModeItem();
		void toggleModeSliceRow();
		void toggleModeSliceColumn();

		void setBlackToYellowGradient();
		void setGreenToRedGradient();

		void setAxisMinSliderX(QSlider *slider);
		void setAxisMaxSliderX(QSlider *slider);
		void setAxisMinSliderZ(QSlider *slider);
		void setAxisMaxSliderZ(QSlider *slider);

		void adjustXMin(int min);
		void adjustXMax(int max);
		void adjustZMin(int min);
		void adjustZMax(int max);

		/// Accept data from the outer world
		void fillData( ThreeDDataContainer& data );

		public Q_SLOTS:
		void changeTheme(int theme);

		private:
		Q3DSurface *m_graph;
		QSurfaceDataProxy *m_sqrtSinProxy;
		QSurface3DSeries *m_sqrtSinSeries;

		QSlider *m_axisMinSliderX;
		QSlider *m_axisMaxSliderX;
		QSlider *m_axisMinSliderZ;
		QSlider *m_axisMaxSliderZ;
		float m_rangeMinX;
		float m_rangeMinZ;
		float m_stepX;
		float m_stepZ;

		void setAxisXRange(float min, float max);
		void setAxisZRange(float min, float max);
		void fillSqrtSinProxy();
};

#endif // SURFACEGRAPH_H
