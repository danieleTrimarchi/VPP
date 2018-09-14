import os, sys
from shutil import copyfile
from shutil import copytree
from shutil import rmtree
import subprocess
import shutil
import re

try:    
    import pip
except ImportError, e:
    print "Installing pip... "
    p = subprocess.Popen("sudo easy_install pip",shell=True)
    if not p.wait():
        raise
    import pip

try:
    import requests # Download files from the web
except ImportError, e:
    p = subprocess.Popen("sudo pip install requests",shell=True)
    if not p.wait():
        raise
    import requests
    
import zipfile
import StringIO

''' Collection of recipes used to download, compile and mantain the third_parties'''

class thirdPartyCompile(object):

    # Ctor
    def __init__(self):
        
                # Name of this third_party. Actually this is the name of the 
        # archive that is to be downloaded from the web - once extracted
        self.__name__=""
    
        # Define the URL from which IpOpt can be downloaded
        # Note that this should be the complete path of the file
        # to be downloaded (Including the filename!)
        self.__url__=""
        
        # Define the name of the archive downloadeed from the web. A priori this is 
        # simply the last entry of the url
        self.__srcArchiveName__=""

        # Src archive downloaded from the web
        self.__zipArchive__=""        
    
        # Where will the downloaded third_party archives be placed to and 
        # where will they be extracted  
        self.__thirdPartySrcFolder__="/Users/dtrimarchi/third_party_src"
    
        # Where will the downloaded third_party archives be placed to? 
        self.__thirdPartyBuildFolder__="/Users/dtrimarchi/third_party_build"
    
        # Where will the compiled packages will be placed to?
        self.__thirdPartyPkgFolder__="/Users/dtrimarchi/third_party_pkg"
        
        # Every third_party object can instantiate and fill this list with the 
        # requirements (or sub-third_parties). Will loop over requirements when 
        # required to download and compile the required packages
        self.__requirements__ = [] 
        
        # verify if the __thirdPartySrcFolder__ and the __thirdPartyPkgFolder__
        # exist. If they do not exist, make them
        if not os.path.exists( self.__thirdPartySrcFolder__ ):
            os.makedirs(self.__thirdPartySrcFolder__)
            
        if not os.path.exists( self.__thirdPartyBuildFolder__ ):
            os.makedirs(self.__thirdPartyBuildFolder__)

        if not os.path.exists( self.__thirdPartyPkgFolder__ ):
            os.makedirs(self.__thirdPartyPkgFolder__)
        
    # Wrapper method used to get, compile and test a third_party     
    def get(self, download=True):
        
        # Downlaod the required files from the web, if this is requested
        # otherwise we skip the download, but this implies that we are 
        # recompiling the third_party
        if(download):
            self.__download__()
                
        # Compile the third party software
        self.__compile__()
        
        # Package the relevant pieces into a dedicated location        
        self.__package__()
        
        # Test the third party   
        self.__test__()
        
    # To be implemented in child classes, describe where the 
    # source package can be downloaded
    def __download__(self):
        raise "thirdPartyCompile::__download__() should never be called"

    # After extracting the third party, it might be the case that we need to 
    # use some tools internal to the package to download some additional reqs. 
    # This is for example the case for ipOpt, that provides scripts to download
    # third parties such as blas, ASL...         
    def __getAdditionalRequirements__(self):
        raise "thirdPartyCompile::__getAdditionalRequirements__() should never be called"
        
    # How to compile the third party
    def __compile__(self,dest=None):
        raise "thirdPartyCompile::__compile__() should never be called"
        
    # How to package the relevant components of this third_party
    def __package__(self):
        raise "thirdPartyCompile::__package__() should never be called"
        
    # Run some test to make sure this third party was compiled 
    # properly
    def __test__(self):
        raise "thirdPartyCompile::__test__() should never be called"
        
    # Wraps a call to subprocess and the required diagnostics
    def __execute__(self,command):
        
        p = subprocess.Popen(command,shell=True)
        if p.wait():
            raise ValueError('\n\nSomething went wrong when trying to execute: {}\n\n'.format(command))

    # Download an archive from a given url and unzip it 
    def __getCompressedArchive__(self,url):
        
        # Todo : add a progression bar...
        localArchive = requests.get(url,stream=True)
        print "...downloading the archive from...{} (this can take a while)".format(url)
        z = zipfile.ZipFile(StringIO.StringIO(localArchive.content))
        print "...extracting the archive..."
        z.extractall()

