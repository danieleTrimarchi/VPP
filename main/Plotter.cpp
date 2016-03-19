#include "Plotter.h"
#include "VPPException.h"
#include "mathUtils.h"

using namespace mathUtils;

// Constructor
Plotter::Plotter():
minX_(1E20),
minY_(1E20),
maxX_(-1E20),
maxY_(-1E20),
nValues_(0),
x_(0), y_(0) {

	// Specify the output device (aquaterm)
	plsdev("aqt");

}

// Reset the ranges to very big values
void Plotter::initRanges() {

	minX_=1E20;
	minY_=1E20;
	maxX_=-1E20;
	maxY_=-1E20;

}

void Plotter::resetRanges(Eigen::ArrayXd& x, Eigen::ArrayXd& y) {

	// Set the ranges for the first array
	if(x.minCoeff()<minX_)
		minX_=x.minCoeff();
	if(y.minCoeff()<minY_)
		minY_=y.minCoeff();
	if(x.maxCoeff()>maxX_)
		maxX_=x.maxCoeff();
	if(y.maxCoeff()>maxY_)
		maxY_=y.maxCoeff();

}

void Plotter::resetRanges(std::vector<double>& x, std::vector<double>& y) {

	// Set the ranges for the first array
	if(min(x)<minX_)
		minX_=min(x);
	if(min(y)<minY_)
		minY_=min(y);
	if(max(x)>maxX_)
		maxX_=max(x);
	if(max(y)>maxY_)
		maxY_=max(y);

}

// Reset the ranges so that it can contain all of the plot
void Plotter::resetRanges(Eigen::ArrayXd& x0,Eigen::ArrayXd& y0,Eigen::ArrayXd& x1,Eigen::ArrayXd& y1) {

	resetRanges(x0,y0);
	resetRanges(x1,y1);

}

// Reset the ranges so that it can contain all of the plot
void Plotter::resetRanges(std::vector<double>& x0,std::vector<double>& y0,std::vector<double>& x1,std::vector<double>& y1) {

	resetRanges(x0,y0);
	resetRanges(x1,y1);

}


// Destructor
Plotter::~Plotter() {

	delete x_;
	delete y_;

}

// Copy the values into plplot compatible containers
void Plotter::setValues(Eigen::ArrayXd& x, Eigen::ArrayXd& y) {

	// make sure the buffers x_ and y_ are init
	delete x_;
	delete y_;

	nValues_=x.size();

	x_ = new double[nValues_];
	y_ = new double[nValues_];

	for ( int i = 0; i < nValues_; i++ )
	{
		x_[i] = x(i);
		y_[i] = y(i);
	}

}

// Copy the values into plplot compatible containers
void Plotter::setValues(std::vector<double>& x, std::vector<double>& y) {

	// make sure the buffers x_ and y_ are init
	delete x_;
	delete y_;

	nValues_=x.size();

	x_ = new double[nValues_];
	y_ = new double[nValues_];

	for ( int i = 0; i < nValues_; i++ )
	{
		x_[i] = x[i];
		y_[i] = y[i];
	}

}

// Find the min of the specified c-style array
double Plotter::min(double* arr) {

	double val=1E+20;
	for(size_t i=0; i<nValues_; i++){
		if(arr[i]<val)
			val=arr[i];
	}
	return val;
}

// Find the min of the specified c-style array
double Plotter::min(std::vector<double>& vec) {

	double val=1E+20;
	for(size_t i=0; i<vec.size(); i++){
		if(vec[i]<val)
			val=vec[i];
	}
	return val;
}

/// Find the max of the specified c-style array
double Plotter::max(double* arr) {
	double val=-1E+20;
	for(size_t i=0; i<nValues_; i++){
		if(arr[i]>val)
			val=arr[i];
	}
	return val;
}

/// Find the max of the specified c-style array
double Plotter::max(std::vector<double>& vec) {
	double val=-1E+20;
	for(size_t i=0; i<vec.size(); i++){
		if(vec[i]>val)
			val=vec[i];
	}
	return val;
}

