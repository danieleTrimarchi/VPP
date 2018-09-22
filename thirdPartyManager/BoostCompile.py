from thirdPartyCompile import thirdPartyCompile
import os, shutil, sys

class BoostCompile(thirdPartyCompile):
    
    # Ctor
    def __init__(self):
        
        # Simply call mother-class init
        super(BoostCompile,self).__init__()

        # Name of this third_party. 
        self.__name__="Boost"
        
        # Version of this third_party
        self.__version__="1.68.0"
        
        # Define the URL from which IpOpt can be downloaded
        # Note that this should be the complete path of the file
        # to be downloaded (Including the filename!)
        self.__url__="https://dl.bintray.com/boostorg/release/1.68.0/source/boost_1_68_0.tar.gz"

        # Define the name of the archive downloadeed from the web. A priori this is 
        # simply the last entry of the url
        self.__srcArchiveName__="boost_1_68_0.tar"
        
        # Define the name of the folder extracted from the archive downloadeed from the web. 
        # A priori this is simply the srcArchiveName without the extension
        self.__srcDirName__="boost_1_68_0"
        
        # Override (specialize) the build folder 
        self.__thirdPartyBuildFolder__= os.path.join(self.__thirdPartyBuildFolder__,self.__srcDirName__)

        # Override (specialize) the package folder 
        self.__thirdPartyPkgFolder__= os.path.join(self.__thirdPartyPkgFolder__,self.__srcDirName__)
         
         # No requirements for Boost (no Python by now)
    
    def __download__(self):
            
        # Go to the __thirdPartySrcFolder__. Its existence was 
        # assured in the init of the class
        os.chdir(self.__thirdPartySrcFolder__)
                 
        # cleanup: remove a previous archive if present
        shutil.rmtree(self.__srcArchiveName__,sys.exc_info())
          
        # Get the sources from the web
        self.__getCompressedArchive__(self.__url__)
                     
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

        self.__execute__("./bootstrap.sh --prefix={} "
                         "--exec-prefix={} "
                         "--libdir={} "
                         "--includedir={} "
                         "--with-libraries=filesystem,system".format(
                             os.path.join(self.__thirdPartyBuildFolder__,"Build"),
                             os.path.join(self.__thirdPartyBuildFolder__,"Build"),
                             os.path.join(self.__thirdPartyBuildFolder__,"Build","lib"),
                             os.path.join(self.__thirdPartyBuildFolder__,"include")
                             ))

    # Package the third party that was build   
    def __package__(self):
        
        # Define the build info. Will use these to copy the components (includes, libs...) to 
        # the package folder
        self.__buildInfo__["INCLUDEPATH"] = os.path.join(self.__thirdPartyPkgFolder__,"include")
        self.__buildInfo__["LIBPATH"] = os.path.join(self.__thirdPartyPkgFolder__,"lib")
        self.__buildInfo__["DOCPATH"] = self.__thirdPartyPkgFolder__
        self.__buildInfo__["LIBS"] = "boost_system","boost_filesystem"
        
        # Write the build info to file. This will be used
        # by the build system to compile and link the program
        self.__writeInfo__()   

        # cleanup: remove a previous package folder if present. Remember that the 
        # package folder was set relative to this third_party in the ctor
        shutil.rmtree(self.__thirdPartyPkgFolder__,sys.exc_info())

        # Make a package folder and enter it 
        os.mkdir(self.__thirdPartyPkgFolder__)

        # Copy the content of include and lib
        shutil.copytree(os.path.join(self.__thirdPartyBuildFolder__,"Build","include"), 
                        self.__buildInfo__["INCLUDEPATH"])
                
        shutil.copytree(os.path.join(self.__thirdPartyBuildFolder__,"Build","lib"), 
                        self.__buildInfo__["LIBPATH"])

        # Also copy the documentation
#        shutil.copyfile(os.path.join(self.__thirdPartyBuildFolder__,"Ipopt","doc","documentation.pdf"), 
#                        os.path.join(self.__buildInfo__["DOCPATH"],"documentation.pdf"))
 
        # Copy the example 
#        shutil.copytree(os.path.join(self.__thirdPartyBuildFolder__,"Ipopt","examples","Cpp_example"), 
#                        os.path.join(self.__thirdPartyPkgFolder__,"Cpp_example"))
                                     
    # Run a simple test to check that the compile was successiful
    def __test__(self):
        
        # Go to the example folder
        os.chdir(os.path.join(self.__thirdPartyPkgFolder__,"Cpp_example"))

        # Write the boost example 
        Source=open("main.cpp","w")
        Source.write('''
#include <boost/shared_ptr.hpp>

int main(int argc, char** argv) {
    boost::shared_ptr<int> myPtr; 
}       
''')
            
        # Write a SConstruct 
        Sconstruct=open("SConstruct","w")
        Sconstruct.write('''import os
env = Environment()  
env.Append( CPPPATH=["{}"] )
env.Append( LIBPATH=["{}"] )
env.Append( LIBS=[""] )
env.Program('boostTest', Glob('*.cpp') )        
'''.format(self.__buildInfo__["INCLUDEPATH"],
           self.__buildInfo__["LIBPATH"],
           self.__buildInfo__["LIBS"]))
        Sconstruct.close()
        
        print "Executing the boost test: we are in folder: ", os.getcwd()
                
        # Compile the example
        self.__execute__("scons -Q")
        
        # Execute the example
        self.__execute__("./boostTest")        
        
        