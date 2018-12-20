import os
from shutil import copyfile
from shutil import copytree
import subprocess
import shutil
import re


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
    def getLibPath(self):
        return self.__libpath__
    
    # Return the name of this thirdParty
    def getName(self):
        return self.__name__
    
    # Return the list of frameworks (to be overwritten by child classes)         
    def getFrameworks(self):
        return self.__frameworks__

    # Retutrn the LIST of framework paths        
    def getFrameworkPath(self):
        if(len(self.__frameworksPaths__)>1):
            raise "The build system must be modified to comply with len(__frameworksPaths__)>1!"
        return self.__frameworksPaths__

    # --- 

    # Copy all the dynamic library of this thirdparty to some dest folder
    def copyDynamicLibs(self,dest):

        print "In copyDynamicLibs for third_party ", self.getName()
        print "                  ",self.getLibs() 
        print "                  ",self.getLibPath()
        for iLibPath in self.getLibPath():
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
    # A note about install_name_tool : after the update to Mojave I found two instances of 
    # install_name_tool, in /opt/local/bin/ and /usr/bin/
    # Using pkgutil --file-info it turned out that the updated version was the one in /usr/bin
    # while the version in /opt/local/bin was some leftout from some previous install (?). 
    # So, I have renamed the instance in /opt/local/bin to install_name_tool_old to avoid the error:
    # -----------------------------------------------------------------------------------------------------------
    # install_name_tool: object: /Users/dtrimarchi/VPP/build/thirdPartyManager/release/VPP.app/Contents/MacOS/VPP 
    # malformed object (unknown load command 9)
    # -----------------------------------------------------------------------------------------------------------
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
        
        from EigenCompile import EigenCompile
        eigen= EigenCompile()

        self.__name__= eigen.__name__

        self.__version__ = eigen.__version__
        
        # Declare class members, to be filled by the children
        self.__includePath__= eigen.getIncludePath()
    
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

        from QtCompile import QtCompile
        qt= QtCompile()

        self.__name__= qt.__name__
        
        self.__version__ = qt.__version__

        # Overrride rootDir that is different for Qt as it is installed via homebrew
        self.__rootDir__ = qt.__thirdPartyPkgFolder__

        # Linkline is treated specially by the Qt5 module. Do not add the frameworks 
        self.__addFrameworkLinkLine__= False

        # Direcly customize the env for the package config
        env['ENV']['PKG_CONFIG_PATH'] = [ os.path.join(self.__VPPprogramSrcDir__,'site_scons','qtPkgConfig') ]    
        
        env['ENV']['PATH'] += ':/opt/local/bin:'+qt.__thirdPartyPkgFolder__

        # Remember rootDir is overwritten for Qt!
        env.Append( QT5DIR = os.path.join(qt.__thirdPartyPkgFolder__) )
        
        # This is for http://stackoverflow.com/questions/37897209/qt-requires-c11-support-make-error
        env.Append( CXXFLAGS =  ['-std=c++11'] )

        env.Tool('qt5')

        env.EnableQt5Modules( qt.getLibs() )
        
        self.__includePath__= qt.getIncludePath()
        
        self.__libpath__= qt.getLibPath()

        # Confuse framework path and lib path as the qt package
        # places everything in the same place     
        self.__frameworksPaths__= qt.getLibPath()

        # Confuse the frameworks and the libs too (required for 
        # self.__fixFrameworksPath__()
        self.__frameworks__ = qt.getLibs()
        
        self.__addTo__(env)
        
    # -- 
    
    # Fix the cross references of the Qt frameworks using install_name_tool.
    # This is required for the MACOSx bundle
    def __fixFrameworksPath__(self,appInstallDir):
        
        for iFramework in self.getFrameworks(): 
            self.__execute__('install_name_tool -change '
                             '@rpath/{iFmwk}.framework/Versions/5/{jFmwk} '
                             '@executable_path/../Frameworks/{kFmwk}.framework/Versions/Current/{lFmwk} '
                             '{exePath}/VPP'.format( 
                                        iFmwk=iFramework,jFmwk=iFramework,
                                        kFmwk=iFramework,lFmwk=iFramework,
                                        exePath=appInstallDir )) 
