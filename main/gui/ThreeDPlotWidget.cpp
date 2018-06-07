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
#include <QtWidgets/QSlider>
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
	pWidget_ = new QWidget;
	pWidget_->setWindowTitle(QStringLiteral("Surface example"));

	// Assign the widget to the current dockable widget
	setWidget(pWidget_);

	// Define a horizontal layout in the widget
	QHBoxLayout *hLayout = new QHBoxLayout(pWidget_);
	// Add the surface container (which is a widget) to the horizontal layout
	hLayout->addWidget(container, 1);
	// Define a vertical layout
	QVBoxLayout* vLayout = new QVBoxLayout();

	// Add the vertical layout to the horizontal layout
	hLayout->addLayout(vLayout);
	// Not sure what this does..?
	//vLayout->setAlignment(Qt::AlignTop);


	// Container for the radio buttons under the label of 'Model'. This
	// GroupBox is populated with radio buttons when data are added to
	// the 3d plot
	QGroupBox *modelGroupBox = new QGroupBox(QStringLiteral("Model"));

	// Set the font
	QFont modelFont = modelGroupBox->font();
	modelFont.setPointSizeF(fontSize_);
	modelGroupBox->setFont(modelFont);

	/// Model choice vertical box, that encloses the combo box
	/// for the user to choose which surface to visualize among the
	/// available surfaces
	QVBoxLayout* pModelVBox= new QVBoxLayout;
	modelGroupBox->setLayout(pModelVBox);

	surfaceList_ = new QComboBox(pWidget_);
	QFont font = surfaceList_->font();
	font.setPointSizeF(fontSize_);
	surfaceList_->setFont(font);
	pModelVBox->addWidget(surfaceList_);

	////////////////////////////////////

	//--
	// Container for the radio buttons under the label of 'Selection Mode'
	QGroupBox* pSelectionModeGroupBox = new QGroupBox(QStringLiteral("Selection Mode"));

	// Set the font
	font=  pSelectionModeGroupBox->font();
	font.setPointSizeF(fontSize_);
	pSelectionModeGroupBox->setFont(font);

	// Define radio button...
	QRadioButton* pModeNoSelection = new QRadioButton(pWidget_);
	pModeNoSelection->setText(QStringLiteral("No selection"));
	pModeNoSelection->setChecked(false);
	font = pModeNoSelection->font();
	font.setPointSizeF(fontSize_);
	pModeNoSelection->setFont(font);


	// Define radio button...
	QRadioButton* pModeItemSelection = new QRadioButton(pWidget_);
	pModeItemSelection->setText(QStringLiteral("Item"));
	pModeItemSelection->setChecked(false);
	font = pModeItemSelection->font();
	font.setPointSizeF(fontSize_);
	pModeItemSelection->setFont(font);

	// Define radio button...
	QRadioButton* pModeSliceRowSelection = new QRadioButton(pWidget_);
	pModeSliceRowSelection->setText(QStringLiteral("Row Slice"));
	pModeSliceRowSelection->setChecked(false);
	font = pModeSliceRowSelection->font();
	font.setPointSizeF(fontSize_);
	pModeSliceRowSelection->setFont(font);


	// Define radio button...
	QRadioButton* pModeSliceColumnSelection = new QRadioButton(pWidget_);
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
	QSlider* axisMinSliderX = new QSlider(Qt::Horizontal, pWidget_);
	axisMinSliderX->setMinimum(0);
	axisMinSliderX->setTickInterval(1);
	axisMinSliderX->setEnabled(true);
	QSlider *axisMaxSliderX = new QSlider(Qt::Horizontal, pWidget_);
	axisMaxSliderX->setMinimum(1);
	axisMaxSliderX->setTickInterval(1);
	axisMaxSliderX->setEnabled(true);
	QSlider *axisMinSliderZ = new QSlider(Qt::Horizontal, pWidget_);
	axisMinSliderZ->setMinimum(0);
	axisMinSliderZ->setTickInterval(1);
	axisMinSliderZ->setEnabled(true);
	QSlider *axisMaxSliderZ = new QSlider(Qt::Horizontal, pWidget_);
	axisMaxSliderZ->setMinimum(1);
	axisMaxSliderZ->setTickInterval(1);
	axisMaxSliderZ->setEnabled(true);

//	// Define a combo-box for the background theme
//	QComboBox* themeList = new QComboBox(pWidget_);
//	themeList->addItem(QStringLiteral("Qt"));
//	themeList->addItem(QStringLiteral("Primary Colors"));
//	themeList->addItem(QStringLiteral("Digia"));
//	themeList->addItem(QStringLiteral("Stone Moss"));
//	themeList->addItem(QStringLiteral("Army Blue"));
//	themeList->addItem(QStringLiteral("Retro"));
//	themeList->addItem(QStringLiteral("Ebony"));
//	themeList->addItem(QStringLiteral("Isabelle"));
//	font = themeList->font();
//	font.setPointSizeF(fontSize_);
//	themeList->setFont(font);

	// Finally define a container for the color-maps to be applied to the 3d surface
//	QGroupBox* colorGroupBox = new QGroupBox(QStringLiteral("Custom gradient"));
//	font = colorGroupBox->font();
//	font.setPointSizeF(fontSize_);
//	colorGroupBox->setFont(font);

