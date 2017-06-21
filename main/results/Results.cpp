#include "Results.h"
#include "VPPException.h"
#include "mathUtils.h"

using namespace mathUtils;

///////// Result Class ///////////////////////////////

const string Result::headerBegin_=string("==RESULTS==");
const string Result::headerEnd_=string("==END RESULTS==");

// (disallowed) Default constructor
Result::Result():
													itwv_(0),
													itwa_(0),
													twv_(0),
													twa_(0),
													discard_(false) {

	// init the result container
	result_.resize(4);
	result_ << 0,0,0,0;
	// init the residuals container
	residuals_.resize(2);
	residuals_<<0,0;

}

// Constructor with no results
Result::Result(	size_t itwv, double twv,
		size_t itwa, double twa,bool discard/*=false*/):
													itwv_(itwv),
													itwa_(itwa),
													twv_(twv),
													twa_(twa),
													discard_(discard) {

	// init the result container
	result_.resize(4);
	result_ << 0,0,0,0;
	// init the residuals container
	residuals_.resize(2);
	residuals_<<0,0;

}



// Constructor -- only doubles
Result::Result(	size_t itwv, double twv, size_t itwa,
		double twa, double  v, double phi,
		double b, double f, double dF, double dM,
		bool discarde /*=false*/):
															itwv_(itwv),
															itwa_(itwa),
															twv_(twv),
															twa_(twa),
															discard_(discarde) {

	// Init the result container
	result_<< v, phi, b, f;

	// init the residuals container
	residuals_<<dF,dM;

}

// Constructor
Result::Result(size_t itwv, double twv, size_t itwa, double twa, std::vector<double>& res,
		double dF, double dM, bool discarde) :
														itwv_(itwv),
														itwa_(itwa),
														twv_(twv),
														twa_(twa),
														discard_(discarde) {

	// Init the result container
	result_<<res[0],res[1],res[2],res[3];

	// init the residuals container
	residuals_<<dF,dM;

}

// Constructor with residual array
Result::Result(	size_t itwv, double twv, size_t itwa, double twa,
		Eigen::VectorXd& result,
		Eigen::VectorXd& residuals, bool discarde ) :
															itwv_(itwv),
															itwa_(itwa),
															twv_(twv),
															twa_(twa),
															result_(result),
															residuals_(residuals),
															discard_(discarde) {
}

// Constructor with residual array
void Result::reset(	size_t itwv, double twv, size_t itwa, double twa,
		Eigen::VectorXd& result,
		Eigen::VectorXd& residuals, bool discarde ){
	itwv_= itwv;
	itwa_= itwa;
	twv_= twv;
	twa_= twa;
	result_= result;
	residuals_= residuals;
	discard_= discarde;
}

// Destructor
Result::~Result(){
	// make nothing
}

// PrintOut the values stored in this result
void Result::print(FILE* outStream) const {

	// Print the header begin
	fprintf(outStream,"%s",headerBegin_.c_str());

	fprintf(outStream,"%zu %8.6f %zu %8.6f  -- ", itwv_, twv_, itwa_, mathUtils::toDeg(twa_));
	for(size_t iRes=0; iRes<result_.size(); iRes++)
		fprintf(outStream,"  %8.6e",result_[iRes]);
	fprintf(outStream,"  --  ");
	for(size_t i=0; i<residuals_.size(); i++)
		fprintf(outStream,"  %8.6e", residuals_(i) );
	fprintf(outStream,"  --  %i ", discard_ );

	// Print the header end
	fprintf(outStream,"%s",headerEnd_.c_str());
}

// How many columns for printing out this result?
// 11 : iTWV  TWV  iTWa  TWA -- V  PHI  B  F -- dF dM -- discard
size_t Result::getTableCols() const {

	// Discard is the last of the list, its value is 10. The number
	// of rows of a table is 11.
	return TableResultType::discard+1;
}

