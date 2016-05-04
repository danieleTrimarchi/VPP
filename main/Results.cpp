#include "Results.h"
#include "VPPException.h"
#include "mathUtils.h"

using namespace mathUtils;

///////// Result Class ///////////////////////////////

// Default constructor
Result::Result():
			twv_(0),
			twa_(0) {

	// init the result container
	result_.resize(4);
	result_ << 0,0,0,0;
	// init the residuals container
	residuals_.resize(4);
	residuals_<<0,0,0,0;

}

// Constructor
Result::Result(double twv, double twa, std::vector<double>& res,
		double dF, double dM) :
				twv_(twv),
				twa_(twa){

	// Init the result container
	result_<<res[0],res[1],res[2],res[3];

	// init the residuals container
	residuals_<<dF,dM,0,0;

}

// Constructor
Result::Result(	double twv, double twa,
				std::vector<double>& results,
				Eigen::VectorXd& residuals ):
				twv_(twv),
				twa_(twa),
				residuals_(residuals){

	result_<<results[0],results[1],results[2],results[3];

}

// Constructor with residual array
Result::Result(	double twv, double twa,
				Eigen::VectorXd& result,
				Eigen::VectorXd& residuals ) :
					twv_(twv),
					twa_(twa),
					result_(result),
					residuals_(residuals){
}

// Destructor
Result::~Result(){
	// make nothing
}

// PrintOut the values stored in this result
void Result::print() {

	printf("%4.2f  %4.2f  -- ", twv_,mathUtils::toDeg(twa_));
	for(size_t iRes=0; iRes<result_.size(); iRes++)
		printf("  %4.2e",result_[iRes]);
	printf("  --  ");
	for(size_t i=0; i<residuals_.size(); i++)
		printf("  %4.2e", residuals_(i) );

	std::cout<<"\n";
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

// get the moment residuals for this result
const double Result::getC1() const {
	return residuals_(2);
}

// get the moment residuals for this result
const double Result::getC2() const {
	return residuals_(3);
}

// get the state vector for this result
const Eigen::VectorXd* Result::getX() const {
	return &result_;
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

	// Allocate enough space in the resMat_[Wv][Wa] and init
    resMat_.clear();
    resMat_.resize(nWv_);
	for(size_t iWv=0; iWv<nWv_; iWv++){
		resMat_[iWv].resize(nWa_);
	}
}

// Destructor
ResultContainer::~ResultContainer() {

}

// push_back a result taking care of the allocation
void ResultContainer::push_back(size_t iWv, size_t iWa,
																Eigen::VectorXd& results,
																double dF, double dM ) {

	// Compile an Eigen-vector and call the push_back method
	Eigen::VectorXd residuals(4);
	residuals << dF,dM,0,0;

	// Call the other signature of the method
	push_back(iWv,iWa,results,residuals);

}

// push_back a result taking care of the allocation
void ResultContainer::push_back(size_t iWv, size_t iWa,
																Eigen::VectorXd& results,
																Eigen::VectorXd& residuals ) {

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
	// todo dtrimarchi: the wind must have calls such as pWind_->getTWV(iWv)
	resMat_[iWv][iWa] = Result(pWind_->getTWV(), pWind_->getTWA(), results, residuals );

}



/// Get the result for a given wind velocity/angle
const Result& ResultContainer::get(size_t iWv, size_t iWa) const {

	if(iWv>=nWv_)
		throw VPPException(HERE,"In OptResultContainer::get, requested out-of-bounds iWv!");
	if(iWa>=nWa_)
		throw VPPException(HERE,"In OptResultContainer::get(), requested out-of-bounds iWa!");

	return resMat_[iWv][iWa];
}

// How many results have been stored?
const size_t ResultContainer::size() const {
	return nWv_*nWa_;
}

/// How many wind velocities?
const size_t ResultContainer::windVelocitySize() const {
	return nWv_;
}

/// How many wind angles?
const size_t ResultContainer::windAngleSize() const {
	return nWa_;
}

/// Printout the list of Opt Results, arranged by twv-twa
void ResultContainer::print() {

	std::cout<<"\n TWV    TWA   --  V    PHI    B    F  --  dF    dM    c1    c2 "<<std::endl;
	std::cout<<"---------------------------------------------------------------"<<std::endl;
	std::cout<<"[m/s]  [deg]  -- [m/s] [deg] [m]  [-] --  [N]  [N*m]  [-]   [-]"<<std::endl;
	std::cout<<"---------------------------------------------------------------"<<std::endl;
	for(size_t iWv=0; iWv<nWv_; iWv++)
		for(size_t iWa=0; iWa<nWa_; iWa++)
			resMat_[iWv][iWa].print();
}

