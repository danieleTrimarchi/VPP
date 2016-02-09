#include "VPPHydroItem.h"
#include "Warning.h"
#include "mathUtils.h"
using namespace mathUtils;
#include "VPPException.h"

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
	fN_= fabs(V_) / sqrt(Physic::g * pParser_->get("LWL"));
	if(isnan(fN_)) throw VPPException(HERE,"fN_ is nan");


	// todo dtrimarchi : possibly re-introduce the warning
//	if(fN_ > 0.6) {
//		char msg[256];
//		sprintf(msg,"The value of the Froude Number %f exceeds the limit value 0.6! "
//				"Results might be incorrect.",fN_);
//		Warning warning(msg);
//	}

}

// Destructor
ResistanceItem::~ResistanceItem() {

}

// Get the value of the resistance for this ResistanceItem
const double ResistanceItem::get() const {
	if(isnan(res_)) throw VPPException(HERE,"res_ is nan");
	return res_;
}

void ResistanceItem::printWhoAmI() {
	std::cout<<"--> WhoAmI of ResistanceItem "<<std::endl;
}

//=========================================================

// Constructor
InducedResistanceItem::InducedResistanceItem(AeroForcesItem* pAeroForcesItem) :
				ResistanceItem(pAeroForcesItem->getParser(), pAeroForcesItem->getSailSet()),
				pAeroForcesItem_(pAeroForcesItem) {

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

	phiD_.resize(4);
	phiD_ << 0, 10, 20, 30;

	double tCan = pParser_->get("TCAN");
	double t= 		pParser_->get("T");
	double bwl = 	pParser_->get("BWL");
	double chtpk=	pParser_->get("CHTPK");
	double chrtk= pParser_->get("CHRTK");

	//geom.TCAN./geom.T (geom.TCAN./geom.T).^2 geom.BWL./geom.TCAN geom.CHTPK./geom.CHRTK;
	vectA_.resize(4);
	vectA_ << tCan/t, (tCan/t)*(tCan/t), bwl/tCan, chtpk/chrtk;

	// coeffA(4x4) * vectA(4x1) = Tegeo(4x1)
	Tegeo_ = (coeffA_ * vectA_).array();

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

	// Note that this is a coefficient-wise operation Tegeo(4x1) * TeFn(4x1) -> TeD(4x1)
	Eigen::ArrayXd TeD = pParser_->get("T") * Tegeo_ * TeFn;

	// Properly interpolate then values of TeD for the current value
	// of the state variable PHI_ (heeling angle)
	SplineInterpolator interpolator(phiD_,TeD);
	double Te= interpolator.interpolate(PHI_);

	//  std::cout<<"phiDArr= "<<phiD_<<std::endl;
	//  std::cout<<"TeD= "<<TeD<<std::endl;
	//
	//  Plotter plotter;
	//  plotter.plot(phiD_,TeD);

	// Make a check plot for the induced resistance
	// WARNING: as we are in update, this potentially leads to a
	// large number of plots!
	//interpolator.plot(0,30,30,"Induced Resistance");

	// Get the aerodynamic side force
	double fHeel= pAeroForcesItem_->getFSide();

	// Compute the induced resistance Ri = Fheel^2 / (0.5 * pi * rho_w * Te^2 * V^2)
	res_ = fHeel * fHeel / ( 0.5 * Physic::rho_w * M_PI * Te * Te * V_ * V_);
	if(isnan(res_)) throw VPPException(HERE,"res_ is nan");

}

void InducedResistanceItem::printWhoAmI() {
	std::cout<<"--> WhoAmI of InducedResistanceItem "<<std::endl;
}

//=================================================================

// Constructor
ResiduaryResistanceItem::ResiduaryResistanceItem(VariableFileParser* pParser, boost::shared_ptr<SailSet> pSailSet):
				ResistanceItem(pParser, pSailSet) {

	// Define an array of coefficients and instantiate an interpolator over it
	Eigen::MatrixXd coeff(11,9);
	coeff << -0.0014,  0.0403,  0.0470, -0.0227, -0.0119,  0.0061, -0.0086, -0.0307, -0.0553,
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
	//  11x1        SCALAR                            11x9  *  9x1            SCALAR
	Eigen::VectorXd RrhD = 	(pParser_->get("DIVCAN") * Physic::g * Physic::rho_w) *
			( std::pow(pParser_->get("DIVCAN"),1./3) / pParser_->get("LWL") ) *
			coeff * vect ;

	// Values of the froude number on which the coefficients of the
	// residuary resistance are computed
	Eigen::ArrayXd fnD(11);
	fnD << 	.1, .15, .2, .25, .3, .35, .4, .45, .5, .55, .6;

	// generate the cubic spline values for the coefficients
	Eigen::ArrayXd RrhDArr=RrhD.array();
	pInterpolator_.reset( new SplineInterpolator(fnD,RrhDArr) );

	// Make a check plot for the induced resistance
	pInterpolator_->plot(0,1,50,"Residuary Resistance","Fn [-]","Resistance [N]" );
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
	if(isnan(res_)) throw VPPException(HERE,"res_ is nan");

}

