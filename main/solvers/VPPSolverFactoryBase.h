#ifndef VPP_SOLVER_FACTORY
#define VPP_SOLVER_FACTORY

#include "VPPItemFactory.h"

#include "VPPSolver.h"

// NLOpt integration
#include "NLOptSolver.h"
#include "SemiAnalyticalOptimizer.h"

/// ipOpt integration
#include "IpIpoptApplication.hpp"
#include "VPP_nlp.h"

//using namespace Ipopt;
//using namespace VPPSolve;

namespace Optim {

/// Factory class in charge for the instantiation
/// and storage of a solver / optimizer for the
/// solution of the VPP problem.
/// The class was made necessary when building the ipOpt
/// integration, in order to create a unique container
/// storing the problem representation (VPP_nlp) and the
/// solving application (IpoptApplication). Other solvers/
/// optimizers are also instantiated from here.
class VPPSolverFactoryBase {

	public:

		/// Ctor
		VPPSolverFactoryBase(std::shared_ptr<VPPItemFactory>);

		/// Virtual Dtor
		virtual ~VPPSolverFactoryBase();

		/// Returns a reference to the underlying problem representation
		virtual VPPSolverBase* get() const =0;

		/// Pure virtual used to execute a VPP-like analysis
		virtual void run(int TWV, int TWA) =0;

	protected:

		/// Disallow default constructor
		VPPSolverFactoryBase();

		/// Ptr to the item factory
		std::shared_ptr<VPPItemFactory> pVppItems_;

};

//////////////////////////////////////////////////////////////

/// Wraps up the solver, that wraps up a NR solver with no optimization
class SolverFactory : public VPPSolverFactoryBase {

	public:

		/// Ctor
		SolverFactory(std::shared_ptr<VPPItemFactory>);

		/// Virtual Dtor
		virtual ~SolverFactory();

		/// Implement pure virtual declared in the mother class
		/// Returns a reference to the underlying problem representation
		VPPSolver* get() const;

		/// Implement pure virtual used to execute a VPP-like analysis
		virtual void run(int TWV, int TWA);

	private:

		/// Ptr to the problem representation
		std::shared_ptr<VPPSolver> pSolver_;

};

//////////////////////////////////////////////////////////////

/// Wraps up the nlOpt optimizer
class NLOptSolverFactory : public VPPSolverFactoryBase {

	public:

		/// Ctor
		NLOptSolverFactory(std::shared_ptr<VPPItemFactory>);

		/// Virtual Dtor
		virtual ~NLOptSolverFactory();

		/// Implement pure virtual declared in the mother class
		/// Returns a reference to the underlying problem representation
		NLOptSolver* get() const;

		/// Implement pure virtual used to execute a VPP-like analysis
		virtual void run(int TWV, int TWA);

	private:

		/// Ptr to the problem representation
		std::shared_ptr<NLOptSolver> pSolver_;

};


//////////////////////////////////////////////////////////////

/// Wraps up the nlOpt optimizer
class SAOASolverFactory : public VPPSolverFactoryBase {

	public:

		/// Ctor
		SAOASolverFactory(std::shared_ptr<VPPItemFactory>);

		/// Virtual Dtor
		virtual ~SAOASolverFactory();

		/// Implement pure virtual declared in the mother class
		/// Returns a reference to the underlying problem representation
		SemiAnalyticalOptimizer* get() const;

		/// Implement pure virtual used to execute a VPP-like analysis
		virtual void run(int TWV, int TWA);

	private:

		/// Ptr to the problem representation
		std::shared_ptr<SemiAnalyticalOptimizer> pSolver_;

};

//////////////////////////////////////////////////////////////

/// Wraps up the ipOpt integration
class IppOptSolverFactory : public VPPSolverFactoryBase {

	public:

		/// Ctor
		IppOptSolverFactory(std::shared_ptr<VPPItemFactory>);

		/// Virtual Dtor
		virtual ~IppOptSolverFactory();

		/// Implement pure virtual declared in the mother class
		/// Returns a reference to the underlying problem representation
		VPP_NLP* get() const;

		/// Implement pure virtual used to execute a VPP-like analysis
		virtual void run(int TWV, int TWA);

	private:

		/// Ptr to the problem representation
		SmartPtr<VPP_NLP> pSolver_;

		/// Ptr to the ipOpt solver
		SmartPtr<IpoptApplication> pApp_;

};
} // End namespace optim

#endif

