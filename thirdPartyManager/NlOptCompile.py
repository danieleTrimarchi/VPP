from thirdPartyCompile import thirdPartyCompile
import os, shutil, sys
import subprocess

# Import markdown which is used for the documentation
try:
    import markdown
except:
    p = subprocess.Popen("pip install python-markdown-math",shell=True)
    if p.wait():
        raise NameError('Something went wrong while installing python-markdown-math')
    import markdown

class NlOptCompile(thirdPartyCompile):
    
    # Ctor
    def __init__(self):
            
        # Simply call mother-class init
        super(NlOptCompile,self).__init__()

        # Name of this third_party. 
        self.__name__="NLOpt"
        
        # Version of this third_party
        self.__version__="2.5.0"
        
        # Define the URL from which cppUnit can be downloaded
        self.__url__="https://github.com/stevengj/nlopt/archive/v2.5.0.tar.gz"

        # Define the name of the archive downloadeed from the web.
        self.__srcArchiveName__="nlopt-2.5.0.tar"
        
        # Define the name of the folder extracted from the archive downloadeed from the web. 
        # A priori this is simply the srcArchiveName without the extension
        self.__srcDirName__="nlopt-2.5.0"
        
        # Override (specialize) the build folder 
        self.__thirdPartyBuildFolder__= os.path.join(self.__thirdPartyBuildFolder__,self.__srcDirName__)

        # Override (specialize) the package folder 
        self.__thirdPartyPkgFolder__= os.path.join(self.__thirdPartyPkgFolder__,self.__srcDirName__)

        # Instantiate no requirements for NLOpt
        
        # Define the build info. Will use these to copy the components (includes, libs...) to 
        # the package folder
        self.__buildInfo__["INCLUDEPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"include")]
        self.__buildInfo__["LIBPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"lib")]
        self.__buildInfo__["BINPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"bin")]
        self.__buildInfo__["DOCPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"doc")]
        self.__buildInfo__["LIBS"] = ["nlopt","nlopt.0","nlopt.0.9.0"]
        
        # Also verify that the package mkdocs is available. This is used to
        # generate the nlOpt documentation 
        try:
            self.__execute__("mkdocs --version")
        except:
            p = subprocess.Popen("sudo pip install mkdocs",shell=True)
            if p.wait():
                raise NameError('Something went wrong while installing mkdocs')

    # Compile this package    
    def __compile__(self,dest=None):

        # Decorates the mother class __compile__ method
        super(NlOptCompile,self).__compile__()
    
        # Make the build folder
        os.mkdir("Build")
        os.chdir("Build")

        # Execute cmake...
        self.__execute__("cmake -DCMAKE_INSTALL_PREFIX={} -DNLOPT_PYTHON=OFF -DNLOPT_OCTAVE=OFF -DNLOPT_MATLAB=OFF -DNLOPT_GUILE=OFF -DNLOPT_SWIG=OFF -DNLOPT_LINK_PYTHON=OFF ..".format(self.__thirdPartyPkgFolder__))
        self.__execute__("make")        

        
    # Package the third party that was build   
    def __package__(self):
                
        # Decorate the mother class __package__ method
        super(NlOptCompile,self).__package__()

        # Make sure to be in the right place, for instance the Build folder 
        os.chdir(os.path.join(self.__thirdPartyBuildFolder__,"Build"))
        
        # install to the package folder, as defined when configuring with the --prefix
        self.__execute__("make install")
        
        # Also package the docs
        
        
    # Run a simple test to check that the compile was successiful
    def __test__(self):
        
        # Decorate the mother class __test__ method
        super(NlOptCompile,self).__test__()

        # Write the cppunit example 
        Source=open("main.cpp","w")
        Source.write('''
#include <iostream>
#include <math.h>
#include <nlopt.h>

using namespace std;

double myfunc(unsigned n, const double *x, double *grad, void *my_func_data) {

    if (grad) {
        grad[0] = 0.0;
        grad[1] = 0.5 / sqrt(x[1]);
    }
    return sqrt(x[1]);
}

typedef struct {
    double a, b;
} my_constraint_data;

double myconstraint(unsigned n, const double *x, double *grad, void *data) {
 
    my_constraint_data *d = (my_constraint_data *) data;
    double a = d->a, b = d->b;
    if (grad) {
        grad[0] = 3 * a * (a*x[0] + b) * (a*x[0] + b);
        grad[1] = -1.0;
    }
    return ((a*x[0] + b) * (a*x[0] + b) * (a*x[0] + b) - x[1]);
 }
 
int main(int argc, char *argv[]) {

    double lb[2] = { -HUGE_VAL, 0 }; /* lower bounds */
    nlopt_opt opt; 

    opt = nlopt_create(NLOPT_LD_MMA, 2); /* algorithm and dimensionality */
    nlopt_set_lower_bounds(opt, lb);
    nlopt_set_min_objective(opt, myfunc, NULL);
  
    my_constraint_data data[2] = { {2,0}, {-1,1} };
  
    nlopt_add_inequality_constraint(opt, myconstraint, &data[0], 1e-8);
    nlopt_add_inequality_constraint(opt, myconstraint, &data[1], 1e-8);
  
    nlopt_set_xtol_rel(opt, 1e-4);

    double x[2] = { 1.234, 5.678 };  /* `*`some` `initial` `guess`*` */
    double minf; /* `*`the` `minimum` `objective` `value,` `upon` `return`*` */
    if (nlopt_optimize(opt, x, &minf) < 0) {
        printf(\"nlopt failed!\\n\");
        return 1; 
    } else {
        printf(\"found minimum at f(%g,%g) = %0.10g\\n\", x[0], x[1], minf);
        return 0; 
    }
  return 0;
}''')
        Source.close()
             
        # Write a SConstruct
        Sconstruct=open("SConstruct","w")
        Sconstruct.write('''import os
env = Environment()  
env.Append( CPPPATH=["{}"] )
env.Append( LIBPATH=["{}"] )
env.Append( LIBS={} )
env.Program('NLOptTest', Glob('*.cpp') )        
'''.format(self.__buildInfo__["INCLUDEPATH"][0],
            self.__buildInfo__["LIBPATH"][0],
            self.__buildInfo__["LIBS"]))
        Sconstruct.close()
                         
        # Compile the example
        self.__execute__("scons -Q")

        # Execute the example
        self.__execute__("export DYLD_LIBRARY_PATH=\"{}\"; ./NLOptTest".format(self.__buildInfo__["LIBPATH"][0]))        
        