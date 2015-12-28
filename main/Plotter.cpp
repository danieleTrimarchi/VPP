#include "Plotter.h"

// Constructor
Plotter::Plotter(int nValues) :
	nValues_(nValues)
{
	x_ = new double[nValues_];
	y_ = new double[nValues_];
}

// Destructor
Plotter::~Plotter() {

	delete x_;
	delete y_;

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

// Produce a 2d plot from Eigen vectors
void Plotter::plot() {

	double xmin = 0., xmax = 1., ymin = 0., ymax = 100.;

	// compute the values to plot
	setTestParabolicFcn(xmin, xmax, ymin, ymax);

	// Specify the output device (aquaterm)
	plsdev("aqt");

	// Specify the background color (rgb) and its transparency
	plscolbga(255,255,255,0.6);

	// Initialize plplot
	plinit();

	// Create a labeled box to hold the plot.
	plenv( xmin, xmax, ymin, ymax, 0, 0 );
	pllab( "x", "y=100 x#u2#d", "Simple PLplot demo of a 2D line plot" );

	// Plot the data that was prepared above.
	plline( nValues_, x_, y_ );

	// Close PLplot library
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

	// Specify the output device (aquaterm)
	plsdev("aqt");

	// Specify the background color (rgb) and its transparency
	plscolbga(255,255,255,.5);

	// Initialize plplot
	plinit();

	// Set up viewport and window, but do not draw box
	plenv( -1.3, 1.3, -1.3, 1.3, 1, -2 );

	plcol0( color::grey );

	// Draw circles for polar grid
	for ( size_t i = 1; i <= 10; i++ )
	{
		plarc( 0.0, 0.0, 0.1 * i, 0.1 * i, 0.0, 360.0, 0.0, 0 );
	}

	// Set the color for the radial lines
	for ( size_t i = 0; i <= 11; i++ )
	{
		double theta = 30.0 * i;
		double dx    = cos( pi_ * theta );
		double dy    = sin( pi_ * theta );

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

	// Draw the graph
	for ( size_t i = 0; i < nValues_; i++ )
	{
		double r    = pi_ * ( .1 * i );
		x_[i] = cos( pi_ * i ) * r;
		y_[i] = sin( pi_ * i ) * r;
	}

	// Set the color for the data line
	plcol0( color::blue );
	plline( nValues_, x_, y_ );

	// set the color
	plcol0( color::red	);
	plmtex( "t", 2.0, 0.5, 0.5, "#frPLplot Example 3 - r(#gh)=sin 5#gh" );

	// Close the plot at end
	plend();

}
