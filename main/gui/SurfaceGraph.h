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

		/// Explicit Ctor
		explicit SurfaceGraph(Q3DSurface *surface);

		/// Dtor
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

		/// Show the i-th data (and provide all operations requested
		/// for the 3d plot to be consistent : labels, rescaling...
		void show( int iDataSet );

		private:
		Q3DSurface *pGraph_;
		vector<QSurfaceDataProxy*> vDataProxy_;
		vector<QSurface3DSeries*> vDataSeries_;
		vector<ThreeDDataContainer> vThreeDDataContainer_;

		QSlider *pAxisMinSliderX_;
		QSlider *pAxisMaxSliderX_;
		QSlider *pAxisMinSliderZ_;
		QSlider *pAxisMaxSliderZ_;
		float rangeMinX_;
		float rangeMinZ_;
		float stepX_;
		float stepZ_;

		void setAxisXRange(float min, float max);
		void setAxisZRange(float min, float max);
		void fillSqrtSinProxy();
};

#endif // SURFACEGRAPH_H
