#include "Plotter.h"

// Constructor
Plotter::Plotter() :
	nValues_(101)
{
	x_ = new double[101];
	y_ = new double[101];
}

// Destructor
Plotter::~Plotter() {

	delete x_;
	delete y_;

}

void Plotter::setTestParabolicFcn(double xmin, double xmax, double ymin, double ymax) {

	for ( int i = 0; i < nValues_; i++ )
	{
		x_[i] = (double) ( i ) / (double) ( nValues_ - 1 );
		y_[i] = ymax * x_[i] * x_[i];
	}

}

// Produce a 2d plot from Eigen vectors
void Plotter::plotXY() {

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

// Produce a 2d plot from Eigen vectors
void Plotter::plotPolar() {

	int          i;
	double        dtr, theta, dx, dy, r, offset;
	char         text[4];
	static double x0[361], y0[361];
	static double x[361], y[361];

	dtr = M_PI / 180.0;
	for ( i = 0; i <= 360; i++ )
	{
		x0[i] = cos( dtr * i );
		y0[i] = sin( dtr * i );
	}


	// Set orientation to portrait - note not all device drivers
	// support this, in particular most interactive drivers do not
	plsori( 1 );

	// Specify the output device (aquaterm)
	plsdev("aqt");

	// Specify the background color (rgb) and its transparency
	plscolbga(255,255,255,0.6);

	// Initialize plplot
	plinit();

	// Set up viewport and window, but do not draw box
	plenv( -1.3, 1.3, -1.3, 1.3, 1, -2 );

	// Draw circles for polar grid
	for ( i = 1; i <= 10; i++ )
	{
		plarc( 0.0, 0.0, 0.1 * i, 0.1 * i, 0.0, 360.0, 0.0, 0 );
	}

	plcol0( 2 );
	for ( i = 0; i <= 11; i++ )
	{
		theta = 30.0 * i;
		dx    = cos( dtr * theta );
		dy    = sin( dtr * theta );

		// Draw radial spokes for polar grid

		pljoin( 0.0, 0.0, dx, dy );
		sprintf( text, "%d", ROUND( theta ) );

		// Write labels for angle

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
	for ( i = 0; i <= 360; i++ )
	{
		r    = sin( dtr * ( 5 * i ) );
		x[i] = x0[i] * r;
		y[i] = y0[i] * r;
	}
	plcol0( 3 );
	plline( 361, x, y );

	plcol0( 4 );
	plmtex( "t", 2.0, 0.5, 0.5, "#frPLplot Example 3 - r(#gh)=sin 5#gh" );

	// Close the plot at end
	plend();

}
