from thirdPartyCompile import thirdPartyCompile
import os, shutil, sys

# Blas is a requirement for IpOopt, that offers utility scripts to download and 
# compile blas with the right bindings
class TclCompile(thirdPartyCompile):

    # Ctor
    def __init__(self):
    
        # Simply call mother-class init
        super(TclCompile,self).__init__()

        # Name of this third_party. 
        self.__name__="Tcl"

        # Version of this third_party
        self.__version__="8.7a1"

        # Define the URL from which IpOpt can be downloaded
        # Note that this should be the complete path of the file
        # to be downloaded (Including the filename!)
        self.__url__="https://prdownloads.sourceforge.net/tcl/tcl8.7a1-src.tar.gz"

        # Define the name of the archive downloadeed from the web. A priori this is 
        # simply the last entry of the url
        self.__srcArchiveName__="tcl-core8.7a1rc1-src.tar"

        # Define the name of the folder extracted from the archive downloadeed from the web. 
        # A priori this is simply the srcArchiveName without the extension
        self.__srcDirName__="tcl8.7a1"

        # Override (specialize) the build folder 
        self.__thirdPartyBuildFolder__= os.path.join(self.__thirdPartyBuildFolder__,self.__srcDirName__)

        # Override (specialize) the package folder 
        self.__thirdPartyPkgFolder__= os.path.join(self.__thirdPartyPkgFolder__,self.__srcDirName__)
        
                # Define the build info. Will use these to copy the components (includes, libs...) to 
        # the package folder
        self.__buildInfo__["INCLUDEPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"include")]
        self.__buildInfo__["LIBPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"lib")]
        self.__buildInfo__["BINPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"bin")]
        self.__buildInfo__["DOCPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"doc")]
        self.__buildInfo__["LIBS"] = [""]
                    
    def __compile__(self,dest=None):

        # Decorates the mother class __compile__ method
        super(TclCompile,self).__compile__()

        # Follow the instructions in file:///Users/dtrimarchi/third_party/opencascade-7.1.0/doc/overview/html/occt_dev_guides__building_3rdparty_osx.html

        # Patch the macosx/GNU_Makefile : ${CURDIR}/../../build => ${CURDIR}/../build
        # Modify the expression to match a regexp
        self.__patch__("\${CURDIR}/../../build","${CURDIR}/../build", os.path.join(self.__thirdPartyBuildFolder__,"macosx","GNUmakefile"))

        # Run the build        
        self.__execute__('''export CFLAGS=\"{cflags}\";export ver=\"{ver}\";
 export BUILD_DIR=\"{builddir}\";make -C macosx deploy;
 sudo make -C macosx install-deploy INSTALL_ROOT=\"{pkgdir}\"'''.format(
            cflags="-arch i386 -arch x86_64 -arch ppc -mmacosx-version-min=10.14",
            ver=self.__version__,
            builddir=self.__thirdPartyBuildFolder__,
            pkgdir=self.__thirdPartyPkgFolder__
            ))

    # Package the third party that was build   
    def __package__(self):
                      
        # Do not call the motherclass __pakcage__ method that 
        # woud erase the components installed!

        # Make the lib folder and move the framework. Remove the folder 'Library'
        self.__makedirs__(self.__buildInfo__["LIBPATH"][0])
        self.__move__(os.path.join(self.__thirdPartyPkgFolder__,"Library","Frameworks","Tcl.framework"),
                      os.path.join(self.__thirdPartyPkgFolder__,"lib","Tcl.framework"))
        self.__remove__(os.path.join(self.__thirdPartyPkgFolder__,"Library"))   
               
        # Rename the folder:    
        #                self.__thirdPartyPkgFolder__/usr/local/bin 
        #                to: bin
        self.__move__(os.path.join(self.__thirdPartyPkgFolder__,"usr","local","bin"),
                            os.path.join(self.__thirdPartyPkgFolder__,self.__buildInfo__["BINPATH"][0]))
        self.__remove__(os.path.join(self.__thirdPartyPkgFolder__,"usr"))   
         
        # Copy the init.tcl file from /Users/dtrimarchi/third_party_build/tcl8.7a1/library 
        # to the lib folder
        self.__copy__(os.path.join(self.__thirdPartyBuildFolder__,"library","init.tcl"),
                      os.path.join(self.__buildInfo__["LIBPATH"][0],"init.tcl"))

    def __test__(self):
        
        # A test target is provided. Go into the build folder of tcl and run 
        # make test TESTFLAGS="-file append.test"
        # The complete test suite appears to hang in socket.test, it might well
        # be that I need to instantiate a server to have this test running. 
        # I do not think it is very important at this stage

        # Go to the build folder
        os.chdir(os.path.join(self.__thirdPartyBuildFolder__,'macosx'))
        
        # Run a couple of test targets
        self.__execute__("make test TESTFLAGS=\"-file append.test\"")
        self.__execute__("make test TESTFLAGS=\"-file eval.test\"")

# ------------------------------------------------------------------------

           
# Blas is a requirement for IpOopt, that offers utility scripts to download and 
# compile blas with the right bindings
class FreeTypeCompile(thirdPartyCompile):

    # Ctor
    def __init__(self):
    
        # Simply call mother-class init
        super(FreeTypeCompile,self).__init__()

        # Name of this third_party. 
        self.__name__="FreeType"

        # Version of this third_party
        self.__version__="2.9.1"

        # Define the URL from which IpOpt can be downloaded
        # Note that this should be the complete path of the file
        # to be downloaded (Including the filename!)
        self.__url__="https://download.savannah.gnu.org/releases/freetype/freetype-2.9.1.tar.gz"

        # Define the name of the archive downloadeed from the web. A priori this is 
        # simply the last entry of the url
        self.__srcArchiveName__="freetype-2.9.1.tar"

        # Define the name of the folder extracted from the archive downloadeed from the web. 
        # A priori this is simply the srcArchiveName without the extension
        self.__srcDirName__="freetype-2.9.1"

        # Override (specialize) the build folder 
        self.__thirdPartyBuildFolder__= os.path.join(self.__thirdPartyBuildFolder__,self.__srcDirName__)

        # Override (specialize) the package folder 
        self.__thirdPartyPkgFolder__= os.path.join(self.__thirdPartyPkgFolder__,self.__srcDirName__)
        
                # Define the build info. Will use these to copy the components (includes, libs...) to 
        # the package folder
        self.__buildInfo__["INCLUDEPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"include")]
        self.__buildInfo__["LIBPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"lib")]
        self.__buildInfo__["BINPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"bin")]
        self.__buildInfo__["DOCPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"doc")]
        self.__buildInfo__["LIBS"] = [""]
                    
    # Follow instructions from OpenCascade in : 
    # opencascade-7.1.0/doc/overview/html/occt_dev_guides__building_3rdparty_osx.html
    def __compile__(self,dest=None):

        # Decorates motherclass method and get to the build folder
        super(FreeTypeCompile,self).__compile__()

        # This is a test. gawk is needed to configure the build, so it must be installed 
        # and functinal. So I introduce a safeguard here.  
        self.__execute__("gawk -h")
        
        # Execute configure        
        self.__execute__("./configure --prefix={} CFLAGS=\'-m64 -fPIC\' CPPFLAGS=\'-m64 -fPIC\'".
                         format(self.__thirdPartyPkgFolder__)) 

        #  compile
        self.__execute__("make")
        
        
    # Package the third party that was build   
    def __package__(self):
        
        # Decorates motherclass method and get to the build folder
        super(FreeTypeCompile,self).__package__()
        
        # Go back to the build folder where configure has been executed
        os.chdir(self.__thirdPartyBuildFolder__)

        # make install
        self.__execute__("make install")
    
    def __test__(self):
        
        pass
# ------------------------------------------------------------------------

class OpenCascadeCompile(thirdPartyCompile):
    
    # Ctor
    def __init__(self):
        
        # Simply call mother-class init
        super(OpenCascadeCompile,self).__init__()

        # Name of this third_party. 
        self.__name__="OpenCascade"
        
        # Version of this third_party
        self.__version__="7.1.0"
        
        # Define the URL from which the src archive can be downloaded
        # Note that this should be the complete path of the file
        # to be downloaded (Including the filename!)
        #self.__url__=????

        # Define the name of the archive downloadeed from the web. A priori this is 
        # simply the last entry of the url
        #self.__srcArchiveName__=???
        
        # Define the name of the folder extracted from the archive downloadeed from the web. 
        # A priori this is simply the srcArchiveName without the extension
        self.__srcDirName__="openCascade-7.1.0"
        
        # Override (specialize) the build folder 
        self.__thirdPartyBuildFolder__= os.path.join(self.__thirdPartyBuildFolder__,self.__srcDirName__)

        # Override (specialize) the package folder 
        self.__thirdPartyPkgFolder__= os.path.join(self.__thirdPartyPkgFolder__,self.__srcDirName__)

        # Instantiate requirements, see file:///Users/dtrimarchi/third_party/opencascade-7.1.0/doc/overview/html/occt_dev_guides__building_3rdparty_osx.html
        # by now I only install the mandatory : Tcl/Tk 8.5 , FreeType 2.4.10
        self.__requirements__.append( TclCompile() )
        self.__requirements__.append( FreeTypeCompile() )

        # Define the build info. Will use these to copy the components (includes, libs...) to 
        # the package folder
        self.__buildInfo__["INCLUDEPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"include")]
        self.__buildInfo__["LIBPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"lib")]
        self.__buildInfo__["DOCPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"doc")]
        self.__buildInfo__["LIBS"] = [] 
        
  
    # Compile this package    
    def __compile__(self,dest=None):
        
        # Decorate the mother class __package__ method
        super(OpenCascadeCompile,self).__compile__()
    
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
        super(OpenCascadeCompile,self).__package__()
                
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
        super(OpenCascadeCompile,self).__test__()

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
            
        # Compile and run the test
        self.__makeTest__()
        
    # -- 
            
    # Import the dynamic libraries from third party to the dest folder (in this case
    # this will be in the app bundle VPP.app/Contents/Frameworks/
    def importDynamicLibs(self,dst):
    
        # Copy the lib to the dest folder
        for iLib in self.__buildInfo__["LIBS"]: 
            shutil.copyfile(self.getFullDynamicLibName(iLib), 
                            os.path.join(dst,self.getFullDynamicLibName(iLib)))
                    
        