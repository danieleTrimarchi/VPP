
#ifndef VPP_TEST_H
#define VPP_TEST_H

// Unit test includes
#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include <cppunit/extensions/HelperMacros.h>

namespace Test {

class TVPPTest : public CppUnit::TestFixture{

  CPPUNIT_TEST_SUITE(TVPPTest);

  /// Test the variables parsed in the variablefile
  CPPUNIT_TEST(variableParseTest);

  /// Instantiate some items, write to xml, read them
  /// back and verify consistency
  CPPUNIT_TEST(xmlIOTest);

  /// Test the variables we get when reading xml in the variablefile

  //
//  /// Resistance and aero force components test
//  CPPUNIT_TEST(itemComponentTest);
//
//  /// Test the regression algorithm
//  CPPUNIT_TEST(regressionTest);
//
//  /// Test the computation of the Jacobian matrix
//  CPPUNIT_TEST(jacobianTest);
//
//  /// Test the computation of the Gradient vector
//  CPPUNIT_TEST(gradientTest);
//
//  /// Test the Newton-Raphson algorithm
//  CPPUNIT_TEST(newtonRaphsonTest);
//
//  /// Test NLOpt -- mma algorithm
//  CPPUNIT_TEST(nlOptTest_mma);
//
//  /// Test NLOpt -- cobyla algorithm
//  CPPUNIT_TEST(nlOptTest_cobyla);
//
//  /// Test NLOpt -- ISRES "Improved Stochastic Ranking Evolution Strategy" algorithm.
//	/// Global optimization algorithm with non-linear equality constraints
//  CPPUNIT_TEST(runISRESTest);
//
//  /// Test NLOpt -- ISRES "Improved Stochastic Ranking Evolution Strategy" algorithm.
//	/// Global optimization algorithm with non-linear equality constraints. Here in then example g06
//  CPPUNIT_TEST(runISRESTest_g06);
//
//  /// Test ipOpt -- from example HS071_NLP
//  CPPUNIT_TEST(ipOptTest);
//
//  /// Test a run on a complete computation point : initial guess, NR and solution with NLOpt
//  CPPUNIT_TEST(vppPointTest);
//
//  /// Make a full run of ipOpt, and compare the full results with a baseline
//  /// Warning : this is a relatively long test
//  CPPUNIT_TEST(ipOptFullRunTest);
//
//  /// Test the SmoothedStepFunction values
//  CPPUNIT_TEST(smoothedTestFunctionTest);
//
//  /// Test the VPPResultIO utility : instantiate results, write them to file,
//  /// read them back and check if the values are unchanged
//  CPPUNIT_TEST(vppResultIOTest);

  CPPUNIT_TEST_SUITE_END();

public:

  /// Test the variables parsed in the variable file
  void variableParseTest();

  /// Instantiate some items, write to xml, read them
  /// back and verify consistency
  void xmlIOTest();

  /// Test the variables we get when reading xml in the variablefile

  /// Resistance and aero force components test
  void itemComponentTest();

  /// Test the regression algorithm
  void regressionTest();

  /// Test the computation of the Jacobian matrix
  void jacobianTest();

  /// Test the computation of the Gradient vector
  void gradientTest();

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

  /// Test ipOpt -- from example HS071_NLP
  void ipOptTest();

  /// Test a run on a complete computation point : initial guess, NR and solution with NLOpt
  void vppPointTest();

  /// Make a full run, and compare the full results with a baseline
  void ipOptFullRunTest();

  /// Test the SmoothedStepFunction values
  void smoothedTestFunctionTest();

  /// Test the VPPResultIO utility : instantiate results, write them to file,
  /// read them back and check if the values are unchanged
  void vppResultIOTest();

};
}; // namespace Test

#endif /* VPP_TEST_H */
