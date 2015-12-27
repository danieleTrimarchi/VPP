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

// ------------------------
// Plotting utility
#include "Plotter.h"

// ------------------------
// Directives for NLOPT
#include <math.h>
#include <nlopt.h>

// Objective function
double myfunc(unsigned n, const double *x, double *grad, void *my_func_data)
{
    if (grad) {
        grad[0] = 0.0;
        grad[1] = 0.5 / sqrt(x[1]);
    }
    return sqrt(x[1]);
}

typedef struct {
    double a, b;
} my_constraint_data;

// Constraint function
double myconstraint(unsigned n, const double *x, double *grad, void *data)
{
    my_constraint_data *d = (my_constraint_data *) data;
    double a = d->a, b = d->b;
    if (grad) {
        grad[0] = 3 * a * (a*x[0] + b) * (a*x[0] + b);
        grad[1] = -1.0;
    }
    return ((a*x[0] + b) * (a*x[0] + b) * (a*x[0] + b) - x[1]);
 }


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
	
	
    printf("\n-- BOOST TESTS -----------\n");
    boost::shared_ptr<int> p(new int);


    printf("\n-- PLOTTER TESTS -----------\n");
    // Instantiate a plotter
    Plotter plotter;

    // plot a XY test plot
    plotter.plotXY();

    // plot a polar test plot
    plotter.plotPolar();

    printf("\n-- NLOPT TESTS -----------\n");

    double lb[2] = { -HUGE_VAL, 0 }; /* lower bounds */
    nlopt_opt opt;
    opt = nlopt_create(NLOPT_LD_MMA, 2); /* algorithm and dimensionality */
    nlopt_set_lower_bounds(opt, lb);
    nlopt_set_min_objective(opt, myfunc, NULL);
    my_constraint_data data[2] = { {2,0}, {-1,1} };
    nlopt_add_inequality_constraint(opt, myconstraint, &data[0], 1e-8);
    nlopt_add_inequality_constraint(opt, myconstraint, &data[1], 1e-8);
    nlopt_set_xtol_rel(opt, 1e-4);
    double xp[2] = { 1.234, 5.678 };  /* some initial guess */
    double minf; /* the minimum objective value, upon return */
    if (nlopt_optimize(opt, xp, &minf) < 0) {
        printf("nlopt failed!\n");
    }
    else {
        printf("found minimum at f(%g,%g) = %0.10g\n", xp[0], xp[1], minf);
    }
    nlopt_destroy(opt);

    // BUILD A PROJECT TREE WITH SRC etc..

    // MAKE A CPPUNIT TEST SUITE AND BUILD A SPECIFIC TARGET IN SCONSTRUCT
	
    // PLACE ALL PREVIOUS TESTS IN CPPUNIT TESTS

    return (0);
	
}
