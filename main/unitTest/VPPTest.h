
#ifndef VPP_TEST_H
#define VPP_TEST_H

// Unit test includes
#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include <cppunit/extensions/HelperMacros.h>

class TVPPTest : public CppUnit::TestFixture{

  CPPUNIT_TEST_SUITE(TVPPTest);

  /// Resistance and aero force components test
  CPPUNIT_TEST(itemComponentTest);

  /// Test the regression algorithm
  CPPUNIT_TEST(regressionTest);

  /// Test the Newton-Raphson algorithm
  CPPUNIT_TEST(newtonRaphsonTest);

  /// Test NLOpt -- mma algorithm
  CPPUNIT_TEST(nlOptTest_mma);

  /// Test NLOpt -- cobyla algorithm
  CPPUNIT_TEST(nlOptTest_cobyla);

  CPPUNIT_TEST_SUITE_END();

public:

  /// Resistance and aero force components test
  void itemComponentTest();

  /// Test the regression algorithm
  void regressionTest();

  /// Test the Newton-Raphson algorithm
  void newtonRaphsonTest();

  /// Test NLOpt
  void nlOptTest_mma();

  /// Test NLOpt -- cobyla algorithm
  void nlOptTest_cobyla();

};

#endif /* VPP_TEST_H */