// Plot the points of some given arrays
void Plotter::plot( std::vector<double>& y, string title) {

	// generate a VectorXd 'x' with default abscissas
	Eigen::ArrayXd xtemp(y.size());
	Eigen::ArrayXd ytemp(y.size());
	for(size_t i=0; i<y.size(); i++){
		xtemp(i)=i;
		ytemp(i)=y[i];
	}

	// Call the std plot method
	plot(xtemp,ytemp,title);
}

// Produce a 2d plot from Eigen vectors
void Plotter::plot(Eigen::ArrayXd& x, Eigen::ArrayXd& y,string title) {

	// Reset the ranges at the very beginning of the plot
	initRanges();

	// Check the size of x and y are consistent
	if(x.size() != y.size())
		throw VPPException(HERE,"In Plotter::plot(x,y). Array sizes mismatch");

	// Specify the background color (rgb) and its transparency
	plscolbga(255,255,255,0.6);

	// Initialize plplot
	plinit();

	// Create a labeled box to hold the plot.
	plcol0( color::grey );

	// Copy the values from the incoming arrays into plplot compatible containers
	setValues(x,y);

	// Reset the range for the plot to contain all data
	resetRanges(x,y);
	plenv( minX_, maxX_, minY_, maxY_, 0, 0 );

	// Plot the data that was prepared above.
	plcol0( color::blue );
	plline( nValues_, x_, y_ );

	// Add labels
	pllab( "x", "y", title.c_str() );

	// Close PLplot library
	plend();

}

void Plotter::plot(	Eigen::ArrayXd& x0,
		Eigen::ArrayXd& y0,
		Eigen::ArrayXd& x1,
		Eigen::ArrayXd& y1,
		std::string title,
		string xLabel,
		string yLabel) {

	// Reset the ranges at the very beginning of the plot
	initRanges();

	// Check the size of x and y are consistent
	if(x0.size() != y0.size())
		throw VPPException(HERE,"In Plotter::plot(x,y). Array _0_ sizes mismatch");

	// Check the size of x and y are consistent
	if(x1.size() != y1.size())
		throw VPPException(HERE,"In Plotter::plot(x,y). Array _1_ sizes mismatch");

	// Specify the background color (rgb) and its transparency
	plscolbga(255,255,255,0.6);

	// Initialize plplot
	plinit();

	// Create a labeled box to hold the plot.
	plcol0( color::grey );

	// Copy the values from the incoming arrays into plplot compatible containers
	setValues(x0,y0);

	// Reset the plot margins so that it can contain the plot
	resetRanges(x0,y0,x1,y1);
	plenv( minX_, maxX_, minY_, maxY_, 0, 0 );

	// Plot the data that was prepared above.
	plcol0( color::blue );
	plpoin( nValues_, x_, y_, 5 );

	// ---

	// Now repeat for X1 and Y1
	// Copy the values from the incoming arrays into plplot compatible containers
	setValues(x1,y1);

	// Plot the data that was prepared above.
	plcol0( color::red );
	plline( nValues_, x_, y_ );

	// Close PLplot library
	pllab( xLabel.c_str(), yLabel.c_str(), title.c_str());
	plend();

}

void Plotter::plot(
		std::vector<double>& x0,
		std::vector<double>& y0,
		std::string title,
		string xLabel,
		string yLabel) {

	// Reset the ranges at the very beginning of the plot
	initRanges();

	// Check the size of x and y are consistent
	if(x0.size() != y0.size())
		throw VPPException(HERE,"In Plotter::plot(x,y). Array _0_ sizes mismatch");

	// Specify the background color (rgb) and its transparency
	plscolbga(255,255,255,0.6);

	// Initialize plplot
	plinit();

	// Create a labeled box to hold the plot.
	plcol0( color::grey );

	// Copy the values from the incoming arrays into plplot compatible containers
	setValues(x0,y0);

	// Reset the plot margins so that it can contain the plot
	resetRanges(x0,y0);
	plenv( minX_, maxX_, minY_, maxY_, 0, 0 );

	// Plot the data that was prepared above.
	plcol0( color::blue );
	plline( nValues_, x_, y_ );

	// Close PLplot library
	pllab( xLabel.c_str(), yLabel.c_str(), title.c_str() );
	plend();

}

