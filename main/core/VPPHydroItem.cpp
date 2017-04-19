#include "VPPHydroItem.h"

#include "IOUtils.h"
#include "VPPPlotter.h"
#include "VPPException.h"
#include "Warning.h"
#include "VPPDialogs.h"

// Constructor
ResistanceItem::ResistanceItem(VariableFileParser* pParser, boost::shared_ptr<SailSet> pSailSet) :
VPPItem(pParser,pSailSet),
fN_(0),
res_(0){
	// make nothing
}

// drag the pure virtual method of the parent class one step down
void ResistanceItem::update(int vTW, int aTW) {

	// Update the Froude number using the state variable boat velocity
	fN_= convertToFn( V_ );
	if(isNotValid(fN_)) throw VPPException(HERE,"fN_ is Nan");

//		if(fN_ > 0.6) {
//			char msg[256];
//			sprintf(msg,"The value of the Froude Number %f exceeds the limit value 0.6! "
//					"Results might be incorrect.",fN_);
//			Warning warning(msg);
//		}
}

// Destructor
ResistanceItem::~ResistanceItem() {

}

// Get the value of the resistance for this ResistanceItem
const double ResistanceItem::get() const {
	if(isNotValid(res_)) throw VPPException(HERE,"res_ is Nan");
	return res_;
}

// Convert a velocity [m/s] to a Fn[-]
double ResistanceItem::convertToFn( double velocity ){

	return velocity / sqrt(Physic::g * pParser_->get("LWL"));
}

// Convert a Fn[-] to a velocity [m/s]
double ResistanceItem::convertToVelocity( double fN ) {

	return fN * sqrt(Physic::g * pParser_->get("LWL"));

}

//=========================================================
// For the definition of the Induced Resistance see DSYHS99 ch4 p128
// Constructor
InducedResistanceItem::InducedResistanceItem(AeroForcesItem* pAeroForcesItem) :
						ResistanceItem(pAeroForcesItem->getParser(), pAeroForcesItem->getSailSet()),
						pAeroForcesItem_(pAeroForcesItem),
						vf_(0.4 * sqrt(Physic::g * pParser_->get("LWL") )),
						a_(1./(2*vf_)),
						c_(vf_/2),
						v_(0) {

	coeffA_.resize(4,4);
	coeffA_ << 	3.7455,	-3.6246,	0.0589,	-0.0296,
			4.4892,	-4.8454,	0.0294,	-0.0176,
			3.9592,	-3.9804,	0.0283,	-0.0075,
			3.4891,	-2.9577,	0.0250,	-0.0272;

	coeffB_.resize(4,2);
	coeffB_ << 	1.2306,	-0.7256,
			1.4231,	-1.2971,
			1.545,	-1.5622,
			1.4744,	-1.3499;

	// Declare the angular value array and convert to RAD
	phiD_.resize(4);
	phiD_ << 0, 10, 20, 30;
	phiD_ *= ( M_PI / 180.0);

	// Draft of the canoe body
	double tCan = pParser_->get("TCAN");
	// Total Draft
	double t= 		pParser_->get("T");
	// Waterline Beam
	double bwl = 	pParser_->get("BWL");
	// Keel tip chord length
	double chtpk=	pParser_->get("CHTPK");
	// Keel root chord length
	double chrtk= pParser_->get("CHRTK");

	//geom.TCAN./geom.T (geom.TCAN./geom.T).^2 geom.BWL./geom.TCAN geom.CHTPK./geom.CHRTK;
	vectA_.resize(4);
	vectA_ << tCan/t, (tCan/t)*(tCan/t), bwl/tCan, chtpk/chrtk;

	// coeffA(4x4) * vectA(4x1) = Tegeo(4x1)
	Tegeo_ = (coeffA_ * vectA_).array();

	// Define a smoothing function for Fn=0-0.2
	pSf_.reset( new SmoothedStepFunction(0, 0.2) );

}

// Destructor
InducedResistanceItem::~InducedResistanceItem() {
}


// Implement pure virtual method of the parent class
void InducedResistanceItem::update(int vTW, int aTW) {

	// Call the parent class update to update the Froude number
	ResistanceItem::update(vTW,aTW);

	Eigen::VectorXd vectB(2);
	vectB << 1, fN_;

	// coeffB(4x2) * vectB(2x1) => TeFn(4x1)
	Eigen::ArrayXd TeFn = coeffB_ * vectB;

	// Note that this is a coefficient-wise operation Tegeo(4x1) * TeFn(4x1) -> Teffective(4x1)
	Eigen::ArrayXd Teffective = pParser_->get("T") * Tegeo_ * TeFn;

	//std::cout<<"Teffective= \n"<<Teffective<<std::endl;

	// Properly interpolate then values of TeD for the current value
	// of the state variable PHI_ (heeling angle)
	SplineInterpolator interpolator(phiD_,Teffective);
	double Te= interpolator.interpolate(PHI_);

	//  std::cout<<"phiDArr= "<<phiD_<<std::endl;
	//  std::cout<<"TeD= "<<TeD<<std::endl;
	//
	//  Plotter plotter;
	//  plotter.plot(phiD_,TeD);

	// Make a check plot for the induced resistance6
	// WARNING: as we are in update, this potentially leads to a
	// large number of plots!
	// interpolator.plot(0,toRad(30),30,"Effective Span","PHIº","Te");

	// Get the aerodynamic side force. See DSYHS99 p 129. AeroForcesItem is supposedly up to
	// date because it is stored in the aeroItems vector that is updated before the hydroItemsVector
	double fHeel= pAeroForcesItem_->getFSide() / cos(PHI_);

	// Compute the induced resistance Ri = Fheel^2 / (0.5 * rho_w * pi * Te^2 * V^2)
	if(V_>0) {

		// The velocity used in the denominator explodes the value of the induced resistance
		// for small values of V_. We limit then the value of res by bounding the lower value
		// of the velocity with a parabola. This happens to preserve c1 continuity at the velocity
		// corresponding to Fn=0.1
		if( V_<vf_)
			v_= a_ * V_ * V_ + c_;
		else
			v_= V_;

		res_ = ( fHeel * fHeel ) / ( 0.5 * Physic::rho_w * M_PI * Te * Te * v_ * v_);

		// Superpose a further smoothing using a step function for  Fn=0-0.2
		res_ *= pSf_->f( fN_ );

	}
	else
		res_= pSf_->f( 0 ) *  ( fHeel * fHeel ) / ( 0.5 * Physic::rho_w * M_PI * Te * Te * c_ * c_);


	// Whatever we have computed, make sure it is a valid number
	if(isNotValid(res_)) throw VPPException(HERE,"res_ is Nan");

}