// Returns the header for the table view
QVariant Result::getColumnHeader(int col) const {

	switch (col) {
		case TableResultType::itwv :
			return QString("iTWV [-]");
		case TableResultType::twv :
			return QString("TWV [m/s]");
		case TableResultType::itwa :
			return QString("iTWA [-]");
		case TableResultType::twa :
			return QString("TWA [Rad]");
			// --
		case TableResultType::u :
			return QString("V [m/s]");
		case TableResultType::phi :
			return QString("PHI [Rad]");
		case TableResultType::crew :
			return QString("CREW [m]");
		case TableResultType::flat :
			return QString("FLAT [-]");
			// --
		case TableResultType::residual_f :
			return QString("dF [N]");
		case TableResultType::residual_m :
			return QString("dM [N*m]");
			// --
		case TableResultType::discard :
			return QString("Discard");
			// --
		default:
			return QVariant();
	}

}

// Return a value for filling the result table (see VppTableModel)
// A table row looks like (see TableResultType)
// iTWV  TWV  iTWa  TWA -- V  PHI  B  F -- dF dM -- discard
double Result::getTableEntry(int col) const {

	switch (col) {
		case TableResultType::itwv :
			return getiTWV();
		case TableResultType::twv :
			return getTWV();
		case TableResultType::itwa :
			return getiTWA();
		case TableResultType::twa :
			return getTWA();
			// --
		case TableResultType::u :
			return result_[0];
		case TableResultType::phi :
			return result_[1];
		case TableResultType::crew :
			return result_[2];
		case TableResultType::flat :
			return result_[3];
			// --
		case TableResultType::residual_f :
			return residuals_[0];
		case TableResultType::residual_m :
			return residuals_[1];
			// --
		case TableResultType::discard :
			return discard_;
			// --
		default:
			return -1;
	}
}

// Get the index of twv for this result
size_t Result::getiTWV() const {
	return itwv_;
}

// Get the index of twa for this result
size_t Result::getiTWA() const {
	return itwa_;
}

// get the twv for this result
const double Result::getTWV() const {
	return twv_;
}

// get the twa for this result
const double Result::getTWA() const {
	return twa_;
}

// get the force residuals for this result
const double Result::getdF() const {
	return residuals_(0);
}

// get the moment residuals for this result
const double Result::getdM() const {
	return residuals_(1);
}

// get the state vector for this result
const Eigen::VectorXd* Result::getX() const {
	return &result_;
}

// Discard this solution: do not plot it
void Result::setDiscard( const bool discard ) {
	discard_= discard;
}

// Discard this solution: do not plot it
const bool Result::discard() const {
	return discard_;
}

/// Comparison operator =
bool Result::operator == (const Result& rhs) const{

	// First compare the integers with operator ==
	if( (itwv_==rhs.itwv_) &&
			(itwa_==rhs.itwa_) &&
			(twv_== rhs.twv_)  &&
			(twa_==rhs.twa_)   &&
			(discard_==rhs.discard_) ) {

		// todo dtrimarchi :shall I use an iterator to be faster?

		// Compare VectorXd result_
		if(result_.size() != rhs.result_.size())
			return false;

		// Compare the values of the results
		for(size_t iRes=0; iRes<result_.size(); iRes++)
			if(fabs((result_(iRes)-rhs.result_(iRes))/result_(iRes))>1e-6)
				return false;

		// Compare VectorXd residuals
		if(residuals_.size() != rhs.residuals_.size())
			return false;

		// Compare the values of the residuals
		for(size_t iRes=0; iRes<residuals_.size(); iRes++)
			if(fabs((residuals_(iRes)-rhs.residuals_(iRes))/residuals_(iRes))>1e-6)
				return false;

		// Nothing was found to differ, return true
		return true;

	}
	else
		return false;
}


/////  ResultContainer   /////////////////////////////////

// Default constructor
ResultContainer::ResultContainer():
											nWv_(0),
											nWa_(0),
											pWind_(0) {

}

// Constructor using a windItem
ResultContainer::ResultContainer(WindItem* pWindItem):
												pWind_(pWindItem) {

	// Get the parser
	VariableFileParser* pParser = pWind_->getParser();

	// Set the variables
	nWv_= pParser->get("N_TWV");
	nWa_= pParser->get("N_ALPHA_TW");

	// Make sure the resMat is empty and correctly resized
	initResultMatrix();

}

// Destructor
ResultContainer::~ResultContainer() {
}

// Alternative signature for push_back (compatibility)
void ResultContainer::push_back(size_t iWv, size_t iWa,
		double v, double phi, double b, double f,
		double dF, double dM ) {

	Eigen::VectorXd results(4);
	results << v, phi, b, f;

	push_back(iWv,iWa,results,dF,dM);

}

