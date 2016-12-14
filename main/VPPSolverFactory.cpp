#include "VPPSolverFactory.h"

// Ctor
VPPSolverFactory::VPPSolverFactory(boost::shared_ptr<VPPItemFactory> pVppItems) :
	pVppItems_(pVppItems) {

}

// Disallowed default constructor
VPPSolverFactory::VPPSolverFactory() {

}

// Dtor
VPPSolverFactory::~VPPSolverFactory() {

}

//////////////////////////////////////////////////////////////

// Ctor
IppOptSolverFactory::IppOptSolverFactory(boost::shared_ptr<VPPItemFactory> pVppItems) :
		VPPSolverFactory(pVppItems),
		pSolver_( new VPP_NLP(pVppItems)),
		pApp_(IpoptApplicationFactory()) {

	pApp_->Options()->SetNumericValue("tol", 1e-3);
	pApp_->Options()->SetStringValue("mu_strategy", "adaptive");
	pApp_->Options()->SetStringValue("output_file", "ipopt.out");
	pApp_->Options()->SetStringValue("hessian_approximation", "limited-memory");

	// Call method VPP_NLP::get_scaling_parameters which is used to set the pb to
	// maximization and eventually to improve the conditioning of the pb
	pApp_->Options()->SetStringValue("nlp_scaling_method", "user-scaling");

	// Set ipOpt verbosity
	pApp_->Options()->SetIntegerValue("print_level",5);
	pApp_->Options()->SetIntegerValue("file_print_level", 5);

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
	else
		throw VPPException(HERE,"ipOpt failed to find the solution!");

}


