#include "Plotter.h"

// Constructor
Plotter::Plotter(int nValues) :
	nValues_(nValues)
{
	x_ = new double[nValues_];
	y_ = new double[nValues_];

	// Specify the output device (aquaterm)
	plsdev("aqt");

}

// Destructor
Plotter::~Plotter() {

	delete x_;
	delete y_;

}

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

void Plotter::setTestParabolicFcn(double xmin, double xmax, double ymin, double ymax) {

	// make sure the buffers x_ and y_ are init
	delete x_;
	delete y_;
	x_ = new double[nValues_];
	y_ = new double[nValues_];

	for ( int i = 0; i < nValues_; i++ )
	{
		x_[i] = (double) ( i ) / (double) ( nValues_ - 1 );
		y_[i] = ymax * x_[i] * x_[i];
	}

}

/// Find the min of the specified c-style array
double Plotter::min(double* arr) {

	double val=1E+20;
	for(size_t i=0; i<nValues_; i++){
		if(arr[i]<val)
			val=arr[i];
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

// Produce a 2d plot from Eigen vectors
void Plotter::plot() {

	// compute the values to plot
	double xmin = 0., xmax = 1., ymin = 0., ymax = 100.;
	setTestParabolicFcn(xmin, xmax, ymin, ymax);

	// Specify the background color (rgb) and its transparency
	plscolbga(255,255,255,0.6);

	// Initialize plplot
	plinit();

	// Create a labeled box to hold the plot.
	plcol0( color::grey );
	plenv( min(x_), max(x_), min(y_), max(y_), 0, 0 );
	pllab( "x", "y", "2D Plot Example" );

	// Plot the data that was prepared above.
	plcol0( color::blue );
	plline( nValues_, x_, y_ );

	// Close PLplot library
	plend();

}


// Produce a 2d plot from Eigen vectors
void Plotter::plot(Eigen::ArrayXd& x, Eigen::ArrayXd& y) {

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
	plenv( min(x_), max(x_), min(y_), max(y_), 0, 0 );
	pllab( "x", "y", "2D Plot Example" );

	// Plot the data that was prepared above.
	plcol0( color::blue );
	plline( nValues_, x_, y_ );

	// Close PLplot library
	plend();

}

void Plotter::plot(Eigen::ArrayXd& x0, Eigen::ArrayXd& y0,Eigen::ArrayXd& x1, Eigen::ArrayXd& y1) {

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

	plenv( min(x_), max(x_), min(y_), max(y_), 0, 0 );

	// Plot the data that was prepared above.
	plcol0( color::blue );
	plline( nValues_, x_, y_ );

	// ---

	// Now repeat for X1 and Y1
	// Copy the values from the incoming arrays into plplot compatible containers
	setValues(x1,y1);

	// Plot the data that was prepared above.
	plcol0( color::red );
	plline( nValues_, x_, y_ );

	// Close PLplot library
	pllab( "x", "y", "2D Plot Example" );
	plend();

}


//=====================================================================

PolarPlotter::PolarPlotter() :
		Plotter(361),
		pi_(M_PI / 180.0) {

}

PolarPlotter::~PolarPlotter() {
	// Do nothing
}

void PolarPlotter::plot() {

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





