/// Implement pure virtual of the parent class
/// Each resistance component knows how to generate a widget
/// to visualize itself in a plot
std::vector<VppXYCustomPlotWidget*> InducedResistanceItem::plot(WindIndicesDialog* wd /*=0*/, StateVectorDialog* sd /*=0*/) {

	// Prepare the vector to be returned
	std::vector<VppXYCustomPlotWidget*> retVec;

	// Fill state vector
	b_=sd->getCrew();
	f_=sd->getFlat();

	// buffer the velocity that is going to be modified by the plot
	double bufferV= V_;
	double bufferPHI= PHI_;
	double bufferb= b_;
	double bufferf= f_;

	// Define the number of velocities and angles (+=4!!)
	size_t nVelocities=40, nAngles=20;

	std::vector<QString> curveLabels;
	std::vector<QVector<double> > froudeNb, indRes;

	// Loop on the heel angles
	for(int i=0; i<nAngles; i+=5){

		PHI_= mathUtils::toRad( i - 5 );
		//std::cout<<"PHI= "<<PHI_<<std::endl;

		// declare some tmp containers
		QVector<double> fn, res;

		// Loop on the velocities
		for(size_t v=0; v<nVelocities; v++){

			// Set a fictitious velocity (Fn=-0.3-0.7)
			V_= ( 1./nVelocities * (v+1) ) * sqrt(Physic::g * pParser_->get("LWL"));

			Eigen::VectorXd x(pbSize_);
			x << V_, PHI_, b_, f_;

			// Update the aeroForceItems
			pAeroForcesItem_->getWindItem()->updateSolution(wd->getTWV(),wd->getTWA(),x);
			pAeroForcesItem_->getSailCoeffItem()->updateSolution(wd->getTWV(),wd->getTWA(),x);
			pAeroForcesItem_->updateSolution(wd->getTWV(),wd->getTWA(),x);

			// Update the induced resistance Item
			update(wd->getTWV(),wd->getTWA());

			// Fill the vectors to be plot
			fn.push_back( fN_ );
			res.push_back( res_ );

		}

		froudeNb.push_back(fn);
		indRes.push_back(res);

		char msg[256];
		sprintf(msg,"%3.1fº", toDeg(PHI_));
		curveLabels.push_back(msg);

	}

	char msg[256];
	sprintf(msg,"Induced Resistance vs boat speed - "
			"twv=%2.2f [m/s], twa=%2.2fº",
			wd->getWind()->getTWV(wd->getTWV()),
			toDeg(wd->getWind()->getTWA(wd->getTWA())) );

	// Instantiate a VppXYCustomPlotWidget and plot Total Resistance. We new with a raw ptr,
	// then the ownership will be assigned to the multiPlotWidget when adding the chart
	VppXYCustomPlotWidget* pResPlot= new VppXYCustomPlotWidget(msg,"Fn [-]","Induced Resistance [N]");
	for(size_t i=0; i<froudeNb.size(); i++)
		pResPlot->addData(froudeNb[i],indRes[i],curveLabels[i]);
	pResPlot->rescaleAxes();

	// Push the chart to the buffer vector to be returned
	retVec.push_back(pResPlot);


	/// ----- Verify the linearity Ri/Fh^2 -----------------------------------
	std::vector<QVector<double> > sideForce2;
	indRes.clear();
	curveLabels.clear();

	// Loop on the heel angles
	for(size_t i=0; i<nAngles; i+=4){

		// Compute the heel angle [Rad]
		PHI_= ( 1./nAngles * (i+1) ) * M_PI/4;

		// Solution-buffer vectors
		QVector<double> fh2, ri;

		// Loop on the velocities
		for(size_t v=0; v<nVelocities-7; v++){

			// Set a fictitious velocity (Fn=0-1)
			V_= ( 1./nVelocities * (v+1) ) * sqrt(Physic::g * pParser_->get("LWL"));

			Eigen::VectorXd x(pbSize_);
			x << V_, PHI_, b_, f_;

			// Update the aeroForceItems
			pAeroForcesItem_->getWindItem()->updateSolution(wd->getTWV(),wd->getTWA(),x);
			pAeroForcesItem_->getSailCoeffItem()->updateSolution(wd->getTWV(),wd->getTWA(),x);
			pAeroForcesItem_->updateSolution(wd->getTWV(),wd->getTWA(),x);

			// Update the induced resistance Item
			update(wd->getTWV(),wd->getTWA());

			// Compute the heeling force
			double fh= pAeroForcesItem_->getFSide() / cos(PHI_);

			// Push the squared heeling force to its buffer vector
			fh2.push_back(fh*fh);

			// Push the Induced Resistance to its buffer vector
			ri.push_back(res_);

		}

		sideForce2.push_back(fh2);
		indRes.push_back(ri);

		char msg[256];
		sprintf(msg,"%3.1f [º]", toDeg(PHI_));
		curveLabels.push_back(msg);

	}

	char msg2[256];
	sprintf(msg2,"Induced Resistance vs Fh^2 - "
			"twv=%2.2f [m/s], twa=%2.2fº",
			wd->getWind()->getTWV(wd->getTWV()),
			toDeg(wd->getWind()->getTWA(wd->getTWA())) );

	// Instantiate a VppXYCustomPlotWidget and plot Total Resistance. We new with a raw ptr,
	// then the ownership will be assigned to the multiPlotWidget when adding the chart
	pResPlot= new VppXYCustomPlotWidget(msg2,"Fh^2 [-]","Induced Resistance [N]");
	for(size_t i=0; i<sideForce2.size(); i++)
		pResPlot->addData(sideForce2[i],indRes[i],curveLabels[i]);
	pResPlot->rescaleAxes();

	// Push the chart to the buffer vector to be returned
	retVec.push_back(pResPlot);


	// Restore the values of the variables
	V_=bufferV;
	PHI_=bufferPHI;

	// Ask the user: do you want to plot Te ?
	//	if(io.askUserBool(" Would you like to plot the effective draugh Te ? "))
	//	plotTe(twv, twa);

	return retVec;
}

