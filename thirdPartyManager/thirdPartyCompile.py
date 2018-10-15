import os, sys
from shutil import copyfile
from shutil import copytree
from shutil import rmtree
import subprocess
import shutil
import re
from email.test.test_email import openfile

try:    
    import pip
except ImportError as error:
    print "Installing pip... ", error.message
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
    
''' Static identifiers '''
__includepathFlag__ = "INCLUDEPATH"    
__libpathFlag__ = "LIBPATH"         
__docpathFlag__ = "DOCPATH"
__libsFlag__ = "LIBS"     
    
''' Collection of recipes used to download, compile and mantain the third_parties'''
class thirdPartyCompile(object):

    # Ctor
    def __init__(self):
        
        # Name of this third_party. 
        self.__name__=""

        # version of this third_aprty
        self.__version__=""
        
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
        
        # Include Path. To be filled
        self.__buildInfo__={__includepathFlag__:[""],
                            __libpathFlag__:[""],
                            __docpathFlag__:[""],
                            __libsFlag__:[""]}
            
        # Abs path of the file info file, featuring all the info required for compiling VPP
        self.__thirdPartyInfoFile__ = os.path.join(self.__thirdPartyPkgFolder__,"third_party_info")
        
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
        
#     # Reads the buildInfo file and returns a dictionary 
#     # with all the entries 
#     def getInfo_full(self):
#         
#         def readIt():
#             
#             if not os.path.isfile(self.__thirdPartyInfoFile__):
#                 msg = "\n==============================================================="
#                 msg += "\nFile " + self.__thirdPartyInfoFile__ + " not found.\nPlease run \'scons third_party_compile\' first\n"
#                 msg += "===============================================================\n\n"
#                 raise ValueError(msg)
# 
#             with open(self.__thirdPartyInfoFile__) as fIn:
#                 #return dict(line.strip().split(' : ') for line in fIn)
#                 outDict={}
#                 for line in fIn: 
#                     myList= line.strip().split(" : ")
#                     print "myList= ", myList
#                     outDict[ myList[0] ]=myList[1]
#                 return outDict
#             
#         return readIt()
# 
#     def getInfo(self):
#         
#         # Filter the full dictionary using the name of this third_party
#         # So, it returns a dictionary containing only the entries for the current 
#         # third_party : i.e: {'ipOpt_LIBS': 'lib1 lib2 lib3', 'ipOpt_DOCPATH': 'doc', ...
#         return {key:value for key,value in self.getInfo_full().items() if self.__name__ in  key}

    def getIncludePath(self):
        
        return self.__buildInfo__[__includepathFlag__]
    
    def getLibPath(self):
        
        return self.__buildInfo__[__libpathFlag__]
    
    def getLibs(self):
        
        return self.__buildInfo__[__libsFlag__]

    def getDocPath(self):
        
        return self.__buildInfo__[__docpathFlag__]
        
    # Warning: this method is duplicated in thirdParties.py. Classes need to merge
    def getFullDynamicLibName(self,shortLibName):
        return "lib"+shortLibName+".dylib"
    
    # To be implemented in child classes, describe where the 
    # source package can be downloaded
    def __download__(self):
        raise ValueError( "thirdPartyCompile::__download__() should never be called" )

    # After extracting the third party, it might be the case that we need to 
    # use some tools internal to the package to download some additional reqs. 
    # This is for example the case for ipOpt, that provides scripts to download
    # third parties such as blas, ASL...         
    def __getAdditionalRequirements__(self):
        raise ValueError( "thirdPartyCompile::__getAdditionalRequirements__() should never be called" )
        
    # How to compile the third party
    def __compile__(self,dest=None):
        raise ValueError( "thirdPartyCompile::__compile__() should never be called" ) 
        
    # How to package the relevant components of this third_party
    def __package__(self):
        raise ValueError( "thirdPartyCompile::__package__() should never be called" ) 
        
    # Run some test to make sure this third party was compiled 
    # properly
    def __test__(self):
        raise ValueError( "thirdPartyCompile::__test__() should never be called" )
        
    # Write the build info to file. This will be used
    # by the build system to compile and link the program
    def __writeInfo__(self) : 

        # Open the thirdPartyInfo file for append write
        fOut = open(self.__thirdPartyInfoFile__,"a+")
                  
        # write the entries. Note that we do not want to write lists, but the plain entries
        for iEntry in self.__buildInfo__:
            if isinstance(self.__buildInfo__[iEntry],list) :
                fOut.write( "{}_{} : ".format(self.__name__,iEntry))  
                for jEntry in self.__buildInfo__[iEntry]: fOut.write( "{} ".format(jEntry) )
                fOut.write("\n")
            else:
                fOut.write( "{}_{} : {}\n".format(self.__name__,iEntry, self.__buildInfo__[iEntry]))    
         
        # close the file
        fOut.close()
                    
    # Wraps a call to subprocess and the required diagnostics
    def __execute__(self,command,myEnv=os.environ):
        
        p = subprocess.Popen(command,shell=True,env=myEnv)
        if p.wait():
            raise ValueError('\n\nSomething went wrong when trying to execute: {}\n\n'.format(command))

    # Download an archive from a given url and unzip it 
    def __getCompressedArchive__(self,url,saveAs="myArch"):

        print "...downloading the archive from: {} (this can take a while)".format(url)

        localArchive = requests.get(url,stream=True)
        
        # This is a zip archive... 
        if ( url.endswith(".zip") or
             url.endswith("/zip/") ) :
            import zipfile
            import StringIO
            print 'extracting a zip file...'
            z = zipfile.ZipFile(StringIO.StringIO(localArchive.content))
            z.extractall()
            z.close()
            
        # This is a tar archive... 
        elif  ( url.endswith(".tar.gz") or
                url.endswith(".tgz") or 
                url.endswith("/tar/") ) :
            import tarfile
            print 'extracting a tar file...'
            tar= tarfile.open(mode='r|gz', fileobj=localArchive.raw)
            tar.extractall()
            tar.close()

    # Import the dynamic libraries from third party to the dest folder (in this case
    # this will be in the app bundle VPP.app/Contents/Frameworks/
    def importDynamicLibs(self,dstFolder):
        raise ValueError( "thirdPartyCompile::fixExecutablePath() should never be called" ) 
                    
    # Decorates copytree of shutil, with some minimal logics to handle lists 
    def __copytree__(self,src,dst):
    
        # Case 1 : src and dst are lists
        if(isinstance(src,list) and isinstance(dst,list)):
            for iSrc in src:
                for iDst in dst:
                    shutil.copytree(iSrc,iDst)

        # Case 2 : src is list but dst is not 
        elif (isinstance(src,list) and not isinstance(dst,list)):
            for iSrc in src:
                shutil.copytree(iSrc,dst)

        # Case 3 : src is not list but dst is  
        elif (not isinstance(src,list) and isinstance(dst,list)):
            for iDst in dst:
                shutil.copytree(src,iDst)
            
        # Case 4 : no lists
        else:
            shutil.copytree(src,dst)
            
        
        