# Blas is a requirement for IpOopt, that offers utility scripts to download and 
# compile blas with the right bindings
class ipOptBlasCompile(thirdPartyCompile):

    # Ctor
    def __init__(self,ipOptBlasPath):
    
        # Simply call mother-class init
        super(ipOptBlasCompile,self).__init__()

        # Name of this third_party. 
        self.__name__="BLAS"

        # Store the location where ipOpt will download and 
        # compile blas
        self.__thirdPartySrcFolder__ = ipOptBlasPath
        
        # We do not need to fill other fileds, as this thirdParty is 
        # entirely handled by IpOpt
            
    def __download__(self):
        
        # Use the IpOpt utilities to download BLAS from the web and compile
        # Note that the __thirdPartySrcFolder__ has been set to the ipOpt/third_party/
        # blas folder
        os.chdir(self.__thirdPartySrcFolder__)

        # Make sure the BLAS script has 755 permissions and execute it
        self.__execute__("chmod 755 get.Blas")
        self.__execute__("./get.Blas")

    def __compile__(self,dest=None):
        # Make nothing, BLAS will be compiled as part of IpOpt
        pass 
    
# Lapack is a requirement for IpOopt, that offers utility scripts to download and 
# compile blas with the right bindings
class ipOptLapackCompile(thirdPartyCompile):

    # Ctor
    def __init__(self,ipOptLapackPath):
    
        # Simply call mother-class init
        super(ipOptLapackCompile,self).__init__()

        # Name of this third_party. 
        self.__name__="LAPACK"

        # Store the location where ipOpt will download and 
        # compile blas
        self.__thirdPartySrcFolder__ = ipOptLapackPath
        
        # We do not need to fill other fileds, as this thirdParty is 
        # entirely handled by IpOpt
            
    def __download__(self):
        
        # Use the IpOpt utilities to download BLAS from the web and compile
        # Note that the __thirdPartySrcFolder__ has been set to the ipOpt/third_party/
        # blas folder
        os.chdir(self.__thirdPartySrcFolder__)

        # Make sure the BLAS script has 755 permissions and execute it
        self.__execute__("chmod 755 get.Lapack")
        self.__execute__("./get.Lapack")

    def __compile__(self,dest=None):
        # Make nothing, LAPACK will be compiled as part of IpOpt
        pass 

# ASL is a requirement for IpOopt, that offers utility scripts to download and 
# compile blas with the right bindings
class ipOptASLCompile(thirdPartyCompile):

    # Ctor
    def __init__(self,ipOptASLPath):
    
        # Simply call mother-class init
        super(ipOptASLCompile,self).__init__()

        # Name of this third_party. 
        self.__name__="ASL"

        # Store the location where ipOpt will download and 
        # compile blas
        self.__thirdPartySrcFolder__ = ipOptASLPath
        
        # We do not need to fill other fileds, as this thirdParty is 
        # entirely handled by IpOpt
            
    def __compile__(self,dest=None):
        # Make nothing, ASL will be compiled as part of IpOpt
        pass 

    def __download__(self):
        
        # Use the IpOpt utilities to download ASL from the web and compile
        # Note that the __thirdPartySrcFolder__ has been set to the ipOpt/third_party/
        # ASL folder
        os.chdir(self.__thirdPartySrcFolder__)

        # Make sure the ASL script has 755 permissions and execute it
        self.__execute__("chmod 755 get.ASL")
        self.__execute__("./get.ASL")

