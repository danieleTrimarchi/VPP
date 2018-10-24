from thirdPartyCompile import thirdPartyCompile
import os
import shutil
import sys 

# Blas is a requirement for IpOopt, that offers utility scripts to download and 
# compile blas with the right bindings
class ipOptBlasCompile(thirdPartyCompile):

    # Ctor
    def __init__(self,ipOptBlasPath):
    
        # Simply call mother-class init
        super(ipOptBlasCompile,self).__init__()

        # Name of this third_party. 
        self.__name__="Blas"

        # Store the location where ipOpt will download and 
        # compile blas
        self.__thirdPartySrcFolder__ = ipOptBlasPath
        
        # We do not need to fill other fileds, as this thirdParty is 
        # entirely handled by IpOpt
            
    def __download__(self):
        
        # Use the IpOpt utilities to download BLAS from the web and compile
        # Note that the __thirdPartySrcFolder__ has been set to the ipOpt/third_party/
        # blas folder
        os.chdir(self.__thirdPartySrcFolder__)

        # Make sure the BLAS script has 755 permissions and execute it
        self.__execute__("chmod 755 get.Blas")
        self.__execute__("./get.Blas")

    def __compile__(self,dest=None):
        # Make nothing, BLAS will be compiled as part of IpOpt
        pass 
    
# Lapack is a requirement for IpOopt, that offers utility scripts to download and 
# compile blas with the right bindings
class ipOptLapackCompile(thirdPartyCompile):

    # Ctor
    def __init__(self,ipOptLapackPath):
    
        # Simply call mother-class init
        super(ipOptLapackCompile,self).__init__()

        # Name of this third_party. 
        self.__name__="Lapack"

        # Store the location where ipOpt will download and 
        # compile blas
        self.__thirdPartySrcFolder__ = ipOptLapackPath
        
        # We do not need to fill other fileds, as this thirdParty is 
        # entirely handled by IpOpt
            
    def __download__(self):
        
        # Use the IpOpt utilities to download BLAS from the web and compile
        # Note that the __thirdPartySrcFolder__ has been set to the ipOpt/third_party/
        # blas folder
        os.chdir(self.__thirdPartySrcFolder__)

        # Make sure the BLAS script has 755 permissions and execute it
        self.__execute__("chmod 755 get.Lapack")
        self.__execute__("./get.Lapack")

    def __compile__(self,dest=None):
        # Make nothing, LAPACK will be compiled as part of IpOpt
        pass 

# ASL is a requirement for IpOopt, that offers utility scripts to download and 
# compile blas with the right bindings
class ipOptASLCompile(thirdPartyCompile):

    # Ctor
    def __init__(self,ipOptASLPath):
    
        # Simply call mother-class init
        super(ipOptASLCompile,self).__init__()

        # Name of this third_party. 
        self.__name__="ASL"

        # Store the location where ipOpt will download and 
        # compile blas
        self.__thirdPartySrcFolder__ = ipOptASLPath
        
        # We do not need to fill other fileds, as this thirdParty is 
        # entirely handled by IpOpt
            
    def __compile__(self,dest=None):
        # Make nothing, ASL will be compiled as part of IpOpt
        pass 

    def __download__(self):
        
        # Use the IpOpt utilities to download ASL from the web and compile
        # Note that the __thirdPartySrcFolder__ has been set to the ipOpt/third_party/
        # ASL folder
        os.chdir(self.__thirdPartySrcFolder__)

        # Make sure the ASL script has 755 permissions and execute it
        self.__execute__("chmod 755 get.ASL")
        self.__execute__("./get.ASL")