void Plotter::plot(	std::vector<double>& x0,
		std::vector<double>& y0,
		std::vector<double>& x1,
		std::vector<double>& y1,
		std::string title,
		string xLabel,
		string yLabel) {

	// Reset the ranges at the very beginning of the plot
	initRanges();

	// Check the size of x and y are consistent
	if(x0.size() != y0.size())
		throw VPPException(HERE,"In Plotter::plot(x,y). Array _0_ sizes mismatch");

	// Check the size of x and y are consistent
	if(x1.size() != y1.size())
		throw VPPException(HERE,"In Plotter::plot(x,y). Array _1_ sizes mismatch");

	// Specify the background color (rgb) and its transparency
	plscolbga(255,255,255,0.6);

	// Initialize plplot
	plinit();

	// Create a labeled box to hold the plot.
	plcol0( color::grey );

	// Copy the values from the incoming arrays into plplot compatible containers
	setValues(x0,y0);

	// Reset the plot margins so that it can contain the plot
	resetRanges(x0,y0,x1,y1);
	plenv( minX_, maxX_, minY_, maxY_, 0, 0 );

	// Plot the data that was prepared above.
	plcol0( color::blue );
	plpoin( nValues_, x_, y_, 5 );

	// ---

	// Now repeat for X1 and Y1
	// Copy the values from the incoming arrays into plplot compatible containers
	setValues(x1,y1);

	// Plot the data that was prepared above.
	plcol0( color::red );
	plline( nValues_, x_, y_ );

	// Close PLplot library
	pllab( xLabel.c_str(), yLabel.c_str(), title.c_str() );
	plend();

}

//=====================================================================

// Constructor
VectorPlotter::VectorPlotter() {

	// Specify the output device (aquaterm)
	plsdev("aqt");

	plinit();

	// PLPLOT example 22
	int i,j;
	PLFLT dx,dy,x,y;
	PLcGrid2 cgrid2;
	PLFLT **u, **v;
	const int nx=20;
	const int ny=20;
	PLFLT xmin,xmax,ymin,ymax;

	dx = 1.0;
	dy = 1.0;

	xmin= -nx / 2 * dx;
	xmax=  nx / 2 * dx;
	ymin= -ny / 2 * dy;
	ymax=  ny / 2 * dy;

	plAlloc2dGrid( &cgrid2.xg, nx, ny );
	plAlloc2dGrid( &cgrid2.yg, nx, ny );
	plAlloc2dGrid( &u, nx, ny );
	plAlloc2dGrid( &v, nx, ny );

	cgrid2.nx= nx;
	cgrid2.ny= ny;

	for( i=0; i<nx; i++) {
		x = ( i - nx / 2 + 0.5 ) * dx;
		for( j=0; j<ny; j++) {

			y=( j - ny / 2 + 0.5 ) * dy;
			cgrid2.xg[i][j]= x;
			cgrid2.yg[i][j]= y;
			u[i][j]	=	 y;
			v[i][j]	=	-x;
		}
	}

	plenv(xmin,xmax,ymin,ymax,0,0);
	pllab("x","y","vector plot");
	plcol0(2);
	plvect(
			(const PLFLT* const *) u,
			(const PLFLT* const *) v,
			nx, ny, 0.0, pltr2,
			(void*) &cgrid2 );

	plcol0(1);

	plFree2dGrid(cgrid2.xg,nx,ny);
	plFree2dGrid(cgrid2.yg,nx,ny);
	plFree2dGrid(u,nx,ny);
	plFree2dGrid(v,nx,ny);

	plend();

}

// Destructor
VectorPlotter::~VectorPlotter() {
	// make nothing
}


//=====================================================================

PolarPlotter::PolarPlotter( string title ) :
	title_(title),
	minAlphaRange_(1E+20),
	maxAlphaRange_(-1E20),
	maxValRange_(-1E20),
	x_(NULL),
	y_(NULL),
	pi_(M_PI / 180.0) {

	// Specify the output device (aquaterm)
	plsdev("aqt");

}

PolarPlotter::~PolarPlotter() {
	// Do nothing
}