# HSL is a requirement for IpOopt. The doownload link must be requested 
# by the user, who will receive a download link by email. We explicitely
#  request the download link to the user 
class IpOptHSLCompile(thirdPartyCompile):
    
    # Ctor
    def __init__(self):
        
        # Simply call mother-class init
        super(IpOptHSLCompile,self).__init__()

        # Name of this third_party. 
        self.__name__="HSL"

        # Define the URL from which IpOpt can be downloaded
        # Note that this should be the complete path of the file
        # to be downloaded (Including the filename!)
        print "\n\nHSL cannot be automatically downloaded, please"
        print "request a download link on : "
        print "     http://www.hsl.rl.ac.uk/ipopt/ "
        print "\nThe download link will be sent to you by email.\n"
        self.__url__ = raw_input("Please enter the HSL download link...\n"); 

        # Define the name of the archive downloadeed from the web.
        self.__srcArchiveName__="tar.tar"

        # Define the name of the folder extracted from the archive downloadeed from the web. 
        # Often  this is simply the srcArchiveName without the extension, but not for HSL!
        self.__srcDirName__="coinhsl-archive-2014.01.17"

        # Override (specialize) the build folder. HSL only requires to be downloaded, and will
        # be build by the ipOpt build system. So the build folder might be of no interest
        #self.__thirdPartyBuildFolder__= os.path.join(self.__thirdPartyBuildFolder__,self.__srcDirName__)

        # Override (specialize) the package folder. HSL only requires to be downloaded, and will
        # be build by the ipOpt build system. So the pkg folder might be of no interest
        #self.__thirdPartyPkgFolder__= os.path.join(self.__thirdPartyPkgFolder__,self.__srcDirName__)
        
    def __download__(self):
            
        # Go to the __thirdPartySrcFolder__. Its existence was 
        # assured in the init of the class
        os.chdir(self.__thirdPartySrcFolder__)

        # cleanup: remove a previous archive if present
        shutil.rmtree(self.__srcArchiveName__,sys.exc_info())

        # Get the sources from the web
        self.__getCompressedArchive__(self.__url__)

        # Also make sure the install-sh script has 755 permissions 
        self.__execute__("chmod 755 {}".format(os.path.join(self.__srcDirName__,"install-sh")))
    
    def __compile__(self,dest=None):
        
        # Simply copy the sources from the source directory - where the sources have been 
        # downloaded - to the dest directory given by dest
        # Unlike the other ipOpt requirements, HSL works differently because the sources 
        # are not automatically downloaded using the ipOpt scripts, we need to work harder
        # and place the sources 'by hand' in ipOpt/Third_party/HSL/coinhsl for the 
        # ipOpt build system to recognize the package
        print "Copying: ", self.__srcDirName__,"\n  to: ",os.path.join(dest,self.__name__,'coinhsl')
        shutil.copytree(self.__srcDirName__,os.path.join(dest,self.__name__,'coinhsl'))
        
        # Make nothing else, the build is taken over by my parent
        
class IpOptCompile(thirdPartyCompile):
    
    # Ctor
    def __init__(self):
        
        # Simply call mother-class init
        super(IpOptCompile,self).__init__()

        # Name of this third_party. 
        self.__name__="Ipopt-3.12.6"
        
        # Define the URL from which IpOpt can be downloaded
        # Note that this should be the complete path of the file
        # to be downloaded (Including the filename!)
        self.__url__="https://www.coin-or.org/download/source/Ipopt/Ipopt-3.12.6.zip"

        # Define the name of the archive downloadeed from the web. A priori this is 
        # simply the last entry of the url
        self.__srcArchiveName__="Ipopt-3.12.6.zip"
        
        # Define the name of the folder extracted from the archive downloadeed from the web. 
        # A priori this is simply the srcArchiveName without the extension
        self.__srcDirName__="Ipopt-3.12.6"
        
        # Override (specialize) the build folder 
        self.__thirdPartyBuildFolder__= os.path.join(self.__thirdPartyBuildFolder__,self.__srcDirName__)

        # Override (specialize) the package folder 
        self.__thirdPartyPkgFolder__= os.path.join(self.__thirdPartyPkgFolder__,self.__srcDirName__)
            
        # Instantiate the requirements... BLAS
        blasSrcFolder = os.path.join(self.__thirdPartySrcFolder__,
                                     self.__srcDirName__,
                                     "ThirdParty","Blas")
        self.__requirements__.append( ipOptBlasCompile(blasSrcFolder))
        
        # Instantiate the requirements... LAPACK
        lapackSrcFolder = os.path.join(self.__thirdPartySrcFolder__,
                                       self.__srcDirName__,
                                       "ThirdParty","Lapack")
        self.__requirements__.append( ipOptLapackCompile(lapackSrcFolder))

        # Instantiate the requirements... ASL
        aslSrcFolder = os.path.join(self.__thirdPartySrcFolder__,
                                       self.__srcDirName__,
                                       "ThirdParty","ASL")
        self.__requirements__.append( ipOptASLCompile(aslSrcFolder))
        
         # Instantiate the requirements... HSL
        self.__requirements__.append( IpOptHSLCompile() )
    
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
        
        # Cleanup previous build folder - if any
        shutil.rmtree(self.__thirdPartyBuildFolder__,
                      sys.exc_info())

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
        os.chdir("Build")

        # Change permissions to the scripts that will be executed during build
        self.__execute__("chmod 755 ../configure")
        self.__execute__("chmod 755 {}".format(os.path.join(
                                                self.__thirdPartyBuildFolder__,
                                                "Ipopt",
                                                "install-sh")))
        self.__execute__("chmod 755 {}".format(os.path.join(
                                                self.__thirdPartyBuildFolder__,
                                                "install-sh")))
        # Make sure the install-sh scripts of the requirements are executable too
        for iReq in self.__requirements__: 
            self.__execute__("chmod 755 {}".format(os.path.join(
                                                    self.__thirdPartyBuildFolder__,
                                                    "ThirdParty",
                                                    iReq.__name__,
                                                    "install-sh")))

        # Launch the configure script with the macOS specific args 
        # (see pdf docs 2.8.2 Adjusting configuration and build of Ipopt)     
        self.__execute__("../configure --disable-linear-solver-loader "
                         "ADD_CFLAGS='-fno-common -fexceptions -no-cpp-precomp' "
                         "ADD_CXXFLAGS='-fno-common -fexceptions -no-cpp-precomp' "
                         "ADD_FFLAGS='-fexceptions -fbackslash' "
                         "--prefix={}".format(os.path.join(self.__thirdPartyBuildFolder__,"Build","lib"))) 

