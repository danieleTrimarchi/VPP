from thirdPartyCompile import thirdPartyCompile
import os, shutil, sys

class CppUnitCompile(thirdPartyCompile):
    
    # Ctor
    def __init__(self):
            
        # Simply call mother-class init
        super(CppUnitCompile,self).__init__()

        # Name of this third_party. 
        self.__name__="CppUnit"
        
        # Version of this third_party
        self.__version__="1.13.2"
        
        # Define the URL from which cppUnit can be downloaded
        self.__url__="http://dev-www.libreoffice.org/src/cppunit-1.13.2.tar.gz"

        # Define the name of the archive downloadeed from the web.
        self.__srcArchiveName__="cppunit-1.13.2.tar.gz"
        
        # Define the name of the folder extracted from the archive downloadeed from the web. 
        # A priori this is simply the srcArchiveName without the extension
        self.__srcDirName__="cppunit-1.13.2"
        
        # Override (specialize) the build folder 
        self.__thirdPartyBuildFolder__= os.path.join(self.__thirdPartyBuildFolder__,self.__srcDirName__)

        # Override (specialize) the package folder 
        self.__thirdPartyPkgFolder__= os.path.join(self.__thirdPartyPkgFolder__,self.__srcDirName__)

        # Instantiate no requirements - Boost only has Python for BoostPython, which we do not want        

        # Define the build info. Will use these to copy the components (includes, libs...) to 
        # the package folder
        self.__buildInfo__["INCLUDEPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"include")]
        self.__buildInfo__["LIBPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"lib")]
        self.__buildInfo__["DOCPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"doc")]
        self.__buildInfo__["LIBS"] = ["cppunit"]
            
    # Compile this package    
    def __compile__(self,dest=None):
        
        # Decorates the mother class __compile__ method
        super(CppUnitCompile,self).__package__()
    
        # Make the build folder
        os.mkdir("Build")

        # Execute configure... Pre-define that the dest folder is the package
        self.__execute__("./configure --prefix={}".format(self.__thirdPartyPkgFolder__))
        self.__execute__("make")
        self.__execute__("make check")        

    # Package the third party that was build   
    def __package__(self):
                
        # Decorate the mother class __package__ method
        super(CppUnitCompile,self).__package__()

        # install to the package folder, as defined when configuring with the --prefix
        self.__execute__("make install")
        
    # Run a simple test to check that the compile was successiful
    def __test__(self):
        
        # Decorate the mother class __test__ method
        super(CppUnitCompile,self).__test__()

        # Write the cppunit example 
        Source=open("main.cpp","w")
        Source.write('''
#include <iostream>
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/ui/text/TestRunner.h>

using namespace std;

class Test : public CppUnit::TestFixture {

  CPPUNIT_TEST_SUITE(Test);

  /// Test the variables parsed in the variablefile
  CPPUNIT_TEST(testThisTest);

  CPPUNIT_TEST_SUITE_END();

public:

    void testThisTest() {
        CPPUNIT_ASSERT_EQUAL( 1., 1. );
    };
};

int main(int argc, char *argv[]) {

  CppUnit::TextUi::TestRunner runner;
  runner.addTest(Test::suite());
  int returnValue = !runner.run();

  return returnValue;
}''')
        Source.close()
       
        # Compile and run the test
        self.__makeTest__()
    
            