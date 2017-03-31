/****************************************************************************
  **
  ** Copyright (C) 2016 The Qt Company Ltd.
  ** Contact: https://www.qt.io/licensing/
  **
  ** This file is part of the Qt Data Visualization module of the Qt Toolkit.
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

  #ifndef SURFACEGRAPH_H
  #define SURFACEGRAPH_H

  #include <QtDataVisualization/Q3DSurface>
  #include <QtDataVisualization/QSurfaceDataProxy>
  #include <QtDataVisualization/QHeightMapSurfaceDataProxy>
  #include <QtDataVisualization/QSurface3DSeries>
  #include <QtWidgets/QSlider>

  using namespace QtDataVisualization;

  class SurfaceGraph : public QObject
  {
      Q_OBJECT
  public:
      explicit SurfaceGraph(Q3DSurface *surface);
      ~SurfaceGraph();

      void enableDataModel(bool enable);

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
      void fillData( QSurfaceDataArray* pData );

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
