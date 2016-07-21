
#include <iostream>
#include "VPPTest.h"
#include <cppunit/ui/text/TestRunner.h>

using namespace std;

int main(int argc, char *argv[])
{
  // Add "--stop" in function call to attach a debugger
  if (argc > 1) {
    if (std::string(argv[1]) == "--stop") {
      std::cout << "Waiting...press a key to continue\n";
      std::cin.ignore();
    }
  }

  CppUnit::TextUi::TestRunner runner;
  runner.addTest(TVPPTest::suite());
  int returnValue = !runner.run();

  return returnValue;
}