// Plot the effective T
void InducedResistanceItem::plotTe(int twv, int twa) {

	// Call the parent class update to update the Froude number
	ResistanceItem::update(twv,twa);

	Eigen::VectorXd vectB(2);
	vectB << 1, fN_;

	// coeffB(4x2) * vectB(2x1) => TeFn(4x1)
	Eigen::ArrayXd TeFn = coeffB_ * vectB;

	// Note that this is a coefficient-wise operation Tegeo(4x1) * TeFn(4x1) -> Teffective(4x1)
	Eigen::ArrayXd Teffective = pParser_->get("T") * Tegeo_ * TeFn;

	// Properly interpolate then values of TeD for the current value
	// of the state variable PHI_ (heeling angle)
	SplineInterpolator interpolator(phiD_,Teffective);
	double Te= interpolator.interpolate(PHI_);

	//  std::cout<<"phiDArr= "<<phiD_<<std::endl;
	//std::cout<<"Teffective= \n"<<Teffective<<std::endl;

	// Make a check plot for the induced resistance6
	interpolator.plot(0,toRad(30),30,"Effective Span","PHI [rad]","Te");

}

//=================================================================
// Residuary Resistance: see DSYHS99 3.1.1.2 p112
// Constructor
ResiduaryResistanceItem::ResiduaryResistanceItem(VariableFileParser* pParser, boost::shared_ptr<SailSet> pSailSet):
						ResistanceItem(pParser, pSailSet) {

	// Define an array of coefficients and instantiate an interpolator over it
	Eigen::MatrixXd coeff(12,9);
	coeff <<  0.,			 0.,			0.,			 0.,			0., 		 0.,			0.,			 0.,			0.,
			-0.0014, 0.0403,  0.0470, -0.0227, -0.0119,  0.0061, -0.0086, -0.0307, -0.0553,
			0.0004, -0.1808,  0.1793, -0.0004,  0.0097,  0.0118, -0.0055,  0.1721, -0.1728,
			0.0014, -0.1071,  0.0637,  0.0090,  0.0153,  0.0011,  0.0012,  0.1021, -0.0648,
			0.0027,  0.0463, -0.1263,  0.0150,  0.0274, -0.0299,  0.0110, -0.0595,  0.1220,
			0.0056, -0.8005,  0.4891,  0.0269,  0.0519, -0.0313,  0.0292,  0.7314, -0.3619,
			0.0032, -0.1011, -0.0813, -0.0382,  0.0320, -0.1481,  0.0837,  0.0223,  0.1587,
			-0.0064,  2.3095, -1.5152,  0.0751, -0.0858, -0.5349,  0.1715, -2.4550,  1.1865,
			-0.0171,  3.4017, -1.9862,  0.3242, -0.1450, -0.8043,  0.2952, -3.5284,  1.3575,
			-0.0201,  7.1576, -6.3304,  0.5829,  0.1630, -0.3966,  0.5023, -7.1579,  5.2534,
			0.0495,  1.5618, -6.0661,  0.8641,  1.1702,  1.7610,  0.9176, -2.1191,  5.4281,
			0.0808, -5.3233, -1.1513,  0.9663,  1.6084,  2.7459,  0.8491,  4.7129,  1.1089;

	// Define the vector with the physical quantities multiplying the
	// polynomial coefficients
	//  vect = [geom.LWL./geom.DIVCAN^(1/3),
	//   				geom.XFB./geom.LWL,
	//   				geom.CPL,
	//   				geom.DIVCAN.^(2/3)./geom.AW,
	//   				geom.BWL./geom.LWL,
	//   				geom.DIVCAN.^(2/3)./geom.SC,
	//   				geom.XFB./geom.XFF,
	//   				(geom.XFB./geom.LWL).^2,
	//   				geom.CPL.^2]';
	Eigen::VectorXd vect(9);
	vect << pParser_->get("LWL") / std::pow(pParser_->get("DIVCAN"),1./3),
			pParser_->get("XFB") / pParser_->get("LWL") ,
			pParser_->get("CPL"),
			std::pow(pParser_->get("DIVCAN"),2./3) / pParser_->get("AW"),
			pParser_->get("BWL") / pParser_->get("LWL"),
			std::pow(pParser_->get("DIVCAN"),2./3) / pParser_->get("SC"),
			pParser_->get("XFB") / pParser_->get("XFF"),
			std::pow(pParser_->get("XFB") / pParser_->get("LWL"),2),
			std::pow(pParser_->get("CPL"),2);

	// Compute the the residuary resistance for each Froude numbers
	//  RrhD = (geom.DIVCAN.*phys.g.*phys.rho_w) .* ( coeff * vect' ) .* (geom.DIVCAN.^(1/3)./geom.LWL);
	//  12x1        SCALAR                            12x9  *  9x1            SCALAR
	Eigen::VectorXd RrhD = 	(pParser_->get("DIVCAN") * Physic::g * Physic::rho_w) *
			( std::pow(pParser_->get("DIVCAN"),1./3) / pParser_->get("LWL") ) *
			coeff * vect ;

	// Values of the froude number on which the coefficients of the
	// residuary resistance are computed
	Eigen::ArrayXd fnD(12);
	fnD << 	0., .1, .15, .2, .25, .3, .35, .4, .45, .5, .55, .6;

	// generate the cubic spline values for the coefficients
	Eigen::ArrayXd RrhDArr=RrhD.array();
	pInterpolator_.reset( new SplineInterpolator(fnD,RrhDArr) );

}

/// Destructor
ResiduaryResistanceItem::~ResiduaryResistanceItem() {
	// make nothing
}

/// Implement pure virtual method of the parent class
void ResiduaryResistanceItem::update(int vTW, int aTW) {

	// Call the parent class update to update the Froude number
	ResistanceItem::update(vTW,aTW);

	// Compute the residuary resistance for the current froude number
	res_ = pInterpolator_->interpolate(fN_);
	if(isNotValid(res_)) throw VPPException(HERE,"res_ is Nan");

}


// Implement pure virtual of the parent class
// Each resistance component knows how to generate a widget
// to visualize itself in a plot
std::vector<VppXYCustomPlotWidget*> ResiduaryResistanceItem::plot(WindIndicesDialog* wd /*=0*/, StateVectorDialog* sd /*=0*/) {

	ResistanceItem::update(wd->getTWV(),wd->getTWA());

	res_ = pInterpolator_->interpolate(fN_);

	// Make a check plot for the residuary resistance
	VppXYCustomPlotWidget* pPlot = new VppXYCustomPlotWidget("Residuary Resistance Hull","Fn [-]","Resistance [N]");
	pInterpolator_->plot(pPlot, 0,.6,50 );
	//multiPlotWidget->addChart(pPlot,0,0);

	return std::vector<VppXYCustomPlotWidget*>(1,pPlot);
}

