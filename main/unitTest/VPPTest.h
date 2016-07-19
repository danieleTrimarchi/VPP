
#ifndef VPP_TEST_H
#define VPP_TEST_H

// Unit test includes
#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include <cppunit/extensions/HelperMacros.h>

class TVPPTest : public CppUnit::TestFixture{

  CPPUNIT_TEST_SUITE(TVPPTest);

  /// Matrices test
  CPPUNIT_TEST(resistanceTest);

  CPPUNIT_TEST_SUITE_END();

public:

  /// Matrices test
  void resistanceTest();

};

#endif /* VPP_TEST_H */
