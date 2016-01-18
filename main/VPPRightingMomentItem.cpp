#include "VppRightingMomentItem.h"
#include "Physics.h"
#include "mathUtils.h"
using namespace mathUtils;

// Constructor
RightingMomentItem::RightingMomentItem(VariableFileParser* pParser, boost::shared_ptr<SailSet> sailSet):
		VPPItem(pParser, sailSet),
		val_(0) {

	m10_= Physic::rho_w * Physic::g *
				(pParser_->get("KM") - pParser_->get("KG")) *
				(pParser_->get("DIVCAN") + pParser_->get("DVK"));

	m20_= pParser_->get("MMVBLCRW") * Physic::g;

}

// Destructor
RightingMomentItem::~RightingMomentItem() {

}

// Get the righting moment value
const double RightingMomentItem::get() const {
	return val_;
}

// Implement the pure virtual update
void RightingMomentItem::update(int vTW, int aTW) {

	// Compute the righting moment
	//M1 = phys.rho_w * phys.g * (geom.KM - geom.KG) * (geom.DIVCAN + geom.DVK) * sin(phi*pi/180);
	//M2 = geom.MMVBLCRW .* phys.g .* b .* cos(phi*pi/180);
	//Mright = M1+M2;
	val_ = m10_ * std::sin( toRad(PHI_)) + m20_ * b_ * std::cos( toRad(PHI_)) ;

	if(isnan(val_)){
		std::cout<<"Righting moment is NAN"<<std::endl;
		throw logic_error("Righting moment is NAN");
	}
}

// Print the class name -> in this case SailCoefficientItem
void RightingMomentItem::printWhoAmI() {
	std::cout<<"--> WhoAmI of VppRightingMomentItem "<<std::endl;
}