//=================================================================
// For the change in Residuary Resistance due to heel see DSYHS99 ch3.1.2.2 p116
// Constructor
Delta_ResiduaryResistance_HeelItem::Delta_ResiduaryResistance_HeelItem(
		VariableFileParser* pParser, boost::shared_ptr<SailSet> pSailSet) :
						ResistanceItem(pParser,pSailSet) {

	// Define an array of coefficients and instantiate an interpolator over it
	Eigen::MatrixXd coeff(7,6);
	coeff << -0.0268, -0.0014, -0.0057, 0.0016, -0.0070, -0.0017,
					  0.6628, -0.0632, -0.0699, 0.0069,  0.0459, -0.0004,
						1.6433, -0.2144, -0.1640, 0.0199, -0.0540, -0.0268,
					 -0.8659, -0.0354,  0.2226, 0.0188, -0.5800, -0.1133,
					 -3.2715,	 0.1372,  0.5547, 0.0268, -1.0064, -0.2026,
					 -0.1976, -0.1480, -0.6593, 0.1862, -0.7489, -0.1648,
					  1.5873, -0.3749, -0.7105, 0.2146, -0.4818, -0.1174;

	// Divided the coefficients by 10^3, as stated in the article table6
	coeff /= 1000.;

	// Define the vector with the physical quantities multiplying the
	// polynomial coefficients
	// vect = [	1
	//					geom.LWL./geom.BWL
	//					geom.BWL./geom.TCAN
	//					(geom.BWL./geom.TCAN).^2
	//					geom.XFB / geom.BWL      // todo WARNING : is this term correct?
	//					(geom.XFB/ geom.BWL ).^2];  // todo WARNING : is this term correct?
	Eigen::VectorXd vect(6);
	vect << 1,
			pParser_->get("LWL") / pParser_->get("BWL"),
			pParser_->get("BWL") / pParser_->get("TCAN"),
			std::pow(pParser_->get("BWL") / pParser_->get("TCAN"),2),
			pParser_->get("XFB")/pParser_->get("LWL"),
			std::pow(pParser_->get("XFB")/pParser_->get("LWL"),2);

	// Compute the resistance @PHI=20deg for each Fn
	// RrhH20D = (geom.DIVCAN.*phys.g.*phys.rho_w) .* coeff*vect';
	Eigen::ArrayXd RrhH20D=
			pParser_->get("DIVCAN") * Physic::g * Physic::rho_w *
			coeff * vect;

	// Values of the froude number on which the coefficients of the
	// residuary resistance are computed
	Eigen::ArrayXd fnD(7);
	fnD << .25, .3, .35, .4, .45, .5, .55;

	// generate the cubic spline values for the coefficients
	Eigen::ArrayXd RrhH20DArr=RrhH20D.array();
	pInterpolator_.reset( new SplineInterpolator(fnD,RrhH20DArr) );

}

// Destructor
Delta_ResiduaryResistance_HeelItem::~Delta_ResiduaryResistance_HeelItem() {
	// make nothing
}

// Implement pure virtual method of the parent class
void Delta_ResiduaryResistance_HeelItem::update(int vTW, int aTW) {

	// Call the parent class update to update the Froude number
	ResistanceItem::update(vTW,aTW);

	// limit the values to positive angles and Fn>0.25, as in the definition of the DHYS
	// todo dtrimarchi : this might generate problems, we need to smooth the function down
	// gently!!!
	if(fN_<0.25) {
		res_=0.;
		return;
	}

	// Compute the residuary resistance for the current froude number
	// RrhH = RrhH20 .* 6 .* ( phi ).^1.7;
	// No matter the sign of phi, this is a positive resistance item and I want to
	// make sure that negative angles increase the total resistance
	res_ = pInterpolator_->interpolate(fN_) * 6. * std::pow( std::fabs(PHI_),1.7) ;
	if(isNotValid(res_)) throw VPPException(HERE,"res_ is Nan");

}

// Implement pure virtual of the parent class
// Each resistance component knows how to generate a widget
// to visualize itself in a plot
std::vector<VppXYCustomPlotWidget*> Delta_ResiduaryResistance_HeelItem::plot(WindIndicesDialog* wd /*=0*/, StateVectorDialog* sd/*=0*/) {

	// Init the state vector
	b_= sd->getCrew();
	f_= sd->getFlat();

	size_t nVelocities=40, maxAngleDeg=30;

	// Instantiate containers for the curve labels, the
	// Fn and the resistance
	std::vector<QString> curveLabels;
	std::vector<QVector<double> > froudeNb, totRes;

	// Loop on the heel angles
	for(size_t iAngle=0; iAngle<maxAngleDeg+1; iAngle+=5){

		// Compute the value of PHI
		PHI_ = mathUtils::toRad(iAngle);

		// declare some tmp containers
		QVector<double> fn, res;

		// Loop on the velocities
		for(size_t v=0; v<nVelocities; v++){

			// Set a fictitious velocity (Fn=-0.3-0.7)
			V_= ( 0 + ( .6 / nVelocities * v ) ) * sqrt(Physic::g * pParser_->get("LWL"));

			// Update all the Items - not just the hydro as indRes requires up-to-date fHeel!
			update(wd->getTWV(),wd->getTWA());

			// Fill the vectors to be plot
			fn.push_back( V_/sqrt(Physic::g * pParser_->get("LWL") ) );
			res.push_back( get() );

		}

		froudeNb.push_back(fn);
		totRes.push_back(res);

		char msg[256];
		sprintf(msg,"%3.1fº", mathUtils::toDeg(PHI_));
		curveLabels.push_back(msg);

	}

	char msg[256];
	sprintf(msg,"Delta Residuary Resistance due to Heel - "
			"twv=%2.2f [m/s], twa=%2.2fº",
			wd->getWind()->getTWV(wd->getTWV()),
			mathUtils::toDeg(wd->getWind()->getTWA(wd->getTWA())) );

	// Instantiate a VppXYCustomPlotWidget and plot Total Resistance. We new with a raw ptr,
	// then the ownership will be assigned to the multiPlotWidget when adding the chart
	VppXYCustomPlotWidget* pResPlot= new VppXYCustomPlotWidget(msg,"Fn [-]","Resistance [N]");
	for(size_t i=0; i<froudeNb.size(); i++)
		pResPlot->addData(froudeNb[i],totRes[i],curveLabels[i]);
	pResPlot->rescaleAxes();

	return std::vector<VppXYCustomPlotWidget*>(1,pResPlot);

	//pMultiPlotWidget->addChart(pResPlot,posx,posy);
}