# HSL is a requirement for IpOopt. The doownload link must be requested 
# by the user, who will receive a download link by email. We explicitely
#  request the download link to the user 
class IpOptHSLCompile(thirdPartyCompile):
    
    # Ctor
    def __init__(self):
        
        # Simply call mother-class init
        super(IpOptHSLCompile,self).__init__()

        # Name of this third_party. 
        self.__name__="HSL"

        # Define the name of the archive downloaded from the web.
        self.__srcArchiveName__="tar.tar"

        # Define the name of the folder extracted from the archive downloadeed from the web. 
        # Often  this is simply the srcArchiveName without the extension, but not for HSL!
        self.__srcDirName__="coinhsl-archive-2014.01.17"

        # Override (specialize) the build folder. HSL only requires to be downloaded, and will
        # be build by the ipOpt build system. So the build folder might be of no interest
        #self.__thirdPartyBuildFolder__= os.path.join(self.__thirdPartyBuildFolder__,self.__srcDirName__)

        # Override (specialize) the package folder. HSL only requires to be downloaded, and will
        # be build by the ipOpt build system. So the pkg folder might be of no interest
        #self.__thirdPartyPkgFolder__= os.path.join(self.__thirdPartyPkgFolder__,self.__srcDirName__)
        
    def __download__(self):
            
        # Define the URL from which IpOpt can be downloaded
        # Note that this should be the complete path of the file
        # to be downloaded (Including the filename!)
        print "\n\nHSL cannot be automatically downloaded, please"
        print "request a download link on : "
        print "     http://www.hsl.rl.ac.uk/ipopt/ "
        print "\nThe download link will be sent to you by email.\n"
        self.__url__ = raw_input("Please enter the HSL download link...\n"); 

        # Go to the __thirdPartySrcFolder__. Its existence was 
        # assured in the init of the class
        os.chdir(self.__thirdPartySrcFolder__)

        # cleanup: remove a previous archive if present
        shutil.rmtree(self.__srcArchiveName__,sys.exc_info())

        # Get the sources from the web
        self.__getCompressedArchive__(self.__url__)

        # Also make sure the install-sh script has 755 permissions 
        self.__execute__("chmod 755 {}".format(os.path.join(self.__srcDirName__,"install-sh")))
    
    def __compile__(self,dest=None):
        
        # Simply copy the sources from the source directory - where the sources have been 
        # downloaded - to the dest directory given by dest
        # Unlike the other ipOpt requirements, HSL works differently because the sources 
        # are not automatically downloaded using the ipOpt scripts, we need to work harder
        # and place the sources 'by hand' in ipOpt/Third_party/HSL/coinhsl for the 
        # ipOpt build system to recognize the package
        print "Copying: ", self.__srcDirName__,"\n  to: ",os.path.join(dest,self.__name__,'coinhsl')
        shutil.copytree(self.__srcDirName__,os.path.join(dest,self.__name__,'coinhsl'))
        
        # Make nothing else, the build is taken over by my parent
        
# Lapack is a requirement for IpOopt, that offers utility scripts to download and 
# compile blas with the right bindings
class ipOptMumpsCompile(thirdPartyCompile):

    # Ctor
    def __init__(self,ipOptMumpsPath):
    
        # Simply call mother-class init
        super(ipOptMumpsCompile,self).__init__()

        # Name of this third_party. 
        self.__name__="Mumps"

        # Store the location where ipOpt will download and 
        # compile blas
        self.__thirdPartySrcFolder__ = ipOptMumpsPath
        
        # We do not need to fill other fileds, as this thirdParty is 
        # entirely handled by IpOpt
            
    def __download__(self):
        
        # Use the IpOpt utilities to download BLAS from the web and compile
        # Note that the __thirdPartySrcFolder__ has been set to the ipOpt/third_party/
        # blas folder
        os.chdir(self.__thirdPartySrcFolder__)
        
        # Patch the script to use curl instead of ftp - which is not installed on 
        # my machine. Why should I use a different command than the other scripts? 
        self.__patch__("wgetcmd=ftp","wgetcmd=\"curl -L -k -O\"","get.Mumps")

        # Make sure the get.Mumps script has 755 permissions and execute it
        self.__execute__("chmod 755 get.Mumps")
        self.__execute__("./get.Mumps")

    def __compile__(self,dest=None):
        # Make nothing, MUMPS will be compiled as part of IpOpt
        pass 

# Lapack is a requirement for IpOopt, that offers utility scripts to download and 
# compile blas with the right bindings
class ipOptMetisCompile(thirdPartyCompile):

    # Ctor
    def __init__(self,ipOptMetisPath):
    
        # Simply call mother-class init
        super(ipOptMetisCompile,self).__init__()

        # Name of this third_party. 
        self.__name__="Metis"

        # Store the location where ipOpt will download and 
        # compile metis
        self.__thirdPartySrcFolder__ = ipOptMetisPath
        
        # We do not need to fill other fileds, as this thirdParty is 
        # entirely handled by IpOpt
            
    def __download__(self):
        
        # Use the IpOpt utilities to download BLAS from the web and compile
        # Note that the __thirdPartySrcFolder__ has been set to the ipOpt/third_party/
        # blas folder
        os.chdir(self.__thirdPartySrcFolder__)
        
        # Patch the script to use curl instead of ftp - which is not installed on 
        # my machine. Why should I use a different command than the other scripts? 
        self.__patch__("wgetcmd=ftp","wgetcmd=\"curl -L -k -O\"","get.Metis")

        # Make sure the get.Mumps script has 755 permissions and execute it
        self.__execute__("chmod 755 get.Metis")
        self.__execute__("./get.Metis")

    def __compile__(self,dest=None):
        # Make nothing, MUMPS will be compiled as part of IpOpt
        pass 
    
        
