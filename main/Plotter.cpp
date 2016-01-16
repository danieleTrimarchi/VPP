#include "Plotter.h"

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
		minX_=x.min(x);
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

	plenv( minX_, maxX_, minY_, maxY_, 0, 0 );

}

// Reset the ranges so that it can contain all of the plot
void Plotter::resetRanges(std::vector<double>& x0,std::vector<double>& y0,std::vector<double>& x1,std::vector<double>& y1) {

	resetRanges(x0,y0);
	resetRanges(x1,y1);

	plenv( minX_, maxX_, minY_, maxY_, 0, 0 );

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

// Produce a 2d plot from Eigen vectors
void Plotter::plot(Eigen::ArrayXd& x, Eigen::ArrayXd& y) {

	// Reset the ranges at the very beginning of the plot
	initRanges();

	// Check the size of x and y are consistent
	if(x.size() != y.size())
		throw std::logic_error("In Plotter::plot(x,y). Array sizes mismatch");

	// Copy the values from the incoming arrays into plplot compatible containers
	setValues(x,y);

	// Specify the background color (rgb) and its transparency
	plscolbga(255,255,255,0.6);

	// Initialize plplot
	plinit();

	// Create a labeled box to hold the plot.
	plcol0( color::grey );

	// Reset the range for the plot to contain all data
	resetRanges(x,y);

	// Add labels
 	pllab( "x", "y", "2D Plot Example" );

	// Plot the data that was prepared above.
	plcol0( color::blue );
	plline( nValues_, x_, y_ );

	// Close PLplot library
	plend();

}

void Plotter::plot(Eigen::ArrayXd& x0, Eigen::ArrayXd& y0,Eigen::ArrayXd& x1, Eigen::ArrayXd& y1) {

	// Reset the ranges at the very beginning of the plot
	initRanges();

	// Check the size of x and y are consistent
	if(x0.size() != y0.size())
		throw std::logic_error("In Plotter::plot(x,y). Array _0_ sizes mismatch");

	// Check the size of x and y are consistent
	if(x1.size() != y1.size())
		throw std::logic_error("In Plotter::plot(x,y). Array _1_ sizes mismatch");

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

	// Plot the data that was prepared above.
	plcol0( color::blue );
	plline( nValues_, x_, y_ );
	plpoin( nValues_, x_, y_, 5 );

	// ---

	// Now repeat for X1 and Y1
	// Copy the values from the incoming arrays into plplot compatible containers
	setValues(x1,y1);

	// Plot the data that was prepared above.
	plcol0( color::red );
	plline( nValues_, x_, y_ );
	plpoin( nValues_, x_, y_, 5 );

	// Close PLplot library
	pllab( "x", "y", "2D Plot Example" );
	plend();

}

//=====================================================================

PolarPlotter::PolarPlotter() :
		Plotter(),
		pi_(M_PI / 180.0) {

	nValues_=361;
	x_= new double[nValues_];
	y_= new double[nValues_];
}

PolarPlotter::~PolarPlotter() {
	// Do nothing
}

void PolarPlotter::testPlot() {

	// Set orientation to portrait - note not all device drivers
	// support this, in particular most interactive drivers do not
	plsori( 1 );

	// Specify the background color (rgb) and its transparency
	plscolbga(255,255,255,.5);

	// Initialize plplot
	plinit();

	// Fill the values array
	for ( size_t i = 0; i < nValues_; i++ )
	{
		double r    = pi_ * ( .1 * i );
		x_[i] = cos( pi_ * i ) * r;
		y_[i] = sin( pi_ * i ) * r;
	}

	// Compute the min/max and set symmetric bounds for this plot
	double xRange = std::max(fabs(min(x_)),fabs(max(x_)));
	double yRange = std::max(fabs(min(y_)),fabs(max(y_)));
	double allrange = std::max(xRange,yRange);

	// Set up viewport and window, but do not draw box
	plenv( -allrange, allrange, -allrange, allrange, 1, -2 );

	plcol0( color::grey );

	// Draw a number of circles for polar grid
	int nCircles = allrange / 0.1;
	for ( size_t i = 1; i <= nCircles; i++ ) {
		// Add the arc
		plarc( 0.0, 0.0, 0.1 * i, 0.1 * i, 0.0, 360.0, 0.0, 0 );
		// Add a label
		char title[256];
		sprintf(title,"%2.1f",0.1*i);

		plptex(0,0.1*i,0.1,0.1 * i,0, title);
	}

	// Set the radial lines
	for ( size_t i = 0; i <= 11; i++ )
	{
		double theta = 30.0 * i;
		double dx    = allrange * cos( pi_ * theta );
		double dy    = allrange * sin( pi_ * theta );

		// Draw radial spokes for polar grid and add the text
		pljoin( 0.0, 0.0, dx, dy );

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

	// Set the color for the data line
	plcol0( color::blue );
	plline( nValues_, x_, y_ );

	// set the color
	plcol0( color::red	);
	plmtex( "t", -1., 0., 0.5, "Polar Plot Example" );

	// Close the plot at end
	plend();

}





















