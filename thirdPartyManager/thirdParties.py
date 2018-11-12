import os
from shutil import copyfile
from shutil import copytree
import subprocess
import shutil
import re


class xCode(object):
    
    def __init__(self, thirdPartyDict):
        
        # Dictionary with the list of third_parties 
        # to be used to compile the VPP project.
        self.__thirdPartyDict__ = thirdPartyDict

        self.__projectFile__ = ""
    # -- 
    
    def makeProjectFile(self, VPPsubFolders):
        
        # Open a new VPP.pro file to be written
        self.__projectFile__ = open("VPP.pro", 'w')

        # Write the file header - Comments only
        self.__writeHeader__()
        
        # Write the modules for Qt, or the string : 
        # Qt += core gui ...
        self.__writeQtModules__()
        
        # Write settings such as target, template, flags...
        self.__writeSettings__()
        
        # Write the include path of all third parties present in the dict - 
        # Except Qt that is handled separately via the __writeQtModules__ method 
        self.__write__(line="INCLUDEPATH += ",
                       functionName="getIncludePath", 
                       prefix="\"", suffix="\"", 
                       excludePackages=["Qt"] )

        # Write the lib path of all third parties present in the dict
        self.__write__("LIBPATH += ", "libPath", "\"", "\"",[] )

        # Write the libs of all third parties present in the dict
        self.__write__("LIBS += ","getLibs", "-l", "",[] )

        # Write the frameworks of all third parties present in the dict
        self.__write__("LIBS += ","getFrameworks","-framework ","",[] )
        
        # Write : QMAKE-LFLAGS+=-F/PATH/TO/FRAMEWORK/dir
        self.__write__("QMAKE-LFLAGS+= ","getFrameworkRoot","-F","",[] )
        
        # Write the source and header file paths
        self.__writeFiles__(VPPsubFolders)

        # Close the VPP.pro project file
        self.__projectFile__.close()
        
        # Call qmake and generate an xcode project 
        p= subprocess.Popen("qmake -spec macx-xcode VPP.pro",shell=True)
        p.wait()

        self.__patchXCodeProject__()
        
     
    # -- 
    
    
    def __writeHeader__(self):
        
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
        self.__projectFile__.write(header)

    # -- 
    
    def __writeQtModules__(self):
        
        Qt = self.__thirdPartyDict__['Qt']
        
        # Loop over the frameworks, remove the pre-pended "Qt"
        # and make the string lower case
        line= "QT += "
        for iFramework in Qt.__frameworks__: 
            line += iFramework.replace("Qt","").lower() + " "
        line += "\n\n"
        
        self.__projectFile__.write(line)

    # -- 

    def __writeSettings__(self) :
        
        line= "TARGET = VPP \n\n"
        self.__projectFile__.write(line)
        
        line= "TEMPLATE = app \n\n"
        self.__projectFile__.write(line)
        
        line='''# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.\n\n'''
        self.__projectFile__.write(line)

        line="DEFINES += QT_DEPRECATED_WARNINGS\n\n"
        self.__projectFile__.write(line)

        # We compile debug mode because this is what we use 
        # xCode for!
        line="CONFIG += qt debug\n\n"
        self.__projectFile__.write(line)
        
        line= "RESOURCES = gui/VPP.qrc \n\n"
        self.__projectFile__.write(line)

        line= "FORMS += gui/MainWindow.ui \n\n"
        self.__projectFile__.write(line)

        line= '''BUILD_DIR =  ../xCodeBuild
OBJECTS_DIR = ../xCodeBuild
MOC_DIR = ../xCodeBuild
RCC_DIR = ../xCodeBuild
UI_DIR = ../xCodeBuild 
DESTDIR = ../xCodeBuild
SYMROOT= ../xCodeBuild\n\n'''
        self.__projectFile__.write(line)

    # -- 

    # Populates 'line' with the specs returned by a function - specified 
    # in 'functionName'. Prepends and appends prefix, suffix to each spec. 
    # i.e: self.__write__(  line="INCLUDEPATH += ",
    #                       functionName="getIncludePath", 
    #                       prefix="\"", suffix="\"", 
    #                       excludePackages=["Qt"] )
    # Will write each include path (except for Qt) in quotes (""):
    #  INCLUDEPATH +=  \ 
    # "/Users/dtrimarchi/third_party/nlopt-2.4.2/api" \ 
    # "/Users/dtrimarchi/third_party/eigen-3.3.4"
    # Finally, writes 'line' to the project file
    def __write__(self, line, functionName, prefix, suffix, excludePackages ):
        
        # Loop on the third_parties in the dictionary
        for depName, iDep in self.__thirdPartyDict__.iteritems() :   
            
            # Do nothing for excluded packages
            if depName in excludePackages:
                continue   
            
            # Loop on the include paths of this dependency
            for iPath in getattr(iDep,functionName)() : 
            
                # Get the third_party include paths. 
                line += " \\ \n" + prefix + iPath + suffix 
                           
        line += "\n\n"  
        self.__projectFile__.write(line)

    # -- 
    
    # Write the source and header file paths
    def __writeFiles__(self, VPPsubFolders) :
        
        line = "SOURCES += main.cxx"
        for iSubFolder in VPPsubFolders : 
            line += " \ \n " + os.path.join(iSubFolder,"*.cpp")
        
        line += "\n\nHEADERS +="
        for iSubFolder in VPPsubFolders : 
            line += " \ \n " + os.path.join(iSubFolder,"*.h")

        line += "\n\n"                                  
        self.__projectFile__.write(line)
        
    def __patchXCodeProject__(self) :
        
        # Read the lines of VPP.xcodeproj/project.pbxproj, modify the
        # required lines (the value of SYMROOT ), write to a new file 
        # and finally rename the new file to VPP.xcodeproj/project.pbxproj
        fXCode = open( "VPP.xcodeproj/project.pbxproj", 'r' )
        fTmp = open("tmpFile", 'w')
     
        newLine=""
        for line in fXCode:
 
            if( re.search("SYMROOT (.*)",line )    ):    
                newLine= line.replace( '/main/', '/' )
     
            else : 
                newLine = line
             
            fTmp.write(newLine)
 
        # Close the file and mv it to its final location
        fTmp.close()
        fXCode.close()
     
        # Now substitute the project.pbxproject with the tmp file that was opportunely modified    
        os.remove( os.path.join("VPP.xcodeproj","project.pbxproj") )
        os.rename("tmpFile", os.path.join("VPP.xcodeproj","project.pbxproj") )
     

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
        self.__frameworksPaths__= []
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
    def getLibs(self):
        return self.__libs__

    # Retutrn the list of include paths        
    def getIncludePath(self):
        return self.__includePath__

    # Retutrn the list of lib paths        
    def libPath(self):
        return self.__libpath__
    
    # Return the name of this thirdParty
    def getName(self):
        return self.__name__
    
    # Return the list of frameworks (to be overwritten by child classes)         
    def getFrameworks(self):
        return self.__frameworks__

    # Retutrn the LIST of framework paths        
    def getFrameworkRoot(self):
        if(len(self.__frameworksPaths__)>1):
            raise "The build system must be modified to comply with len(__frameworksPaths__)>1!"
        return self.__frameworksPaths__

    # --- 

    # Copy all the dynamic library of this thirdparty to some dest folder
    def copyDynamicLibs(self,dest):

        print "In copyDynamicLibs for third_party ", self.getName()
        print "                  ",self.getLibs() 
        for iLibPath in self.libPath():
            for iLib in self.getLibs():
                print "In try for ",self.getName()
                dyLibName= self.getFullDynamicLibName(iLib)
                print "dyLibName= ",dyLibName
                print "libpath=", iLibPath
                print "Copying:", os.path.join(iLibPath,dyLibName)
                print "to:", os.path.join(dest,dyLibName)
                try:
                    shutil.copyfile(os.path.join(iLibPath,dyLibName), 
                                    os.path.join(dest,dyLibName))
                except IOError:
                    print "\n\n===>>>  IOError!\n\n"
                except OSError:
                    print "\n\n===>>>  OSError!\n\n"
                except SpecialFileError:
                    print "\n\n===>>>  SpecialFileError!\n\n"
                except: 
                    print "\n\n===>>>  General Exception!\n\n"

    # --- 

    # Method required to fix the MACOSx app bundle. I hate to be obliged to do this... 
    # Use this method to fix the references of the (VPP) executable to the dynamic libs 
    # and the cross references betweeen the dynamic libs. Invoke install_name_tool to 
    # act directly on the compiled files.
    # dstRelativeToBin is the relative path between dest (the framewoks folder) and the 
    # bin folder, meaning something like ../Plugins/<libName>/<dyLibName>
    # Remember how to inspect binaries: otool -l <binaryFile>
    def fixDynamicLibPath(self,dst,dstRelativeToBin,appInstallDir):   
         
        # Fix the cross refereences of the libraries belonging to this third party
        for jLib in self.__libs__:
            for iLib in self.__libs__: 
                
                if jLib==iLib:
                    continue
                
                ilibFullName = self.getFullDynamicLibName(iLib)
                relPathName = os.path.join(dstRelativeToBin,self.getName(),ilibFullName)
                jlibAbsPath = os.path.join(dst,self.getFullDynamicLibName(jLib))
                self.__execute__("install_name_tool -change {} @executable_path/{} {}".format(
                                                            ilibFullName,
                                                            relPathName,
                                                            jlibAbsPath))

        # Now loop on the libs and fix the references to the libraries directly in the VPP executable
        for iLib in self.__libs__: 
            
            ilibFullName = self.getFullDynamicLibName(iLib)
            relPathName = os.path.join(dstRelativeToBin,self.getName(),ilibFullName)
            self.__execute__("install_name_tool -change {} @executable_path/{} {}/VPP".format(
                                                            ilibFullName,
                                                            relPathName,
                                                            appInstallDir))
            
        # Still we need to fix the frameworks. This can only work on the a 
        # third_party specific basis
        self.__fixFrameworksPath__(appInstallDir)
    
    # -- 
    
    def __fixFrameworksPath__(self,appInstallDir):
        # do nothing 
        return 

    # So, given "ipopt", return "libipopt.dylib"
    # Warning: this method is duplicated in thirdPartyCompile.py.
    # Classes need to merge. todo dtrimarchi
    def getFullDynamicLibName(self,libName):
        return "lib"+libName+".dylib"
    
    # Wraps a call to subprocess and the required diagnostics
    # Warning: this method is duplicated in thirdPartyCompile.py.
    # Classes need to merge. todo dtrimarchi
    def __execute__(self,command,myEnv=os.environ):
        
        print "Executing command: {}".format(command)
        
        p = subprocess.Popen(command,shell=True,env=myEnv)
        if p.wait():
            raise ValueError('\n\nSomething went wrong when trying to execute: {}\n\n'.format(command))


    def __printEnv__(self):
        print "\n----------------\nEnv= "
        for param in os.environ.keys():
                print "%20s %s" % (param,os.environ[param])
        print "\n----------------\n"

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
                           "/opt/local/lib"#,
                           #"/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/System/Library/Frameworks"
                          ]
        
        self.__frameworksPaths__= [
                                # "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/System/Library/Frameworks"
                                  ]
        self.__addTo__(env)

