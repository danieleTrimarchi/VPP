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
    def __compile__(self):
        raise "thirdPartyCompile::__compile__() should never be called"

        
    # How to package the relevant components of this third_party
    def __package__(self):
        raise "thirdPartyCompile::__package__() should never be called"
        
    # Run some test to make sure this third party was compiled 
    # properly
    def __test__(self):
        raise "thirdPartyCompile::__test__() should never be called"
        

class IpOptCompile(thirdPartyCompile):
    
    # Ctor
    def __init__(self):
        
        # Simply call mother-class init
        super(IpOptCompile,self).__init__()

        # Name of this third_party. Actually this is the name of the 
        # archive that is to be downloaded from the web - once extracted
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

    def __download__(self):
        
        print "beginning of __download__"
        # Go to the __thirdPartySrcFolder__. Its existence was 
        # assured in the init of the class
        os.chdir(self.__thirdPartySrcFolder__)
                 
        # cleanup: remove a previous archive if present
        shutil.rmtree(self.__srcArchiveName__,sys.exc_info())
          
        # Todo : add a progression bar... 
        localArchive = requests.get(self.__url__, stream=True)
        print "...downloading the archive... (this can take a while)"
        z = zipfile.ZipFile(StringIO.StringIO(localArchive.content))
        print "...extracting the archive..."
        z.extractall()
                     
         # I can now use the scripts provided by ipOpt to download the 
         # required third_party. 
        self.__getAdditionalRequirements__()

    # After extracting we use the script provided to download
    # third parties such as blas, ASL...  
    def __getAdditionalRequirements__(self):
        
        # IpOpt requires some pre-requisites. It offers utilities to download
        # them from the web. 
        # BLAS... 
        os.chdir(os.path.join(self.__thirdPartySrcFolder__,
                              self.__name__,
                              "ThirdParty","Blas"))

        # Make sure the script has 755 permissions
        p = subprocess.Popen("chmod 755 get.Blas",shell=True)

        # Execute the script        
        p = subprocess.Popen("./get.Blas",shell=True)
        if p.wait():
            raise ValueError('\n\nSomething went wrong when trying to get BLAS\n\n')
        
        # LAPACK... 
        # Execute the script        
        os.chdir(os.path.join(self.__thirdPartySrcFolder__,
                              self.__name__,
                              "ThirdParty","Lapack"))

        # Make sure the script has 755 permissions
        p = subprocess.Popen("chmod 755 get.Lapack",shell=True)

        p= subprocess.Popen("./get.Lapack",shell=True)
        if p.wait():
            raise ValueError('\n\nSomething went wrong when trying to get LAPACK\n\n')

        # ASL... 
        os.chdir(os.path.join(self.__thirdPartySrcFolder__,
                               self.__name__,
                               "ThirdParty","ASL"))
         
         # Make sure the script has 755 permissions
        p = subprocess.Popen("chmod 755 get.ASL",shell=True)
 
        p= subprocess.Popen("./get.ASL",shell=True)
        if p.wait():
            raise ValueError('\n\nSomething went wrong when trying to get ASL\n\n')

    def __stdcompileProcedure__(self,libName):
        
        p = subprocess.Popen("chmod 755 configure",shell=True)
        if p.wait():
            raise ValueError('\n\nSomething went wrong when trying to change permissions to the configure script for %\n\n'.format(libName))
        
        # Standard compile procedure: configure + make 
        p= subprocess.Popen("./configure",shell=True)
        if p.wait():
            raise ValueError('\n\nSomething went wrong when trying to configure for %s\n\n'.format(libName))
        
        # Note that the generated libs will be stored in ./.libs
        p= subprocess.Popen("make",shell=True)
        if p.wait():
            raise ValueError('\n\nSomething went wrong when trying to compile %s\n\n'.format(libName))

    # Compile this package    
    def __compile__(self):
        
        # Cleanup previous build folder - if any
        shutil.rmtree(os.path.join(self.__thirdPartyBuildFolder__,self.__srcDirName__),
                      sys.exc_info())

        # Copy the sources to the build dir
        # First, make sure we are in the right place
        os.chdir(self.__thirdPartySrcFolder__)
        shutil.copytree(self.__srcDirName__, 
                        os.path.join(self.__thirdPartyBuildFolder__,self.__srcDirName__))

        # Go to the ipOpt build folder
        os.chdir(os.path.join(self.__thirdPartyBuildFolder__,self.__srcDirName__))

        # Make the build folder
        os.mkdir("Build")
        os.chdir("Build")

        p = subprocess.Popen("chmod 755 ../configure",shell=True)
        if p.wait():
            raise ValueError('\n\nSomething went wrong when trying to change permissions to the configure script for IPOPT\n\n')

        # Launch the configure script        
        p= subprocess.Popen("../configure ADD_CFLAGS='-fno-common -fexceptions -no-cpp-precomp' "
                             "ADD_CXXFLAGS='-fno-common -fexceptions -no-cpp-precomp' "
                             "ADD_FFLAGS='-fexceptions -fbackslash'"
                             "--prefix={}".format(os.path.join(self.__thirdPartyBuildFolder__,self.__srcDirName__,"Build","lib")),shell=True)
        if p.wait():
            raise ValueError('\n\nSomething went wrong when trying to configure for IPOPT\n\n')

        # Launch the build    
        p= subprocess.Popen("make",shell=True)
        if p.wait():
            raise ValueError('\n\nSomething went wrong when trying to compile IPOPT\n\n')

        # Make the tests    
        p= subprocess.Popen("make test",shell=True)
        if p.wait():
            raise ValueError('\n\nSomething went wrong when trying to test the IPOPT compile\n\n')

    def __package__(self):
        
        # cleanup: remove a previous package folder if present
        shutil.rmtree(self.__thirdPartyPkgFolder__,sys.exc_info())

        # Make a package folder and enter it 
        os.mkdir(self.__thirdPartyPkgFolder__)
        os.chdir(self.__thirdPartyPkgFolder__)

        
        