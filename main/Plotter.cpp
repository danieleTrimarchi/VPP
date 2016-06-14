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
	double dx=fabs(maxX_-minX_)/10;
	double dy=fabs(maxY_-minY_)/10;

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

    if(!y.size()){
        std::cout<<"\n WARNING: Attempting to plot an empty vector. Returning \n"<<std::endl;
        return;
    }
    
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


// Append a set of data
void Plotter::append(string curveLabel, ArrayXd& xs, ArrayXd& ys) {

	// make sure the size of the arrays are the same
	if(xs.size() != ys.size())
		throw VPPException(HERE, "In Plotter::append, the size of the arrays does not match");

	// push back the angles expressed in deg
	xs_.push_back(xs);

	// push back the values
	ys_.push_back(ys);

	// push back the curve titles
	curveLabels_.push_back(curveLabel);

	// place the labels in the center of the curve
	size_t pos= xs.size() / 2;
	idx_.push_back(pos);

	// Set the ranges
	resetRanges(xs,ys);

}

// Plot the data that have been previously appended to the buffer vectors
void Plotter::plot(string xLabel,string yLabel,string plotTitle) {

	// Create a labeled box to hold the plot.
	plcol0( color::grey );

	// Set up viewport and window, but do not draw box
	plenv( minX_, maxX_, minY_, maxY_, 0, 0 );

	// Set the color for the data line
	plcol0( color::red );

	for(size_t iLine=0; iLine<xs_.size(); iLine++) {

		// Copy the vals into a c-stile array
		setValues(xs_[iLine], ys_[iLine]);

		// And now define the line
		plline( xs_[iLine].size(), x_, y_ );

		// Place the curve label in the center of the curve
		plptex(x_[idx_[iLine]],y_[idx_[iLine]],1,0,0,curveLabels_[iLine].c_str());
	}

	// set the color
	pllab( xLabel.c_str(), yLabel.c_str(), plotTitle.c_str() );

	// Close the plot at end
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
		double scale,
		string title,
		string xLabel,
		string yLabel) {

	if(x.rows()!=y.rows() || x.cols()!=y.cols())
		throw VPPException(HERE,"VectorPlot size mismatch");

	// Diagnostics
//	std::cout <<"\n\n Plotting: "<<title<<std::endl;
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

	// scale each du, dv vector according to its norm to obtain 1-normed vectors
	for ( int i = 0; i < x.rows(); i++ )
		for ( int j = 0; j < x.cols(); j++ ) {

			// Compute the scale for this vector
			double norm = std::sqrt( du(i,j)*du(i,j) + dv(i,j)*dv(i,j));
			if(norm>0){
				// scale the u-v component according to the scale
				du(i,j) *= scale/norm;
        dv(i,j) *= scale/norm;
			}

		}

//	std::cout<<"scaled du= "<<du<<std::endl;
//	std::cout<<"scaled dv= "<<dv<<std::endl;

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

// Append a set of polar data
void PolarPlotter::append(string curveLabel, std::vector<double>& alphaV, std::vector<double>& valsV){

	// Convert vector->ArrayXd and call the append method for ArrayXd
	ArrayXd alpha(alphaV.size());
	ArrayXd vals(valsV.size());

	// Fill the buffers
	for(size_t i=0; i<alphaV.size(); i++){
		alpha(i)= alphaV[i];
		vals(i)= valsV[i];
	}

	append(curveLabel, alpha, vals);

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
void PolarPlotter::plot(size_t skipCircles) {

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
	for ( size_t i = 1; i <= nCircles; i+=skipCircles ) {
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

// Plotter3d class ////////////////////////////////////////
// Init static members
double Plotter3d::alt_[]={60.0,40.0};
double Plotter3d::az_[]={30.0,-30.0};
int Plotter3d::rosen_=0;

const char* Plotter3d::title_[]= {
		"Alt=60, Az=30",
		"Alt=60, Az=-30",
};

PLOptionTable Plotter3d::options_[]=
{
		{
				"rosen",             // Turns on use of Rosenbrock function
				NULL,
				NULL,
				&Plotter3d::rosen_,
				PL_OPT_BOOL,
				"-rosen",
				"Use the log_e of the \"Rosenbrock\" function"
		},
		{
				NULL,                     // option
				NULL,                     // handler
				NULL,                     // client data
				NULL,                     // address of variable to set
				0,                        // mode flag
				NULL,                     // short syntax
				NULL
		}                             // long syntax
};

// Plotter3d Constructor
//Plotter3d::Plotter3d( int argc, const char **argv )
Plotter3d::Plotter3d() :
		nPtsX_(35),
		nPtsY_(45)
{

	// Specify the output device (aquaterm)
	plsdev("aqt");

	// Initialize plplot
	plinit();

	// Declare the value array z
	double** z;

	// Allocate a two-d array with the -z values
	plAlloc2dGrid( &z, nPtsX_, nPtsY_ );

	const int LEVELS = 10;
	double* clevel = new double[LEVELS];

	double* x = new double[ nPtsX_ ];
	double* y = new double[ nPtsY_ ];
	double dx = 2. / ( nPtsX_ - 1 );
	double dy = 2. / ( nPtsY_ - 1 );

	double xx, yy, r;
	double zmin = 0.0, zmax = 0.0;

	int ifshade;

	int     indexxmin  = 0;
	int     indexxmax  = nPtsX_;
	int     *indexymin = new int[ nPtsX_ ];
	int     *indexymax = new int[ nPtsX_ ];
	double  **zlimited;

	// parameters of ellipse (in x, y index coordinates) that limits the data.
	// x0, y0 correspond to the exact floating point center of the index range.
	double x0 = 0.5 * ( nPtsX_ - 1 );
	double a  = 0.9 * x0;
	double y0 = 0.5 * ( nPtsY_ - 1 );
	double b  = 0.7 * y0;


	for ( int i = 0; i < nPtsX_; i++ )
	{
		x[i] = -1. + (double) i * dx;
		if ( rosen_ )
			x[i] *= 1.5;
	}

	for ( size_t j = 0; j < nPtsY_; j++ )
	{
		y[j] = -1. + (double) j * dy;
		if ( rosen_ )
			y[j] += 0.5;
	}

	for ( int i = 0; i < nPtsX_; i++ )
	{
		xx = x[i];
		for ( int j = 0; j < nPtsY_; j++ )
		{
			yy = y[j];
			if ( rosen_ )
			{
				z[i][j] = pow( (double) ( 1. - xx ), 2. ) + 100 * pow( (double) ( yy - pow( (double) xx, 2. ) ), 2. );
				// The log argument might be zero for just the right grid.
				if ( z[i][j] > 0. )
					z[i][j] = log( z[i][j] );
				else
					z[i][j] = -5.; // -MAXFLOAT would mess-up up the scale
			}
			else
			{
				r       = sqrt( xx * xx + yy * yy );
				z[i][j] = exp( -r * r ) * cos( 2.0 * M_PI * r );
			}
			if ( i == 0 && j == 0 )
			{
				zmin = z[i][j];
				zmax = zmin;
			}
			if ( zmin > z[i][j] )
				zmin = z[i][j];
			if ( zmax < z[i][j] )
				zmax = z[i][j];
		}
	}

	//  Allocate and calculate y index ranges and corresponding z-limited.
	plAlloc2dGrid( &zlimited, nPtsX_, nPtsY_ );

	for ( int i = indexxmin; i < indexxmax; i++ )
	{
		double square_root = sqrt( 1. - MIN( 1., pow( ( (double) i - x0 ) / a, 2. ) ) );
		// Add 0.5 to find nearest integer and therefore preserve symmetry
		// with regard to lower and upper bound of y range.
		indexymin[i] = MAX( 0, (int) ( 0.5 + y0 - b * square_root ) );
		// indexymax calculated with the convention that it is 1
		// greater than highest valid index.
		indexymax[i] = MIN( nPtsY_, 1 + (int) ( 0.5 + y0 + b * square_root ) );
		for ( int j = indexymin[i]; j < indexymax[i]; j++ )
			zlimited[i][j] = z[i][j];
	}

	double step = ( zmax - zmin ) / ( LEVELS + 1 );
	for ( int i = 0; i < LEVELS; i++ )
		clevel[i] = zmin + step * ( i + 1 );

	pllightsource( 1., 1., 1. );
	for ( int k = 0; k < 2; k++ )
	{
		for ( ifshade = 0; ifshade < 5; ifshade++ )
		{
			pladv( 0 );
			plvpor( 0.0, 1.0, 0.0, 0.9 );
			plwind( -1.0, 1.0, -0.9, 1.1 );
			plcol0( 3 );
			plmtex( "t", 1.0, 0.5, 0.5, title_[k] );
			plcol0( 1 );
			if ( rosen_ )
				plw3d( 1.0, 1.0, 1.0, -1.5, 1.5, -0.5, 1.5, zmin, zmax,
						alt_[k], az_[k] );
			else
				plw3d( 1.0, 1.0, 1.0, -1.0, 1.0, -1.0, 1.0, zmin, zmax,
						alt_[k], az_[k] );
			plbox3( "bnstu", "x axis", 0.0, 0,
					"bnstu", "y axis", 0.0, 0,
					"bcdmnstuv", "z axis", 0.0, 0 );

			plcol0( 2 );

			switch ( ifshade )
			{
				case 0: // diffuse light surface plot
					cmap1_init( 1 );
					plsurf3d( x, y, z, nPtsX_, nPtsY_, 0, NULL, 0 );
					break;
				case 1: // magnitude colored plot
					cmap1_init( 0 );
					plsurf3d( x, y, z, nPtsX_, nPtsY_, MAG_COLOR, NULL, 0 );
					break;
				case 2: //  magnitude colored plot with faceted squares
					cmap1_init( 0 );
					plsurf3d( x, y, z, nPtsX_, nPtsY_, MAG_COLOR | FACETED, NULL, 0 );
					break;
				case 3: // magnitude colored plot with contours
					cmap1_init( 0 );
					plsurf3d( x, y, z, nPtsX_, nPtsY_, MAG_COLOR | SURF_CONT | BASE_CONT, clevel, LEVELS );
					break;
				case 4:  // magnitude colored plot with contours and index limits.
					cmap1_init( 0 );
					plsurf3dl( x, y, (const double * const *) zlimited, nPtsX_, nPtsY_, MAG_COLOR | SURF_CONT | BASE_CONT, clevel, LEVELS, indexxmin, indexxmax, indexymin, indexymax );
			}
		}
	}

	plFree2dGrid( z, nPtsX_, nPtsY_ );

	delete[] x;
	delete[] y;
	delete[] clevel;
}

// Destructor
Plotter3d::~Plotter3d() {

}

// Init the colormap
void Plotter3d::cmap1_init( int gray )
{
	double* intensity   = new double[2];
	double* hue = new double[2];
	double* lightness = new double[2];
	double* saturation = new double[2];
	PLBOOL* rev = new PLBOOL[2];

	intensity[0] = 0.0;       // left boundary
	intensity[1] = 1.0;       // right boundary

	if ( gray == 1 )
	{
		hue[0] = 0.0;     // hue -- low: red (arbitrary if s=0)
		hue[1] = 0.0;     // hue -- high: red (arbitrary if s=0)

		lightness[0] = 0.5;     // lightness -- low: half-dark
		lightness[1] = 1.0;     // lightness -- high: light

		saturation[0] = 0.0;     // minimum saturation
		saturation[1] = 0.0;     // minimum saturation
	}
	else
	{
		hue[0] = 240; // blue -> green -> yellow ->
		hue[1] = 0;   // -> red

		lightness[0] = 0.6;
		lightness[1] = 0.6;

		saturation[0] = 0.8;
		saturation[1] = 0.8;
	}

	rev[0] = false;       // interpolate on front side of color wheel.
	rev[1] = false;       // interpolate on front side of color wheel.

	plscmap1n(256);
	plscmap1l( 0, 2, intensity, hue, lightness, saturation, rev );

	//c_plscmap1l( PLBOOL itype, PLINT npts, const PLFLT *intensity,
	// const PLFLT *coord1, const PLFLT *coord2, const PLFLT *coord3, const PLBOOL *alt_hue_path );

	delete[] intensity;
	delete[] hue;
	delete[] lightness;
	delete[] saturation;
	delete[] rev;
}



