class IpOptCompile(thirdPartyCompile):
    
    # Ctor
    def __init__(self):
        
        # Simply call mother-class init
        super(IpOptCompile,self).__init__()

        # Name of this third_party. 
        self.__name__="Ipopt"
        
        # Version of this third_party
        self.__version__="3.12.6"
        
        # Define the URL from which IpOpt can be downloaded
        # Note that this should be the complete path of the file
        # to be downloaded (Including the filename!)
        self.__url__="https://www.coin-or.org/download/source/Ipopt/Ipopt-3.12.6.zip"

        # Define the name of the archive downloadeed from the web. A priori this is 
        # simply the last entry of the url
        self.__srcArchiveName__="Ipopt-3.12.6.zip"
        
        # Define the name of the folder extracted from the archive downloadeed from the web. 
        # A priori this is simply the srcArchiveName without the extension
        self.__srcDirName__="Ipopt-3.12.6"
        
        # Override (specialize) the build folder 
        self.__thirdPartyBuildFolder__= os.path.join(self.__thirdPartyBuildFolder__,self.__srcDirName__)

        # Override (specialize) the package folder 
        self.__thirdPartyPkgFolder__= os.path.join(self.__thirdPartyPkgFolder__,self.__srcDirName__)
            
        # Instantiate the requirements... BLAS
        blasSrcFolder = os.path.join(self.__thirdPartySrcFolder__,
                                     self.__srcDirName__,
                                     "ThirdParty","Blas")
        self.__requirements__.append( ipOptBlasCompile(blasSrcFolder))
        
        # Instantiate the requirements... LAPACK
        lapackSrcFolder = os.path.join(self.__thirdPartySrcFolder__,
                                       self.__srcDirName__,
                                       "ThirdParty","Lapack")
        self.__requirements__.append( ipOptLapackCompile(lapackSrcFolder))

        # Instantiate the requirements... ASL
        aslSrcFolder = os.path.join(self.__thirdPartySrcFolder__,
                                       self.__srcDirName__,
                                       "ThirdParty","ASL")
        self.__requirements__.append( ipOptASLCompile(aslSrcFolder))
        
        # Instantiate the requirements... HSL <-- Instead, use MUMPS!
        #self.__requirements__.append( IpOptHSLCompile() )
    
        # Instantiate the requirements... MUMPS
        mumpsSrcFolder = os.path.join(self.__thirdPartySrcFolder__,
                                       self.__srcDirName__,
                                       "ThirdParty","Mumps")
        self.__requirements__.append( ipOptMumpsCompile(mumpsSrcFolder))


        # Instantiate the requirements... METIS
        metisSrcFolder = os.path.join(self.__thirdPartySrcFolder__,
                                       self.__srcDirName__,
                                       "ThirdParty","Metis")
        self.__requirements__.append( ipOptMetisCompile(metisSrcFolder))
        
        # Define the build info. Will use these to copy the components (includes, libs...) to 
        # the package folder
        self.__buildInfo__["INCLUDEPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"include")]
        self.__buildInfo__["LIBPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"lib")]
        self.__buildInfo__["DOCPATH"] = [self.__thirdPartyPkgFolder__]
        self.__buildInfo__["LIBS"] = ["ipopt"]

    def __download__(self):
            
        # Go to the __thirdPartySrcFolder__. Its existence was 
        # assured in the init of the class
        os.chdir(self.__thirdPartySrcFolder__)
                 
        # cleanup: remove a previous archive if present
        shutil.rmtree(self.__srcArchiveName__,sys.exc_info())
          
        # Get the sources from the web
        print "Downloading the ipOpt sources..."
        self.__getCompressedArchive__(self.__url__)
        print "Downloaded the ipOpt sources!"
                     
         # I can now use the scripts provided by ipOpt to download the 
         # required third_party. Kq[gr]pe
        for iReq in self.__requirements__:
            iReq.__download__()
            
    # Compile this package    
    def __compile__(self,dest=None):
        
        # Cleanup previous build folder - if any
        shutil.rmtree(self.__thirdPartyBuildFolder__,
                      sys.exc_info())

        # Copy the sources to the build dir
        # First, make sure we are in the right place
        os.chdir(self.__thirdPartySrcFolder__)
        shutil.copytree(self.__srcDirName__,self.__thirdPartyBuildFolder__)

        # What about the requirements? Actually this is a mis-use of the __compile__
        # method, that does not compile for the requirements, because the ipOpt build 
        # is able to handle compiling. So, for HSL only we move the sources to the 
        # ThirdParty folder of ipOpt, so to complete the package 
        for iReq in self.__requirements__:
            iReq.__compile__(dest=os.path.join(self.__thirdPartyBuildFolder__,
                                          "ThirdParty"))
    
        # Go to the ipOpt build folder
        os.chdir(self.__thirdPartyBuildFolder__)

        # Make the build folder
        os.mkdir("Build")
        os.chdir("Build")

        # Change permissions to the scripts that will be executed during build
        self.__execute__("chmod 755 ../configure")
        self.__execute__("chmod 755 {}".format(os.path.join(
                                                self.__thirdPartyBuildFolder__,
                                                "Ipopt",
                                                "install-sh")))
        self.__execute__("chmod 755 {}".format(os.path.join(
                                                self.__thirdPartyBuildFolder__,
                                                "install-sh")))
        # Make sure the install-sh scripts of the requirements are executable too
        for iReq in self.__requirements__: 
            self.__execute__("chmod 755 {}".format(os.path.join(
                                                    self.__thirdPartyBuildFolder__,
                                                    "ThirdParty",
                                                    iReq.__name__,
                                                    "install-sh")))

        # Launch the configure script with the macOS specific args 
        # (see pdf docs 2.8.2 Adjusting configuration and build of Ipopt)     
        self.__execute__("../configure "
                         "--prefix={}".format(os.path.join(self.__thirdPartyBuildFolder__,"Build","lib"))) 

# --disable-linear-solver-loader "
#                         "ADD_CFLAGS='-fno-common -fexceptions -no-cpp-precomp' "
#                         "ADD_CXXFLAGS='-fno-common -fexceptions -no-cpp-precomp' "
#                         "ADD_FFLAGS='-fexceptions -fbackslash'#--with-blas='-framework vecLib' --with-lapack='-framework vecLib'
#--disable-shared --with-pic "

        # Launch the build    
        self.__execute__("make")

        # Install the build (in the location specified as prefix while running the 
        # configure script)
        self.__execute__("make install")

        # Install the doxygen doc - is this really worthit?
        #self.__execute__("make install-doxydoc")

        # Do NOT make the tests, see https://github.com/Homebrew/legacy-homebrew/issues/13617
        # Will add a simple local test example that will use the basic functionalities

    # Package the third party that was build   
    def __package__(self):
                
        # Write the build info to file. This will be used
        # by the build system to compile and link the program
        self.__writeInfo__()   

        # cleanup: remove a previous package folder if present. Remember that the 
        # package folder was set relative to this third_party in the ctor
        shutil.rmtree(self.__thirdPartyPkgFolder__,sys.exc_info())

        # Make a package folder and enter it 
        os.mkdir(self.__thirdPartyPkgFolder__)

        # Copy the content of include and lib
        shutil.copytree(os.path.join(self.__thirdPartyBuildFolder__,"Build","lib","include","coin"), 
                        self.__buildInfo__["INCLUDEPATH"][0], symlinks=True)
                
        shutil.copytree(os.path.join(self.__thirdPartyBuildFolder__,"Build","lib","lib"), 
                        self.__buildInfo__["LIBPATH"][0], symlinks=True)

        # Also copy the documentation
        shutil.copyfile(os.path.join(self.__thirdPartyBuildFolder__,"Ipopt","doc","documentation.pdf"), 
                        os.path.join(self.__buildInfo__["DOCPATH"][0],"documentation.pdf"))
 
        # Copy the example 
        shutil.copytree(os.path.join(self.__thirdPartyBuildFolder__,"Ipopt","examples","Cpp_example"), 
                        os.path.join(self.__thirdPartyPkgFolder__,"Cpp_example"))
                                     
    # Run a simple test to check that the compile was successiful
    def __test__(self):
        
        # Also compile the c++ test, that will be run by __test__
        os.chdir(os.path.join(self.__thirdPartyPkgFolder__,"Cpp_example"))
        
        # Write a SConstruct 
        Sconstruct=open("SConstruct","w")
        Sconstruct.write('''import os
env = Environment()  
env.Append( CPPPATH=["{}/include"] )
env.Append( LIBPATH=["{}/lib"] )
env.Append( LIBS=["ipopt"] )
env.Program('ipOptTest', Glob('*.cpp') )        
'''.format(self.__thirdPartyPkgFolder__,self.__thirdPartyPkgFolder__))
        Sconstruct.close()
        
        print "Executing the ipOpt test: we are in folder: ", os.getcwd()
                
        # Compile the example
        self.__execute__("scons -Q")
        
        # Execute the example
        self.__execute__("./ipOptTest")        
        
        