import os
from shutil import copyfile
from shutil import copytree
import subprocess
import shutil
import re
import requests # Download files from the web
import zipfile
import StringIO

''' Collection of recipes used to download, compile and mantain the third_parties'''

class thirdPartyCompile(object):

    # Name of this third_party. Actually this is the name of the 
    # archive that is to be downloaded from the web - once extracted
    self.__name__=""

    # Define the URL from which IpOpt can be downloaded
    # Note that this should be the complete path of the file
    # to be downloaded (Including the filename!)
    self.__url__=""
    
    self.__zipArchive__=""        

    # Where will the downloaded third_party archives be placed to? 
    self.__thirdPartySrcFolder__="/Users/dtrimarchi/third_party_src"

    # Where will the downloaded third_party archives be placed to? 
    self.__thirdPartyBuildFolder__="/Users/dtrimarchi/third_party_build"

    # Where will the compiled packages will be placed to?
    self.__thirdPartyPkgFolder__="/Users/dtrimarchi/third_party"
    
    # Ctor
    def __init__(self):
        
        # verify if the __thirdPartySrcFolder__ and the __thirdPartyPkgFolder__
        # exist. If they do not exist, make them
        if not os.path.exists( self.__thirdPartySrcFolder__ ):
            os.makedirs(self.__thirdPartySrcFolder__)
            
        if not os.path.exists( self.__thirdPartyBuildFolder__ ):
            os.makedirs(self.__thirdPartyBuildFolder__)

        if not os.path.exists( self.__thirdPartyPkgFolder__ ):
            os.makedirs(self.__thirdPartyPkgFolder__)
        
    # Wrapper method used to get, compile and test a third_party     
    def get(self):
        
        # Downlaod the required files from the web
        self.__download__()
        
        # Extract the files - normally we download 
        # compressed archives
        self.__extract__()
        
        # Compile the third party software
        self.__compile__()
        
        # Package the relevant pieces into a dedicated location        
        self.__package__()
        
    # To be implemented in child classes, describe where the 
    # source package can be downloaded
    def __download__(self):
        raise "thirdPartyCompile::__download__() should never be called"

    # To be implemented in child classes, describe how to extract
    # the package that has been downloaded
    def __extract__(self):
        raise "thirdPartyCompile::__extract__() should never be called"
        
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

    # Name of this third_party. Actually this is the name of the 
    # archive that is to be downloaded from the web - once extracted
    self.__name__="Ipopt-3.12.6"
    
    # Define the URL from which IpOpt can be downloaded
    # Note that this should be the complete path of the file
    # to be downloaded (Including the filename!)
    self.__url__="www.something " todo!
    
    # Ctor
    def __init__(self):
        
        # Simply call mother-class init
        super(IpOptCompile,self).__init__()

    def self.__download__(self):
        
        # Go to the __thirdPartySrcFolder__. Its existence was 
        # assured in the init of the class
        os.chdir(self.__thirdPartySrcFolder__)
         
        # Todo : add a progression bar... 
        localArchive = requests.get(self.__url__, stream=True)
        self.__zipArchive__ = zipfile.ZipFile(StringIO.StringIO(localArchive.content))

    def __extract__(self):       

        # Extract the zip archive 
        # Todo : add a progression bar... 
        self.__zipArchive__.extractall()

        todo:
            this should be extracted to : __thirdPartyBuildFolder__
                
    # Compile this package    
    def __compile__(self):
        
        # Go to the buld folder
        os.chdir(self.__thirdPartyBuildFolder__)

        # IpOpt requires some pre-requisites. It offers utilities to download
        # them from the web
        os.chdir(os.path.join(self.__thirdPartyBuildFolder__,
                              self.__name__,
                              "Third_Party","Blas")

        p= subprocess.Popen("./get.Blas",shell=True)
        if !p.wait():
            raise
        

        cd ../Lapack
        ./get.Lapack
        cd ../ASL
        ./get.ASL  
        
           
        