# -- 

class Boost( thirdParty ) :

    def __init__(self, env):
        
        # Call mother-class constructor
        super(Boost,self).__init__()
        
        from BoostCompile import BoostCompile
        boostPkg= BoostCompile()

        self.__name__= boostPkg.__name__
        
        self.__version__ = boostPkg.__version__

        self.__includePath__= boostPkg.getIncludePath()
                
        self.__libpath__= boostPkg.getLibPath()
        
        self.__libs__= boostPkg.getLibs() 
            
        self.__addTo__(env)
    
# -- 

class CppUnit( thirdParty ) :

    def __init__(self, env):
        
        # Call mother-class constructor
        super(CppUnit,self).__init__()

        from CppUnitCompile import CppUnitCompile
        cppUnitPkg= CppUnitCompile()

        self.__name__= cppUnitPkg.__name__

        self.__version__ = cppUnitPkg.__version__

        # Declare class members, to be filled by the children
        self.__includePath__= cppUnitPkg.getIncludePath()
        self.__libpath__= cppUnitPkg.getLibPath()
        self.__libs__= cppUnitPkg.getLibs()
    
        self.__addTo__(env)

# -- 

class Eigen( thirdParty ) :

    def __init__(self, env):
        
        # Call mother-class constructor
        super(Eigen,self).__init__()
        
        self.__name__= "Eigen"

        self.__version__ = "3.3.4"
        # Declare class members, to be filled by the children
        self.__includePath__= [ os.path.join(self.__rootDir__, 'eigen-' + self.__version__) ]
    
        self.__addTo__(env)

