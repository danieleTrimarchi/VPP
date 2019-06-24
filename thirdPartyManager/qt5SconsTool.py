from thirdPartyCompile import thirdPartyCompile
import os, shutil, sys, glob, fnmatch

class qt5SconsTool(thirdPartyCompile):
    
    def __init__(self):
        
        # Simply call mother-class init
        super(qt5SconsTool,self).__init__()

        # Name of this third_party. 
        self.__name__="SCons_Qt5"

        # Version of this third_party
        self.__version__="1.0"

        # Define the URL from which IpOpt can be downloaded
        # Note that this should be the complete path of the file
        # to be downloaded (Including the filename!)
        self.__url__="https://bitbucket.org/dirkbaechle/scons_qt5/get/9caa36eb469d.zip"

        # Define the name of the archive downloadeed from the web. A priori this is 
        # simply the last entry of the url
        self.__srcArchiveName__="dirkbaechle-scons_qt5-9caa36eb469d.zip"
        
        # Define the name of the folder extracted from the archive downloadeed from the web. 
        # A priori this is simply the srcArchiveName without the extension
        self.__srcDirName__="dirkbaechle-scons_qt5-9caa36eb469d"

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
        self.__buildInfo__["LIBS"] = []

    def __compile__(self,dest=None):
        
        # Decorate the mother class __package__ method
        super(qt5SconsTool,self).__compile__()
        
        # I need to package here. This is akward and shows the 
        # limit of my framework, too bad. This is a requirement 
        # for Qt, and I need to have it set completely before 
        # running __compile__ on qt. 
        # Decorate the mother class __package__ method
        super(qt5SconsTool,self).__package__()
        
        # Now copy the content of the src folder to the folder: 
        # package_folder/site_tools/qt5
        self.__copytree__(self.__thirdPartyBuildFolder__,
                          os.path.join(self.__buildInfo__["INCLUDEPATH"][0],"site_tools","qt5"))
        
    def __package__(self):
        
        # All was already done in __compile__
        pass

    def __test__(self):
        
        # We do not test here, because I need qt that 
        # I do not have yet. The test will be performed 
        # while compiling the Qt test
        pass