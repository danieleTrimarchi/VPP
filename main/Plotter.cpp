#include "Plotter.h"
#include "VPPException.h"
#include "mathUtils.h"

using namespace mathUtils;

PlotterBase::PlotterBase():
		minX_(1E20),
		minY_(1E20),
		maxX_(-1E20),
		maxY_(-1E20) {

	// Specify the output device (aquaterm)
	plsdev("aqt");

	// Specify the background color (rgb) and its transparency
	plscolbga(255,255,255,0.6);

	// Initialize plplot
	plinit();

	// Define the color of the labeled box holding the plot.
	plcol0( color::grey );

}

// Destructor
PlotterBase::~PlotterBase(){
	// make nothing
}

// Reset the ranges to very big values
void PlotterBase::initRanges() {

	minX_=1E20;
	minY_=1E20;
	maxX_=-1E20;
	maxY_=-1E20;

}

void PlotterBase::resetRanges(ArrayXd& x, ArrayXd& y) {

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

void PlotterBase::resetRanges(MatrixXd& x, MatrixXd& y, bool axisEqual) {

	// Set the ranges for the first array
	if(x.minCoeff()<minX_)
		minX_=x.minCoeff();
	if(y.minCoeff()<minY_)
		minY_=y.minCoeff();
	if(x.maxCoeff()>maxX_)
		maxX_=x.maxCoeff();
	if(y.maxCoeff()>maxY_)
		maxY_=y.maxCoeff();

	// In the case the max and min are the same, give an arbitrary depht
	if(minX_==maxX_){
		minX_ -= 0.5;
		maxX_ += 0.5;
	}
	if(minY_==maxY_){
		minY_ -= 0.5;
		maxY_ += 0.5;
	}

	if(axisEqual){
		double tmp=std::min(minX_,minY_);
		minX_=tmp;
		minY_=tmp;

		tmp=std::max(maxX_,maxY_);
		maxX_=tmp;
		maxY_=tmp;
	}

	// Now increase the spacing to visualize all of the plot
	double dx=fabs(maxX_-minX_)/50;
	double dy=fabs(maxY_-minY_)/50;

	minX_-=dx;
	maxX_+=dx;
	minY_-=dy;
	maxY_+=dy;

}

void PlotterBase::resetRanges(std::vector<double>& x, std::vector<double>& y) {

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
void PlotterBase::resetRanges(ArrayXd& x0,ArrayXd& y0,ArrayXd& x1,ArrayXd& y1) {

	resetRanges(x0,y0);
	resetRanges(x1,y1);

}

// Reset the ranges so that it can contain all of the plot
void PlotterBase::resetRanges(std::vector<double>& x0,std::vector<double>& y0,std::vector<double>& x1,std::vector<double>& y1) {

	resetRanges(x0,y0);
	resetRanges(x1,y1);

}

// Find the min of the specified c-style array
double PlotterBase::min(std::vector<double>& vec) {

	double val=1E+20;
	for(size_t i=0; i<vec.size(); i++){
		if(vec[i]<val)
			val=vec[i];
	}
	return val;
}

// Find the max of the specified c-style array
double PlotterBase::max(std::vector<double>& vec) {
	double val=-1E+20;
	for(size_t i=0; i<vec.size(); i++){
		if(vec[i]>val)
			val=vec[i];
	}
	return val;
}

// Plotter class ////////////////////////////////////////

// Constructor
Plotter::Plotter():
				PlotterBase(),
				nValues_(0),
				x_(0), y_(0) {

}

// Destructor
Plotter::~Plotter() {

	delete x_;
	delete y_;

}

// Copy the values into plplot compatible containers
void Plotter::setValues(ArrayXd& x, ArrayXd& y) {

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

/// Find the max of the specified c-style array
double Plotter::max(double* arr) {
	double val=-1E+20;
	for(size_t i=0; i<nValues_; i++){
		if(arr[i]>val)
			val=arr[i];
	}
	return val;
}

// Plot the points of some given arrays
void Plotter::plot( std::vector<double>& y, string title) {

	// generate a VectorXd 'x' with default abscissas
	ArrayXd xtemp(y.size());
	ArrayXd ytemp(y.size());
	for(size_t i=0; i<y.size(); i++){
		xtemp(i)=i;
		ytemp(i)=y[i];
	}

	// Call the std plot method
	plot(xtemp,ytemp,title);
}

// Produce a 2d plot from Eigen vectors
void Plotter::plot(ArrayXd& x, ArrayXd& y,string title) {

	// Reset the ranges at the very beginning of the plot
	initRanges();

	// Check the size of x and y are consistent
	if(x.size() != y.size())
		throw VPPException(HERE,"In Plotter::plot(x,y). Array sizes mismatch");

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

void Plotter::plot(	ArrayXd& x0,
		ArrayXd& y0,
		ArrayXd& x1,
		ArrayXd& y1,
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
VectorPlotter::VectorPlotter() :
				PlotterBase(),
				nX_(0),
				nY_(0) {

}

//// instantiate a vectorPlotter and quit. Usage for a single vector plot:
// VectorPlotter testplot;
// Eigen::MatrixXd x(1,4), y(1,4), du(1,4), dv(1,4);
//// Coordinate vectors. Will form a coordinate matrix
//x  << 0, 1, 2, 3;
//y  << 1., 1., 1., 1.;
//// For each point of the grid, u-value of the
//du << 0.5, 0.2, 1., 0.5;
//dv << 0.5, 0.2, 1., 0.5;
//testplot.plot(x,y,du,dv,"vector plot","x","y");
void VectorPlotter::plot(
		MatrixXd& x,  MatrixXd& y,
		MatrixXd& du, MatrixXd& dv,
		string title,
		string xLabel,
		string yLabel) {

	if(x.rows()!=y.rows() || x.cols()!=y.cols())
		throw VPPException(HERE,"VectorPlot size mismatch");

	// Diagnostics
//	std::cout<<"x= "<<x<<std::endl;
//	std::cout<<"y= "<<y<<std::endl;
//	std::cout<<"du= "<<du<<std::endl;
//	std::cout<<"dv= "<<dv<<std::endl;

	// Declare arrays
	PLcGrid2 cgrid2;
	double **u, **v;

	// Allocate arrays
	plAlloc2dGrid( &cgrid2.xg, x.rows(), x.cols() );
	plAlloc2dGrid( &cgrid2.yg, x.rows(), x.cols() );
	plAlloc2dGrid( &u, x.rows(), x.cols() );
	plAlloc2dGrid( &v, x.rows(), x.cols() );
	cgrid2.nx = x.rows();
	cgrid2.ny = x.cols();

	// Create data - vectors are placed half-way through the coordinate pts
	for ( int i = 0; i < x.rows(); i++ )
		for ( int j = 0; j < y.cols(); j++ ) {

			cgrid2.xg[i][j] = x(i,j);
			cgrid2.yg[i][j] = y(i,j);
			u[i][j]         = du(i,j);
			v[i][j]         = dv(i,j);
		}

	// Set the ranges and the bounding box
	resetRanges(x,y);
	plenv( minX_, maxX_, minY_, maxY_, 0, 0 );

	// Set the plot labels
	pllab(xLabel.c_str(),yLabel.c_str(),title.c_str());

	// Set the color for the vectors
	plcol0( color::red );

	// plot the vectors
	plvect( u, v, x.rows(), x.cols(), 1.0, plcallback::tr2, (void *) &cgrid2 );

	// Free memory for the current grid, finalize the plot
	plFree2dGrid( cgrid2.xg,x.rows(),x.cols() );
	plFree2dGrid( cgrid2.yg,x.rows(),x.cols() );
	plFree2dGrid( u,x.rows(), x.cols() );
	plFree2dGrid( v,x.rows(), x.cols() );
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
void PolarPlotter::append(string curveLabel, ArrayXd& alpha, ArrayXd& vals) {

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
void PolarPlotter::setValues(ArrayXd& x, ArrayXd& vals) {

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



