// push_back a result taking care of the allocation
void ResultContainer::push_back(size_t iWv, size_t iWa,
		Eigen::VectorXd& results,
		double dF, double dM) {

	// Compile an Eigen-vector and call the push_back method
	Eigen::VectorXd residuals(2);
	residuals << dF,dM;

	// Call the other signature of the method
	push_back(iWv,iWa,results,residuals);

}


// push_back a result taking care of the allocation
void ResultContainer::push_back(size_t iWv, size_t iWa,
		Eigen::VectorXd& results,
		Eigen::VectorXd& residuals,
		bool discard/*=false*/ ) {

	if(iWv>=nWv_){
		char msg[256];
		int v=iWv, n=nWv_;
		sprintf(msg,"In OptResultContainer, requested out-of-bounds iWv: %i on %i",v,n );
		throw VPPException(HERE,msg);
	}
	if(iWa>=nWa_) {
		char msg[256];
		int a=iWa, n=nWa_;
		sprintf(msg,"In OptResultContainer, requested out-of-bounds iWa: %i on %i",a,n );
		throw VPPException(HERE,msg);
	}

	// Ask the wind to get the current wind velocity/angles. Note that this
	// implies that the call must be in sync, which seems rather dangerous!
	resMat_[iWv][iWa].reset(iWv, pWind_->getTWV(iWv), iWa, pWind_->getTWA(iWa), results, residuals, discard );

}

// Push a trivial result marked as to be discarded -> not plot
void ResultContainer::remove(size_t iWv, size_t iWa){
	resMat_[iWv][iWa].setDiscard(true);
}

/// Get the result for a given wind velocity/angle
const Result& ResultContainer::get(size_t iWv, size_t iWa) const {

	if(iWv>=nWv_)
		throw VPPException(HERE,"In OptResultContainer::get, requested out-of-bounds iWv!");
	if(iWa>=nWa_)
		throw VPPException(HERE,"In OptResultContainer::get(), requested out-of-bounds iWa!");

	return resMat_[iWv][iWa];
}

// Get the result for a given wind velocity/angle. Assume
// the results are shown (as in print) by WA first, and then
// by WV
const Result& ResultContainer::get(size_t idx) const {

	if(idx >= size() )
		throw VPPException(HERE,"In OptResultContainer::get(idx), requested out-of-bounds!");

	// idx TWA  TWV
	// 0    0    0
	// 1    1    0
	// 2    2    0
	// 3    0    1
	// 4    1    1
	// 5    2    1
	// 6 		0    2
	// 7 		1    2
	// 8 		2    2

	// idx = 2 => tWv = 0 ; tWa = 2
	// nWa = 3
	// tWv = idx / nWa_ = 2 / 3 = 0
	// tWa = idx - tWv * nWa;

	// idx = 3; tWa = 0 ; tWv = 1
	// nWa = 3
	// tWv = idx / nWa_ = 3 / 3  = 1
	// tWa = idx - tWv * nWa = 3 - 1 * 3 = 0

	// idx = 7; tWa = 1 ; tWv = 2
	// nWa = 3
	// tWv = idx / nWa_ = 7 / 3  = 2
	// tWa = idx - tWv * nWa = 7 - 2 * 3 = 1
	size_t iWv= idx / nWa_;
	size_t iWa= idx - iWv * nWa_;

	if(iWv>=nWv_)
		throw VPPException(HERE,"Requested out-of-bounds iWv!");
	if(iWa>=nWa_)
		throw VPPException(HERE,"Requested out-of-bounds iWa!");

	return resMat_[iWv][iWa];

}

// How many results have been stored?
const size_t ResultContainer::size() const {
	return nWv_*nWa_;
}

// Count the number of results that must not be plotted
// Note that the method is brute force, but it has the
// advantage of assuring the sync
const size_t ResultContainer::getNumDiscardedResultsForAngle(size_t iWa) const {

	size_t numDiscarded=0;

	for(size_t iWv=0; iWv<nWv_; iWv++ )
		if( resMat_[iWv][iWa].discard() )
			numDiscarded++;

	return numDiscarded;
}

