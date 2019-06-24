import os, shutil, sys, glob, fnmatch
from thirdPartyCompile import thirdPartyCompile

# intermediate class used by all Qt based classes, that implements 
# the knowledge required to handle the .pc files 
class QtCompileBase(thirdPartyCompile):

    def __init(self):
                 
        super(QtCompileBase,self).__init__()
    
    
    def __copyPkgConfigFiles__(self) : 
                
        # Now package the .pc files required to compile with scons
        if not os.path.isdir(os.path.join(self.__thirdPartyPkgFolder__,"qtPkgConfig")):
            shutil.copytree("{VPPsrcDir}/site_scons/qtPkgConfig".format(VPPsrcDir=self.__VppSrcTreeFolder__), 
                            os.path.join(self.__thirdPartyPkgFolder__,"qtPkgConfig"))
         
        # Go to the pkgconfig folder
        os.chdir(os.path.join(self.__thirdPartyPkgFolder__,"qtPkgConfig"))
 
        # Update the .pc files with the current version
        self.__patch__("QtRoot = (.*)","QtRoot = \"{}\"".format(self.__thirdPartyPkgFolder__),
                       "update.py")
        self.__patch__("QtVersion = (.*)","QtVersion = \"{}\"".format(self.__version__),
                       "update.py")
        self.__execute__("python update.py")
        