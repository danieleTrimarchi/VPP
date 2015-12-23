#include <stdio.h>
#include <iostream>
#include <fstream>
using namespace std;

// ------------------------
// Directives for EIGEN
#include <Eigen/Core>
using namespace Eigen;

// ------------------------
// Directives for UMFPACK
#include <umfpack.h>

// ------------------------
// Directives for BOOST
#include "boost/shared_ptr.hpp"

// Plotting utility
#include "Plotter.h"

// ------------------------
// Directives for OPT++
//
//#include "OPT++_config.h"
//#include "newmat.h"
//#include "NLF.h"
//#include "NLP.h"
//#include "OptQNewton.h"
//
//using NEWMAT::ColumnVector;
//using namespace OPTPP;
//
//// The following two lines serve as the declarations of the pointers to the subroutines that initialize the problem and evaluate the objective function, respectively
//void init_rosen(int ndim, ColumnVector& x){
// if (ndim != 2)
//    exit (1);
//
// // ColumnVectors are indexed from 1, and they use parentheses around
// // the index.
//
//  x(1) = -1.2;
//  x(2) =  1.0;
//}
//
//void rosen(int ndim, const ColumnVector& x, double& fx, int& result){
//
//  double f1, f2, x1, x2;
//
//  if (ndim != 2)
//    exit (1);
//
//  x1 = x(1);
//  x2 = x(2);
//  f1 = (x2 - x1 * x1);
//  f2 = 1. - x1;
//
//  fx  = 100.* f1*f1 + f2*f2;
//  result = NLPFunction;
//}

// MAIN
int main(int argc, const char *argv[]) {

    // TRIVIAL TEST
    printf("\nHello, world!\n");
    printf("\n-- EIGEN TEST -----------\n");
	
    //EIGEN TEST
    int rows=5, cols=5;
    MatrixXf m(rows,cols);
    m << (Matrix3f() << 1, 2, 3, 4, 5, 6, 7, 8, 9).finished(),
    MatrixXf::Zero(3.,cols-3),
    MatrixXf::Zero(rows-3,3),
    MatrixXf::Identity(rows-3,cols-3);
    cout << m << endl;
    printf("-------------\n");


    printf("\n-- UMFPACK TESTS -----------\n");
    int n=5; 
    int	Ap[]={0,2,5,9,10,12};
    int	Ai[]={0, 1, 0, 2, 4, 1, 2, 3, 4, 2, 1, 4};
    double Ax [ ] = {2., 3., 3., -1., 4., 4., -3., 1., 2., 2., 6., 1.}; 
    double b [ ] = {8., 45., -3., 3., 19.}; 
    double x [5];
	
    double* null = (double *) NULL ;
    int i ; 
    void* Symbolic, *Numeric ;
    (void) umfpack_di_symbolic (n, n, Ap, Ai, Ax, &Symbolic, null, null) ; 
    (void) umfpack_di_numeric (Ap, Ai, Ax, Symbolic, &Numeric, null, null) ; 
    umfpack_di_free_symbolic (&Symbolic) ;
    (void) umfpack_di_solve (UMFPACK_A, Ap, Ai, Ax, x, b, Numeric, null, null) ; 
    umfpack_di_free_numeric (&Numeric) ; 
    for (i = 0 ; i < n ; i++) printf ("x [%d] = %g\n", i, x [i]) ; 
    printf("-------------\n");
	

//    printf("\n-- OPT++ TESTS -- See output in example1.out -----------\n");
//
//    int ndim = 2;
//    FDNLF1 nlp(ndim, rosen, init_rosen);
//
//    OptQNewton objfcn(&nlp);
//
//    objfcn.setSearchStrategy(TrustRegion);
//    objfcn.setMaxFeval(200);
//    objfcn.setFcnTol(1.e-4);
//
//    // The "0" in the second argument says to create a new file.  A "1"
//    // would signify appending to an existing file.
//    if (!objfcn.setOutputFile("example1.out", 0))
//      cerr << "main: output file open failed" << endl;
//
//    objfcn.optimize();
//
//    objfcn.printStatus("Solution from quasi-newton");
//    objfcn.cleanup();
 
	
    printf("\n-- BOOST TESTS -----------\n");
    boost::shared_ptr<int> p(new int);

    Plotter plotter;
    plotter.plotXY();

    // BUILD A PROJECT TREE WITH SRC etc..

    // SEGREGATE PLPLOT AND OPT++, THAT SOMEHOW CONFLICT

    // MAKE A CPPUNIT TEST SUITE AND BUILD A SPECIFIC TARGET IN SCONSTRUCT
	
    // PLACE ALL PREVIOUS TESTS IN CPPUNIT TESTS

    // HOST THE SVN PROJECT TO A SERVER

    return (0);
	
}