// Append a set of polar data
void PolarPlotter::append(string curveLabel, Eigen::ArrayXd& alpha, Eigen::ArrayXd& vals) {

	// make sure the size of the arrays are the same
	if(alpha.size() != vals.size())
		throw VPPException(HERE, "In PolarPlotter, the size of the arrays does not match");

	// push back the angles expressed in deg
	alphas_.push_back(alpha);

	// push back the values
	vals_.push_back(vals);

	// push back the curve titles
	curveLabels_.push_back(curveLabel);

	// place the labels in the center of the curve
	size_t pos= vals.size() / 2;
	idx_.push_back(pos);

	// Set the ranges
	if(alpha.minCoeff()<minAlphaRange_)
		minAlphaRange_=alpha.minCoeff();
	if(alpha.maxCoeff()>maxAlphaRange_)
		maxAlphaRange_=alpha.maxCoeff();
	if(vals.maxCoeff()>maxValRange_)
		maxValRange_=ceil(vals.maxCoeff());

}

// Copy the values into plplot compatible containers
void PolarPlotter::setValues(Eigen::ArrayXd& x, Eigen::ArrayXd& vals) {

	// make sure the size of x and y are the same
	if(x.size() != vals.size())
		throw VPPException(HERE,"In PolarPlotter::setValues. Array size mismatch");

	// make sure the buffers x_ and y_ are init
	delete x_;
	delete y_;

	x_ = new double[x.size()];
	y_ = new double[x.size()];

	// x contains the angle [deg]
	// and y contains the |val|
	// rotated 90 degrees
	// x_ = sin(toRad(Phi[i])) * val[i]
	// y_ = cos(toRad(Phi[i])) * val[i]
	for ( int i = 0; i < x.size(); i++ ) {
		x_[i] = sin( toRad(double(x(i)))) * vals(i);
		y_[i] = cos( toRad(double(x(i)))) * vals(i);
	}

}

// Plot the data appended to the plotter
void PolarPlotter::plot() {

	// Specify the output device (aquaterm)
	plsdev("aqt");

	// Specify the background color (rgb) and its transparency
	plscolbga(255,255,255,.5);

	// Initialize plplot
	plinit();

	// Set up viewport and window, but do not draw box
	plenv( -maxValRange_, maxValRange_, -maxValRange_, maxValRange_, 1, -2 );

	// Define the color for the outer circles
	plcol0( color::grey );

	// Draw a number of circles for polar grid
	int nCircles = maxValRange_;
	for ( size_t i = 1; i <= nCircles; i++ ) {
		// Add the arc
		plarc( 0.0, 0.0, i, i, 0.0, 360.0, 0.0, 0 );
		// Add a label
		char title[256];
		sprintf(title,"%d",i);
		plptex(0,i,0.1,i,2, title);
	}

	// Set the radial lines, divide in 30deg sectors
	for ( size_t i = 0; i <= 11; i++ )
	{
		double theta = 30.0 * i;

		// Note: invert x and y to get the right orientation of the
		// polar plot
		double dx = maxValRange_ * sin( toRad( theta ) );
		double dy = maxValRange_ * cos( toRad( theta ) );

		// Draw radial spokes for polar grid and add the text
		pljoin( 0.0, 0.0, dx, dy );

		// Only add a label for theta > 0
		if(i>0){
			char text[4];
			sprintf( text, "%d", ROUND( theta ) );

			// Write labels for angle
			double offset;
			if ( theta < 9.99 )	{
				offset = 0.45;
			}	else if ( theta < 99.9 ){
				offset = 0.30;
			}	else {
				offset = 0.15;
			}

			// Slightly off zero to avoid floating point logic flips at 90 and 270 deg.
			if ( dx >= -0.00001 )
				plptex( dx, dy, dx, dy, -offset, text );
			else
				plptex( dx, dy, -dx, -dy, 1. + offset, text );
		}
	}
	// Set the color for the data line
	plcol0( color::blue );

	for(size_t iLine=0; iLine<alphas_.size(); iLine++) {

		// Copy the vals into a c-stile array
		setValues(alphas_[iLine], vals_[iLine]);

		// And now define the line
		plline( alphas_[iLine].size(), x_, y_ );

		// Place the curve label somewhere on the curve
		plptex(x_[idx_[iLine]],y_[idx_[iLine]],1,0,0,curveLabels_[iLine].c_str());
	}

	// set the color
	plcol0( color::red	);
	plmtex( "t", -1., 0., 0.5, title_.c_str() );

	// Close the plot at end
	plend();

}



