# --- 
        
class IPOpt( thirdParty ) :

    def __init__(self, env):
        
        # Call mother-class constructor
        super(IPOpt,self).__init__()
        
        # Instantiate the thirdPartyCompile object
        from ipOptCompile import IpOptCompile
        ipOptPkg= IpOptCompile()

        # Get the name of the third_party
        self.__name__= ipOptPkg.__name__

        # Get the version of the third_party
        self.__version__ = ipOptPkg.__version__
        
        # Declare class members, to be filled by the children
        self.__includePath__= ipOptPkg.getIncludePath()
 
        # set libpath
        self.__libpath__= ipOptPkg.getLibPath()

        # onfuse frameworks and libs        
        self.__frameworksPaths__= ipOptPkg.getLibPath()
        
        # Define the list of libs
        self.__libs__= ipOptPkg.getLibs()
            
        self.__addTo__(env)
        
# -- 

class NLOpt( thirdParty ) :

    def __init__(self, env):
        
        # Call mother-class constructor
        super(NLOpt,self).__init__()
        
        from NlOptCompile import NlOptCompile
        nlOptCompile= NlOptCompile()

        self.__name__= nlOptCompile.__name__
        
        self.__version__ = nlOptCompile.__version__

        # Declare class members, to be filled by the children
        self.__includePath__= nlOptCompile.getIncludePath()
        self.__libpath__= nlOptCompile.getLibPath()
        self.__libs__= nlOptCompile.getLibs()
        
        self.__addTo__(env)
    