// Count the number of results that must not be plotted
// Note that the method is brute force, but it has the
// advantage of assuring the sync
const size_t ResultContainer::getNumDiscardedResultsForVelocity(size_t iWv) const {

	size_t numDiscarded=0;

	for(size_t iWa=0; iWa<nWa_; iWa++ )
		if( resMat_[iWv][iWa].discard() )
			numDiscarded++;

	return numDiscarded;
}

// Count the number of results that must not be plotted
// Note that the method is brute force, but it has the
// advantage of assuring the sync
const size_t ResultContainer::getNumDiscardedResults() const{

	size_t numDiscarded=0;

	for(size_t iWa=0; iWa<nWa_; iWa++ )
		numDiscarded+=getNumDiscardedResultsForAngle(iWa);

	return numDiscarded;
}

/// How many wind velocities?
const size_t ResultContainer::windVelocitySize() const {
	return nWv_;
}

/// How many wind angles?
const size_t ResultContainer::windAngleSize() const {
	return nWa_;
}

// Return the total number of valid results: the results that have not
// been discarded
const size_t ResultContainer::getNumValidResults() const {
	return nWv_*nWa_ - getNumDiscardedResults();
}

// Return the number of valid velocity-wise results for a given angle
const size_t ResultContainer::getNumValidResultsForAngle( size_t iWa ) const {
	return nWv_ - getNumDiscardedResultsForAngle(iWa);
}

// Get a ptr to the wind item
const WindItem* ResultContainer::getWind() const {
	return pWind_;
}

/// Printout the list of Opt Results, arranged by twv-twa
void ResultContainer::print(FILE* outStream) {

	fprintf(outStream,"\n%%  iTWV    TWV    iTWa    TWA   --  V    PHI    B    F  --  dF    dM    -- discard \n");
	fprintf(outStream,  "%%----------------------------------------------------------------------------------\n");
	fprintf(outStream,  "%%  [-]    [m/s]    [-]    [º]  -- [m/s] [rad] [m]  [-] --  [N]  [N*m]  --         \n");
	fprintf(outStream,  "%%----------------------------------------------------------------------------------\n");

	for(size_t iWv=0; iWv<nWv_; iWv++)
		for(size_t iWa=0; iWa<nWa_; iWa++)
			resMat_[iWv][iWa].print(outStream);
}

// CLear the result vector
void ResultContainer::initResultMatrix() {

	// Reserve the space for the results
	resMat_.clear();
	resMat_.resize(nWv_);
	for(size_t iWv=0; iWv<nWv_; iWv++){
		resMat_[iWv].reserve(nWa_);
	}

	// Init the counters itw and itwa for each result. The allocation
	// should not take too much time because the space has been preallocated
	// Set the optional argument bool discarde to true because this is a
	// placeHolder for a result, not  an actual result yet
	for(size_t iWv=0; iWv<nWv_; iWv++){
		for(size_t iWa=0; iWa<nWa_; iWa++){
			resMat_[iWv].push_back(
					Result(iWv, pWind_->getTWV(iWv),iWa, pWind_->getTWA(iWa),true)
			);
		}
	}
}

