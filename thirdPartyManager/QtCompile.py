from thirdPartyCompile import thirdPartyCompile
import os, shutil, sys

class QtCompile(thirdPartyCompile):
    
    # Ctor
    def __init__(self):
        
        # Simply call mother-class init
        super(QtCompile,self).__init__()

        # Name of this third_party. 
        self.__name__="Qt"
        
        # Version of this third_party
        self.__version__="5.9.1"
        
        # Define the URL from which IpOpt can be downloaded
        # Note that this should be the complete path of the file
        # to be downloaded (Including the filename!)
        self.__url__=todo

        # Define the name of the archive downloadeed from the web. A priori this is 
        # simply the last entry of the url
        self.__srcArchiveName__=todo
        
        # Define the name of the folder extracted from the archive downloadeed from the web. 
        # A priori this is simply the srcArchiveName without the extension
        self.__srcDirName__=todo
        
        # Override (specialize) the build folder 
        self.__thirdPartyBuildFolder__= os.path.join(self.__thirdPartyBuildFolder__,self.__srcDirName__)

        # Override (specialize) the package folder 
        self.__thirdPartyPkgFolder__= os.path.join(self.__thirdPartyPkgFolder__,self.__srcDirName__)

        # Instantiate no requirements - Boost only has Python for BoostPython, which we do not want        

        # Define the build info. Will use these to copy the components (includes, libs...) to 
        # the package folder
        self.__buildInfo__["INCLUDEPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"include")]
        self.__buildInfo__["LIBPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"lib")]
        self.__buildInfo__["DOCPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"doc")]
        self.__buildInfo__["LIBS"] = todo
          
    # Compile this package    
    def __compile__(self,dest=None):
        
        pass
#         # Decorate the mother class __package__ method
#         super(QtCompile,self).__compile__()
#     
#         # Make the build folder
#         os.mkdir("Build")
# 
#         # Execute bootstrap...
#         self.__execute__("./bootstrap.sh "
#                          "--with-libraries=filesystem,system")
# 
#         # Execute b2, which will build the requested libs
#         self.__execute__("./b2")         

#     def __copySelectedDocs__(self):
#         
#         # Loop on the modules we seek to compile 
#         for iLib in self.__buildInfo__["LIBS"] :
#             # Boost libs are named boost_something. Here we only seek the 'something'
#             cleanName = iLib.replace("boost_","")
# 
#             # Verify the object we get is in the form we need            
#             # and define the doc directory as pkgDir/doc/libDir
#             checkListOfSizeOne(self.__buildInfo__["DOCPATH"],"DocPath")             
#             
#             # Make the doc directory
#             docDir = os.path.join(self.__buildInfo__["DOCPATH"][0],cleanName)
#             os.makedirs(docDir)
# 
#             # Copy the files to the doc directory
#             shutil.copy(os.path.join(self.__thirdPartyBuildFolder__,"libs",cleanName,"index.html"), 
#                     docDir)
#             self.__copytree__(os.path.join(self.__thirdPartyBuildFolder__,"libs",cleanName,"doc"), 
#                               os.path.join(docDir,"doc"))

    # Package the third party that was build   
    def __package__(self):
              
        pass
#         # Decorate the mother class __package__ method
#         super(QtCompile,self).__package__()
#                 
#         # Copy the content of include
#         self.__copytree__(os.path.join(self.__thirdPartyBuildFolder__,"boost"), 
#                           os.path.join(self.__buildInfo__["INCLUDEPATH"][0],"boost"))
# 
#         self.__copytree__(os.path.join(self.__thirdPartyBuildFolder__,"stage","lib"), 
#                           self.__buildInfo__["LIBPATH"][0])
# 
#         # Also copy the documentation of the modules we are compiling
#         self.__copySelectedDocs__()
                                      
    # Run a simple test to check that the compile was successiful
    def __test__(self):

        # Decorate the mother class __test__ method
        super(QtCompile,self).__test__()

        # Write the boost example 
        Source=open("main.cpp","w")
        Source.write('''
#include <iostream>
using std::cout;

int main(int argc, char** argv) {
    return 0; 
}       
''')
        Source.close()
            
        # Write a SConstruct 
        Sconstruct=open("SConstruct","w")
        Sconstruct.write('''import os
env = Environment()  
env.Append( CPPPATH=["{}"] )
env.Append( LIBPATH=["{}"] )
env.Append( LIBS={} )
env.Program('qtTest', Glob('*.cpp') )        
'''.format(self.__buildInfo__["INCLUDEPATH"],
           self.__buildInfo__["LIBPATH"],
           self.__buildInfo__["LIBS"]))
        Sconstruct.close()
                        
        # Compile the example
        self.__execute__("scons -Q")
        
        # Before execution, add symbolic links to the dylibs. Why cannot I 
        # just set LD_LIBRARY_PATH..? Weird. Looks like MacOS security stuff.
        #for iLib in self.__buildInfo__["LIBS"]:
        #    os.symlink(os.path.join(self.__buildInfo__["LIBPATH"],self.getFullDynamicLibName(iLib)), 
        #               self.getFullDynamicLibName(iLib) )
        
        # Execute the example
        self.__execute__("export DYLD_LIBRARY_PATH=\"{}\"; ./qtTest {}".format(self.__buildInfo__["LIBPATH"][0],os.getcwd()))        

    # Import the dynamic libraries from third party to the dest folder (in this case
    # this will be in the app bundle VPP.app/Contents/Frameworks/
    def importDynamicLibs(self,dst):
        verify what this does!
        # Copy the lib to the dest folder
        for iLib in self.__buildInfo__["LIBS"]: 
            shutil.copyfile(self.getFullDynamicLibName(iLib), 
                            os.path.join(dst,self.getFullDynamicLibName(iLib)))
                    
        