void ResiduaryResistanceItem::printWhoAmI() {
	std::cout<<"--> WhoAmI of ResiduaryResistanceItem "<<std::endl;
}

//=================================================================

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
	//					geom.XFB./geom.LWL
	//					(geom.XFB./geom.LWL).^2];
	Eigen::VectorXd vect(6);
	vect << 1,
			pParser_->get("LWL") / pParser_->get("BWL"),
			pParser_->get("BWL") / pParser_->get("TCAN"),
			std::pow(pParser_->get("BWL") / pParser_->get("TCAN"),2),
			pParser_->get("XFB") / pParser_->get("LWL"),
			std::pow(pParser_->get("XFB") / pParser_->get("LWL"),2);

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

	// Make a check plot for the induced resistance
	pInterpolator_->plot(0,.7,20,"Delta Residuary Resistance","Fn [-]","Resistance [N]" );

}

// Destructor
Delta_ResiduaryResistance_HeelItem::~Delta_ResiduaryResistance_HeelItem() {
	// make nothing
}

// Implement pure virtual method of the parent class
void Delta_ResiduaryResistance_HeelItem::update(int vTW, int aTW) {

	// Call the parent class update to update the Froude number
	ResistanceItem::update(vTW,aTW);

	// Compute the residuary resistance for the current froude number
	// RrhH = RrhH20 .* 6 .* ( toRad(phi) ).^1.7;
	res_ = pInterpolator_->interpolate(fN_) * 6. * std::pow( toRad(PHI_),1.7) ;
	if(isnan(res_)) throw VPPException(HERE,"res_ is nan");

}

void Delta_ResiduaryResistance_HeelItem::printWhoAmI() {
	std::cout<<"--> WhoAmI of Delta_ResiduaryResistance_HeelItem "<<std::endl;
}

//=================================================================

// Constructor
ResiduaryResistanceKeelItem::ResiduaryResistanceKeelItem(VariableFileParser* pParser, boost::shared_ptr<SailSet> pSailSet):
				ResistanceItem(pParser,pSailSet) {

	// Define an array of coefficients and instantiate an interpolator over it
	Eigen::MatrixXd coeff(9,4);
	coeff <<	-0.00104, 0.00172, 0.00117, -0.00008,
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
	Eigen::ArrayXd fnD(9);
	fnD << .2, .25, .3, .35, .4, .45, .5, .55, .6;

	// generate the cubic spline values for the coefficients
	Eigen::ArrayXd RrkDArr=RrkD.array();
	pInterpolator_.reset( new SplineInterpolator(fnD,RrkDArr) );

	// Make a check plot for the Residuary resistance
	pInterpolator_->plot(0,.8,40,"Residuary Resistance Keel","Fn [-]","Resistance [N]" );

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
	if(isnan(res_)) throw VPPException(HERE,"res_ is nan");

}

void ResiduaryResistanceKeelItem::printWhoAmI() {
	std::cout<<"--> WhoAmI of ResiduaryResistanceKeelItem "<<std::endl;
}

//=================================================================

// Express the change in Appendage Resistance due to Heel.
// See Keuning 3.2.2 p 126
// Constructor
Delta_ResiduaryResistanceKeel_HeelItem::Delta_ResiduaryResistanceKeel_HeelItem(
		VariableFileParser* pParser, boost::shared_ptr<SailSet> pSailSet):
				ResistanceItem(pParser,pSailSet) {

	// Define an array of coefficients and instantiate an interpolator over it
	Eigen::Vector4d coeff;
	coeff << -3.5837, -0.0518,	0.5958,	0.2055;

	// Define the vector with the physical quantities multiplying the
	// polynomial coefficients
	//vect = [
	//				geom.TCAN./geom.T
	//				geom.BWL./geom.TCAN
	//				(geom.BWL.*geom.TCAN)./(geom.T.*geom.TCAN)
	//				geom.LWL./geom.DIVCAN.^1/3 ];
	Eigen::Vector4d vect;
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
	res_= Ch_ * fN_ * fN_ * toRad(PHI_);
	if(isnan(res_)) throw VPPException(HERE,"res_ is nan");

}

