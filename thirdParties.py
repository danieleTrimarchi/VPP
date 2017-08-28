import os
from shutil import copyfile
from shutil import copytree
import subprocess
import shutil

class thirdParty(object) : 
    
    def __init__(self):
        
        # Declare the root-folder for all third_parties
        self.__rootDir__ = "/Users/dtrimarchi/third_party/"

        # Declare class members, to be filled by the children
        self.__includePath__= ""
        self.__libpath__= ""
        self.__libs__= []

    # Public method, called by children once the 
    # members have been defined
    def __addTo__(self, environment):
        
        environment.Append( CPPPATH= self.__includePath__ )
        environment.Append( LIBPATH= self.__libpath__ )
        environment.Append( LIBS= self.__libs__ )

    # Retutrn the list of libs (to be overwritten by children)         
    def libs(self):
        return []

class Eigen( thirdParty ) :

    def __init__(self, env):
        
        # Call mother-class constructor
        super(Eigen,self).__init__()
        
        # Declare class members, to be filled by the children
        self.__includePath__= [self.__rootDir__ + 'eigen-3.2.7']
        self.__libpath__= []
        self.__libs__= []
    
        self.__addTo__(env)
        
#-- 

class NLOpt( thirdParty ) :

    def __init__(self, env):
        
        # Call mother-class constructor
        super(NLOpt,self).__init__()
        
        # Declare class members, to be filled by the children
        self.__includePath__= [self.__rootDir__ + 'nlopt-2.4.2/api']
        self.__libpath__= [self.__rootDir__ + 'nlopt-2.4.2/libs']
        self.__libs__= ['nlopt']
    
        self.__addTo__(env)
        
#-- 

class IPOpt( thirdParty ) :

    def __init__(self, env):
        
        # Call mother-class constructor
        super(IPOpt,self).__init__()
        
        # Declare class members, to be filled by the children
        self.__includePath__= [self.__rootDir__ + 'Ipopt-3.12.6/Ipopt/src/Interfaces',
                               self.__rootDir__ +'Ipopt-3.12.6/include/coin']
        self.__libpath__= [self.__rootDir__ + 'Ipopt-3.12.6/lib']
        self.__libs__= ['ipopt']
    
        self.__addTo__(env)

# -- 

class Boost( thirdParty ) :

    def __init__(self, env):
        
        # Call mother-class constructor
        super(Boost,self).__init__()
        
        # Declare class members, to be filled by the children
        self.__includePath__= [self.__rootDir__ + 'boost_1_60_0']
        self.__libpath__= []
        self.__libs__= []
    
        self.__addTo__(env)

# -- 

class CppUnit( thirdParty ) :

    def __init__(self, env):
        
        # Call mother-class constructor
        super(CppUnit,self).__init__()
        
        # Declare class members, to be filled by the children
        self.__includePath__= [self.__rootDir__ + 'cppunit-1.13.2/build/include']
        self.__libpath__= [self.__rootDir__ + 'cppunit-1.13.2/build/lib']
        self.__libs__= ['cppunit']
    
        self.__addTo__(env)

# -- 

class Qt( thirdParty ) :

    def __init__(self, env):
        
        # Call mother-class constructor
        super(Qt,self).__init__()
        
        # Overrride rootDir that is different for Qt as it is installed via homebrew
        self.__rootDir__ = '/usr/local/Cellar/qt5/5.7.0'
        
        # Direcly customize the env for the package config
        env['ENV']['PKG_CONFIG_PATH'] = [ os.path.join(self.__rootDir__,'lib/pkgconfig') ]    
        env['ENV']['PATH'] += ':/opt/local/bin:'+self.__rootDir__

        env.Append( QT5DIR = self.__rootDir__ )
        
        # This is for http://stackoverflow.com/questions/37897209/qt-requires-c11-support-make-error
        env.Append( CXXFLAGS =  ['-std=c++11'] )

        env.Tool('qt5')

        env.EnableQt5Modules( self.libs() )

        self.__includePath__= [ os.path.join(self.__rootDir__,'/include/QtWidgets') ]
        
        # Returns the absolute path of the Qt Framework folder
        self.__libpath__= [  os.path.join(self.__rootDir__, 'lib'), 
                           "/usr/local/opt/qt5/lib"]

        self.__addTo__(env)
        
    # Override method libs to return the Qt framework list
    def libs(self):
        
        return [
            'QtCore',
            'QtGui',
            'QtWidgets',
            'QtDataVisualization', 
            'QtPrintSupport'
            ]

