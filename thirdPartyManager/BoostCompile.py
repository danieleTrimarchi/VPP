from thirdPartyCompile import thirdPartyCompile
import os, shutil, sys

class BoostCompile(thirdPartyCompile):
    
    # Ctor
    def __init__(self):
        
        # Simply call mother-class init
        super(BoostCompile,self).__init__()

        # Name of this third_party. 
        self.__name__="Boost"
        
        # Version of this third_party
        self.__version__="1.68.0"
        
        # Define the URL from which IpOpt can be downloaded
        # Note that this should be the complete path of the file
        # to be downloaded (Including the filename!)
        self.__url__="https://dl.bintray.com/boostorg/release/1.68.0/source/boost_1_68_0.tar.gz"

        # Define the name of the archive downloadeed from the web. A priori this is 
        # simply the last entry of the url
        self.__srcArchiveName__="boost_1_68_0.tar"
        
        # Define the name of the folder extracted from the archive downloadeed from the web. 
        # A priori this is simply the srcArchiveName without the extension
        self.__srcDirName__="boost_1_68_0"
        
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
        self.__buildInfo__["LIBS"] = ["boost_system","boost_filesystem"]
        
  
    # Compile this package    
    def __compile__(self,dest=None):
        
        # Decorate the mother class __package__ method
        super(BoostCompile,self).__compile__()
    
        # Make the build folder
        os.mkdir("Build")

        # Execute bootstrap...
        self.__execute__("./bootstrap.sh "
                         "--with-libraries=filesystem,system")

        # Execute b2, which will build the requested libs
        self.__execute__("./b2")

        # Note, I could set the prefix and run 'b2 install'. This leads to a lot of
        # duplicated code. I do not want to create the package here, so I will copy
        # selected elements 'by hand' when packaging
        # The includes are already in ./boost
        # The compiled libss are in ./stage/lib
         

    def __copySelectedDocs__(self):
        
        # Loop on the modules we seek to compile 
        for iLib in self.__buildInfo__["LIBS"] :
            # Boost libs are named boost_something. Here we only seek the 'something'
            cleanName = iLib.replace("boost_","")

            # Verify the object we get is in the form we need            
            # and define the doc directory as pkgDir/doc/libDir
            checkListOfSizeOne(self.__buildInfo__["DOCPATH"],"DocPath")             
            
            # Make the doc directory
            docDir = os.path.join(self.__buildInfo__["DOCPATH"][0],cleanName)
            os.makedirs(docDir)

            # Copy the files to the doc directory
            shutil.copy(os.path.join(self.__thirdPartyBuildFolder__,"libs",cleanName,"index.html"), 
                    docDir)
            self.__copytree__(os.path.join(self.__thirdPartyBuildFolder__,"libs",cleanName,"doc"), 
                              os.path.join(docDir,"doc"))

    # Package the third party that was build   
    def __package__(self):
              
        # Decorate the mother class __package__ method
        super(BoostCompile,self).__package__()
                
        # Copy the content of include
        self.__copytree__(os.path.join(self.__thirdPartyBuildFolder__,"boost"), 
                          os.path.join(self.__buildInfo__["INCLUDEPATH"][0],"boost"))

        self.__copytree__(os.path.join(self.__thirdPartyBuildFolder__,"stage","lib"), 
                          self.__buildInfo__["LIBPATH"][0])

        # Also copy the documentation of the modules we are compiling
        self.__copySelectedDocs__()
                                      
    # Run a simple test to check that the compile was successiful
    def __test__(self):

        # Decorate the mother class __test__ method
        super(BoostCompile,self).__test__()

        # Write the boost example 
        Source=open("main.cpp","w")
        Source.write('''
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>
#include <boost/core/lightweight_test.hpp>
#include <cerrno>
using std::cout;
using namespace boost::filesystem;
using namespace boost::system;

static error_code e1( 1, system_category() );
static std::string m1 = e1.message();

static error_code e2( ENOENT, generic_category() );
static std::string m2 = e2.message();

int main(int argc, char** argv) {

    // Test instantiating a shared ptr
    boost::shared_ptr<int> myPtr; 

    // 
    path p (argv[1]);
    
    try {
        if (exists(p))
            cout<<\"Ok!\\n\";
    } catch (const filesystem_error& ex) {
        cout << ex.what() << \"\\n\";
    }
}       
''')
        Source.close()
            
        # Write a SConstruct 
        Sconstruct=open("SConstruct","w")
        Sconstruct.write('''import os
env = Environment()  
env.Append( CPPPATH=["{}"] )
env.Append( LIBPATH=["{}"] )
env.Append( LIBS={} )
env.Program('boostTest', Glob('*.cpp') )        
'''.format(self.__buildInfo__["INCLUDEPATH"][0],
           self.__buildInfo__["LIBPATH"][0],
           self.__buildInfo__["LIBS"]))
        Sconstruct.close()
                        
        # Compile the example
        self.__execute__("scons -Q")
        
        # Before execution, add symbolic links to the dylibs. Why cannot I 
        # just set LD_LIBRARY_PATH..? Weird. Looks like MacOS security stuff.
        #for iLib in self.__buildInfo__["LIBS"]:
        #    os.symlink(os.path.join(self.__buildInfo__["LIBPATH"],self.getFullDynamicLibName(iLib)), 
        #               self.getFullDynamicLibName(iLib) )
        
        # Execute the example
        self.__execute__("export DYLD_LIBRARY_PATH=\"{}\"; ./boostTest {}".format(self.__buildInfo__["LIBPATH"][0],os.getcwd()))        

    # Import the dynamic libraries from third party to the dest folder (in this case
    # this will be in the app bundle VPP.app/Contents/Frameworks/
    def importDynamicLibs(self,dst):
    
        # Copy the lib to the dest folder
        for iLib in self.__buildInfo__["LIBS"]: 
            shutil.copyfile(self.getFullDynamicLibName(iLib), 
                            os.path.join(dst,self.getFullDynamicLibName(iLib)))
                    
        