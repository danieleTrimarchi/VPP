import os
from shutil import copyfile
from shutil import copytree
import subprocess
import shutil

class xCode(object):
    
    def __init__(self, thirdPartyDict):
        
        # Dictionary with the list of third_parties 
        # to be used to compile the VPP project.
        self.__thirdPartyDict__ = thirdPartyDict

    # -- 
    
    def makeProjectFile(self, VPPsubFolders):
        
        # Open a new VPP.pro file to be written
        projectFile = open("VPP.pro", 'w')

        # Write the file header - Comments only
        self.__writeHeader__(projectFile)
        
        # Write the modules for Qt, or the string : 
        # Qt += core gui ...
        self.__writeQtModules__(projectFile)
        
        # Write settings such as target, template, flags...
        self.__writeSettings__(projectFile)
        
        # Write the include path of all third parties in the dict
        self.__writeIncludePath__(projectFile)

        # Write the include path of all third parties in the dict
        self.__writeLibPath__(projectFile)

        # Write the include path of all third parties in the dict
        self.__writeLibs__(projectFile)
        
        # Write the source and header file paths
        self.__writeFiles__(projectFile,VPPsubFolders)

    # -- 
    
    def __writeHeader__(self, projectFile):
        
        header= '''#-------------------------------------------------
# README dtrimarchi!
#  
# This file is required - with mainWindow.ui - in order for 
# QMake to create a local XCode project with : 
#
# qmake -spec macx-xcode VPP.pro 
# 
# Note that this will NOT set the paths : 
#
#     Per-Configuration Build Product Path
#     Per-Configuration Intermediate Build Files Path
#
# These are defined by the xCode variable SYMROOT
#
# with the effect of creating a local Debug/VPP.app folder
# This is unwanted because ideally we should not pollute 
# the content of 'main' with build outputs, that should be 
# sent to the xCodeBuild folder (See DEST_BIN) 
# By now the easiest possibility is to reset these two 
# variables by hand when regenerating the xCode project
# 
# One (dirty) possibility - when including the xcode project
# generation into a scons script, is to 'sed' VPP.xcodeproj
# and change the value of SYMROOT in VPP.xcodeproj/project.pbxproj
# 
#-------------------------------------------------
'''
        projectFile.write(header)

    # -- 
    
    def __writeQtModules__(self,projectFile):
        
        Qt = self.__thirdPartyDict__['Qt']
        
        # Loop over the frameworks, remove the pre-pended "Qt"
        # and make the string lower case
        line= "QT += "
        for iFramework in Qt.__frameworks__: 
            line += iFramework.replace("Qt","").lower() + " "
        line += "\n\n"
        
        projectFile.write(line)

    # -- 
    
    def __writeSettings__(self,projectFile) :
        
        line= "TARGET = VPP \n\n"
        projectFile.write(line)
        
        line= "TEMPLATE = app \n\n"
        projectFile.write(line)
        
        line='''# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.\n\n'''
        projectFile.write(line)

        line="DEFINES += QT_DEPRECATED_WARNINGS\n\n"
        projectFile.write(line)
        
        line= "RESOURCES = gui/VPP.qrc \n\n"
        projectFile.write(line)

        line= "FORMS += gui/MainWindow.ui \n\n"
        projectFile.write(line)

        line= '''BUILD_DIR =  ../xCodeBuild
OBJECTS_DIR = ../xCodeBuild
MOC_DIR = ../xCodeBuild
RCC_DIR = ../xCodeBuild
UI_DIR = ../xCodeBuild 
DESTDIR = ../xCodeBuild
SYMROOT= ../xCodeBuild\n\n'''
        projectFile.write(line)

    # -- 
    
    def __writeIncludePath__(self,projectFile) :

        line = "INCLUDEPATH += "
                
        # Loop on the third_parties in the dictionary
        for depName, iDep in self.__thirdPartyDict__.iteritems() :      
            # Loop on the include paths of this dependency
            for iPath in iDep.includePath() : 
            
                # Get the third_party include paths. 
                line += "\"" + iPath + "\"" 
                           
                #Add the return statement for all but the last line
                if( depName != self.__thirdPartyDict__.keys()[-1] ) :
                    line += " \\ \n"

                if( depName == self.__thirdPartyDict__.keys()[-1] and iPath != iDep.includePath()[-1] ):
                    line += " \\ \n"

        line += "\n\n"  
        projectFile.write(line)

    # ---

    def __writeLibPath__(self,projectFile) :

        line = "LIBPATH += "
        
        # Loop on the third_parties in the dictionary
        for depName, iDep in self.__thirdPartyDict__.iteritems() : 
            # Loop on the lib paths of this dependency
            for iPath in iDep.libPath() : 

                # Get the third_party include paths. 
                line += "\"" + iPath + "\"" 

                #Add the return statement for all but the last line
                if( depName != self.__thirdPartyDict__.keys()[-1] ) :
                    line += " \\ \n"

                if( depName == self.__thirdPartyDict__.keys()[-1] and iPath != iDep.libPath()[-1] ):
                    line += " \\ \n"
        
        line += "\n\n"                      
        projectFile.write(line)

    # ---

    def __writeLibs__(self,projectFile) :

        line = "LIBS += "
        
        # Loop on the third_parties in the dictionary
        for depName, iDep in self.__thirdPartyDict__.iteritems() : 
            # Loop on the libs of this dependency
            for iLib in iDep.libs() : 

                # Get the third_party include paths. 
                line += "-l" + iLib  

                #Add the return statement for all but the last line
                if( depName != self.__thirdPartyDict__.keys()[-1] ) :
                    line += " \\ \n"

                if( depName == self.__thirdPartyDict__.keys()[-1] and iLib != iDep.libs()[-1] ):
                    line += " \\ \n"
                    
        line += "\n\n"                      
        projectFile.write(line)

    # ---
    
    # Write the source and header file paths
    def __writeFiles__(self,projectFile, VPPsubFolders) :
        
        line = "SOURCES += main.cxx \ \n"
        for iSubFolder in VPPsubFolders : 
            line += os.path.join(iSubFolder,"*.cpp")
            if(iSubFolder != VPPsubFolders[-1] ):
                line += " \ \n"
        
        line += "\n\nHEADERS +="
        for iSubFolder in VPPsubFolders : 
            line += os.path.join(iSubFolder,"*.h")
            if(iSubFolder != VPPsubFolders[-1] ):
                line += " \ \n"

        line += "\n\n"                                  
        projectFile.write(line)

