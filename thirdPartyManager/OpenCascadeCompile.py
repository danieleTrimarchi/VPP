from thirdPartyCompile import thirdPartyCompile
import os, shutil, sys

# Blas is a requirement for IpOopt, that offers utility scripts to download and 
# compile blas with the right bindings
class TclCompile(thirdPartyCompile):

    # Ctor
    def __init__(self):
    
        # Simply call mother-class init
        super(TclCompile,self).__init__()

        # Name of this third_party. 
        self.__name__="Tcl"

        # Version of this third_party
        self.__version__="8.7a1"

        # Define the URL from which IpOpt can be downloaded
        # Note that this should be the complete path of the file
        # to be downloaded (Including the filename!)
        self.__url__="https://prdownloads.sourceforge.net/tcl/tcl8.7a1-src.tar.gz"

        # Define the name of the archive downloadeed from the web. A priori this is 
        # simply the last entry of the url
        self.__srcArchiveName__="tcl-core8.7a1rc1-src.tar"

        # Define the name of the folder extracted from the archive downloadeed from the web. 
        # A priori this is simply the srcArchiveName without the extension
        self.__srcDirName__="tcl8.7a1"

        # Override (specialize) the build folder 
        self.__thirdPartyBuildFolder__= os.path.join(self.__thirdPartyBuildFolder__,self.__srcDirName__)

        # Override (specialize) the package folder 
        self.__thirdPartyPkgFolder__= os.path.join(self.__thirdPartyPkgFolder__,self.__srcDirName__)
        
                # Define the build info. Will use these to copy the components (includes, libs...) to 
        # the package folder
        self.__buildInfo__["INCLUDEPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"include")]
        self.__buildInfo__["LIBPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"lib")]
        self.__buildInfo__["BINPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"bin")]
        self.__buildInfo__["DOCPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"doc")]
        self.__buildInfo__["LIBS"] = ["tclstub8.7"]
                    
    def __compile__(self,dest=None):

        # Decorates the mother class __compile__ method
        super(TclCompile,self).__compile__()

        # Cleanup previous package folder - if any. I need to do this here
        # because I will be sending the compiled directly to the pkg folder.
        # So I need to cleanup before the build starts 
        shutil.rmtree(self.__thirdPartyPkgFolder__,sys.exc_info())

        # Follow the instructions in file:///Users/dtrimarchi/third_party/opencascade-7.1.0/doc/overview/html/occt_dev_guides__building_3rdparty_osx.html

        # Patch the macosx/GNU_Makefile : ${CURDIR}/../../build => ${CURDIR}/../build
        # Modify the expression to match a regexp
        self.__patch__("\${CURDIR}/../../build","${CURDIR}/../build", os.path.join(self.__thirdPartyBuildFolder__,"macosx","GNUmakefile"))

        # Run the build        
        self.__execute__('''export CFLAGS=\"{cflags}\";export ver=\"{ver}\";
 export BUILD_DIR=\"{builddir}\";make -C macosx deploy;
 sudo make -C macosx install-deploy INSTALL_ROOT=\"{pkgdir}\"'''.format(
            cflags="-arch i386 -arch x86_64 -arch ppc -mmacosx-version-min=10.14",
            ver=self.__version__,
            builddir=self.__thirdPartyBuildFolder__,
            pkgdir=self.__thirdPartyPkgFolder__
            ))

    # Package the third party that was build   
    def __package__(self):
                      
        # Do not call the motherclass __pakcage__ method that 
        # woud erase the components installed!

        # Make the lib folder and move the framework. Remove the folder 'Library'
        self.__makedirs__(self.__buildInfo__["LIBPATH"][0])
        self.__move__(os.path.join(self.__thirdPartyPkgFolder__,"Library","Frameworks","Tcl.framework"),
                          os.path.join(self.__thirdPartyPkgFolder__,"lib","Tcl.framework"))
        self.__remove__(os.path.join(self.__thirdPartyPkgFolder__,"Library"))   
          
        # Rename the folder:    
        #                self.__thirdPartyPkgFolder__/usr/local/bin 
        #                to: bin
        self.__move__(os.path.join(self.__thirdPartyPkgFolder__,"usr","local","bin"),
                      os.path.join(self.__thirdPartyPkgFolder__,self.__buildInfo__["BINPATH"][0]))
        self.__remove__(os.path.join(self.__thirdPartyPkgFolder__,"usr"))   
             
        # Copy the init.tcl file from /Users/dtrimarchi/third_party_build/tcl8.7a1/library 
        # to the lib folder
        self.__copy__(os.path.join(self.__thirdPartyBuildFolder__,"library","init.tcl"),
                      os.path.join(self.__buildInfo__["LIBPATH"][0],"init.tcl"))

        # copy the library from pkg/lib/Tck.framework/Versions/8.7/libtclstub8.7.a to pkg/lib/libtclstub8.7.a
        self.__copy__(os.path.join(self.__buildInfo__["LIBPATH"][0],"Tck.framework","Versions","8.7","libtclstub8.7.a"), 
                      os.path.join(self.__buildInfo__["LIBPATH"][0],"libtclstub8.7.a") )

        # Make an include dir and set a link to the includes in the framework
        self.__copy__(os.path.join(self.__thirdPartyPkgFolder__,"lib","Tcl.framework","Headers"),
                      self.__buildInfo__["INCLUDEPATH"][0])

    def __test__(self):
        
        # A test target is provided. Go into the build folder of tcl and run 
        # make test TESTFLAGS="-file append.test"
        # The complete test suite appears to hang in socket.test, it might well
        # be that I need to instantiate a server to have this test running. 
        # I do not think it is very important at this stage

        # Go to the build folder
        os.chdir(os.path.join(self.__thirdPartyBuildFolder__,'macosx'))
        
        # Run a couple of test targets
        self.__execute__("make test TESTFLAGS=\"-file append.test\"")
        self.__execute__("make test TESTFLAGS=\"-file eval.test\"")


# ------------------------------------------------------------------------

class TkCompile(thirdPartyCompile):

    # Ctor
    def __init__(self):
    
        # Simply call mother-class init
        super(TkCompile,self).__init__()

        # Name of this third_party. 
        self.__name__="Tk"

        # Version of this third_party
        self.__version__= "8.7a1"

        # Define the URL from which IpOpt can be downloaded
        # Note that this should be the complete path of the file
        # to be downloaded (Including the filename!)
        self.__url__= "http://www.tcl.tk/software/tcltk/download.html"

        # Define the name of the archive downloadeed from the web. A priori this is 
        # simply the last entry of the url
        self.__srcArchiveName__= "tk8.7a1rc0-src.tar"

        # Define the name of the folder extracted from the archive downloadeed from the web. 
        # A priori this is simply the srcArchiveName without the extension
        self.__srcDirName__= "tk8.7a1"

        # Override (specialize) the build folder 
        self.__thirdPartyBuildFolder__= os.path.join(self.__thirdPartyBuildFolder__,self.__srcDirName__)

        # Override (specialize) the package folder 
        self.__thirdPartyPkgFolder__= os.path.join(self.__thirdPartyPkgFolder__,self.__srcDirName__)
        
        # Instantiate requirements, in this case Tcl
        self.__requirements__.append( TclCompile() )
        
        # Define the build info. Will use these to copy the components (includes, libs...) to 
        # the package folder
        self.__buildInfo__["INCLUDEPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"include")]
        self.__buildInfo__["LIBPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"lib")]
        self.__buildInfo__["BINPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"bin")]
        self.__buildInfo__["DOCPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"doc")]
        self.__buildInfo__["LIBS"] = ["tkstub8.7"]
                    
    def __compile__(self,dest=None):

        # Decorates the mother class __compile__ method
        super(TkCompile,self).__compile__()

        # Cleanup previous package folder - if any. I need to do this here
        # because I will be sending the compiled directly to the pkg folder.
        # So I need to cleanup before the build starts 
        shutil.rmtree(self.__thirdPartyPkgFolder__,sys.exc_info())

        # Follow the instructions in file:///Users/dtrimarchi/third_party/opencascade-7.1.0/doc/overview/html/occt_dev_guides__building_3rdparty_osx.html

        # Patch the macosx/GNU_Makefile : ${CURDIR}/../../build => ${CURDIR}/../build
        # Modify the expression to match a regexp
        self.__patch__("\${CURDIR}/../../build","${CURDIR}/../build", os.path.join(self.__thirdPartyBuildFolder__,"macosx","GNUmakefile"))

        # Run the build        
        self.__execute__('''export CFLAGS=\"{cflags}\";export ver=\"{ver}\";
 export BUILD_DIR=\"{builddir}\";make -C macosx deploy;
 sudo make -C macosx install-deploy INSTALL_ROOT=\"{pkgdir}\"'''.format(
            cflags="-arch i386 -arch x86_64 -arch ppc -mmacosx-version-min=10.14 -enable-aqua --enable-framework",
            ver=self.__version__,
            builddir=self.__thirdPartyBuildFolder__,
            pkgdir=self.__thirdPartyPkgFolder__
            ))

    # Package the third party that was build   
    def __package__(self):
                      
        # Do not call the motherclass __pakcage__ method that 
        # woud erase the components installed! So call the requirements package by hand
        for iReq in self.__requirements__:
            iReq.__package__()

        # Make the lib folder and move the framework. Remove the folder 'Library'
        self.__makedirs__(self.__buildInfo__["LIBPATH"][0])
        self.__move__(os.path.join(self.__thirdPartyPkgFolder__,"Library","Frameworks","Tk.framework"),
                      os.path.join(self.__thirdPartyPkgFolder__,"lib","Tk.framework"))
        self.__remove__(os.path.join(self.__thirdPartyPkgFolder__,"Library"))   
               
        # Rename the folder:    
        #                self.__thirdPartyPkgFolder__/usr/local/bin 
        #                to: bin
        self.__move__(os.path.join(self.__thirdPartyPkgFolder__,"usr","local","bin"),
                            os.path.join(self.__thirdPartyPkgFolder__,self.__buildInfo__["BINPATH"][0]))
        self.__remove__(os.path.join(self.__thirdPartyPkgFolder__,"usr"))   
         
        # Copy pkg/lib/Tk.framework/Versions/8.7/libtkstub8.7.a to pkg/lib/libtkstub8.7.a
        os.__copy__(os.path.join(self.__buildInfo__["LIBPATH"][0],"Tck.framework","Versions","8.7","libtkstub8.7.a"), 
                    os.path.join(self.__buildInfo__["LIBPATH"][0],"libtkstub8.7.a") )
         
        # Make an include dir and set a link to the includes in the framework
        self.__copy__(os.path.join(self.__thirdPartyPkgFolder__,"lib","Tk.framework","Headers"),
                      self.__buildInfo__["INCLUDEPATH"][0])

    def __test__(self):
        pass
    
# ------------------------------------------------------------------------
           
# Blas is a requirement for IpOopt, that offers utility scripts to download and 
# compile blas with the right bindings
class FreeTypeCompile(thirdPartyCompile):

    # Ctor
    def __init__(self):
    
        # Simply call mother-class init
        super(FreeTypeCompile,self).__init__()

        # Name of this third_party. 
        self.__name__="FreeType"

        # Version of this third_party
        self.__version__="2.9.1"

        # Define the URL from which IpOpt can be downloaded
        # Note that this should be the complete path of the file
        # to be downloaded (Including the filename!)
        self.__url__="https://download.savannah.gnu.org/releases/freetype/freetype-2.9.1.tar.gz"

        # Define the name of the archive downloadeed from the web. A priori this is 
        # simply the last entry of the url
        self.__srcArchiveName__="freetype-2.9.1.tar"

        # Define the name of the folder extracted from the archive downloadeed from the web. 
        # A priori this is simply the srcArchiveName without the extension
        self.__srcDirName__="freetype-2.9.1"

        # Override (specialize) the build folder 
        self.__thirdPartyBuildFolder__= os.path.join(self.__thirdPartyBuildFolder__,self.__srcDirName__)

        # Override (specialize) the package folder 
        self.__thirdPartyPkgFolder__= os.path.join(self.__thirdPartyPkgFolder__,self.__srcDirName__)
        
                # Define the build info. Will use these to copy the components (includes, libs...) to 
        # the package folder
        self.__buildInfo__["INCLUDEPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"include")]
        self.__buildInfo__["LIBPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"lib")]
        self.__buildInfo__["BINPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"bin")]
        self.__buildInfo__["DOCPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"doc")]
        self.__buildInfo__["LIBS"] = ["freetype"]
                    
    # Follow instructions from OpenCascade in : 
    # opencascade-7.1.0/doc/overview/html/occt_dev_guides__building_3rdparty_osx.html
    def __compile__(self,dest=None):

        # Decorates motherclass method and get to the build folder
        super(FreeTypeCompile,self).__compile__()

        # This is a test. gawk is needed to configure the build, so it must be installed 
        # and functinal. So I introduce a safeguard here.  
        self.__execute__("gawk -h")
        
        # Execute configure        
        self.__execute__("./configure --prefix={} CFLAGS=\'-m64 -fPIC\' CPPFLAGS=\'-m64 -fPIC\'".
                         format(self.__thirdPartyPkgFolder__)) 

        #  compile
        self.__execute__("make")
        
        
    # Package the third party that was build   
    def __package__(self):
        
        # Decorates motherclass method and get to the build folder
        super(FreeTypeCompile,self).__package__()
        
        # Go back to the build folder where configure has been executed
        os.chdir(self.__thirdPartyBuildFolder__)

        # make install
        self.__execute__("make install")
    
    def __test__(self):
        
        # Decorate the mother class __test__ method
        super(FreeTypeCompile,self).__test__()

        # Write the cppunit example. This comes from the tutorial section of the docs: 
        # https://www.freetype.org/freetype2/docs/tutorial/step1.html#section-1
        Source=open("main.cpp","w")
        Source.write('''
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#define WIDTH   120
#define HEIGHT  60

/* origin is the upper left corner */
unsigned char image[HEIGHT][WIDTH];

/* Replace this function with something useful. */
void draw_bitmap( FT_Bitmap*  bitmap,
                 FT_Int      x,
                 FT_Int      y) {
    FT_Int  i, j, p, q;
    FT_Int  x_max = x + bitmap->width;
    FT_Int  y_max = y + bitmap->rows;

    /* for simplicity, we assume that `bitmap->pixel_mode' */
    /* is `FT_PIXEL_MODE_GRAY' (i.e., not a bitmap font)   */

    for ( i = x, p = 0; i < x_max; i++, p++ ) {
        for ( j = y, q = 0; j < y_max; j++, q++ ) {
            if ( i < 0      || j < 0       ||
                   i >= WIDTH || j >= HEIGHT )
                continue;

        image[j][i] |= bitmap->buffer[q * bitmap->width + p];
    }
  }
}

void show_image( void ) {
  int  i, j;

  for ( i = 0; i < HEIGHT; i++ ) {
    for ( j = 0; j < WIDTH; j++ )
      putchar( image[i][j] == 0 ? \' \'
                                : image[i][j] < 128 ? \'+\'
                                                    : \'*\' );
    putchar( \'\\n\' );
  }
}

int main( int argc, char**  argv ) {
  FT_Library    library;
  FT_Face       face;

  FT_GlyphSlot  slot;
  FT_Matrix     matrix;                 /* transformation matrix */
  FT_Vector     pen;                    /* untransformed origin  */
  FT_Error      error;

  char*         filename;
  char*         text;

  double        angle;
  int           target_height;
  int           n, num_chars;

  if ( argc != 3 ) {
    fprintf ( stderr, \"usage: %s font sample-text\\n\", argv[0] );
    exit( 1 );
  }

  filename      = argv[1];                           /* first argument     */
  text          = argv[2];                           /* second argument    */
  num_chars     = strlen( text );
  //angle         = ( 25.0 / 360 ) * 3.14159 * 2;      /* use 25 degrees     */
  angle         = 0;
  target_height = HEIGHT;

  error = FT_Init_FreeType( &library );              /* initialize library */
  /* error handling omitted */

  error = FT_New_Face( library, filename, 0, &face );/* create face object */
  /* error handling omitted */

  /* use 50pt at 100dpi */
  int char_width=320, char_height=50;
  int horz_resolution=1000, vert_resolution=5000;
  
  error = FT_Set_Char_Size( face, char_width, char_height, horz_resolution, vert_resolution );                /* set character size */

  /* error handling omitted */

  /* cmap selection omitted;                                        */
  /* for simplicity we assume that the font contains a Unicode cmap */

  slot = face->glyph;

  /* set up matrix */
  matrix.xx = (FT_Fixed)( cos( angle ) * 0x10000L );
  matrix.xy = (FT_Fixed)(-sin( angle ) * 0x10000L );
  matrix.yx = (FT_Fixed)( sin( angle ) * 0x10000L );
  matrix.yy = (FT_Fixed)( cos( angle ) * 0x10000L );

  /* the pen position in 26.6 cartesian space coordinates; */
  /* start at (0,500) relative to the upper left corner  */
  pen.x = 0;
  pen.y = 500;

  for ( n = 0; n < num_chars; n++ ) {
    // set transformation 
    FT_Set_Transform( face, &matrix, &pen );

    // load glyph image into the slot (erase previous one) 
    error = FT_Load_Char( face, text[n], FT_LOAD_RENDER );
    if ( error )
      continue;                 // ignore errors 

    // now, draw to our target surface (convert position) 
    draw_bitmap( &slot->bitmap,
                 slot->bitmap_left,
                 target_height - slot->bitmap_top );

    // increment pen position 
    pen.x += slot->advance.x;
    pen.y += slot->advance.y;
  }
  
  show_image();

  FT_Done_Face    ( face );
  FT_Done_FreeType( library );

  return 0;
}''')
        
        Source.close()
             
        # Compile and run the test. This will work if the chosen 
        # font style *.ttf file exists
        #fontFile="/Library/Fonts/Arial.ttf"
        fontFile="/Library/Fonts/Herculanum.ttf"
        self.__makeTest__("{} vpp".format(fontFile))

# ------------------------------------------------------------------------

class OpenCascadeCompile(thirdPartyCompile):
    
    # Ctor
    def __init__(self):
        
        # Simply call mother-class init
        super(OpenCascadeCompile,self).__init__()

        # Name of this third_party. 
        self.__name__="OpenCascade"
        
        # Version of this third_party
        self.__version__="7.1.0"
        
        # Define the URL from which the src archive can be downloaded
        # Note that this should be the complete path of the file
        # to be downloaded (Including the filename!)
        #self.__url__=????

        # Define the name of the archive downloadeed from the web. A priori this is 
        # simply the last entry of the url
        #self.__srcArchiveName__=???
        
        # Define the name of the folder extracted from the archive downloadeed from the web. 
        # A priori this is simply the srcArchiveName without the extension
        self.__srcDirName__="openCascade-7.1.0"
        
        # Override (specialize) the build folder 
        self.__thirdPartyBuildFolder__= os.path.join(self.__thirdPartyBuildFolder__,self.__srcDirName__)

        # Override (specialize) the package folder 
        self.__thirdPartyPkgFolder__= os.path.join(self.__thirdPartyPkgFolder__,self.__srcDirName__)

        # Instantiate requirements, see file:///Users/dtrimarchi/third_party/opencascade-7.1.0/doc/overview/html/occt_dev_guides__building_3rdparty_osx.html
        # by now I only install the mandatory : Tcl/Tk 8.5 , FreeType 2.4.10
        self.__requirements__.append( TkCompile() )
        self.__requirements__.append( FreeTypeCompile() )

        # Define the build info. Will use these to copy the components (includes, libs...) to 
        # the package folder
        self.__buildInfo__["INCLUDEPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"include")]
        self.__buildInfo__["LIBPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"lib")]
        self.__buildInfo__["DOCPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"doc")]
        self.__buildInfo__["LIBS"] = [] 
        
  
    # Compile this package    
    def __compile__(self,dest=None):
        
        # Decorate the mother class __package__ method
        #super(OpenCascadeCompile,self).__compile__()
        os.chdir(self.__thirdPartyBuildFolder__)
         
        buildFolderName= "Build"
      
        # Delete and remake the build folder
        shutil.rmtree(buildFolderName)
        os.mkdir(buildFolderName)
 
        # Patch CMakeLists.txt
        self.__append__("\nif(APPLE)\nset( CMAKE_MACOSX_RPATH 1)\nendif()\n",os.path.join(self.__thirdPartyBuildFolder__,"CMakeLists.txt"))
 
        # Get the requirements : tcl and tk
        tcl= self.__getReqByName__("Tcl")
        tk = self.__getReqByName__("Tk")
        
        # Execute Cmake
        self.__execute__('cmake -s . -B {buildDir} '
'-D 3RDPARTY_DIR:PATH={thirdPartyDir} '
'-D 3RDPARTY_TCL_DIR:PATH={tclDir} '
'-D 3RDPARTY_TCL_INCLUDE_DIR:PATH={tclIncludeDir} '
'-D 3RDPARTY_TCL_LIBRARY:FILE={tclLib} '
'-D 3RDPARTY_TCL_LIBRARY_DIR:PATH={tclLibDir} '
'-D 3RDPARTY_TK_DIR:PATH={tkDir} ' 
'-D 3RDPARTY_TK_INCLUDE_DIR:PATH={tkIncludeDir} '
'-D 3RDPARTY_TK_LIBRARY:FILE={tkLib} '
'-D 3RDPARTY_TK_LIBRARY_DIR:PATH={tkLibDir}'.format(
                             buildDir= buildFolderName,
                             thirdPartyDir=os.path.join(self.__thirdPartyPkgFolder__,".."),
                             tclDir= tcl.__thirdPartyPkgFolder__,
                             tclIncludeDir= tcl.getIncludePath()[0],
                             tclLib= os.path.join(
                                 tcl.getLibPath()[0],
                                 self.getFullStaticLibName(tcl.getLibs()[0])
                                 ),
                             tclLibDir= tcl.getLibPath()[0],
                             tkDir= tk.__thirdPartyPkgFolder__,
                             tkIncludeDir= tk.getIncludePath()[0],
                             tkLib= os.path.join(
                                 tk.getLibPath()[0],
                                 self.getFullStaticLibName(tk.getLibs()[0])
                                 ),
                             tkLibDir= tk.getLibPath()[0]
                             )
                         )
        
    def __copySelectedDocs__(self):
        
        # Loop on the modules we seek to compile 
        for iLib in self.__buildInfo__["LIBS"] :
            # Boost libs are named boost_something. Here we only seek the 'something'
            cleanName = iLib.replace("boost_","")

            # Verify the object we get is in the form we need            
            # and define the doc directory as pkgDir/doc/libDir
            checkListOfSizeOne(self.__buildInfo__["DOCPATH"],"DocPath")             
            
            # Make the doc directory
            docDir = os.path.join(self.__buildInfo__["DOCPATH"][0],cleanName)
            os.makedirs(docDir)

            # Copy the files to the doc directory
            shutil.copy(os.path.join(self.__thirdPartyBuildFolder__,"libs",cleanName,"index.html"), 
                    docDir)
            self.__copytree__(os.path.join(self.__thirdPartyBuildFolder__,"libs",cleanName,"doc"), 
                              os.path.join(docDir,"doc"))

    # Package the third party that was build   
    def __package__(self):
              
        # Decorate the mother class __package__ method
        super(OpenCascadeCompile,self).__package__()
                
        # Copy the content of include
        self.__copytree__(os.path.join(self.__thirdPartyBuildFolder__,"boost"), 
                          os.path.join(self.__buildInfo__["INCLUDEPATH"][0],"boost"))

        self.__copytree__(os.path.join(self.__thirdPartyBuildFolder__,"stage","lib"), 
                          self.__buildInfo__["LIBPATH"][0])

        # Also copy the documentation of the modules we are compiling
        self.__copySelectedDocs__()
                                      
    # Run a simple test to check that the compile was successiful
    def __test__(self):

        # Decorate the mother class __test__ method
        super(OpenCascadeCompile,self).__test__()

        # Write the boost example 
        Source=open("main.cpp","w")
        Source.write('''
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>
#include <boost/core/lightweight_test.hpp>
#include <cerrno>
using std::cout;
using namespace boost::filesystem;
using namespace boost::system;

static error_code e1( 1, system_category() );
static std::string m1 = e1.message();

static error_code e2( ENOENT, generic_category() );
static std::string m2 = e2.message();

int main(int argc, char** argv) {

    // Test instantiating a shared ptr
    boost::shared_ptr<int> myPtr; 

    // 
    path p (argv[1]);
    
    try {
        if (exists(p))
            cout<<\"Ok!\\n\";
    } catch (const filesystem_error& ex) {
        cout << ex.what() << \"\\n\";
    }
}       
''')
        Source.close()
            
        # Compile and run the test
        self.__makeTest__()
        
    # -- 
            
    # Import the dynamic libraries from third party to the dest folder (in this case
    # this will be in the app bundle VPP.app/Contents/Frameworks/
    def importDynamicLibs(self,dst):
    
        # Copy the lib to the dest folder
        for iLib in self.__buildInfo__["LIBS"]: 
            shutil.copyfile(self.getFullDynamicLibName(iLib), 
                            os.path.join(dst,self.getFullDynamicLibName(iLib)))
                    
        