void Delta_ResiduaryResistanceKeel_HeelItem::printWhoAmI() {
	std::cout<<"--> WhoAmI of Delta_ResiduaryResistanceKeel_HeelItem "<<std::endl;
}

//=================================================================

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
	if(isnan(res_)) throw VPPException(HERE,"res_ is nan");

}

void FrictionalResistanceItem::printWhoAmI() {
	std::cout<<"--> WhoAmI of FrictionalResistanceItem "<<std::endl;
}

//=================================================================

// Constructor
Delta_FrictionalResistance_HeelItem::Delta_FrictionalResistance_HeelItem(
		VariableFileParser* pParser, boost::shared_ptr<SailSet> pSailSet):
						ResistanceItem(pParser,pSailSet) {

	// Pre-compute the velocity independent part of rN_
	rN0_= pParser->get("LWL") * 0.7 / Physic::ni_w;

	// Define an array of coefficients and instantiate an interpolator over it
	Eigen::MatrixXd coeff(8,4);
	coeff <<	 0.000,	 0.000,	 0.000,	0.000,
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

	// Values of the heel angle on which the coefficients
	Eigen::ArrayXd phiD(8);
	phiD << 0, 5, 10, 15, 20, 25, 30, 35;

	// Compute the coefficients for the current geometry
	Eigen::ArrayXd SCphiDArr = pParser_->get("SC") *
			( 1 + 1./100. * (coeff * vect).array() );

	// generate the cubic spline values for the coefficients
	pInterpolator_.reset( new SplineInterpolator(phiD,SCphiDArr) );

	// Make a check plot for the induced resistance
	pInterpolator_->plot(0,40,20,"Frictional Resistance due to HEEL","Fn [-]","Resistance [N]" );

}

// Destructor
Delta_FrictionalResistance_HeelItem::~Delta_FrictionalResistance_HeelItem() {

}

// Implement pure virtual method of the parent class
void Delta_FrictionalResistance_HeelItem::update(int vTW, int aTW) {

	// Call the parent class update to update the Froude number
	ResistanceItem::update(vTW,aTW);

	// Compute the Reynolds number
	// Rn = geom.LWL .* 0.7 .* V ./ phys.ni_w;
	double rN = rN0_ * V_;

	// Compute the Frictional coefficient
	double cF = 0.075 / std::pow( (std::log10(rN) - 2), 2);

	// Compute the interpolated value of the change in wetted area wrt PHI [deg]
	double SCphi = pInterpolator_->interpolate(PHI_);

	// Compute the Frictional resistance of the bare hull
	// Rfh = 1/2 .* phys.rho_w .* V.^2 .* geom.SC .* Cf;
	double rfhH = 0.5 * Physic::rho_w * V_ * V_ * cF * (SCphi-pParser_->get("SC"));

	// todo dtrimarchi: does it make sense to use the same hull form factor both for the upright and the heeled hull?
	res_ = rfhH * pParser_->get("HULLFF");
	if(isnan(res_)) throw VPPException(HERE,"res_ is nan");

}

void Delta_FrictionalResistance_HeelItem::printWhoAmI() {
	std::cout<<"--> WhoAmI of Delta_FrictionalResistance_HeelItem "<<std::endl;
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

	double rN = pParser_->get("CHMEK") * V_ / Physic::ni_w;
	double cf = 0.075 / std::pow((std::log10(rN) - 2),2);
	double rfk= 0.5 * Physic::rho_w * V_ * V_ * pParser_->get("SK") * cf;

	// todo dtrimarchi : this form factor can be computed from the
	// Keel geometry (see DSYHS99) Ch.3.2.11
	res_ = rfk * pParser_->get("KEELFF");
	if(isnan(res_)) throw VPPException(HERE,"res_ is nan");

}

void ViscousResistanceKeelItem::printWhoAmI() {
	std::cout<<"--> WhoAmI of ViscousResistanceKeelItem "<<std::endl;
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

	double rN = pParser_->get("CHMEK") * V_ / Physic::ni_w;
	double cf = 0.075 / std::pow((std::log10(rN) - 2),2);
	double rfr= 0.5 * Physic::rho_w * V_ * V_ * pParser_->get("SK") * cf;

	// todo dtrimarchi : this form factor can be computed from the
	// Rudder geometry (see DSYHS99) Ch.3.2.11
	res_ = rfr * pParser_->get("RUDDFF");
	if(isnan(res_)) throw VPPException(HERE,"res_ is nan");

}

void ViscousResistanceRudderItem::printWhoAmI() {
	std::cout<<"--> WhoAmI of ViscousResistanceRudderItem "<<std::endl;
}