//=================================================================
// For the definition of the Residuary Resistance of the Keel see DSYHS99 3.2.1.2 p.120 and following
// Constructor
ResiduaryResistanceKeelItem::ResiduaryResistanceKeelItem(VariableFileParser* pParser, boost::shared_ptr<SailSet> pSailSet):
						ResistanceItem(pParser,pSailSet) {

	// Define an array of coefficients and instantiate an interpolator over it
	Eigen::MatrixXd coeff(11,4);
	coeff <<	 0.,			0.,			 0.,			 0.,
			0.,			0.,			 0.,			 0.,
			-0.00104, 0.00172, 0.00117, -0.00008,
			-0.00550, 0.00597, 0.00390, -0.00009,
			-0.01110, 0.01421, 0.00069,  0.00021,
			-0.00713, 0.02632,-0.00232,  0.00039,
			-0.03581, 0.08649, 0.00999,  0.00017,
			-0.00470, 0.11592,-0.00064,  0.00035,
			0.00553, 0.07371, 0.05991, -0.00114,
			0.04822, 0.00660, 0.07048, -0.00035,
			0.01021, 0.14173, 0.06409, -0.00192;

	// Define the vector with the physical quantities multiplying the
	// polynomial coefficients
	// vect = [	1
	//					geom.T./geom.BWL
	//					(geom.T-geom.ZCBK)./geom.DVK.^(1/3)
	//					geom.DIVCAN./geom.DVK];
	Eigen::VectorXd vect(4);
	vect << 1,
			pParser_->get("T")/pParser_->get("BWL"),
			(pParser_->get("T")-pParser_->get("ZCBK")) / std::pow(pParser_->get("DVK"),1./3),
			pParser_->get("DIVCAN")/pParser_->get("DVK");

	// Compute the Keel Residuary Resistance / Fn vector
	// RrkD = (geom.DVK.*phys.rho_w.*phys.g).* coeff*vect;
	Eigen::ArrayXd RrkD=
			pParser_->get("DVK") * Physic::rho_w * Physic::g *
			coeff * vect;

	// Values of the froude number on which the coefficients of the
	// residuary resistance are computed
	Eigen::ArrayXd fnD(11);
	fnD << 0., 0.1, .2, .25, .3, .35, .4, .45, .5, .55, .6;

	// generate the cubic spline values for the coefficients
	Eigen::ArrayXd RrkDArr=RrkD.array();
	pInterpolator_.reset( new SplineInterpolator(fnD,RrkDArr) );

}

// Destructor
ResiduaryResistanceKeelItem::~ResiduaryResistanceKeelItem() {
}

// Implement pure virtual method of the parent class
void ResiduaryResistanceKeelItem::update(int vTW, int aTW) {

	// Call the parent class update to update the Froude number
	ResistanceItem::update(vTW,aTW);

	// Updates the value of the Residuary Resistance due to the Keel
	res_= pInterpolator_->interpolate(fN_);
	if(isNotValid(res_)) throw VPPException(HERE,"res_ is Nan");

}

// Implement pure virtual of the parent class
// Each resistance component knows how to generate a widget
// to visualize itself in a plot
std::vector<VppXYCustomPlotWidget*> ResiduaryResistanceKeelItem::plot(WindIndicesDialog* wd /*=0*/, StateVectorDialog* /*=0*/) {

	// Make a check plot for the Residuary resistance of the keel
	VppXYCustomPlotWidget* pPlot = new VppXYCustomPlotWidget("Residuary Resistance Keel","Fn [-]","Resistance [N]");
	pInterpolator_->plot(pPlot, 0,.6,50 );
//multiPlotWidget->addChart(pPlot,posx,posy);

	return std::vector<VppXYCustomPlotWidget*>(1,pPlot);
}

//=================================================================

// Express the change in Appendage Resistance due to Heel.
// See DSYHS99 3.2.2 p 126
// Constructor
Delta_ResiduaryResistanceKeel_HeelItem::Delta_ResiduaryResistanceKeel_HeelItem(
		VariableFileParser* pParser, boost::shared_ptr<SailSet> pSailSet):
						ResistanceItem(pParser,pSailSet) {

	// Define an array of coefficients and instantiate an interpolator over it
	Eigen::VectorXd coeff(4);
	coeff << -3.5837, -0.0518,	0.5958,	0.2055;

	// Define the vector with the physical quantities multiplying the
	// polynomial coefficients
	//vect = [
	//				geom.TCAN./geom.T
	//				geom.BWL./geom.TCAN
	//				(geom.BWL.*geom.TCAN)./(geom.T.*geom.TCAN)
	//				geom.LWL./geom.DIVCAN.^1/3 ];
	Eigen::VectorXd vect(4);
	vect << pParser_->get("TCAN")/pParser_->get("T"),
			pParser_->get("BWL")/pParser_->get("TCAN"),
			pParser_->get("BWL")/pParser_->get("T"),
			pParser_->get("LWL")/(std::pow(pParser_->get("DIVCAN"),1./3));

	// Express the resistance coefficient
	Ch_ = coeff.transpose() * vect;

	// And multiply for the const coefficients :
	// Ch_ = Ch_ *  (geom.DVK.*phys.rho_w.*phys.g.*Ch.)
	Ch_ *= pParser_->get("DVK") * Physic::rho_w * Physic::g;

}

// Destructor
Delta_ResiduaryResistanceKeel_HeelItem::~Delta_ResiduaryResistanceKeel_HeelItem() {

}

// Implement pure virtual method of the parent class
void Delta_ResiduaryResistanceKeel_HeelItem::update(int vTW, int aTW) {

	// Call the parent class update to update the Froude number
	ResistanceItem::update(vTW,aTW);

	// Compute the resistance
	// RrkH = (geom.DVK.*phys.rho_w.*phys.g.*Ch)*Fn.^2.*phi*pi/180;
	res_= Ch_ * fN_ * fN_ * PHI_;
	if(isNotValid(res_)) throw VPPException(HERE,"res_ is Nan");

}

