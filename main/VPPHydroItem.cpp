#include "VPPHydroItem.h"
#include "Warning.h"

// Constructor
ResistanceItem::ResistanceItem(VariableFileParser* pParser, boost::shared_ptr<SailSet> pSailSet) :
	VPPItem(pParser,pSailSet),
	fN_(0) {
	// make nothing
}

// drag the pure virtual method of the parent class one step down
void ResistanceItem::update(int vTW, int aTW) {

	// Update the Froude number using the state variable boat velocity
	fN_= fabs(V_) / sqrt(Physic::g * pParser_->get("LWL"));

	if(fN_ > 0.6)
		Warning warning("The value of the Froude Number exceeds the limit value 0.6! "
				"Results might be incorrect.");

}

// Destructor
ResistanceItem::~ResistanceItem() {

}

//=========================================================

// Constructor
InducedResistanceItem::InducedResistanceItem(AeroForcesItem* pAeroForcesItem) :
		ResistanceItem(pAeroForcesItem->getParser(), pAeroForcesItem->getSailSet()),
		pAeroForcesItem_(pAeroForcesItem),
		ri_(0) {

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
  vectA_.resize(16);
  vectA_ << tCan/t, (tCan/t)*(tCan/t), bwl/tCan, chtpk/chrtk;

  // coeffA(4x4) * vectA'(4x1) = Tegeo(4x1)
  Tegeo_ = coeffA_.matrix() * vectA_.matrix().transpose();

}

// Destructor
InducedResistanceItem::~InducedResistanceItem() {

}

// Implement pure virtual method of the parent class
void InducedResistanceItem::update(int vTW, int aTW) {

	// Call the parent class update to update the Froude number
	ResistanceItem::update(vTW,aTW);

  Eigen::ArrayXd vectB(2);
  vectB << 1, fN_;

  // coeffB(4x2) * vectB'(2x1) => TeFn(4x1)
  // todo dtrimarchi - WARNING : is this going to do the right
  // operation or shall we be using MatrixXd??
  Eigen::ArrayXd TeFn = coeffB_.matrix() * vectB.matrix().transpose();

  // Note that this is a coefficient-wise operation Tegeo(4x1) * TeFn(4x1) -> TeD(4x1)
  Eigen::ArrayXd TeD = pParser_->get("T") * Tegeo_ * TeFn;

  // Properly interpolate then values of TeD for the current value
  // of the state variable PHI_ (heeling angle)
  SplineInterpolator interpolator(phiD_,TeD);
  double Te= interpolator.interpolate(PHI_);

  // Get the aerodynamic side force
  double fHeel= pAeroForcesItem_->getFSide();

  // Compute the induced resistance Ri = Fheel^2 / (0.5 * pi * rho_w * Te^2 * V^2)
  ri_ = fHeel * fHeel / ( 0.5 * Physic::rho_w * M_PI * Te * Te * V_ * V_);

}

//=================================================================

// Constructor
ResiduaryResistanceItem::ResiduaryResistanceItem(VariableFileParser* pParser, boost::shared_ptr<SailSet> pSailSet):
		ResistanceItem(pParser, pSailSet),
		rrh_(0) {

	// Define an array of coefficients and instantiate an interpolator over it
	Eigen::MatrixXd coeff(11,9);
	coeff << 	-0.0014,  0.0403,  0.0470, -0.0227, -0.0119,  0.0061, -0.0086, -0.0307, -0.0553,
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

	//  vect = [geom.LWL./geom.DIVCAN^(1/3),
	//   				geom.XFB./geom.LWL,
	//   				geom.CPL,
	//   				geom.DIVCAN.^(2/3)./geom.AW,
	//   				geom.BWL./geom.LWL,
	//   				geom.DIVCAN.^(2/3)./geom.SC,
	//   				geom.XFB./geom.XFF,
	//   				(geom.XFB./geom.LWL).^2,
	//   				geom.CPL.^2]';
	/// Vector with the physical quantities multiplying the
	/// polynomial coefficients
	Eigen::VectorXd vect(9);
	vect << pParser_->get("LWL") / std::pow(pParser_->get("DIVCAN"),1./3),
					pParser_->get("XBF") / pParser_->get("LWL") ,
					pParser_->get("CPL"),
					std::pow(pParser_->get("DIVCAN"),2./3) / pParser_->get("AW"),
					pParser_->get("BWL") / pParser_->get("LWL"),
					std::pow(pParser_->get("DIVCAN"),2./3) / pParser_->get("SC"),
					pParser_->get("XFB") / pParser_->get("XFF"),
					std::pow(pParser_->get("XFB") / pParser_->get("LWL"),2),
					std::pow(pParser_->get("CPL"),2);

	// Compute the the residuary resistance for all froude numbers
	//  RrhD = (geom.DIVCAN.*phys.g.*phys.rho_w) .* ( coeff * vect' ) .* (geom.DIVCAN.^(1/3)./geom.LWL);
	//  11x1        SCALAR                            11x9  *  9x1            SCALAR

	// Residuary resistance values for each Froude number
	Eigen::VectorXd RrhD = 	(pParser_->get("DIVCAN") * Physic::g * Physic::rho_w) *
													( std::pow(pParser_->get("DIVCAN"),1./3) / pParser_->get("LWL") ) *
													coeff * vect.transpose() ;

	// Values of the froude number on which the coefficients of the
	// residuary resistance are computed
	Eigen::ArrayXd fnD(11);
	fnD << 	.1, .15, .2, .25, .3, .35, .4, .45, .5, .55, .6;

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
	rrh_ = pInterpolator_->interpolate(fN_);

}

