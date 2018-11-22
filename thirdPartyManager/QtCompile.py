from thirdPartyCompile import thirdPartyCompile
import os, shutil, sys
import multiprocessing

class QtCompile(thirdPartyCompile):
    
    # Ctor
    def __init__(self):
        
        # Simply call mother-class init
        super(QtCompile,self).__init__()

        # Name of this third_party. 
        self.__name__="Qt"
        
        # Version of this third_party
        self.__version__="5.9.1"
        
        # Define the URL from which IpOpt can be downloaded
        # Note that this should be the complete path of the file
        # to be downloaded (Including the filename!)
        self.__url__="http://download.qt.io/official_releases/qt/5.11/5.11.2/single/qt-everywhere-src-5.11.2.tar.xz"

        # Define the name of the archive downloadeed from the web. A priori this is 
        # simply the last entry of the url
        self.__srcArchiveName__="qt-everywhere-src-5.11.2.tar.xz"
        
        # Define the name of the folder extracted from the archive downloadeed from the web. 
        # A priori this is simply the srcArchiveName without the extension
        self.__srcDirName__="qt-everywhere-src-5.11.2"
        
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
        self.__buildInfo__["LIBS"] = ['QtCore','QtGui',
                                      'QtWidgets',
                                      #'QtDataVisualization', 
                                      'QtPrintSupport']
          
    # Compile this package    
    def __compile__(self,dest=None) : 

        # Decorate the mother class __package__ method
        super(QtCompile,self).__compile__()
        
        # todo:
        # patch qtbase/configure.pri to avoid requesting for the license type
        
        # Configure the build 
        self.__execute__("./configure "
                         "-prefix {} "                      # Prefix
                         "-release "                        # Build release
                         "-no-debug-and-release "           # Do NOT build release AND debug
                         "-opensource -confirm-license "    # Get the opensource licensing
                         "-no-dbus -no-gstreamer "
                         "-nomake tests -nomake examples".format(   # Do not bother with tests and examples 
                            self.__thirdPartyPkgFolder__))

#         'QtCore',                -> module qtbase
#         'QtGui',                 -> module qtbase
#         'QtWidgets',             -> module qtbase
#         'QtDataVisualization',   -> module qtdatavis3d
#         'QtPrintSupport'         -> module qtbase

        # Modules to be compiled: 
        # TEST : Add qttools to get qtattributionsscanner
        # required for the target: make docs in the hope this
        # fixes the build. If this is the case, this is a bug 
        # in the qt build system...
        compileModules= ['qtbase','qtdatavis3d','QtDataVisualization','qttools']
#???????????????????????????????????????????????????????????????????????????????????
# modules that have been compiled - though they were not explicitely requested. Why? 
#         qtdeclarative
#         qtmultimedia
#         qtsvg
#         qtxmlpatterns        
#???????????????????????????????????????????????????????????????????????????????????
    
        # Multi-threaded compile for selected modules
        for iModule in compileModules:
            self.__execute__("make -j {} module-{}".format(multiprocessing.cpu_count(),iModule))

        # Install the compile code in the -prefix location : the __package__ folder
        self.__execute__("sudo make -j 1 install") 

        # Also make the documentation
        self.__execute__("make docs")
        self.__execute__("make install_docs")
      
                
    # Package the third party that was build   
    def __package__(self):
              
        pass
#         # Decorate the mother class __package__ method
#         super(QtCompile,self).__package__()
#                 
#         # Copy the content of include
#         self.__copytree__(os.path.join(self.__thirdPartyBuildFolder__,"boost"), 
#                           os.path.join(self.__buildInfo__["INCLUDEPATH"][0],"boost"))
# 
#         self.__copytree__(os.path.join(self.__thirdPartyBuildFolder__,"stage","lib"), 
#                           self.__buildInfo__["LIBPATH"][0])
# 
#         # Also copy the documentation of the modules we are compiling
#         self.__copySelectedDocs__()
                                      
    # Run a simple test to check that the compile was successiful
    def __test__(self):

        # Decorate the mother class __test__ method
        super(QtCompile,self).__test__()

        # Write the boost example 
        Source=open("main.cpp","w")
        Source.write('''
#include <iostream>
using std::cout;

int main(int argc, char** argv) {
    return 0; 
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
env.Program('qtTest', Glob('*.cpp') )        
'''.format(self.__buildInfo__["INCLUDEPATH"],
           self.__buildInfo__["LIBPATH"],
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
        self.__execute__("export DYLD_LIBRARY_PATH=\"{}\"; ./qtTest {}".format(self.__buildInfo__["LIBPATH"][0],os.getcwd()))        

    # Import the dynamic libraries from third party to the dest folder (in this case
    # this will be in the app bundle VPP.app/Contents/Frameworks/
    def importDynamicLibs(self,dst):
        #verify what this does!
        # Copy the lib to the dest folder
        for iLib in self.__buildInfo__["LIBS"]: 
            shutil.copyfile(self.getFullDynamicLibName(iLib), 
                            os.path.join(dst,self.getFullDynamicLibName(iLib)))
                    
        