//	// Define a color gradient and assign it to a button.
//	QLinearGradient grGtoR(0, 0, 1, 100);
//	grGtoR.setColorAt(1.0, Qt::darkGreen);
//	grGtoR.setColorAt(0.5, Qt::yellow);
//	grGtoR.setColorAt(0.2, Qt::red);
//	grGtoR.setColorAt(0.0, Qt::darkRed);
//	QPixmap pm(12, 100);
//	QPainter pmp(&pm);
//	pmp.setBrush(QBrush(grGtoR));
//	pmp.drawRect(0, 0, 12, 100);
//	QPushButton *gradientGtoRPB = new QPushButton(pWidget_);
//	gradientGtoRPB->setIcon(QIcon(pm));
//	gradientGtoRPB->setIconSize(QSize(12, 100));

	// Define a layout and assign it to the container
//	QHBoxLayout *colorHBox = new QHBoxLayout;
//	colorHBox->addWidget(gradientBtoYPB);
//	colorHBox->addWidget(gradientGtoRPB);
//	colorGroupBox->setLayout(colorHBox);

//	gradientBtoYPB->setMinimumWidth(6);
//	gradientGtoRPB->setMinimumWidth(6);
//	colorGroupBox->setMinimumWidth(12);
	//colorHBox->setMargin(0);

	//--

	// Add all the containers to the global layout -> add the Model, Selection Mode,
	// Sliders... to the vertical layout that will be used to populate the right widget
	vLayout->addWidget(modelGroupBox);
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

//	QWidget* pThemeWidget= new QLabel(QStringLiteral("Theme"));
//	font = pThemeWidget->font();
//	font.setPointSizeF(fontSize_);
//	pThemeWidget->setFont(font);
//	vLayout->addWidget(pThemeWidget);

	//vLayout->addWidget(themeList);
	//vLayout->addWidget(colorGroupBox);

	pWidget_->show();

	//----------------------------------------------------------------------------------

	// Instantiate a surfaceGraph on top of the Q3DSurface. The surfaceGraph defines the
	// actual surface to plot
	surfaceGraph_ = new SurfaceGraph(p3dSurface);

	QObject::connect(pModeNoSelection, &QRadioButton::toggled,
			surfaceGraph_, &SurfaceGraph::toggleModeNone);
	QObject::connect(pModeItemSelection,  &QRadioButton::toggled,
			surfaceGraph_, &SurfaceGraph::toggleModeItem);
	QObject::connect(pModeSliceRowSelection,  &QRadioButton::toggled,
			surfaceGraph_, &SurfaceGraph::toggleModeSliceRow);
	QObject::connect(pModeSliceColumnSelection,  &QRadioButton::toggled,
			surfaceGraph_, &SurfaceGraph::toggleModeSliceColumn);
	QObject::connect(axisMinSliderX, &QSlider::valueChanged,
			surfaceGraph_, &SurfaceGraph::adjustXMin);
	QObject::connect(axisMaxSliderX, &QSlider::valueChanged,
			surfaceGraph_, &SurfaceGraph::adjustXMax);
	QObject::connect(axisMinSliderZ, &QSlider::valueChanged,
			surfaceGraph_, &SurfaceGraph::adjustZMin);
	QObject::connect(axisMaxSliderZ, &QSlider::valueChanged,
			surfaceGraph_, &SurfaceGraph::adjustZMax);
//	QObject::connect(	themeList, 		SIGNAL(currentIndexChanged(int)),
//										surfaceGraph_,SLOT(changeTheme(int)));
//	QObject::connect(gradientBtoYPB, &QPushButton::pressed,
//			surfaceGraph_, &SurfaceGraph::setBlackToYellowGradient);
//	QObject::connect(gradientGtoRPB, &QPushButton::pressed,
//			surfaceGraph_, &SurfaceGraph::setGreenToRedGradient);
	// Connect the choice of the surface to plot
	QObject::connect(	surfaceList_,  SIGNAL(currentIndexChanged(int)),
										surfaceGraph_, SLOT(show(int)) );

	surfaceGraph_->setAxisMinSliderX(axisMinSliderX);
	surfaceGraph_->setAxisMaxSliderX(axisMaxSliderX);
	surfaceGraph_->setAxisMinSliderZ(axisMinSliderZ);
	surfaceGraph_->setAxisMaxSliderZ(axisMaxSliderZ);

	// Use the Qt theme by default with a green to red colormap
	surfaceGraph_->changeTheme(Q3DTheme::Theme::ThemeDigia);

	pModeItemSelection->setChecked(true);
	//themeList->setCurrentIndex(2);
}

// Dtor
ThreeDPlotWidget::~ThreeDPlotWidget() {

}

// Add a surface chart to this ThreeDPlotWidget
void ThreeDPlotWidget::addChart( vector<ThreeDDataContainer> vData ) {

	for(size_t i=0; i<vData.size(); i++){

		// Add an entry to the list of plottable surfaces
		surfaceList_->addItem(vData[i].yAxisLabel_);

		// Add the surface point data to the plot
		surfaceGraph_->fillData(vData[i]);

	}

	// Visualize the very first surface available, to start with
	surfaceGraph_->show(0);

}


