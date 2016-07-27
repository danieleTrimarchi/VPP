
#ifndef VPP_TEST_H
#define VPP_TEST_H

// Unit test includes
#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include <cppunit/extensions/HelperMacros.h>

namespace Test {

class TVPPTest : public CppUnit::TestFixture{

  CPPUNIT_TEST_SUITE(TVPPTest);

  /// Resistance and aero force components test
  CPPUNIT_TEST(itemComponentTest);

  /// Test the regression algorithm
  CPPUNIT_TEST(regressionTest);

  /// Test the computation of the Jacobian matrix
  CPPUNIT_TEST(jacobianTest);

  /// Test the Newton-Raphson algorithm
  CPPUNIT_TEST(newtonRaphsonTest);

  /// Test NLOpt -- mma algorithm
  CPPUNIT_TEST(nlOptTest_mma);

  /// Test NLOpt -- cobyla algorithm
  CPPUNIT_TEST(nlOptTest_cobyla);

  /// Test NLOpt -- ISRES "Improved Stochastic Ranking Evolution Strategy" algorithm.
	/// Global optimization algorithm with non-linear equality constraints
  CPPUNIT_TEST(runISRESTest);

  /// Test NLOpt -- ISRES "Improved Stochastic Ranking Evolution Strategy" algorithm.
	/// Global optimization algorithm with non-linear equality constraints. Here in then example g06
  CPPUNIT_TEST(runISRESTest_g06);

  /// Test a run on a complete computation point : initial guess, NR and solution with NLOpt
  CPPUNIT_TEST(vppPointTest);

  CPPUNIT_TEST_SUITE_END();

public:

  /// Resistance and aero force components test
  void itemComponentTest();

  /// Test the regression algorithm
  void regressionTest();

  /// Test the computation of the Jacobian matrix
  void jacobianTest();

  /// Test the Newton-Raphson algorithm
  void newtonRaphsonTest();

  /// Test NLOpt
  void nlOptTest_mma();

  /// Test NLOpt -- cobyla algorithm
  void nlOptTest_cobyla();

  /// Test NLOpt -- ISRES "Improved Stochastic Ranking Evolution Strategy" algorithm.
	/// Global optimization algorithm with non-linear equality constraints
	void runISRESTest();

  /// Test NLOpt -- ISRES "Improved Stochastic Ranking Evolution Strategy" algorithm.
	/// Global optimization algorithm with non-linear equality constraints. Here in then example g06
  void runISRESTest_g06();

  /// Test a run on a complete computation point : initial guess, NR and solution with NLOpt
  void vppPointTest();

};
}; // namespace Test

#endif /* VPP_TEST_H */