// Implement pure virtual of the parent class
// Each resistance component knows how to generate a widget
// to visualize itself in a plot
std::vector<VppXYCustomPlotWidget*> Delta_ResiduaryResistanceKeel_HeelItem::plot(WindIndicesDialog* wd /*=0*/, StateVectorDialog* sd /*=0*/) {
	// Init the state vector
	b_= sd->getCrew();
	f_= sd->getFlat();

	size_t nVelocities=40, maxAngleDeg=30;

	// Instantiate containers for the curve labels, the
	// Fn and the resistance
	std::vector<QString> curveLabels;
	std::vector<QVector<double> > froudeNb, totRes;

	// Loop on the heel angles
	for(size_t iAngle=0; iAngle<maxAngleDeg+1; iAngle+=5){

		// Compute the value of PHI
		PHI_ = mathUtils::toRad(iAngle);

		// declare some tmp containers
		QVector<double> fn, res;

		// Loop on the velocities
		for(size_t v=0; v<nVelocities; v++){

			// Set a fictitious velocity (Fn=-0.3-0.7)
			V_= ( ( 0.7 / nVelocities * v ) ) * sqrt(Physic::g * pParser_->get("LWL"));

			// Update all the Items - not just the hydro as indRes requires up-to-date fHeel!
			update(wd->getTWV(),wd->getTWA());

			// Fill the vectors to be plot
			fn.push_back( V_/sqrt(Physic::g * pParser_->get("LWL") ) );
			res.push_back( get() );

		}

		froudeNb.push_back(fn);
		totRes.push_back(res);

		char msg[256];
		sprintf(msg,"%3.1fº", mathUtils::toDeg(PHI_));
		curveLabels.push_back(msg);

	}

	char msg[256];
	sprintf(msg,"plot Delta Residuary Resistance of the keel due to Heel - "
			"twv=%2.2f [m/s], twa=%2.2fº",
			wd->getWind()->getTWV(wd->getTWV()),
			mathUtils::toDeg(wd->getWind()->getTWA(wd->getTWA())) );

	// Instantiate a VppXYCustomPlotWidget and plot Total Resistance. We new with a raw ptr,
	// then the ownership will be assigned to the multiPlotWidget when adding the chart
	VppXYCustomPlotWidget* pResPlot= new VppXYCustomPlotWidget(msg,"Fn [-]","Resistance [N]");
	for(size_t i=0; i<froudeNb.size(); i++)
		pResPlot->addData(froudeNb[i],totRes[i],curveLabels[i]);
	pResPlot->rescaleAxes();

	return std::vector<VppXYCustomPlotWidget*>(1,pResPlot);
	//pMultiPlotWidget->addChart(pResPlot,posx,posy);
}

//=================================================================
// For the definition of the Frictional Resistance see DSYHS99 2.1 p108
// Constructor
FrictionalResistanceItem::FrictionalResistanceItem(VariableFileParser* pParser, boost::shared_ptr<SailSet> pSailSet):
						ResistanceItem(pParser,pSailSet) {

	// Pre-compute the velocity independent part of rN_
	rN0_= pParser->get("LWL") * 0.7 / Physic::ni_w;

	// Pre-compute the velocity independent part of rF_
	rfh0_= 0.5 * Physic::rho_w * pParser_->get("SC");

}

// Destructor
FrictionalResistanceItem::~FrictionalResistanceItem() {

}

// Implement pure virtual method of the parent class
void FrictionalResistanceItem::update(int vTW, int aTW) {

	// Call the parent class update to update the Froude number
	ResistanceItem::update(vTW,aTW);

	// Limit the computations to positive values
	if(V_<=0) {
		res_=0.;
		return;
	}

	// Compute the Reynolds number
	// Rn = geom.LWL .* 0.7 .* V ./ phys.ni_w;
	double rN = rN0_ * V_;

	// Compute the Frictional coefficient
	double cF = 0.075 / std::pow( (std::log10(rN) - 2), 2);

	// Compute the Frictional resistance of the bare hull
	// Rfh = 1/2 .* phys.rho_w .* V.^2 .* geom.SC .* Cf;
	double rfh = rfh0_ * V_ * V_ * cF;

	// Compute the frictional resistance
	res_ = rfh * pParser_->get("HULLFF");
	if(isNotValid(res_)) throw VPPException(HERE,"res_ is Nan");

}

// Implement pure virtual of the parent class
// Each resistance component knows how to generate a widget
// to visualize itself in a plot
std::vector<VppXYCustomPlotWidget*> FrictionalResistanceItem::plot(WindIndicesDialog* wd /*=0*/, StateVectorDialog* sd /*=0*/) {

	// buffer the velocity that is going to be modified by the plot
	double bufferV= V_;

	int nVals=20;
	QVector<double> fN(nVals), y(nVals);

	for(size_t i=0; i<nVals; i++) {

		// Set a fictitious velocity (Fn=0-1)
		V_= ( 1./nVals * (i+1) ) * sqrt(Physic::g * pParser_->get("LWL"));

		// Update the item
		update(0,0);

		// Fill the vectors to be plot
		fN[i]= fN_;
		y[i]= res_;

	}

	// Restore the initial buffered values
	V_= bufferV;
	update(0,0);

	// Instantiate a plotter and plot the curves
	VppXYCustomPlotWidget* pFrictionalResPlot= new VppXYCustomPlotWidget("Frictional Resistance Hull","Fn [-]","Resistance [N]");
	pFrictionalResPlot->addData(fN,y,"Frict Res");
	pFrictionalResPlot->rescaleAxes();

	return std::vector<VppXYCustomPlotWidget*>(1,pFrictionalResPlot);
	//multiPlotWidget->addChart(pFrictionalResPlot,0,0);

}

//=================================================================

// For the definition of the Change in wetted surface see DSYHS99 3.1.2.1 p115-116
// For the definition of the Frictional Resistance use the std definition of
// DSYHS99

// Constructor
Delta_FrictionalResistance_HeelItem::Delta_FrictionalResistance_HeelItem(
		VariableFileParser* pParser, boost::shared_ptr<SailSet> pSailSet):
								ResistanceItem(pParser,pSailSet) {

	// Pre-compute the velocity independent part of rN_. The definition of the Rn
	// is given by DSYHS99, p109. 0.7 is an arbitrary factor used to reduce the lwl
	rN0_= pParser->get("LWL") * 0.7 / Physic::ni_w;

	// Define an array of coefficients and instantiate an interpolator over it
	Eigen::MatrixXd coeff(7,4);
	coeff <<
			-4.112,  0.054, -0.027,	6.329,
			-4.522, -0.132, -0.077,	8.738,
			-3.291, -0.389, -0.118,	8.949,
			 1.850, -1.200, -0.109,	5.364,
			 6.510, -2.305, -0.066,	3.443,
      12.334, -3.911,	 0.024,	1.767,
			14.648, -5.182,	 0.102,	3.497;

	// Define the vector with the physical quantities multiplying the
	// polynomial coefficients
	//vect = [1
	//				geom.BWL./geom.TCAN
	//				(geom.BWL./geom.TCAN).^2
	//				geom.CMS];
	Eigen::VectorXd vect(4);
	vect << 1,
			pParser_->get("BWL")/pParser_->get("TCAN"),
			std::pow( (pParser_->get("BWL") / pParser_->get("TCAN")),2),
			pParser_->get("CMS");

	// Values of the heel angle on which the coefficients and convert to radians
	Eigen::ArrayXd phiD(7);
	phiD << 5, 10, 15, 20, 25, 30, 35;
	phiD *= ( M_PI / 180.0);

	// Compute the coefficients for the current geometry
	Eigen::ArrayXd SCphiDArr = pParser_->get("SC") *
			( 1 + 1./100. * (coeff * vect).array() );

	// generate the cubic spline values for the coefficients
	pInterpolator_.reset( new SplineInterpolator(phiD,SCphiDArr) );

	// Plot the change in wetted surf due to heel
	// pInterpolator_->plot(0,mathUtils::toRad(20),20,"Change in wetted area due to HEEL","PHI [rad]","dS [m**2]" );

}