# ----------------------------------------

class thirdParty(object) : 
    
    def __init__(self):
        
        # Declare the root-folder for all third_parties
        self.__rootDir__ = "/Users/dtrimarchi/third_party"
        
        # Declare the src folder for the program to be compiled. Not ideal...
        # This is done because the Qt .pc files currently live in the src tree... 
        self.__VPPprogramSrcDir__ = "/Users/dtrimarchi/VPP"

        # Define the name of this third_party
        self.__name__= ""
        
        # Switch used to add the framweworks to the linking line. 
        # This is set to False for third_parties that treat linking 
        # in a special way, such as for example Qt
        self.__addFrameworkLinkLine__= True
        
        # Declare class members, to be filled by the children
        self.__includePath__= []
        self.__frameworksPath__= []
        self.__libpath__= []

        self.__libs__= []
        self.__frameworks__= []
        
        self.__version__= ""

    # Public method, called by children once the 
    # members have been defined
    def __addTo__(self, environment):
        
        environment.Append( CPPPATH= self.__includePath__ )
        environment.Append( LIBPATH= self.__libpath__ )
        environment.Append( LIBS= self.__libs__ )
        
        if(self.__addFrameworkLinkLine__):
            for iFramework in self.__frameworks__:
                environment.Append( LINKFLAGS= ['-framework', iFramework ] )
        
    # Retutrn the list of libs (to be overwritten by child classes)         
    def libs(self):
        return self.__libs__

    # Retutrn the list of include paths        
    def includePath(self):
        return self.__includePath__

    # Retutrn the list of lib paths        
    def libPath(self):
        return self.__libpath__
    
    # Retutrn the list of frameworks (to be overwritten by child classes)         
    def getFrameworks(self):
        return self.__frameworks__

    def getFrameworkRoot(self):
        return self.__frameworksPath__

# --- 

# Place-holder class for the system global libs 
class System( thirdParty ) :

    def __init__(self, env):
        
        # Call mother-class constructor
        super(System,self).__init__()
    
        self.__name__= "System"
        
        self.__frameworks__= ["CoreFoundation"]
        
        self.__libpath__= [
                           "/System/Library/Frameworks/Accelerate.framework/Versions/Current/Frameworks/vecLib.framework/Versions/A",
                           "/opt/local/lib",
                           "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/System/Library/Frameworks"
                          ]
        
        self.__addTo__(env)

# --- 

class Eigen( thirdParty ) :

    def __init__(self, env):
        
        # Call mother-class constructor
        super(Eigen,self).__init__()
        
        self.__name__= "Eigen"

        self.__version__ = "3.2.7"
        # Declare class members, to be filled by the children
        self.__includePath__= [ os.path.join(self.__rootDir__, 'eigen-' + self.__version__) ]
    
        self.__addTo__(env)
        
#-- 

class NLOpt( thirdParty ) :

    def __init__(self, env):
        
        # Call mother-class constructor
        super(NLOpt,self).__init__()
        
        self.__name__= "NLOpt"
        
        self.__version__ = "2.4.2"

        # Declare class members, to be filled by the children
        self.__includePath__= [ os.path.join( self.__rootDir__,'nlopt-' + self.__version__,'api') ]
        self.__libpath__= [ os.path.join( self.__rootDir__,'nlopt-' + self.__version__,'libs')]
        self.__libs__= ['nlopt']
    
        self.__addTo__(env)
        
#-- 

