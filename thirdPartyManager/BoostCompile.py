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
        self.__buildInfo__["INCLUDEPATH"] = os.path.join(self.__thirdPartyPkgFolder__,"include")
        self.__buildInfo__["LIBPATH"] = os.path.join(self.__thirdPartyPkgFolder__,"lib")
        self.__buildInfo__["DOCPATH"] = os.path.join(self.__thirdPartyPkgFolder__,"doc")
        self.__buildInfo__["LIBS"] = ["boost_system","boost_filesystem"]
    

    def __download__(self):
            
        # Go to the __thirdPartySrcFolder__. Its existence was 
        # assured in the init of the class
        os.chdir(self.__thirdPartySrcFolder__)
                 
        # cleanup: remove a previous archive if present
        shutil.rmtree(self.__srcArchiveName__,sys.exc_info())
          
        # Get the sources from the web
        self.__getCompressedArchive__(self.__url__)
                     
         # I can now use the scripts provided by ipOpt to download the 
         # required third_party. Kq[gr]pe
        for iReq in self.__requirements__:
            iReq.__download__()
            
    # Compile this package    
    def __compile__(self,dest=None):
        
        print "Cleaning up..."

        # Cleanup previous build folder - if any
        shutil.rmtree(self.__thirdPartyBuildFolder__,
                      sys.exc_info())

        print "Copying srcs to the build folder..."

        # Copy the sources to the build dir
        # First, make sure we are in the right place
        os.chdir(self.__thirdPartySrcFolder__)
        shutil.copytree(self.__srcDirName__,self.__thirdPartyBuildFolder__)

        # What about the requirements? Actually this is a mis-use of the __compile__
        # method, that does not compile for the requirements, because the ipOpt build 
        # is able to handle compiling. So, for HSL only we move the sources to the 
        # ThirdParty folder of ipOpt, so to complete the package 
        for iReq in self.__requirements__:
            iReq.__compile__(dest=os.path.join(self.__thirdPartyBuildFolder__,
                                          "ThirdParty"))
    
        # Go to the ipOpt build folder
        os.chdir(self.__thirdPartyBuildFolder__)

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
            # Define the doc directory as pkgDir/doc/libDir
            docDir = os.path.join(self.__buildInfo__["DOCPATH"],cleanName)
            # Make the doc directory
            os.makedirs(docDir)
            # Copy
            shutil.copy(os.path.join(self.__thirdPartyBuildFolder__,"libs",cleanName,"index.html"), 
                    docDir)
            shutil.copytree(os.path.join(self.__thirdPartyBuildFolder__,"libs",cleanName,"doc"), 
                            os.path.join(docDir,"doc"))

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

        # Copy the content of include
        shutil.copytree(os.path.join(self.__thirdPartyBuildFolder__,"boost"), 
                        os.path.join(self.__buildInfo__["INCLUDEPATH"],"boost"))
                
        shutil.copytree(os.path.join(self.__thirdPartyBuildFolder__,"stage","lib"), 
                        self.__buildInfo__["LIBPATH"])

        # Also copy the documentation of the modules we are compiling
        self.__copySelectedDocs__()
                              
        # Finally, prepare the test folder 
        os.mkdir(os.path.join(self.__thirdPartyPkgFolder__,"Cpp_example"))
        
    # Run a simple test to check that the compile was successiful
    def __test__(self):
        
        exampleFolder= os.path.join(self.__thirdPartyPkgFolder__,"Cpp_example")

        # cleanup
        shutil.rmtree(exampleFolder)
        os.mkdir(exampleFolder)
        
        # Go to the example folder
        os.chdir(exampleFolder)

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
'''.format(self.__buildInfo__["INCLUDEPATH"],
           self.__buildInfo__["LIBPATH"],
           self.__buildInfo__["LIBS"]))
        Sconstruct.close()
        
        print "Compiling the boost test: we are in folder: ", os.getcwd()
                
        # Compile the example
        self.__execute__("scons -Q")
        
        # Before execution, add symbolic links to the dylibs. Why cannot I 
        # just set LD_LIBRARY_PATH..? Weird. Looks like MacOS security stuff.
        for iLib in self.__buildInfo__["LIBS"]:
            os.symlink(os.path.join(self.__buildInfo__["LIBPATH"],self.getFullDynamicLibName(iLib)), 
                       self.getFullDynamicLibName(iLib) )
        
        # Execute the example
        self.__execute__("./boostTest {}".format(os.getcwd()))        

    # Import the dynamic libraries from third party to the dest folder (in this case
    # this will be in the app bundle VPP.app/Contents/Frameworks/
    def importDynamicLibs(self,dst):
    
        # Copy the lib to the dest folder
        for iLib in self.__buildInfo__["LIBS"]: 
            shutil.copyfile(self.getFullDynamicLibName(iLib), 
                            os.path.join(dst,self.getFullDynamicLibName(iLib)))
                    
        