#--with-blas='-framework vecLib' --with-lapack='-framework vecLib'
#--disable-shared --with-pic "

        # Launch the build    
        self.__execute__("make")

        # Install the build (in the location specified as prefix while running the 
        # configure script)
        self.__execute__("make install")

        # Install the doxygen doc - is this really worthit?
        #self.__execute__("make install-doxydoc")

        # Do NOT make the tests, see https://github.com/Homebrew/legacy-homebrew/issues/13617
        # Will add a simple local test example that will use the basic functionalities

    # Package the third party that was build   
    def __package__(self):
        
        # cleanup: remove a previous package folder if present. Remember that the 
        # package folder was set relative to this third_party in the ctor
        shutil.rmtree(self.__thirdPartyPkgFolder__,sys.exc_info())

        # Make a package folder and enter it 
        os.mkdir(self.__thirdPartyPkgFolder__)

        # Copy the content of include and lib
        shutil.copytree(os.path.join(self.__thirdPartyBuildFolder__,"Build","lib","include","coin"), 
                        os.path.join(self.__thirdPartyPkgFolder__,"include"))
        shutil.copytree(os.path.join(self.__thirdPartyBuildFolder__,"Build","lib","lib"), 
                        os.path.join(self.__thirdPartyPkgFolder__,"lib"))

        # Also copy the documentation
        shutil.copyfile(os.path.join(self.__thirdPartyBuildFolder__,"Ipopt","doc","documentation.pdf"), 
                        os.path.join(self.__thirdPartyPkgFolder__,"documentation.pdf"))
 
        # Copy the example 
        shutil.copytree(os.path.join(self.__thirdPartyBuildFolder__,"Ipopt","examples","Cpp_example"), 
                        os.path.join(self.__thirdPartyPkgFolder__,"Cpp_example"))
        
    # Run a simple test to check that the compile was successiful
    def __test__(self):
        
        # Also compile the c++ test, that will be run by __test__
        os.chdir(os.path.join(self.__thirdPartyPkgFolder__,"Cpp_example"))
        
        # Write a SConstruct 
        Sconstruct=open("SConstruct","w")
        Sconstruct.write('''import os
env = Environment()  
env.Append( CPPPATH=["{}/include"] )
env.Append( LIBPATH=["{}/lib"] )
env.Append( LIBS=["ipopt"] )
env.Program('ipOptTest', Glob('*.cpp') )        
'''.format(self.__thirdPartyPkgFolder__,self.__thirdPartyPkgFolder__))
        Sconstruct.close()
        
        print "Executing the ipOpt test: we are in folder: ", os.getcwd()
                
        # Compile the example
        self.__execute__("scons -Q")
        
        # Execute the example
        self.__execute__("./ipOptTest")        
        