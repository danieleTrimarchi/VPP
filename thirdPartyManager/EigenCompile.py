from thirdPartyCompile import thirdPartyCompile
import os, shutil, sys

class EigenCompile(thirdPartyCompile):
    
    # Ctor
    def __init__(self):
            
        # Simply call mother-class init
        super(EigenCompile,self).__init__()

        # Name of this third_party. 
        self.__name__="Eigen"
        
        # Version of this third_party
        self.__version__="3.3.5"
        
        # Define the URL from which cppUnit can be downloaded
        self.__url__="http://bitbucket.org/eigen/eigen/get/3.3.5.tar.gz"

        # Define the name of the archive downloadeed from the web.
        self.__srcArchiveName__="eigen-eigen-b3f3d4950030.tar"
        
        # Define the name of the folder extracted from the archive downloadeed from the web. 
        # A priori this is simply the srcArchiveName without the extension
        self.__srcDirName__="eigen-eigen-b3f3d4950030"
        
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
        self.__buildInfo__["LIBS"] = []
            
#     # Download an archive from a given url and unzip it 
#     # Overwrite the method defined in thirdPartyCompile 
#     # to specialize this for the tar.bz2 format to be downloaded
#     # from sourgeforce. At some point this needs to be refactored 
#     def __getCompressedArchive__(self,url,saveAs="myArch"):
# 
#        # download the archive
#        self.__execute__("curl {} -L -k -o {}".format(self.__url__,self.__srcDirName__))
# 
#        # Then unzip and go...  for a tar.bz2...
#        import tarfile
#        tar= tarfile.open(mode='r|bz2', fileobj=self.__srcDirName__)
#        tar.extractall()
#        tar.close()

    # Compile this package    
    def __compile__(self,dest=None):
    
        # Eigen does not require to be compiled!     
        pass     

    # Package the third party that was build   
    def __package__(self):
                
        # Write the build info to file. This will be used
        # by the build system to compile and link the program
        self.__writeInfo__()   

        # cleanup: remove a previous package folder if present. Remember that the 
        # package folder was set relative to this third_party in the ctor
        shutil.rmtree(self.__thirdPartyPkgFolder__,sys.exc_info())

        # Make a package folder and enter it 
        os.mkdir(self.__thirdPartyPkgFolder__)
        
         Compile with CMAKE to get the docs

         Copy the 'Eigen' folder to the include 
        
        
    # Run a simple test to check that the compile was successiful
    def __test__(self):
        
        TODO
        
        exampleFolder= os.path.join(self.__thirdPartyPkgFolder__,"Cpp_example")

        # cleanup
        shutil.rmtree(exampleFolder)
        os.mkdir(exampleFolder)
        
        # Go to the example folder
        os.chdir(exampleFolder)

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
             
        # Write a SConstruct
        Sconstruct=open("SConstruct","w")
        Sconstruct.write('''import os
env = Environment()  
env.Append( CPPPATH=["{}"] )
env.Append( LIBPATH=["{}"] )
env.Append( LIBS={} )
env.Program('cppUnitTest', Glob('*.cpp') )        
'''.format(self.__buildInfo__["INCLUDEPATH"],
            self.__buildInfo__["LIBPATH"],
            self.__buildInfo__["LIBS"]))
        Sconstruct.close()
                         
        # Compile the example
        self.__execute__("scons -Q")
         
#         # Before execution, add symbolic links to the dylibs. Why cannot I 
#         # just set LD_LIBRARY_PATH..? Weird. Looks like MacOS security stuff.
#         for iLib in self.__buildInfo__["LIBS"]:
#             os.symlink(os.path.join(self.__buildInfo__["LIBPATH"],self.getFullDynamicLibName(iLib)), 
#                        self.getFullDynamicLibName(iLib) )
#         
        # Execute the example
        self.__execute__("./cppUnitTest {}".format(os.getcwd()))        

#     # Import the dynamic libraries from third party to the dest folder (in this case
#     # this will be in the app bundle VPP.app/Contents/Frameworks/
#     def importDynamicLibs(self,dst):
#     
#         # Copy the lib to the dest folder
#         for iLib in self.__buildInfo__["LIBS"]: 
#             shutil.copyfile(self.getFullDynamicLibName(iLib), 
#                             os.path.join(dst,self.getFullDynamicLibName(iLib)))
#                     
        