// Returns all is required to plot the polar plots
std::vector<VppPolarCustomPlotWidget*> ResultContainer::plotPolars() {

	// Prepare the vector to be returned
	std::vector<VppPolarCustomPlotWidget*> retVec;

	// Instantiate the polar plot widget that will be assigned to retVec
	VppPolarCustomPlotWidget* pUPlot= new VppPolarCustomPlotWidget("Boat velocity [m/s]");
	VppPolarCustomPlotWidget* pPhiPlot= new VppPolarCustomPlotWidget("Heeling angle [deg]");
	VppPolarCustomPlotWidget* pCrewBPlot= new VppPolarCustomPlotWidget("Crew position [m]");
	VppPolarCustomPlotWidget* pFlatPlot= new VppPolarCustomPlotWidget("Sail flat [-]");

	// Loop on the wind velocities
	for(size_t iWv=0; iWv<windVelocitySize(); iWv++) {

		// Get the number of valid results for this velocity : all minus discarded
		size_t numValidResults= windAngleSize() - getNumDiscardedResultsForVelocity(iWv);

		// Store the wind velocity as a label for this curve
		char windVelocityLabel[256];
		sprintf(windVelocityLabel,"%3.1f", get(iWv,0).getTWV() );

		// Instantiate the series required to plot the polars
		QVector<double> ux( numValidResults ), uy( numValidResults ),
				phix( numValidResults ), phiy( numValidResults ),
				crewBx( numValidResults ), crewBy( numValidResults ),
				sailFlatx( numValidResults ), sailFlaty( numValidResults );

		// Loop on the wind angles
		size_t idx=0;
		for(size_t iWa=0; iWa<windAngleSize(); iWa++) {

			if(!get(iWv,iWa).discard()){

				//				transform to polar :
				//					x = rho cos(theta)
				//					y = rho sin(theta)

				// Compute the angle, considering that the angle 'zero' is on pi/2,
				// and the direction is reversed..
				double angle = M_PI/2 - get(iWv,iWa).getTWA();

				// Fill the boat velocity series and add to the plot
				ux[idx]=	get(iWv,iWa).getX()->coeff(0) * cos(angle);
				uy[idx]=	get(iWv,iWa).getX()->coeff(0) * sin(angle);

				// Fill the boat heel
				phix[idx]= mathUtils::toDeg(get(iWv,iWa).getX()->coeff(1)) * cos(angle);
				phiy[idx]= mathUtils::toDeg(get(iWv,iWa).getX()->coeff(1)) * sin(angle);

				// Fill the crew position
				crewBx[idx]= get(iWv,iWa).getX()->coeff(2) * cos(angle);
				crewBy[idx]= get(iWv,iWa).getX()->coeff(2) * sin(angle);

				// Fill the sail flat
				sailFlatx[idx]= get(iWv,iWa).getX()->coeff(3) * cos(angle);
				sailFlaty[idx]= get(iWv,iWa).getX()->coeff(3) * sin(angle);

				// Increment the counter
				idx++;
			}
		}

		// Append the angles-data to the relevant plotter
		pUPlot->addData(ux, uy,windVelocityLabel);
		pPhiPlot->addData(phix,phiy,windVelocityLabel);
		pCrewBPlot->addData(crewBx,crewBy,windVelocityLabel);
		pFlatPlot->addData(sailFlatx,sailFlaty,windVelocityLabel);

	}

	// Push the chart to the buffer vector to be returned
	retVec.push_back(pUPlot);
	retVec.push_back(pPhiPlot);
	retVec.push_back(pCrewBPlot);
	retVec.push_back(pFlatPlot);

	// Connect the legend markers to the click action
	for(size_t i=0; i<retVec.size(); i++){

		// Rescales the axes such that all plottables (like graphs) in the plot are fully visible
		retVec[i]->rescaleAxes();

		// Add polar plot circles
		retVec[i]->addCircles();

	}

	return retVec;
}

