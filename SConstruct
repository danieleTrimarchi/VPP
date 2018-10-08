import os 
from shutil import copyfile
from shutil import copytree
import subprocess
import shutil
    
from thirdPartyManager import thirdParties

# Define a common build environment
common_env = Environment()

# Our release build is derived from the common build environment...
releaseEnv = common_env.Clone()

# Get the name of the active mercurial branch? 
#p = subprocess.Popen('hg branch', shell=True)
#p.wait()        
#print "BRANCH= ", p.communicate()

p = subprocess.Popen('hg branch', shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
branch= p.stdout.read().strip()
print "Current Mercurial branch: ", branch

# Call scons -Q debug 1
debug= ARGUMENTS.get('debug',0)
if int(debug) :    
    releaseEnv.Append( CPPDEFINES=['DEBUG'] )
    releaseEnv.Append( CCFLAGS='-g' )
    releaseEnv.Append( variant_dir = 'build/{}/debug'.format(branch) )
    buildMode='debug'
    print "==>> building debug mode "

else:
    releaseEnv.Append( CPPDEFINES=['RELEASE'] )
    releaseEnv.Append( variant_dir = 'build/{}/release'.format(branch) )
    buildMode='release'
    print "==>> building release mode "

# Set the terminal title to reflect the branch / buildType
msg="Branch={}; build_type={}".format(branch,buildMode)
pt = subprocess.Popen('echo \"\033]0;{}\007\"'.format(msg), shell=True)

releaseEnv.Append( root_dir = Dir('.').srcnode().abspath )
releaseEnv.Append( variantDirAbsPath = os.path.join( releaseEnv['root_dir' ],releaseEnv['variant_dir']) )

# ... and release builds end up in the "build/release" dir
releaseEnv.VariantDir( releaseEnv['variant_dir'], 'main', duplicate=0)

#----------------------------------------------------------------
# Static utility method
def error(msg):
    print "\n--------------------------------------------------\n"
    print  msg
    print "\n--------------------------------------------------\n"
    Exit(2)
    
#----------------------------------------------------------------

# Returns the absolute path of the Main folder, the root of the source tree
# # /Users/dtrimarchi/VPP/main
def getSrcTreeRoot(self):
    return os.path.join( self['root_dir'], "main" )

releaseEnv.AddMethod(getSrcTreeRoot, 'getSrcTreeRoot')

# --

def getExecutableName(self):

    return "VPP"

releaseEnv.AddMethod(getExecutableName, 'getExecutableName')

# --

# Returns the absolute path of the app bundle
# /Users/dtrimarchi/VPP/VPP.app
def getAppPath(self):
    
    return os.path.join(releaseEnv['variantDirAbsPath'],"VPP.app")

releaseEnv.AddMethod(getAppPath, 'getAppPath')

#-- 

# Returns the absolute path of the Contents folder in the app bundle
# /Users/dtrimarchi/VPP/VPP.app/Contents
def getAppContentsDir(self):
    
    return os.path.join(releaseEnv.getAppPath(),"Contents")

releaseEnv.AddMethod(getAppContentsDir, 'getAppContentsDir')

#-- 

# Returns the absolute path of the Resources folder in the app bundle
# /Users/dtrimarchi/VPP/VPP.app/Contents/Resources
def getAppResourcesDir(self):

    return os.path.join( releaseEnv.getAppContentsDir(), "Resources" )

releaseEnv.AddMethod(getAppResourcesDir, 'getAppResourcesDir')

#-- 

# Returns the absolute path of the Frameworks folder in the app bundle
# /Users/dtrimarchi/VPP/VPP.app/Contents/Frameworks
def getAppFrameworksDir(self):

    return os.path.join( releaseEnv.getAppContentsDir(), "Frameworks" )

releaseEnv.AddMethod(getAppFrameworksDir, 'getAppFrameworksDir')

# --

# Returns the absolute path of the Frameworks folder in the app bundle
# /Users/dtrimarchi/VPP/VPP.app/Contents/Frameworks
def getAppPlugInsDir(self):

    return os.path.join( releaseEnv.getAppContentsDir(), "PlugIns" )

releaseEnv.AddMethod(getAppPlugInsDir, 'getAppPlugInsDir')


# --

# Returns the absolute path of the Frameworks folder in the app bundle
# /Users/dtrimarchi/VPP/VPP.app/Contents/Frameworks
def getAppPlattformsDir(self):

    return os.path.join( releaseEnv.getAppPlugInsDir(), "platforms" )

releaseEnv.AddMethod(getAppPlattformsDir, 'getAppPlattformsDir')


# --


# Returns the absolute path of the Install folder in the app bundle
# /Users/dtrimarchi/VPP/VPP.app/Contents/MacOS
def getAppInstallDir(self):

    return os.path.join( self.getAppContentsDir(), "MacOS")

releaseEnv.AddMethod(getAppInstallDir, 'getAppInstallDir')

#----------------------------------------------------------------

# Create (if required) the app folder structure : 
# VPP.app/
#    Contents/
#        info.pList
#        MacOS/
#            VPP.exe
#        Resources/
#            VPP.icns
#        Frameworks/
#             All Frameworks (Qt)
#        PlugIns/
#            LibraryName/
#                library.dylib
def makeAppFolderStructure(self, thirdPartyDict):
    
    print "===>>>>  makeAppFolderStructure!  <<<<<================"
    
    # info.plist file...
    if not os.path.exists( self.getAppContentsDir() ):
        os.makedirs(self.getAppContentsDir())
        copyfile( os.path.join( self.getSrcTreeRoot(),"gui/Info.pList"), os.path.join( self.getAppContentsDir(),"Info.pList") )
 
    # Resources directory...
    if not os.path.exists(self.getAppResourcesDir() ):     
        os.makedirs(self.getAppResourcesDir() )
        copyfile( os.path.join( self.getSrcTreeRoot(), "Icons/VPP.icns"), os.path.join(self.getAppResourcesDir(),"VPP.icns") )

    # Framework directory...
    if not os.path.exists( self.getAppFrameworksDir() ): 
        os.makedirs( self.getAppFrameworksDir() )

        frameworkRoot= thirdPartyDict['Qt'].getFrameworkRoot()[0]
        frameworkList= thirdPartyDict['Qt'].getFrameworks()
    
        # Copy the Qt frameworks to the APP bundle 
        # Note that at this point what we do is silly, as we do copy over for each build
        # better would be only copying the pieces that we need to copy over!
        for iFramework in frameworkList: 

            print "==>> Copying framework : ", iFramework
            
            # Def the dest folder
            dst = os.path.join( iFramework + ".framework","Versions","Current" )  
            
            # Make the dest folder
            os.makedirs( os.path.join( self.getAppFrameworksDir(), dst ) )
                        
            # Copy the Qt frameworks to the APP bundle - just to the APP dir to start with
            shutil.copy( os.path.join( frameworkRoot, dst, iFramework ),
                         os.path.join( self.getAppFrameworksDir(), dst, iFramework )
                         )
            
            # After the copy give full permissions to the frameworks in the app
            p = subprocess.Popen('chmod -R 755 {}'.format( 
                                                          os.path.join( self.getAppFrameworksDir(), iFramework + ".framework" ) ), 
                                 shell=True)
            p.wait()     
            
    # Plugin directory... Copy here all the required .dylibs         
    if not os.path.exists( self.getAppPlugInsDir() ): 
        os.makedirs( self.getAppPlugInsDir() )

    # Loop on all the third parties 
    for iTh in thirdPartyDict :         
        
        # Get the i-th third party
        iThirdParty = thirdPartyDict[iTh]

        # Does the plugins folder for this third party exist in the app bundle?     
        if not os.path.exists( os.path.join(self.getAppPlugInsDir(),iThirdParty.getName()) ): 
            os.makedirs( os.path.join(self.getAppPlugInsDir(),iThirdParty.getName()) )

            # Assume we have a unique libpath. If we have none, just continue 
            # to the next third_party
            try:
                srcLibPath= iThirdParty.libPath() 
            except:
                continue

            for iLib in iThirdParty.getLibs():
                try:
                    dyLibName= iThirdParty.getFullDynamicLibName(iLib)
#                    print "Copying ", os.path.join(srcLibPath,dyLibName)
#                    print "to... ", os.path.join(self.getAppPlugInsDir(),iThirdParty.getName(),dyLibName)
                    shutil.copyfile(os.path.join(srcLibPath,dyLibName), 
                                    os.path.join(self.getAppPlugInsDir(),iThirdParty.getName(),dyLibName))
                except:
                    continue
                
releaseEnv.AddMethod(makeAppFolderStructure, 'makeAppFolderStructure')

# ---------------------------------------------------------------

def fixDynamicLibPath(self,source,target,env):
    
    print "==>>  fixDynamicLibPath <<=="

    # Modify the executable in order to change @rpath -> @executable_path thus making 
    # it executable. Not sure why @rpath would not work though... 
        
    # Loop on the frameworks
    for iFramework in self['THIRDPARTYDICT']['Qt'].getFrameworks(): 

        print "RUNNING : "
        print ('install_name_tool -change '
               '@rpath/{}.framework/Versions/5/{} '
               '@executable_path/../Frameworks/{}.framework/Versions/Current/{} '
               '{}/VPP'.format( 
                        iFramework,iFramework,
                        iFramework,iFramework,
                        self.getAppInstallDir()
                        )
            )

        # Also change the reference to the frameworks from @rpath to @executable_path
        p = subprocess.Popen('install_name_tool -change '
                             '@rpath/{}.framework/Versions/5/{} '
                             '@executable_path/../Frameworks/{}.framework/Versions/Current/{} '
                             '{}/VPP'.format( 
                                             iFramework,iFramework,
                                             iFramework,iFramework,
                                             self.getAppInstallDir()
                                             ), 
                             shell=True )
        p.wait()
        
        # Now call the method defined in the thirdPartyCompile
        for iTh in self['THIRDPARTYDICT'] :
            iThirdParty = self['THIRDPARTYDICT'][iTh]
            iThirdParty.fixDynamicLibPath(os.path.join(self.getAppPlugInsDir(),iThirdParty.getName()),
                                          "../PlugIns",self.getAppInstallDir())
        
releaseEnv.AddMethod(fixDynamicLibPath, 'fixDynamicLibPath')

# ---------------------------------------------------------------

def fixDynamicLibPathTest(self,source,target,env):
    
    print "==>>  fixDynamicLibPathTest <<=="

    # Modify the executable in order to change @rpath -> @executable_path thus making 
    # it executable. Not sure why @rpath would not work though... 
    testExecutablePath= self['TEST_EXE_PATH']
    
    
    print '==>>' , self['THIRDPARTYDICT']['Qt'].getFrameworkRoot()
    
    # Not ideal, but it does the job by now. getFrameworkRoot returns a list, 
    # because potentially we have mutliple paths. I do not know how to behave in 
    # that case, which is only theoretical by now. So if this is the case I stop
    # the execution. So I am simply post-poning the problem to later on...
    if(len(self['THIRDPARTYDICT']['Qt'].getFrameworkRoot())>1):
        error("Too many framework roots, this is not implemented!")
    QtFrameworkRoot= self['THIRDPARTYDICT']['Qt'].getFrameworkRoot()[0]

    QtFrameworkList= self['THIRDPARTYDICT']['Qt'].getFrameworks()

    # Loop on the frameworks
    for iFramework in QtFrameworkList: 

        print "RUNNING : "
        print ('install_name_tool -change '
               '@rpath/{}.framework/Versions/5/{} '
               '{}/{}.framework/Versions/Current/{} '
               '{}'.format( 
                        iFramework,iFramework,
                        QtFrameworkRoot,iFramework,iFramework,
                        testExecutablePath
                        )
            )

        # Also change the reference to the frameworks from @rpath to @executable_path
        p = subprocess.Popen('install_name_tool -change '
                             '@rpath/{}.framework/Versions/5/{} '
                             '{}/{}.framework/Versions/Current/{} '
                             '{}'.format( 
                                             iFramework,iFramework,
                                             QtFrameworkRoot,iFramework,iFramework,
                                             testExecutablePath
                                             ), 
                             shell=True )
        p.wait()
        
releaseEnv.AddMethod(fixDynamicLibPathTest, 'fixDynamicLibPathTest')

# ---------------------------------------------------------------

# We define our debug build environment in a similar fashion...
#debug_env = common_env.Clone()
#debug_env.Append(CPPDEFINES=['DEBUG'])
#debug_env.VariantDir('build/debug', 'src')

# Now that all build environment have been defined, let's iterate over
# them and invoke the lower level SConscript files.
#for mode, env in dict(release=releaseEnv, 
#                   debug=debug_env).iteritems():
# print 'Mode= ', mode,'  env = ', env
# env.SConscript('build/%s/SConscript' % mode, {'env': env})
releaseEnv.SConscript(os.path.join("build",branch,buildMode,"SConscript"),{'releaseEnv': releaseEnv} )

