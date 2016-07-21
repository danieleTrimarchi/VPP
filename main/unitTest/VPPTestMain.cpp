
#include <iostream>

using namespace std;
struct InitHook {
  InitHook() {
    cout << "Waiting...";
    cin.ignore();
  }
};

// Add this line to make the test stop and wait for a key press
//InitHook init;

#include "VPPTest.h"
#include <cppunit/ui/text/TestRunner.h>

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