// Destructor
Delta_FrictionalResistance_HeelItem::~Delta_FrictionalResistance_HeelItem() {

}

// Implement pure virtual method of the parent class
void Delta_FrictionalResistance_HeelItem::update(int vTW, int aTW) {

	// Call the parent class update to update the Froude number
	ResistanceItem::update(vTW,aTW);

	// Limit the computations to positive values and the heeling angles -> defined by the DHYS
	//if(V_<=0. || PHI_ < mathUtils::toRad(5) ) {
	if(V_<=0.){
		res_=0.;
		return;
	}

	// Compute the Reynolds number
	// Rn = geom.LWL .* 0.7 .* V ./ phys.ni_w;
	double rN = rN0_ * V_;

	// Compute the Frictional coefficient
	double cF = 0.075 / std::pow( (std::log10(rN) - 2), 2);

	// Compute the interpolated value of the change in wetted area wrt PHI [rad]
	double SCphi = pInterpolator_->interpolate(PHI_);

	// Compute the change in Frictional resistance using the delta of wetted surface
	// Apart for the def. of the surface, the frictional resistance uses the std definition
	// see DSYHS99 3.2.1.1 p119
	// Rfh = 1/2 .* phys.rho_w .* V.^2 .* Cf .* ( S - S0 );
	double rfhH = 0.5 * Physic::rho_w * V_ * V_ * cF * ( SCphi - pParser_->get("SC") );

	// todo dtrimarchi: does it make sense to use the same hull form factor both for the upright and the heeled hull?
	// See DSYHS99 p119, where the form factor is also defined. Here we ask the user to prompt a value
	res_ = rfhH * pParser_->get("HULLFF");
	if(isNotValid(res_)) throw VPPException(HERE,"res_ is Nan");

}

// Plot the Frictional Resistance due to heel versus Fn curve
std::vector<VppXYCustomPlotWidget*> Delta_FrictionalResistance_HeelItem::plot_deltaWettedArea_heel() {

	// Make a check plot for the Residuary resistance of the keel
	VppXYCustomPlotWidget* pPlot = new VppXYCustomPlotWidget("Change in wetted area due to HEEL","PHI [rad]","dS [m**2]");
	pInterpolator_->plot(pPlot, 0,0.6,20);

	return std::vector<VppXYCustomPlotWidget*>(1,pPlot);

}

/// Implement pure virtual of the parent class
/// Each resistance component knows how to generate a widget
/// to visualize itself in a plot
std::vector<VppXYCustomPlotWidget*> Delta_FrictionalResistance_HeelItem::plot(WindIndicesDialog* wd /*=0*/, StateVectorDialog* sd /*=0*/) {

	size_t nVelocities=40, maxAngleDeg=40;

	// Instantiate containers for the curve labels, the
	// Fn and the resistance
	std::vector<QString> curveLabels;
	std::vector<QVector<double> > froudeNb, totRes;

	// Loop on the heel angles
	for(int iAngle=0; iAngle<maxAngleDeg+1; iAngle+=10){

		// Compute the value of PHI
		PHI_ = mathUtils::toRad(iAngle-10);

		// declare some tmp containers
		QVector<double> fn, res;

		// Loop on the velocities
		for(size_t v=0; v<nVelocities; v++){

			// Set a fictitious velocity (Fn=-0.-0.7)
			V_= ( ( .7 / nVelocities * v ) ) * sqrt(Physic::g * pParser_->get("LWL"));

			update(wd->getTWV(),wd->getTWA());

			// Fill the vectors to be plot
			fn.push_back( V_/sqrt(Physic::g * pParser_->get("LWL") ) );
			res.push_back( get() );

		}

		froudeNb.push_back(fn);
		totRes.push_back(res);

		char msg[256];
		sprintf(msg,"%3.1fº", mathUtils::toDeg(PHI_));
		curveLabels.push_back(msg);

	}

	char msg[256];
	sprintf(msg,"plot Delta Frictional Resistance due to Heel - "
			"twv=%2.2f [m/s], twa=%2.2fº",
			wd->getWind()->getTWV(wd->getTWV()),
			mathUtils::toDeg(wd->getWind()->getTWA(wd->getTWA())) );

	// Instantiate a VppXYCustomPlotWidget and plot Total Resistance. We new with a raw ptr,
	// then the ownership will be assigned to the multiPlotWidget when adding the chart
	VppXYCustomPlotWidget* pResPlot= new VppXYCustomPlotWidget(msg,"Fn [-]","Resistance [N]");
	for(size_t i=0; i<froudeNb.size(); i++)
		pResPlot->addData(froudeNb[i],totRes[i],curveLabels[i]);
	pResPlot->rescaleAxes();

	return std::vector<VppXYCustomPlotWidget*>(1,pResPlot);

}

//=================================================================

// Constructor
ViscousResistanceKeelItem::ViscousResistanceKeelItem(
		VariableFileParser* pParser, boost::shared_ptr<SailSet> pSailSet):
												ResistanceItem(pParser,pSailSet) {
}

// Destructor
ViscousResistanceKeelItem::~ViscousResistanceKeelItem() {

}

// Implement pure virtual method of the parent class
void ViscousResistanceKeelItem::update(int vTW, int aTW) {

	// Call the parent class update to update the Froude number
	ResistanceItem::update(vTW,aTW);

	// Limit the computations to positive values
	if(V_<=0.) {
		res_=0.;
		return;
	}

	// Define the Reynolds number using the mean chord length of the keel,
	// this is a value provided by the user. The viscous resistance is
	// defined in the std way, see DSYHS99 3.2.1.1 p 119
	double rN = pParser_->get("CHMEK") * V_ / Physic::ni_w;
	double cf = 0.075 / std::pow((std::log10(rN) - 2),2);
	double rfk= 0.5 * Physic::rho_w * V_ * V_ * pParser_->get("SK") * cf;

	// todo dtrimarchi : this form factor can be computed from the
	// Keel geometry (see DSYHS99) Ch.3.2.11
	res_ = rfk * pParser_->get("KEELFF");
	if(isNotValid(res_)) throw VPPException(HERE,"res_ is Nan");

}

