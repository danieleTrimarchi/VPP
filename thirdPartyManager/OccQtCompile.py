from thirdPartyCompile import thirdPartyCompile
import os, shutil, sys, glob, fnmatch
import multiprocessing
from qt5SconsTool import qt5SconsTool
from QtCompileBase import QtCompileBase
from QtCompile import QtCompile
from OpenCascadeCompile import OpenCascadeCompile
    
        
# This is not a full third_party. Actually it is a simple usage example 
# of how to use OpenCascade with Qt and create a visualizer. We indeed 
# need to bring this in the code, so I believe it is a good idea to 
# store that knowledge here for the future developments
class OccQtCompile(QtCompileBase):
    
    # Ctor
    def __init__(self):
        
        # Simply call mother-class init
        super(OccQtCompile,self).__init__()

        # Name of this third_party. 
        self.__name__="OocQt"
        
        # Version of this third_party. 
        self.__version__="1"
        
        # Define the URL from which IpOpt can be downloaded
        # Note that this should be the complete path of the file
        # to be downloaded (Including the filename!)
        self.__url__="https://github.com/eryar/occQt/archive/master.zip"

        # Define the name of the archive downloadeed from the web. A priori this is 
        # simply the last entry of the url
        self.__srcArchiveName__="occQt-master"
        
        # Define the name of the folder extracted from the archive downloadeed from the web. 
        # A priori this is simply the srcArchiveName without the extension
        self.__srcDirName__="occQt-master"
        
        # Override (specialize) the build folder 
        self.__thirdPartyBuildFolder__= os.path.join(self.__thirdPartyBuildFolder__,self.__srcDirName__)

        # Override (specialize) the package folder 
        self.__thirdPartyPkgFolder__= os.path.join(self.__thirdPartyPkgFolder__,self.__srcDirName__)

        # Instantiate the requirement: Qt and OpenCascade        
        self.__requirements__.append( QtCompile() )
        self.__requirements__.append( OpenCascadeCompile() )

        # Define the build info. Will use these to copy the components (includes, libs...) to 
        # the package folder
        self.__buildInfo__["LIBPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"lib")]
        self.__buildInfo__["DOCPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"doc")]
        self.__buildInfo__["INCLUDEPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"include")]
        self.__buildInfo__["BINPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"bin")]
        self.__buildInfo__["LIBS"] = []
          
    # Compile this package    
    def __compile__(self,dest=None) : 

        # there is nothing to compile here. This is not a third_party but
        # an example that will be compiled and executed by the test method
        # Call the mother class compile but specify to NOT compile anything!
        super(OccQtCompile,self).__compile__(makeRequirements=False)
                          
    # Package the third party that was build   
    def __package__(self):

        # Make sure to start in the build folder
        os.chdir(self.__thirdPartyBuildFolder__)
              
        # Decorate the mother class __package__ method
        super(OccQtCompile,self).__package__()
        
        # Handle the .pc files
        self.__copyPkgConfigFiles__()

    # The test is visual, run the exe that was packaged
    def __test__(self):

        # Decorate the mother class __test__ method
        super(OccQtCompile,self).__test__()

        # Copy the source files from the src folder to the 
        self.__copyFiles__(self.__thirdPartyBuildFolder__,os.getcwd())

        # Instantiate the reqs to get all the paths
        binPathRequirements=self.stringify(self.__buildInfo__["BINPATH"])
        for iReq in self.__requirements__:
            binPathRequirements += self.stringify( iReq.__buildInfo__["BINPATH"] )

        libPathRequirements=self.__buildInfo__["LIBPATH"]        
        for iReq in self.__requirements__:
            libPathRequirements.extend( iReq.__buildInfo__["LIBPATH"] )

        includePathRequirements=self.__buildInfo__["INCLUDEPATH"]
        for iReq in self.__requirements__:
            includePathRequirements.extend( iReq.__buildInfo__["INCLUDEPATH"] )
        
        # Get a handle on the requirements
        qt = self.__getReqByName__("Qt")
        occ = self.__getReqByName__("OpenCascade")
        
        buildExeName= "OccQt"

        # Write the SConstruct
        Sconstruct=open("SConstruct","w")
        Sconstruct.write('''
import os
import subprocess

env = Environment().Clone()
executableName= \"{exe}\"

qtdir = \"{qtDir}\" 
env['QT5DIR']= qtdir
env['ENV']['PKG_CONFIG_PATH'] = [ os.path.join(qtdir,\"qtPkgConfig\") ]
env['ENV']['PATH'] += \":/opt/local/bin{binPath}\"
env['CXXFLAGS'] +=  ['-std=c++11']

env['LINKFLAGS'] += [\"{rPathLibPath}\"] 

env.Tool('qt5')
env.EnableQt5Modules([
                      'QtGui',
                      'QtCore',
                      'QtNetwork',
                      'QtWidgets',
                      'QtOpenGL'
                      ])
QT_LIBS={qtLibs}
QT_INCS= {includePath}
QT_LIBPATH= {libPath}
env.Program(executableName, Glob('*.cpp')+ Glob('*.qrc'),
            CPPPATH = QT_INCS,
            LIBS = QT_LIBS,
            LIBPATH = QT_LIBPATH )        
'''.format(exe= buildExeName,
           qtDir= qt.__thirdPartyPkgFolder__,
           binPath= binPathRequirements,
           qtLibs=occ.getLibs(),
           includePath= includePathRequirements,
           rPathLibPath = self.makeRPath(libPathRequirements),
           libPath= libPathRequirements))
        Sconstruct.close()        

        # Get the qt5 scons build tool and make a symbolic link        
        os.symlink(qt5SconsTool().getIncludePath()[0], "site_scons")
    
        # Patch the source 
        self.__patch__(pattern="#include <QApplication>",
                       replace="#include <QApplication>\n#include <QDir>\n",
                       srcFileName="main.cpp")

        self.__patch__(pattern="QApplication a(argc, argv);",
                       replace='''
// Instantiate the utilities to get the external resources such
// as the icons
Q_INIT_RESOURCE(OccQt);
 
QApplication a(argc, argv);
 
QDir dir(QCoreApplication::applicationDirPath());
dir.cdUp();
dir.cd("PlugIns/platforms");
QCoreApplication::setLibraryPaths(QStringList(dir.absolutePath()));
'''
                        ,srcFileName="main.cpp") 
            
        self.__patch__(pattern="#include <QGLWidget>",
                       replace="#include <QtOpenGL/QGLWidget>",
                       srcFileName="OccView.h")                        

        os.rename("occqt.qrc", "OccQt.qrc")
        
        # Execute qmake on the .pro file            
        self.__execute__("{qmake} -o Makefile occQt.pro".format(
                            qmake=os.path.join(qt.getBinPath()[0],"qmake")))
        
        # Execute make to generate the ui_occtqt file. Make will fail but it is not a pb, 
        # once the ui_ file is there we can build more conveniently with SCons
        try:
            print "-----------------------------------------------------"
            print "WARNING: Make will fail but this is not a problem! "
            print "We are using the make to build the file : ui_occQt.h"
            self.__execute__("make")
            print "-----------------------------------------------------"
        except:
            pass
            
        # Compile the example
        self.__execute__("scons -Q")
    
        print "buildPath=", self.__buildInfo__ 
        # Execute the example
        self.__execute__("export DYLD_LIBRARY_PATH=\"{libpath}\"; ./{exe}".format(
                            libpath=qt.__buildInfo__["LIBPATH"][0],
                            exe=buildExeName ))        
