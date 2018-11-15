from thirdPartyCompile import thirdPartyCompile
import os, shutil, sys

class EigenCompile(thirdPartyCompile):
    
    # Ctor
    def __init__(self):
            
        # Simply call mother-class init
        super(EigenCompile,self).__init__()

        # Name of this third_party. 
        self.__name__="Eigen"
        
        # Version of this third_party
        self.__version__="3.3.4"
        
        # Define the URL from which cppUnit can be downloaded
        self.__url__="http://bitbucket.org/eigen/eigen/get/3.3.4.tar.gz"

        # Define the name of the archive downloadeed from the web.
        self.__srcArchiveName__="eigen-eigen-5a0156e40feb.tar"
        
        # Define the name of the folder extracted from the archive downloadeed from the web. 
        # A priori this is simply the srcArchiveName without the extension
        self.__srcDirName__="eigen-eigen-5a0156e40feb"
        
        # Override (specialize) the build folder 
        self.__thirdPartyBuildFolder__= os.path.join(self.__thirdPartyBuildFolder__,self.__srcDirName__)

        # Override (specialize) the package folder 
        self.__thirdPartyPkgFolder__= os.path.join(self.__thirdPartyPkgFolder__,self.__srcDirName__)

        # Instantiate no requirements - Boost only has Python for BoostPython, which we do not want        

        # Define the build info. Will use these to copy the components (includes, libs...) to 
        # the package folder
        self.__buildInfo__["INCLUDEPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"include")]
        self.__buildInfo__["LIBPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"lib")]
        self.__buildInfo__["BINPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"bin")]
        self.__buildInfo__["DOCPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"doc")]
        self.__buildInfo__["LIBS"] = []
            
#     # Download an archive from a given url and unzip it 
#     # Overwrite the method defined in thirdPartyCompile 
#     # to specialize this for the tar.bz2 format to be downloaded
#     # from sourgeforce. At some point this needs to be refactored 
#     def __getCompressedArchive__(self,url,saveAs="myArch"):
# 
#        # download the archive
#        self.__execute__("curl {} -L -k -o {}".format(self.__url__,self.__srcDirName__))
# 
#        # Then unzip and go...  for a tar.bz2...
#        import tarfile
#        tar= tarfile.open(mode='r|bz2', fileobj=self.__srcDirName__)
#        tar.extractall()
#        tar.close()

    # Compile this package    
    def __compile__(self,dest=None):
    
        # Decorate the mother class __compile__ method
        super(EigenCompile,self).__compile__()

        # Eigen does not require to be compiled. However, 
        # we compile with cMake to get the docs. We compile 
        # with cmake as explained in the INSTALL file. We assume
        # that cmake is installed and accessible on the machine, 
        # which is not a too strong assumption 
        
        # Make the build folder and access it
        os.mkdir("Build")
        os.chdir("Build")

        self.__execute__("cmake {} -DCMAKE_INSTALL_PREFIX={} -DINCLUDE_INSTALL_DIR={}".format(
                                    self.__thirdPartyBuildFolder__,
                                    os.getcwd(),
                                    self.__buildInfo__["INCLUDEPATH"][0]))
        self.__execute__("make doc")

    # Package the third party that was build   
    def __package__(self):
                   
        # Decorate the mother class __package__ method
        super(EigenCompile,self).__package__()
                     
        # Get back to the Eigen Build folder if we are not still there
        os.chdir(os.path.join(self.__thirdPartyBuildFolder__,"Build"))

        # install to the package folder, as defined when configuring with the --prefix
        self.__execute__("make install")
        
        # Copy the docs that are in __thirdPartyBuildFolder__/build/doc/html
        shutil.copytree(os.path.join(self.__thirdPartyBuildFolder__,"Build","doc","html"), 
                        os.path.join(self.__buildInfo__["DOCPATH"][0],"html"), symlinks=True)
                        
    # Run a simple test to check that the compile was successiful
    def __test__(self):
                
        # Decorate the mother class __test__ method
        super(EigenCompile,self).__test__()

        # Write the cppunit example 
        Source=open("main.cpp","w")
        Source.write('''
#include <iostream>
#include <Eigen/Core>

using namespace std;

int main(int argc, char *argv[]) {

    Eigen::VectorXd v(4);
    v << 1.0, 2.1, 3.2, 4.3;
    std::cout<<"Eigen Vector:"<<v.transpose()<<std::endl;
    return 0;
}''')
        Source.close()
             
        # Write a SConstruct
        Sconstruct=open("SConstruct","w")
        Sconstruct.write('''import os
env = Environment()  
env.Append( CPPPATH=["{}"] )
env.Append( LIBPATH=["{}"] )
env.Program('cppUnitTest', Glob('*.cpp') )        
'''.format(self.__buildInfo__["INCLUDEPATH"][0],
            self.__buildInfo__["LIBPATH"][0]))
        Sconstruct.close()
                         
        # Compile the example
        self.__execute__("scons -Q")
         
        # Execute the example
        self.__execute__("./cppUnitTest {}".format(os.getcwd()))        
            
        