class IPOpt( thirdParty ) :

    def __init__(self, env):
        
        # Call mother-class constructor
        super(IPOpt,self).__init__()
        
        self.__name__= "IPOpt"

        self.__version__ = "3.12.6"

        # Declare class members, to be filled by the children
        self.__includePath__= [os.path.join(self.__rootDir__,'Ipopt-'+self.__version__,'Ipopt/src/Interfaces'),
                               os.path.join(self.__rootDir__,'Ipopt-'+self.__version__,'include/coin') ]
        self.__libpath__= [ os.path.join(self.__rootDir__,'Ipopt-'+self.__version__,'lib') ]
        
        self.__frameworksPath__= os.path.join(self.__rootDir__,'Ipopt-'+self.__version__,'lib')

        # Define the list of libs
        self.__libs__= ['ipopt']
        
        # Define the list of frameworks         
        #self.__frameworks__= [
        #                      'libipopt.1.dylib', 
        #                      'libcoinmumps.1.dylib', 
        #                      'libcoinmetis.1.dylib'
        #                    ]
    
        self.__addTo__(env)

# -- 

class Boost( thirdParty ) :

    def __init__(self, env):
        
        # Call mother-class constructor
        super(Boost,self).__init__()
        
        self.__name__= "Boost"

        self.__version__ = "1_60_0"

        # Declare class members, to be filled by the children
        self.__includePath__= [ os.path.join( self.__rootDir__,'boost_'+self.__version__) ]
        self.__libpath__= [ os.path.join( self.__rootDir__,'boost_'+self.__version__,
                                          "bin.v2","libs","system","build","darwin-4.2.1",
                                          "release","link-static","threading-multi"),
                           os.path.join( self.__rootDir__,'boost_'+self.__version__,
                                          "bin.v2","libs","fileSystem","build","darwin-4.2.1",
                                          "release","link-static","threading-multi")                            
                           ]
        
        self.__libs__= ["boost_system","boost_filesystem"]
    
        self.__addTo__(env)

# -- 

class CppUnit( thirdParty ) :

    def __init__(self, env):
        
        # Call mother-class constructor
        super(CppUnit,self).__init__()

        self.__name__= "CppUnit"

        self.__version__ = "1.13.2"

        # Declare class members, to be filled by the children
        self.__includePath__= [ os.path.join(self.__rootDir__,'cppunit-'+self.__version__,'build/include') ]
        self.__libpath__= [ os.path.join(self.__rootDir__,'cppunit-'+self.__version__,'build/lib') ]
        self.__libs__= ['cppunit']
    
        self.__addTo__(env)

# -- 

class Qt( thirdParty ) :

    def __init__(self, env):
        
        # Call mother-class constructor
        super(Qt,self).__init__()

        self.__name__= "Qt"
        
        # ==>> self.__version__ = "5.9.1"
        self.__version__ = "5.7.0"

        # Overrride rootDir that is different for Qt as it is installed via homebrew
        # ==>> self.__rootDir__ = os.path.join(self.__rootDir__,'Qt',self.__version__,'clang_64')
        self.__rootDir__ = os.path.join("/usr/local/Cellar/qt5",self.__version__)

        # Linkline is treated specially by the Qt5 module. Do not add the frameworks 
        self.__addFrameworkLinkLine__= False

        # Define the list of the libs (frameworks in this case)         
        self.__frameworks__= [
                              'QtCore',
                              'QtGui',
                              'QtWidgets',
                              'QtDataVisualization', 
                              'QtPrintSupport'
                              ]

        # Direcly customize the env for the package config
        # ==>> env['ENV']['PKG_CONFIG_PATH'] = [ os.path.join(self.__VPPprogramSrcDir__,'site_scons','qtPkgConfig') ]    
        env['ENV']['PKG_CONFIG_PATH'] = [ os.path.join(self.__VPPprogramSrcDir__,'site_scons','qt_5_7_PkgConfig') ]  
        
        env['ENV']['PATH'] += ':/opt/local/bin:'+self.__rootDir__

        # Remember rootDir is overwritten for Qt!
        env.Append( QT5DIR = self.__rootDir__ )
        
        # This is for http://stackoverflow.com/questions/37897209/qt-requires-c11-support-make-error
        env.Append( CXXFLAGS =  ['-std=c++11'] )

        env.Tool('qt5')

        env.EnableQt5Modules( self.__frameworks__ )

        self.__frameworksPath__ = os.path.join(self.__rootDir__,'lib')

        self.__includePath__= [ os.path.join(self.__frameworksPath__,'QtCore.framework/Versions/Current/Headers'),
                                os.path.join(self.__frameworksPath__,'QtWidgets.framework/Versions/Current/Headers'),
                                os.path.join(self.__frameworksPath__,'QtGui.framework/Versions/Current/Headers')
                               ]
        
        # Returns the absolute path of the Qt Framework folder
        self.__libpath__= [ os.path.join(self.__rootDir__, 'lib') ]

        self.__addTo__(env)
        
