
#ifndef VPP_TEST_H
#define VPP_TEST_H

// Unit test includes
#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include <cppunit/extensions/HelperMacros.h>

class TVPPTest : public CppUnit::TestFixture{

  CPPUNIT_TEST_SUITE(TVPPTest);

  /// Resistance component tests
  CPPUNIT_TEST(resistanceTest);

  /// Test the regression algorithm
  CPPUNIT_TEST(regressionTest);

  CPPUNIT_TEST_SUITE_END();

public:

  /// Resistance components test
  void resistanceTest();

  /// Test the regression algorithm
  void regressionTest();

};

#endif /* VPP_TEST_H */
