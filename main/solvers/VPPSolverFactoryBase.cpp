#include "VPPSolverFactoryBase.h"

namespace Optim {

// Ctor
VPPSolverFactoryBase::VPPSolverFactoryBase(boost::shared_ptr<VPPItemFactory> pVppItems) :
	pVppItems_(pVppItems) {

}

// Disallowed default constructor
VPPSolverFactoryBase::VPPSolverFactoryBase() {

}

// Dtor
VPPSolverFactoryBase::~VPPSolverFactoryBase() {

}

//////////////////////////////////////////////////////////////

// Ctor
SolverFactory::SolverFactory(boost::shared_ptr<VPPItemFactory> pVppItems) :
				VPPSolverFactoryBase(pVppItems),
				pSolver_(new VPPSolver(pVppItems) ){

}

// Virtual Dtor
SolverFactory::~SolverFactory() {

}

// Implement pure virtual declared in the mother class
// Returns a reference to the underlying problem representation
VPPSolver* SolverFactory::get() const {
	return pSolver_.get();
}

// Implement pure virtual used to execute a VPP-like analysis
// simply call the run of the solver
void SolverFactory::run(int TWV, int TWA) {
	pSolver_->run(TWV,TWA);
}

//////////////////////////////////////////////////////////////

// Ctor
NLOptSolverFactory::NLOptSolverFactory(boost::shared_ptr<VPPItemFactory> pVppItems) :
		VPPSolverFactoryBase(pVppItems),
		pSolver_(new NLOptSolver(pVppItems) ){
}

// Virtual Dtor
NLOptSolverFactory::~NLOptSolverFactory() {

}

// Implement pure virtual declared in the mother class
// Returns a reference to the underlying problem representation
NLOptSolver* NLOptSolverFactory::get() const {
	return pSolver_.get();
}

// Implement pure virtual used to execute a VPP-like analysis
void NLOptSolverFactory::run(int TWV, int TWA) {
	pSolver_->run(TWV,TWA);
}

//////////////////////////////////////////////////////////////

// Ctor
SAOASolverFactory::SAOASolverFactory(boost::shared_ptr<VPPItemFactory> pVppItems) :
				VPPSolverFactoryBase(pVppItems),
				pSolver_(new SemiAnalyticalOptimizer(pVppItems) ) {

}

// Virtual Dtor
SAOASolverFactory::~SAOASolverFactory() {

}

// Implement pure virtual declared in the mother class
// Returns a reference to the underlying problem representation
SemiAnalyticalOptimizer* SAOASolverFactory::get() const {
	return pSolver_.get();
}

// Implement pure virtual used to execute a VPP-like analysis
void SAOASolverFactory::run(int TWV, int TWA) {
	pSolver_->run(TWV,TWA);
}

//////////////////////////////////////////////////////////////

// Ctor
IppOptSolverFactory::IppOptSolverFactory(boost::shared_ptr<VPPItemFactory> pVppItems) :
		VPPSolverFactoryBase(pVppItems),
		pApp_(IpoptApplicationFactory()),
		pSolver_(new VPP_NLP(pVppItems)) {

	pApp_->Options()->SetNumericValue("tol", 1e-3);
	pApp_->Options()->SetStringValue("mu_strategy", "adaptive");
	pApp_->Options()->SetStringValue("output_file", "ipopt.out");
	pApp_->Options()->SetStringValue("hessian_approximation", "limited-memory");

	// Call method VPP_NLP::get_scaling_parameters which is used to set the pb to
	// maximization and eventually to improve the conditioning of the pb
	pApp_->Options()->SetStringValue("nlp_scaling_method", "user-scaling");

	// Set ipOpt verbosity
	pApp_->Options()->SetIntegerValue("print_level",0);
	pApp_->Options()->SetIntegerValue("file_print_level", 0);

	if (pApp_->Initialize() != Solve_Succeeded)
		throw VPPException(HERE,"Error during initialization of ipOpt!");

}

// Dtor
IppOptSolverFactory::~IppOptSolverFactory() {

}

// Dereference the smart ptr and return a reference
// to the underlying problem representation
VPP_NLP* IppOptSolverFactory::get() const {
	return &(*pSolver_);
}

// Implement pure virtual used to execute a VPP-like analysis
void IppOptSolverFactory::run(int vTW, int aTW) {

	// Set the wind indexes
	pSolver_->run(vTW,aTW);

	ApplicationReturnStatus status= pApp_->OptimizeTNLP(pSolver_);

	if (status == Solve_Succeeded) {
		std::cout << std::endl << std::endl << "*** The problem solved!" << std::endl;
	}
	else {
		throw NonConvergedException(HERE,"ipOpt failed to find the solution!");
	}
}

} // End namespace Optim