# -- 

class Qt( thirdParty ) :

    def __init__(self, env):
        
        # Call mother-class constructor
        super(Qt,self).__init__()

        self.__name__= "Qt"
        
        self.__version__ = "5.9.1"

        # Overrride rootDir that is different for Qt as it is installed via homebrew
        self.__rootDir__ = os.path.join(self.__rootDir__,'Qt',self.__version__)
        #self.__rootDir__ = os.path.join("/usr/local/Cellar/qt5",self.__version__)

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
        env['ENV']['PKG_CONFIG_PATH'] = [ os.path.join(self.__VPPprogramSrcDir__,'site_scons','qtPkgConfig') ]    
        
        env['ENV']['PATH'] += ':/opt/local/bin:'+os.path.join(self.__rootDir__,'clang_64')

        # Remember rootDir is overwritten for Qt!
        env.Append( QT5DIR = os.path.join(self.__rootDir__, 'clang_64') )
        
        # This is for http://stackoverflow.com/questions/37897209/qt-requires-c11-support-make-error
        env.Append( CXXFLAGS =  ['-std=c++11'] )

        env.Tool('qt5')

        env.EnableQt5Modules( self.__frameworks__ )

        self.__frameworksPaths__ = [ os.path.join(self.__rootDir__, 'clang_64', 'lib') ]
        
        self.__includePath__= [
                               # Framework-wise include for SCons
                               os.path.join(self.__frameworksPaths__[0],'QtCore.framework/Versions/Current/Headers'),
                               os.path.join(self.__frameworksPaths__[0],'QtWidgets.framework/Versions/Current/Headers'),
                               os.path.join(self.__frameworksPaths__[0],'QtGui.framework/Versions/Current/Headers')
                               ]
        
        # Returns the absolute path of the Qt Framework folder
        self.__libpath__= []

        self.__addTo__(env)
        
    # -- 
    
    # Fix the cross references of the Qt frameworks using install_name_tool.
    # This is required for the MACOSx bundle
    def __fixFrameworksPath__(self,appInstallDir):
        
        for iFramework in self.getFrameworks(): 
            self.__execute__('install_name_tool -change '
                             '@rpath/{}.framework/Versions/5/{} '
                             '@executable_path/../Frameworks/{}.framework/Versions/Current/{} '
                             '{}/VPP'.format( 
                                        iFramework,iFramework,
                                        iFramework,iFramework,
                                        appInstallDir )) 
