
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

  CPPUNIT_TEST_SUITE_END();

public:

  /// Resistance and aero force components test
  void itemComponentTest();

  /// Test the regression algorithm
  void regressionTest();

  /// Test the Newton-Raphson algorithm
  void newtonRaphsonTest();

};

#endif /* VPP_TEST_H */
