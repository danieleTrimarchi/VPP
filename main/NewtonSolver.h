#ifndef NEWTONSOLVER_H
#define NEWTONSOLVER_H

class NewtonSolver {

	public:

		/// Constructor
		NewtonSolver();

		/// Destructor
		~NewtonSolver();

		/// Solve
		void solve();

	private:

		/// Get the Numerical Jacobian for this problem
		void computeJacobian();

		/// stopping criterion
		double tol_;

		/// This Newton solver is specifically designed for the
		/// VPP with 4 variables. The Jacobian contains dF/dx,
		/// or:
		// F(x1,x2,x3,x4) = | dF_i/dx_j | * X
		// for i < X.size()
		//  	compute vector F0=F(X)
		//		perturb X(i) += eps;
		//  	compute vector F1=F(X)
		// 		J(:,i) = ( F1 - F0 ) / eps
		Eigen::Matrix4d jacobian_;

};

#endif
