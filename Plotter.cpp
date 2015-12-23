#include "Plotter.h"

// Constructor
Plotter::Plotter() {
	
}

// Destructor
Plotter::~Plotter() {
	
}

// Produce a 2d plot from Eigen vectors
void Plotter::plotXY() {


  // Prepare data to be plotted.
	int nPts=101;

	double xmin = 0., xmax = 1., ymin = 0., ymax = 100.;
	double px[nPts], py[nPts];
	for ( int i = 0; i < nPts; i++ )
	{
		px[i] = (double) ( i ) / (double) ( nPts - 1 );
		py[i] = ymax * px[i] * px[i];
	}

	Eigen::ArrayXd x, y;
	x.resize(nPts);
	y.resize(nPts);
	for ( int i = 0; i < nPts; i++ )
	{
		x(i) = (double) ( i ) / (double) ( nPts - 1 );
		y(i) = ymax * x(i) * x(i);
	}


	// Specify the output device (aquaterm)
	plsdev("aqt");

	// Initialize plplot
	plinit();

	// Create a labeled box to hold the plot.
	plenv( xmin, xmax, ymin, ymax, 0, 0 );
	pllab( "x", "y=100 x#u2#d", "Simple PLplot demo of a 2D line plot" );

	// Plot the data that was prepared above.
	plline( nPts, px, py );

	// Close PLplot library
	plend();

}

// Produce a 2d plot from Eigen vectors
void plotPolar(VectorXd& theta, VectorXd val) {
	// do nothing 
}
