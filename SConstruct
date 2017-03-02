import os 
from shutil import copyfile
from shutil import copytree

# Define a common build environment
common_env = Environment()

# Our release build is derived from the common build environment...
releaseEnv = common_env.Clone()

# ... and adds a RELEASE preprocessor symbol ...
releaseEnv.Append( CPPDEFINES=['RELEASE'] )

releaseEnv.Append( root_dir = Dir('.').srcnode().abspath )
releaseEnv.Append( variant_dir = 'build/release' )

# ... and release builds end up in the "build/release" dir
releaseEnv.VariantDir( releaseEnv['variant_dir'], 'main', duplicate=0)

# ---------------------------------------------------------------
# Define the location of the third_parties
third_party_root='/Users/dtrimarchi/third_party/'
#Qt lives in its own universe...
qtdir='/usr/local/Cellar/qt5/5.7.0'

#----------------------------------------------------------------

# Returns the absolute path of the Main folder, the root of the source tree
# # /Users/dtrimarchi/VPP/main
def getSrcTreeRoot(self):
    return os.path.join( self['root_dir'], "main" )

releaseEnv.AddMethod(getSrcTreeRoot, 'getSrcTreeRoot')

# --

# Returns the absolute path of the Contents folder in the app bundle
# /Users/dtrimarchi/VPP/VPP.app/Contents
def getAppDirContents(self):

    return os.path.join( self['root_dir'], "VPP.app/Contents" )

releaseEnv.AddMethod(getAppDirContents, 'getAppDirContents')

# --
# Returns the absolute path of the Install folder in the app bundle
# /Users/dtrimarchi/VPP/VPP.app/Contents/MacOS
def getAppInstallDir(self):

    return os.path.join( self.getAppDirContents(), "MacOS")

releaseEnv.AddMethod(getAppInstallDir, 'getAppInstallDir')

# --

# Create (if required) the app folder structure : 
# VPP.app/
#    Contents/
#        info.pList
#        MacOS/
#            VPP.exe
#        Resources/
#            VPP.icns
def makeAppFolderStructure(self):
    
    if not os.path.exists( self.getAppDirContents() ):
        os.makedirs(self.getAppDirContents())
        copyfile( os.path.join( self.getSrcTreeRoot(),"gui/Info.pList"), os.path.join( self.getAppDirContents(),"Info.pList") )

    if not os.path.exists(self.getAppDirContents()+"/Resources"):
        os.makedirs(self.getAppDirContents()+"/Resources")
        copyfile( os.path.join( self.getSrcTreeRoot(), "Icons/VPP.icns"), self.getAppDirContents()+"/Resources/VPP.icns")

releaseEnv.AddMethod(makeAppFolderStructure, 'makeAppFolderStructure')

#----------------------------------------------------------------

# Also copy the input file 'variableFile.txt' to the build folder
def copyInputFileToFolderStructure(self):
    
    srcFile= str(File('variableFile.txt').srcnode())
    destFile=  self['root_dir'] + '/' + self['variant_dir'] +'/variableFile.txt'
    copyfile(srcFile,destFile)

releaseEnv.AddMethod(copyInputFileToFolderStructure, 'copyInputFileToFolderStructure')

#----------------------------------------------------------------

def getGlobalIncludePath(self) : 
    self.Append( CPPPATH=["/opt/local/include" ] ) 

releaseEnv.AddMethod(getGlobalIncludePath, 'getGlobalIncludePath')

def getGlobalLibPath(self) : 
    self.Append( LIBPATH=[
                          "/System/Library/Frameworks/Accelerate.framework/Versions/Current/Frameworks/vecLib.framework/Versions/A",
                          '/opt/local/lib'] ) 

releaseEnv.AddMethod(getGlobalLibPath, 'getGlobalLibPath')

#--

def getEigenIncludePath(self) : 
    self.Append( CPPPATH=[third_party_root+'eigen-3.2.7'] ) 

releaseEnv.AddMethod(getEigenIncludePath, 'getEigenIncludePath')

#-- 

def getUMFPackIncludePath(self) : 
    self.Append( CPPPATH=[third_party_root+"SuiteSparse-4.5.2/UMFPACK/Include",
                          third_party_root+"SuiteSparse-4.5.2/SuiteSparse_config",
                          third_party_root+"SuiteSparse-4.5.2/AMD/Include" ] ) 

releaseEnv.AddMethod(getUMFPackIncludePath, 'getUMFPackIncludePath')

def getUMFPackLib(self) : 
    self.Append( LIBS=["umfpack",
                       "BLAS",
                       "amd",
                       "colamd",
                       "cholmod",
                       "suitesparseconfig" ] ) 

releaseEnv.AddMethod(getUMFPackLib, 'getUMFPackLib')