// Implement pure virtual of the parent class
// Each resistance component knows how to generate a widget
// to visualize itself in a plot
std::vector<VppXYCustomPlotWidget*> ViscousResistanceKeelItem::plot(WindIndicesDialog* wd /*=0*/, StateVectorDialog* /*=0*/) {

	// buffer the velocity that is going to be modified by the plot
	double bufferV= V_;

	int nVals=10;
	QVector<double> x(nVals), y(nVals);

	for(size_t i=0; i<nVals; i++) {

		// Set a fictitious velocity
		V_= ( 1./nVals * (i+1) ) * sqrt(Physic::g * pParser_->get("LWL"));

		// Update the item
		update(0,0);

		// Fill the vectors to be plot
		x[i]= fN_;
		y[i]= res_;

	}

	// Restore the initial buffered values
	V_= bufferV;
	update(0,0);

	// Instantiate a VppXYCustomPlotWidget and plot the resistance. We new with a raw ptr,
	// then the ownership will be assigned to the multiPlotWidget when adding the chart
	VppXYCustomPlotWidget* pResPlot= new VppXYCustomPlotWidget("Viscous Resistance Keel","Fn [-]","Resistance [N]");
	pResPlot->addData(x,y,"Viscous Resistance Keel");
	pResPlot->rescaleAxes();
	//multiPlotWidget->addChart(pResPlot,posx,posy);

	return std::vector<VppXYCustomPlotWidget*>(1,pResPlot);
}

//=================================================================

// Constructor
ViscousResistanceRudderItem::ViscousResistanceRudderItem(
		VariableFileParser* pParser, boost::shared_ptr<SailSet> pSailSet):
																		ResistanceItem(pParser,pSailSet) {
}

// Destructor
ViscousResistanceRudderItem::~ViscousResistanceRudderItem() {

}

/// Implement pure virtual method of the parent class
void ViscousResistanceRudderItem::update(int vTW, int aTW) {

	// Call the parent class update to update the Froude number
	ResistanceItem::update(vTW,aTW);

	// Limit the computations to positive values
	if(V_<=0.) {
		res_=0.;
		return;
	}

	// Define the Reynolds number using the mean chord length of the rudder,
	// this is a value provided by the user. The viscous resistance is
	// defined in the std way, see DSYHS99 3.2.1.1 p 119
	double rN = pParser_->get("CHMER") * V_ / Physic::ni_w;
	double cf = 0.075 / std::pow((std::log10(rN) - 2),2);
	double rfr= 0.5 * Physic::rho_w * V_ * V_ * pParser_->get("SR") * cf;

	// todo dtrimarchi : this form factor can be computed from the
	// Rudder geometry (see DSYHS99) Ch.3.2.11
	res_ = rfr * pParser_->get("RUDDFF");
	if(isNotValid(res_)) throw VPPException(HERE,"res_ is Nan");

}

// Implement pure virtual of the parent class
// Each resistance component knows how to generate a widget
// to visualize itself in a plot
std::vector<VppXYCustomPlotWidget*> ViscousResistanceRudderItem::plot(WindIndicesDialog* wd /*=0*/, StateVectorDialog* /*=0*/) {

	// buffer the velocity that is going to be modified by the plot
	double bufferV= V_;

	int nVals=10;
	QVector<double> x(nVals), y(nVals);

	for(size_t i=0; i<nVals; i++) {

		// Set a fictitious velocity
		V_= ( 1./nVals * (i+1) ) * sqrt(Physic::g * pParser_->get("LWL"));

		// Update the item
		update(0,0);

		// Fill the vectors to be plot
		x[i]= fN_;
		y[i]= res_;

	}

	// Restore the initial buffered values
	V_= bufferV;
	update(0,0);

	// Instantiate a VppXYCustomPlotWidget and plot Total Resistance. We new with a raw ptr,
	// then the ownership will be assigned to the multiPlotWidget when adding the chart
	VppXYCustomPlotWidget* pResPlot= new VppXYCustomPlotWidget("Viscous Resistance Rudder","Fn [-]","Resistance [N]");
	pResPlot->addData(x,y,"Viscous Resistance Rudder");
	pResPlot->rescaleAxes();
	//multiPlotWidget->addChart(pResPlot,posx,posy);

	return std::vector<VppXYCustomPlotWidget*>(1,pResPlot);
}

//=================================================================

// Constructor
NegativeResistanceItem::NegativeResistanceItem(
		VariableFileParser* pParser, boost::shared_ptr<SailSet> pSailSet):
																		ResistanceItem(pParser,pSailSet) {
}

// Destructor
NegativeResistanceItem::~NegativeResistanceItem() {

}

/// Implement pure virtual method of the parent class
void NegativeResistanceItem::update(int vTW, int aTW) {

	// Call the parent class update to update the Froude number
	ResistanceItem::update(vTW,aTW);

	// Limit the computations to negative values. In this case
	// define negative resistance as v^3
	if(V_<0.)
		res_=V_*V_*V_;
	else
		res_=0;

}

// Implement pure virtual of the parent class
// Each resistance component knows how to generate a widget
// to visualize itself in a plot
std::vector<VppXYCustomPlotWidget*> NegativeResistanceItem::plot(WindIndicesDialog* wd /*=0*/, StateVectorDialog* sd /*=0*/) {

	// buffer the velocity that is going to be modified by the plot
	double bufferV= V_;

	int nVals=10;
	QVector<double> x(nVals), y(nVals);

	for(size_t i=0; i<nVals; i++) {

		// Set a fictitious -negative- velocity
		V_= - ( 1./nVals * (i+1) ) * sqrt(Physic::g * pParser_->get("LWL"));

		// Update the item
		update(0,0);

		// Fill the vectors to be plot
		x[i]= fN_;
		y[i]= res_;

	}

	// Restore the initial buffered values
	V_= bufferV;
	update(0,0);

	// Instantiate a plotter and plot the curves
	VppXYCustomPlotWidget* pResPlot= new VppXYCustomPlotWidget("Negative Resistance","Fn [-]","Resistance [N]");
	pResPlot->addData(x,y,"Negative Resistance");
	pResPlot->rescaleAxes();
	//multiPlotWidget->addChart(pResPlot,0,0);

	return std::vector<VppXYCustomPlotWidget*>(1,pResPlot);
}

