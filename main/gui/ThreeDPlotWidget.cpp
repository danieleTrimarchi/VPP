#include "ThreeDPlotWidget.h"
#include <QtDataVisualization/Q3DSurface>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>
#include <QtGui/QPainter>
#include <QtGui/QScreen>

using namespace QtDataVisualization;

// Explicit Constructor
ThreeDPlotWidget::ThreeDPlotWidget(QWidget* parent /*=Q_NULLPTR*/, Qt::WindowFlags flags/*=0*/) :
				VppTabDockWidget(parent, flags),
				fontSize_(8){

	setObjectName("3D plot");
	setWindowTitle("3D plot");

	// The Q3DSurface provides methods for rendering 3d surface plots
	// it supports rotations and zooming
	// it supports selection by showing an highlighted ball on the data point where
	//    the user has clicked or selected via QSurface3DSeries
	// The value range and the label format shown on the axis can be controlled
	//    through QValue3DAxis.
	// See also the minimal example in the docs for Q3DSurface class
	Q3DSurface* p3dSurface = new Q3DSurface();

	// Creates a QWidget that makes it possible to embed window into a QWidget-based application.
	// Once the window (in this case the 3d surface!) has been embedded into the container,
	// the container will control the window's geometry and visibility. The container takes
	// over ownership of window
	// !! The window container has a number of known limitations, among which the stacking order
	QWidget* container = QWidget::createWindowContainer(p3dSurface);

	if (!p3dSurface->hasContext()) {
		QMessageBox msgBox;
		msgBox.setText("Couldn't initialize the OpenGL context.");
		msgBox.exec();
		// TODO dtrimarchi: add some sort of throw!!!
	}


	// Define the size and the policy of the container, which is the widget with the 3d view
	QSize screenSize = p3dSurface->screen()->size();
	container->setMinimumSize(QSize(screenSize.width() / 3, screenSize.height() / 3));
	//container->setMaximumSize(screenSize);
	//container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	//container->setFocusPolicy(Qt::StrongFocus);

	// Create a widget and set its title
	QWidget* widget = new QWidget;
	widget->setWindowTitle(QStringLiteral("Surface example"));

	// Assign the widget to the current dockable widget
	setWidget(widget);

	// Define a horizontal layout in the widget
	QHBoxLayout *hLayout = new QHBoxLayout(widget);
	// Add the surface container (which is a widget) to the horizontal layout
	hLayout->addWidget(container, 1);
	// Define a vertical layout
	QVBoxLayout* vLayout = new QVBoxLayout();

	// Add the vertical layout to the horizontal layout
	hLayout->addLayout(vLayout);
	// Not sure what this does..?
	//vLayout->setAlignment(Qt::AlignTop);

	//--
	// Container for the radio buttons under the label of 'Selection Mode'
	QGroupBox* pSelectionModeGroupBox = new QGroupBox(QStringLiteral("Selection Mode"));

	// Set the font
	QFont font = pSelectionModeGroupBox->font();
	font.setPointSizeF(fontSize_);
	pSelectionModeGroupBox->setFont(font);

	// Define radio button...
	QRadioButton* pModeNoSelection = new QRadioButton(widget);
	pModeNoSelection->setText(QStringLiteral("No selection"));
	pModeNoSelection->setChecked(false);
	font = pModeNoSelection->font();
	font.setPointSizeF(fontSize_);
	pModeNoSelection->setFont(font);


	// Define radio button...
	QRadioButton* pModeItemSelection = new QRadioButton(widget);
	pModeItemSelection->setText(QStringLiteral("Item"));
	pModeItemSelection->setChecked(false);
	font = pModeItemSelection->font();
	font.setPointSizeF(fontSize_);
	pModeItemSelection->setFont(font);

	// Define radio button...
	QRadioButton* pModeSliceRowSelection = new QRadioButton(widget);
	pModeSliceRowSelection->setText(QStringLiteral("Row Slice"));
	pModeSliceRowSelection->setChecked(false);
	font = pModeSliceRowSelection->font();
	font.setPointSizeF(fontSize_);
	pModeSliceRowSelection->setFont(font);


	// Define radio button...
	QRadioButton* pModeSliceColumnSelection = new QRadioButton(widget);
	pModeSliceColumnSelection->setText(QStringLiteral("Column Slice"));
	pModeSliceColumnSelection->setChecked(false);
	font = pModeSliceColumnSelection->font();
	font.setPointSizeF(fontSize_);
	pModeSliceColumnSelection->setFont(font);


	// Define a vertical layout and assign to the radio button container
	QVBoxLayout* pSelectionVLayout = new QVBoxLayout;
	pSelectionVLayout->addWidget(pModeNoSelection);
	pSelectionVLayout->addWidget(pModeItemSelection);
	pSelectionVLayout->addWidget(pModeSliceRowSelection);
	pSelectionVLayout->addWidget(pModeSliceColumnSelection);
	pSelectionModeGroupBox->setLayout(pSelectionVLayout);


	// --

	// Define the sliders (used to hide part of the surface)
	QSlider* axisMinSliderX = new QSlider(Qt::Horizontal, widget);
	axisMinSliderX->setMinimum(0);
	axisMinSliderX->setTickInterval(1);
	axisMinSliderX->setEnabled(true);
	QSlider *axisMaxSliderX = new QSlider(Qt::Horizontal, widget);
	axisMaxSliderX->setMinimum(1);
	axisMaxSliderX->setTickInterval(1);
	axisMaxSliderX->setEnabled(true);
	QSlider *axisMinSliderZ = new QSlider(Qt::Horizontal, widget);
	axisMinSliderZ->setMinimum(0);
	axisMinSliderZ->setTickInterval(1);
	axisMinSliderZ->setEnabled(true);
	QSlider *axisMaxSliderZ = new QSlider(Qt::Horizontal, widget);
	axisMaxSliderZ->setMinimum(1);
	axisMaxSliderZ->setTickInterval(1);
	axisMaxSliderZ->setEnabled(true);

	// Define a combo-box for the background theme
	QComboBox* themeList = new QComboBox(widget);
	themeList->addItem(QStringLiteral("Qt"));
	themeList->addItem(QStringLiteral("Primary Colors"));
	themeList->addItem(QStringLiteral("Digia"));
	themeList->addItem(QStringLiteral("Stone Moss"));
	themeList->addItem(QStringLiteral("Army Blue"));
	themeList->addItem(QStringLiteral("Retro"));
	themeList->addItem(QStringLiteral("Ebony"));
	themeList->addItem(QStringLiteral("Isabelle"));
	font = themeList->font();
	font.setPointSizeF(fontSize_);
	themeList->setFont(font);

	// Finally define a container for the color-maps to be applied to the 3d surface
	QGroupBox* colorGroupBox = new QGroupBox(QStringLiteral("Custom gradient"));
	font = colorGroupBox->font();
	font.setPointSizeF(fontSize_);
	colorGroupBox->setFont(font);

	// Define a color gradient and assign it to a button.
	QLinearGradient grBtoY(0, 0, 1, 100);
	grBtoY.setColorAt(1.0, Qt::black);
	grBtoY.setColorAt(0.67, Qt::blue);
	grBtoY.setColorAt(0.33, Qt::red);
	grBtoY.setColorAt(0.0, Qt::yellow);
	QPixmap pm(12, 100);
	QPainter pmp(&pm);
	pmp.setBrush(QBrush(grBtoY));
	pmp.setPen(Qt::NoPen);
	pmp.drawRect(0, 0, 12, 100);
	QPushButton *gradientBtoYPB = new QPushButton(widget);
	gradientBtoYPB->setIcon(QIcon(pm));
	gradientBtoYPB->setIconSize(QSize(12, 100));

	// Define a color gradient and assign it to a button.
	QLinearGradient grGtoR(0, 0, 1, 100);
	grGtoR.setColorAt(1.0, Qt::darkGreen);
	grGtoR.setColorAt(0.5, Qt::yellow);
	grGtoR.setColorAt(0.2, Qt::red);
	grGtoR.setColorAt(0.0, Qt::darkRed);
	pmp.setBrush(QBrush(grGtoR));
	pmp.drawRect(0, 0, 12, 100);
	QPushButton *gradientGtoRPB = new QPushButton(widget);
	gradientGtoRPB->setIcon(QIcon(pm));
	gradientGtoRPB->setIconSize(QSize(12, 100));

	// Define a layout and assign it to the container
	QHBoxLayout *colorHBox = new QHBoxLayout;
	colorHBox->addWidget(gradientBtoYPB);
	colorHBox->addWidget(gradientGtoRPB);
	colorGroupBox->setLayout(colorHBox);

	gradientBtoYPB->setMinimumWidth(6);
	gradientGtoRPB->setMinimumWidth(6);
	colorGroupBox->setMinimumWidth(12);
	//colorHBox->setMargin(0);
	//--

	// Add all the containers to the global layout -> add the Model, Selection Mode,
	// Sliders... to the vertical layout that will be used to populate the right widget
	//vLayout->addWidget(modelGroupBox);
	vLayout->addWidget(pSelectionModeGroupBox);

	QWidget* pColRangeWidget= new QLabel(QStringLiteral("Column range"));
	font = pColRangeWidget->font();
	font.setPointSizeF(fontSize_);
	pColRangeWidget->setFont(font);
	vLayout->addWidget(pColRangeWidget);

	vLayout->addWidget(axisMinSliderX);
	vLayout->addWidget(axisMaxSliderX);

	QWidget* pRowRangeWidget= new QLabel(QStringLiteral("Row range"));
	font = pRowRangeWidget->font();
	font.setPointSizeF(fontSize_);
	pRowRangeWidget->setFont(font);
	vLayout->addWidget(pRowRangeWidget);

	vLayout->addWidget(axisMinSliderZ);
	vLayout->addWidget(axisMaxSliderZ);

	QWidget* pThemeWidget= new QLabel(QStringLiteral("Theme"));
	font = pThemeWidget->font();
	font.setPointSizeF(fontSize_);
	pThemeWidget->setFont(font);
	vLayout->addWidget(pThemeWidget);

	vLayout->addWidget(themeList);
	vLayout->addWidget(colorGroupBox);

	widget->show();

	//----------------------------------------------------------------------------------

	// Instantiate a surfaceGraph on top of the Q3DSurface. The surfaceGraph defines the
	// actual surface to plot
	modifier_ = new SurfaceGraph(p3dSurface);

	// Simply set the 3d surface
	//	// Connect the actions with relevant methods of the surfaceGraph
	//	QObject::connect(heightMapModelRB, &QRadioButton::toggled,
	//			modifier, &SurfaceGraph::enableHeightMapModel);
	//	QObject::connect(sqrtSinModelRB, &QRadioButton::toggled,
	//			modifier, &SurfaceGraph::enableSqrtSinModel);

	QObject::connect(pModeNoSelection, &QRadioButton::toggled,
			modifier_, &SurfaceGraph::toggleModeNone);
	QObject::connect(pModeItemSelection,  &QRadioButton::toggled,
			modifier_, &SurfaceGraph::toggleModeItem);
	QObject::connect(pModeSliceRowSelection,  &QRadioButton::toggled,
			modifier_, &SurfaceGraph::toggleModeSliceRow);
	QObject::connect(pModeSliceColumnSelection,  &QRadioButton::toggled,
			modifier_, &SurfaceGraph::toggleModeSliceColumn);
	QObject::connect(axisMinSliderX, &QSlider::valueChanged,
			modifier_, &SurfaceGraph::adjustXMin);
	QObject::connect(axisMaxSliderX, &QSlider::valueChanged,
			modifier_, &SurfaceGraph::adjustXMax);
	QObject::connect(axisMinSliderZ, &QSlider::valueChanged,
			modifier_, &SurfaceGraph::adjustZMin);
	QObject::connect(axisMaxSliderZ, &QSlider::valueChanged,
			modifier_, &SurfaceGraph::adjustZMax);
	QObject::connect(themeList, SIGNAL(currentIndexChanged(int)),
			modifier_, SLOT(changeTheme(int)));
	QObject::connect(gradientBtoYPB, &QPushButton::pressed,
			modifier_, &SurfaceGraph::setBlackToYellowGradient);
	QObject::connect(gradientGtoRPB, &QPushButton::pressed,
			modifier_, &SurfaceGraph::setGreenToRedGradient);

	modifier_->setAxisMinSliderX(axisMinSliderX);
	modifier_->setAxisMaxSliderX(axisMaxSliderX);
	modifier_->setAxisMinSliderZ(axisMinSliderZ);
	modifier_->setAxisMaxSliderZ(axisMaxSliderZ);

	//	sqrtSinModelRB->setChecked(true);
	pModeItemSelection->setChecked(true);
	themeList->setCurrentIndex(2);

}

// Dtor
ThreeDPlotWidget::~ThreeDPlotWidget() {

}

// Get the underlying surfaceGraph, trough which we will be adding
// the data to plot
SurfaceGraph* ThreeDPlotWidget::getSurfaceGraph() {
	return modifier_;
}

// Add a surface chart to this ThreeDPlotWidget
void ThreeDPlotWidget::addChart( vector<ThreeDDataContainer> vData ) {
	for(size_t i=0; i<vData.size(); i++)
		getSurfaceGraph()->fillData(vData[i]);
}