def getUMFPackLibPath(self) : 
    self.Append( LIBPATH=[third_party_root+"SuiteSparse-4.5.2/UMFPACK/Lib",
                          third_party_root+"SuiteSparse-4.5.2/SuiteSparse_config",
                          third_party_root+"SuiteSparse-4.5.2/AMD/Lib",
                          third_party_root+"SuiteSparse-4.5.2/CHOLMOD/Lib",
                          third_party_root+"SuiteSparse-4.5.2/COLAMD/Lib" ] ) 

releaseEnv.AddMethod(getUMFPackLibPath, 'getUMFPackLibPath')

#-- 

def getBoostIncludePath(self) : 
    self.Append( CPPPATH=[third_party_root+"boost_1_60_0" ] ) 

releaseEnv.AddMethod(getBoostIncludePath, 'getBoostIncludePath')

#-- 

def getNlOptIncludePath(self) : 
    self.Append( CPPPATH=[third_party_root+"nlopt-2.4.2/api" ] ) 

releaseEnv.AddMethod(getNlOptIncludePath, 'getNlOptIncludePath')

def getNLOptLib(self) : 
    self.Append( LIBS=["nlopt" ] ) 

releaseEnv.AddMethod(getNLOptLib, 'getNLOptLib')

def getNLOptLibPath(self) : 
    self.Append( LIBPATH=[third_party_root+"nlopt-2.4.2/libs" ] ) 

releaseEnv.AddMethod(getNLOptLibPath, 'getNLOptLibPath')

#-- 

def getIpOptIncludePath(self) : 
    self.Append( CPPPATH=[ third_party_root+'Ipopt-3.12.6/Ipopt/src/Interfaces',
                           third_party_root+'Ipopt-3.12.6/include/coin'] ) 

releaseEnv.AddMethod(getIpOptIncludePath, 'getIpOptIncludePath')

def getIpOptLib(self) : 
    self.Append( LIBS=["ipopt" ] ) 

releaseEnv.AddMethod(getIpOptLib, 'getIpOptLib')

def getIpOptLibPath(self) : 
    self.Append( LIBPATH=[third_party_root+"Ipopt-3.12.6/lib" ] ) 

releaseEnv.AddMethod(getIpOptLibPath, 'getIpOptLibPath')

#-- 

def getPlPlotIncludePath(self) : 
    self.Append( CPPPATH=["/opt/local/share/plplot5.11.1/examples/c++",
                          '/opt/local/include/plplot'] ) 

releaseEnv.AddMethod(getPlPlotIncludePath, 'getPlPlotIncludePath')

def getPlPlotLib(self) : 
    self.Append( LIBS=["plplot",
                       "plplotcxx"] ) 

releaseEnv.AddMethod(getPlPlotLib, 'getPlPlotLib')

#--

def getCppUnitIncludePath(self) : 
    self.Append( CPPPATH=[third_party_root+"cppunit-1.13.2/build/include"] ) 

releaseEnv.AddMethod(getCppUnitIncludePath, 'getCppUnitIncludePath')

def getCppUnitLib(self) : 
    self.Append( LIBS=["cppunit"] ) 

releaseEnv.AddMethod(getCppUnitLib, 'getCppUnitLib')

def getCppUnitLibPath(self) : 
    self.Append( LIBPATH=[third_party_root+"cppunit-1.13.2/build/lib" ] ) 

releaseEnv.AddMethod(getCppUnitLibPath, 'getCppUnitLibPath')

#--

def getQtPKGConfig(self,qtEnv):

    qtEnv['ENV']['PKG_CONFIG_PATH'] = [ os.path.join(qtdir,'lib/pkgconfig') ]    
    qtEnv['ENV']['PATH'] += ':/opt/local/bin:/usr/local/Cellar/qt5/5.7.0/bin'
    
releaseEnv.AddMethod(getQtPKGConfig, 'getQtPKGConfig')

#--

def getQt(self):
    
    self.Append( QT5DIR = qtdir ) 
    
    # This is for http://stackoverflow.com/questions/37897209/qt-requires-c11-support-make-error
    self.Append( CXXFLAGS =  ['-std=c++11'] )

    self.Tool('qt5')

    self.EnableQt5Modules([
                      'QtGui',
                      'QtCore',
                      'QtNetwork',
                      'QtWidgets',
                      'QtCharts',
                      'QtDataVisualization', 
                      'QtPrintSupport'
                      ])

    self.Append( CPPPATH=['/usr/local/Cellar/qt5/5.7.0/include/QtWidgets'] ) 
    self.Append( LIBPATH=[ os.path.join(qtdir,'lib') ] )     

    #Compile Debug mode
    #env.Append(CCFLAGS= '-g')
    #env.Append(CPPFLAGS ='-Wc++11-extensions')

    #Compile Release
    #self.Append(CPPDEFINES=['RELEASE'])

releaseEnv.AddMethod(getQt, 'getQt')
    
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
 
mode = 'release'
releaseEnv.SConscript('build/%s/SConscript' % mode,{'releaseEnv': releaseEnv} )