// Returns all is required to plot the XY result plots
std::vector<VppXYCustomPlotWidget*> ResultContainer::plotXY() {

	// For which TWV, TWA shall we plot the aero forces/moments?
	WindIndicesDialog wd(pWind_);
	if (wd.exec() == QDialog::Rejected)
		return std::vector<VppXYCustomPlotWidget*>(0);;

	size_t iWa = wd.getTWA();

	// Get the number of valid results (discard==false)
	size_t numValidResults = getNumValidResultsForAngle(iWa);

	if(!numValidResults){
		std::cout<<"No valid results found for plotXY! \n";
		return std::vector<VppXYCustomPlotWidget*>(0);
	}

	// Prepare the vector to be returned
	std::vector<VppXYCustomPlotWidget*> retVec;

	char title[256];
	sprintf(title,"AWA= %4.2f[º]", toDeg(getWind()->getTWA(iWa)) );

	// Instantiate the polar plot widget that will be assigned to retVec
	VppXYCustomPlotWidget* pUPlot= new VppXYCustomPlotWidget(
			QString("Boat velocity [m/s] - ")+QString(title),
			QString("Wind Speed [m/s]"),
			QString("Boat Speed [m/s]")
			);

	VppXYCustomPlotWidget* pPhiPlot= new VppXYCustomPlotWidget(
			QString("Heeling angle [deg] - ")+QString(title),
			QString("Wind Speed [m/s]"),
			QString("Boat Heel [º]")
			);

	VppXYCustomPlotWidget* pCrewBPlot= new VppXYCustomPlotWidget(
			QString("Crew position [m] - ")+QString(title),
			QString("Wind Speed [m/s]"),
			QString("Crew Position [m]")
			);

	VppXYCustomPlotWidget* pFlatPlot= new VppXYCustomPlotWidget(
			QString("Sail flat [-] - ")+QString(title),
			QString("Wind Speed [m/s]"),
			QString("Sail Flat [-]")
			);

	VppXYCustomPlotWidget* pResidualPlot= new VppXYCustomPlotWidget(
			QString("Residuals [-] - ")+QString(title),
			QString("Wind Speed [m/s]"),
			QString("Residuals [-]")
	);

	QVector<double> windSpeeds(numValidResults),
			boatVelocity(numValidResults),
			boatHeel(numValidResults),
			boatFlat(numValidResults),
			boatB(numValidResults),
			dF(numValidResults),
			dM(numValidResults);

	// Loop on all results but only plot the valid ones
	size_t idx=0;
	for(size_t iWv=0; iWv<windVelocitySize(); iWv++) {

		if(!get(iWv,wd.getTWA()).discard()) {

			windSpeeds[idx]  = get(iWv,iWa).getTWV();
			boatVelocity[idx]= get(iWv,iWa).getX()->coeff(0);
			boatHeel[idx]    = mathUtils::toDeg( get(iWv,iWa).getX()->coeff(1));
			boatB[idx]    	 = get(iWv,iWa).getX()->coeff(2);
			boatFlat[idx]    = get(iWv,iWa).getX()->coeff(3);
			dF[idx]          = get(iWv,iWa).getdF();
			dM[idx]          = get(iWv,iWa).getdM();
			idx++;

		}
	}

	// Append the angles-data to the relevant plotter
	pUPlot->addData(windSpeeds, boatVelocity, "Boat Speed [m/s]",VppXYCustomPlotWidget::lineStyle::showPoints);
	pPhiPlot->addData(windSpeeds,boatHeel,"Boat Heel [º]",VppXYCustomPlotWidget::lineStyle::showPoints);
	pCrewBPlot->addData(windSpeeds,boatB,"Crew [m]",VppXYCustomPlotWidget::lineStyle::showPoints);
	pFlatPlot->addData(windSpeeds,boatFlat,"Sail Flat [-]",VppXYCustomPlotWidget::lineStyle::showPoints);
	pResidualPlot->addData(windSpeeds,dF,"dF [N]",VppXYCustomPlotWidget::lineStyle::showPoints);
	pResidualPlot->addData(windSpeeds,dM,"dM [N*m]",VppXYCustomPlotWidget::lineStyle::showPoints);

	// Push the chart to the buffer vector to be returned
	retVec.push_back(pUPlot);
	retVec.push_back(pPhiPlot);
	retVec.push_back(pCrewBPlot);
	retVec.push_back(pFlatPlot);
	retVec.push_back(pResidualPlot);

	// Rescales the axes such that all plottables (like graphs) in the plot are fully visible
	for(size_t i=0; i<retVec.size(); i++)
		retVec[i]->rescaleAxes();

	return retVec;

}

// Printout the bounds of the Results for the whole run
void ResultContainer::printBounds() {

	double minV=1e22;
	double maxV=-1e22;
	double minPhi=1e22;
	double maxPhi=-1e22;

	for(size_t iWv=0; iWv<nWv_; iWv++)
		for(size_t iWa=0; iWa<nWa_; iWa++){

			const Eigen::VectorXd* x( resMat_[iWv][iWa].getX() );

			if(x->coeff(0)>maxV)
				maxV=x->coeff(0);
			if(x->coeff(0)<minV)
				minV=x->coeff(0);
			if(x->coeff(1)>maxPhi)
				maxPhi=x->coeff(1);
			if(x->coeff(1)<minPhi)
				minPhi=x->coeff(1);

		}

	std::cout<<"---------------------------------------------------------------"<<std::endl;
	std::cout<<"\n MinV [m/s]    MaxV [m/s]  --   MinPhi [º]    MaxPhi [def]"<<std::endl;
	std::cout<<"---------------------------------------------------------------"<<std::endl;
	std::cout<<"  "<<minV<<"     "<<maxV<<"     --     "<<mathUtils::toDeg(minPhi)<<"     "<<mathUtils::toDeg(maxPhi)<<std::endl;
	std::cout<<"---------------------------------------------------------------"<<std::endl;

}
