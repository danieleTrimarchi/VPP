#include "VPPHydroItem.h"

// Constructor
ResistanceItem::ResistanceItem(VariableFileParser* pParser, boost::shared_ptr<SailSet> pSailSet) :
	VPPItem(pParser,pSailSet) {
	// make nothing
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

	// Get the Froude number using the state variable boat velocity
	double Fn= fabs(V_) / sqrt(Physic::g * pParser_->get("LWL"));

  if(Fn > 0.6)
  	Fn = 0.6; // todo dtrimarchi: understand this comment:
  						// this is quite a rough approximation, but
  						// over Fn 0.6 everything is roughly approximated...
  						// and this prevent Te to become negative!

  Eigen::ArrayXd vectB(2);
  vectB << 1, Fn;

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


