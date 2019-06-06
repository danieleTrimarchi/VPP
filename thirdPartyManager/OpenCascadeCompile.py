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
             
        # Copy the init.tcl file from thirdPartyBuildFolder/tcl8.7a1/library 
        # to the lib folder
        self.__copy__(os.path.join(self.__thirdPartyBuildFolder__,"library","init.tcl"),
                      os.path.join(self.__buildInfo__["LIBPATH"][0],"init.tcl"))

        # make a symbolic link the library from pkg/lib/Tck.framework/Versions/8.7/libtclstub8.7.a to pkg/lib/libtclstub8.7.a
        self.__copy__(os.path.join(self.__buildInfo__["LIBPATH"][0],"Tcl.framework","Versions","8.7","libtclstub8.7.a"), 
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
        # The requirements must be already packaged when compiling, otherwise it is not possible to compile!

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
         
        # Make a symbolic link pkg/lib/Tk.framework/Versions/8.7/libtkstub8.7.a to pkg/lib/libtkstub8.7.a
        self.__copy__(os.path.join(self.__buildInfo__["LIBPATH"][0],"Tk.framework","Versions","8.7","libtkstub8.7.a"), 
                      os.path.join(self.__buildInfo__["LIBPATH"][0],"libtkstub8.7.a") )
         
        # Make an include dir and set a link to the includes in the framework
        self.__copy__(os.path.join(self.__thirdPartyPkgFolder__,"lib","Tk.framework","Headers"),
                      self.__buildInfo__["INCLUDEPATH"][0])

    def __test__(self):

        pass
        # A test target is provided. Go into the build folder of tk and run 
        # make test
        # The test suite appears to hang, I am not sure ewhat is wron so I  
        # comment out for the moment

#         # Go to the build folder
#         os.chdir(os.path.join(self.__thirdPartyBuildFolder__,'macosx'))
#         
#         # Run a couple of test targets
#         self.__execute__("make test")
    
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
        self.__buildInfo__["BINPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"bin")]
        self.__buildInfo__["DOCPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"doc")]
        self.__buildInfo__["LIBS"] = ['TKBO','TKBRep','TKFillet','TKGeomBase',
                                      'TKG2d','TKG3d','TKernel','TKMath',
                                      'TKOffset','TKPrim','TKTopAlgo'] 
          
    # Compile this package    
    def __compile__(self,dest=None):
        
        # Decorate the mother class __package__ method
        super(OpenCascadeCompile,self).__compile__()
                
        # Delete and remake the Build folder
        buildFolderName= "Build"
        self.__remove__(buildFolderName)
        os.mkdir(buildFolderName)
  
        # Patch CMakeLists.txt for MACOSX
        self.__append__("\nif(APPLE)\nset( CMAKE_MACOSX_RPATH 1)\nendif()\n",os.path.join(self.__thirdPartyBuildFolder__,"CMakeLists.txt"))
  
        # Get the requirements : tcl and tk
        tcl= self.__getReqByName__("Tcl")
        tk = self.__getReqByName__("Tk")
         
        # Execute the Cmake configuration
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
        
        # Go to the build folder
        os.chdir(buildFolderName)

        # Now opencascade can be made
        self.__execute__("make")
        
    # Package the third party that was build   
    def __package__(self):
               
        # Decorate the mother class __package__ method. Do not make directories 
        # as we are handling this direcly here
        super(OpenCascadeCompile,self).__package__(makeDirs=False)
                 
        # Cleanup and copy the content of include
        shutil.rmtree(self.__buildInfo__["INCLUDEPATH"][0],sys.exc_info())
        self.__copytree__(os.path.join(self.__thirdPartyBuildFolder__,"Build","inc"), 
                          self.__buildInfo__["INCLUDEPATH"][0])
 
        # Clenanup and copy the libs 
        shutil.rmtree(self.__buildInfo__["LIBPATH"][0],sys.exc_info())
        self.__copytree__(os.path.join(self.__thirdPartyBuildFolder__,"Build","mac64","clang","lib"), 
                          self.__buildInfo__["LIBPATH"][0])

        # Cleanup and copy the binaries 
        shutil.rmtree(self.__buildInfo__["BINPATH"][0],sys.exc_info())
        self.__copytree__(os.path.join(self.__thirdPartyBuildFolder__,"Build","mac64","clang","bin"), 
                          self.__buildInfo__["BINPATH"][0])

        # Cleanup and copy the documentation  
        shutil.rmtree(self.__buildInfo__["DOCPATH"][0],sys.exc_info())
        self.__copytree__(os.path.join(self.__thirdPartyBuildFolder__,"Build","doc","overview"), 
                          self.__buildInfo__["DOCPATH"][0])

     # Run a simple test to check that the compile was successiful
    def __test__(self):
 
         # Decorate the mother class __test__ method
         super(OpenCascadeCompile,self).__test__()
 
         # Write the test code 
         Source=open("main.cpp","w")
         Source.write('''
#include <iostream>

#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>

#include <TopExp_Explorer.hxx>

#include <GC_MakeArcOfCircle.hxx>
#include <GC_MakeSegment.hxx>

#include <gp_Ax1.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>

#include <BRepLib.hxx>

#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepOffsetAPI_MakeThickSolid.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Transform.hxx>

#include <BRepAlgoAPI_Fuse.hxx>

#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_TrimmedCurve.hxx>

#include <GCE2d_MakeSegment.hxx>

using std::cout;
 
// This code runs the bottle tutorial described in the OpenCascade documentation

int main(int argc, char** argv) {
     
    try {
        // Define size
        double myWidth=20, myThickness=2, myHeight=50; 
        
        // Define points
        gp_Pnt aPnt1(-myWidth / 2., 0, 0);
        gp_Pnt aPnt2(-myWidth / 2., -myThickness / 4., 0);
        gp_Pnt aPnt3(0, -myThickness / 2., 0);
        gp_Pnt aPnt4(myWidth / 2., -myThickness / 4., 0);
        gp_Pnt aPnt5(myWidth / 2., 0, 0);
        
        // Get the x value of a point
        Standard_Real xValue1 = aPnt1.X();

        // ------ GEOMETRY -----------------------------
        // Make an arc of circle
        GC_MakeArcOfCircle mkArc(aPnt2,aPnt3,aPnt4);
        Handle(Geom_TrimmedCurve) arc1;
        if(mkArc.IsDone()){
            arc1 = mkArc.Value();
        } else 
            throw "ERROR : Could not build the arc"; 
            
        // Make a segment
        GC_MakeSegment mkSeg(aPnt1, aPnt2);
        Handle(Geom_TrimmedCurve) aSegment1;    
        if(mkSeg.IsDone()){
            aSegment1 = mkSeg.Value();
        } else 
            throw "***ERROR : Could not build the Seg1"; 

        // Make a segment
        GC_MakeSegment mkSeg2(aPnt4, aPnt5);
        Handle(Geom_TrimmedCurve) aSegment2;    
        if(mkSeg2.IsDone()){
            aSegment2 = mkSeg2.Value();
        } else 
            throw "***ERROR : Could not build the Seg2"; 

        // ------ TOPOLOGY -----------------------------
    
        // Create some geometry-based edges
        TopoDS_Edge aEdge1 = BRepBuilderAPI_MakeEdge(aSegment1);
        TopoDS_Edge aEdge2 = BRepBuilderAPI_MakeEdge(arc1);
        TopoDS_Edge aEdge3 = BRepBuilderAPI_MakeEdge(aSegment2);

        // Create a wire to connect edges 
        TopoDS_Wire aWire = BRepBuilderAPI_MakeWire(aEdge1, aEdge2, aEdge3);
    
        // ---- COMPLETING THE PROFILE ----------------
        gp_Pnt aOrigin(0, 0, 0);
        gp_Dir xDir(1, 0, 0);
        gp_Ax1 xAxis(aOrigin, xDir);
    
        // The x-axis can also be retrieved as a geometric constant
        // gp_Ax1 xAxis = gp::OX();
    
        // ---- DEFINE GEOMETRIC TRANSFORMATIONS -------------
        gp_Trsf aTrsf;
        aTrsf.SetMirror(xAxis);

        // APPLY GEOMETRIC TRANSFORMATIONS
        
        // Note: BRepBuilderAPI_Transform does not modify the nature 
        // of the shape: the result of the reflected wire remains a wire...
        BRepBuilderAPI_Transform aBRepTrsf(aWire, aTrsf);

        // ...but function-like call or the BRepBuilderAPI_Transform::Shape 
        // method returns a TopoDS_Shape object
        TopoDS_Shape aMirroredShape = aBRepTrsf.Shape();
    
        // Now cast the transformed wire
        TopoDS_Wire aMirroredWire = TopoDS::Wire(aMirroredShape);
        
        // Boolean the original & the mirrored wire
        BRepBuilderAPI_MakeWire mkWire;
        mkWire.Add(aWire);
        mkWire.Add(aMirroredWire);
        TopoDS_Wire myWireProfile = mkWire.Wire();        
    
        // ---- BUILD THE BODY -------------
        // 1_ generate a face out of the wire
        TopoDS_Face myFaceProfile = BRepBuilderAPI_MakeFace(myWireProfile);
    
        // 2_ Define a prism using the wire shape to sweep and sweep vector 
        gp_Vec aPrismVec(0, 0, myHeight);
        TopoDS_Shape myBody = BRepPrimAPI_MakePrism(myFaceProfile, aPrismVec);    
   
        // 3_ Apply fillets    
        BRepFilletAPI_MakeFillet mkFillet(myBody);
        
        // 4_ Explore the edges of the body to apply the fillet
        TopExp_Explorer anEdgeExplorer(myBody, TopAbs_EDGE);
        
        while(anEdgeExplorer.More()){
            TopoDS_Edge anEdge = TopoDS::Edge(anEdgeExplorer.Current());
            //Add edge to fillet algorithm
            mkFillet.Add(myThickness / 12., anEdge);
            anEdgeExplorer.Next();    
        }

        // ---- ADDING THE NECK -------------
        // Define a local reference system 
        gp_Pnt neckLocation(0, 0, myHeight);
        gp_Dir neckAxis = gp::DZ();
        gp_Ax2 neckAx2(neckLocation, neckAxis);
        
        // Make a cylinder using this new reference system
        Standard_Real myNeckRadius = myThickness / 4.;
        Standard_Real myNeckHeight = myHeight / 10;
        BRepPrimAPI_MakeCylinder MKCylinder(neckAx2, myNeckRadius, myNeckHeight);
        TopoDS_Shape myNeck = MKCylinder.Shape();
        
        // Boolean the main body and the neck
        myBody = BRepAlgoAPI_Fuse(myBody, myNeck);

        // ---- CREATING A HOLLOWED BODY -------------
        // In Open CASCADE, a hollowed solid is called a Thick Solid
       
        // Define placeholder for the face to remove 
        TopoDS_Face faceToRemove;
        Standard_Real zMax = -1e30;

        // Iterate on the faces of the body to find the top face of the neck. We want to remove this
        for(TopExp_Explorer aFaceExplorer(myBody, TopAbs_FACE) ; aFaceExplorer.More() ; aFaceExplorer.Next()){
            
            TopoDS_Face aFace = TopoDS::Face(aFaceExplorer.Current());
            
            // Access the surface of this face
            Handle(Geom_Surface) aSurface = BRep_Tool::Surface(aFace);

            // If this surface is a plane 
            if(aSurface->DynamicType() == STANDARD_TYPE(Geom_Plane)){

                // Downcast to a plane
                Handle(Geom_Plane) aPlane = Handle(Geom_Plane)::DownCast(aSurface);

                // Find a point on the plane. Extract its Z value
                gp_Pnt aPnt = aPlane->Location();
                Standard_Real aZ = aPnt.Z();
                if(aZ > zMax){
                    zMax = aZ;
                    faceToRemove = aFace;
                }

            }            
        }
        
        // Append the face to remove to a list - required by the API
        TopTools_ListOfShape facesToRemove;
        facesToRemove.Append(faceToRemove);
        
        // Create the 'Thick body' by removing the identified face(s). Assign a thickness
        myBody = BRepOffsetAPI_MakeThickSolid(myBody, facesToRemove, -myThickness / 50, 1.e-3);
        
        
        // ---- CREATING THE THREADING (FILETAGE) -------------
        
        // Create an inner and an outer threading cylinder on the NECK
        Handle(Geom_CylindricalSurface) aCyl1 = new Geom_CylindricalSurface(neckAx2, myNeckRadius * 0.99); 
        Handle(Geom_CylindricalSurface) aCyl2 = new Geom_CylindricalSurface(neckAx2, myNeckRadius * 1.05);
    
        // Define the starting point and the direction of the Threading 
        gp_Pnt2d aPnt(2. * M_PI, myNeckHeight / 2.);
        gp_Dir2d aDir(2. * M_PI, myNeckHeight / 4.);
        gp_Ax2d anAx2d(aPnt, aDir);

        // Define the ellipses
        Standard_Real aMajor = 2. * M_PI;
        Standard_Real aMinor = myNeckHeight / 10;
        Handle(Geom2d_Ellipse) anEllipse1 = new Geom2d_Ellipse(anAx2d, aMajor, aMinor);
        Handle(Geom2d_Ellipse) anEllipse2 = new Geom2d_Ellipse(anAx2d, aMajor, aMinor / 4);

        // As the parametric equation of an ellipse is 
        // P(U) = O + (MajorRadius * cos(U) * XDirection) + (MinorRadius * sin(U) * YDirection), 
        // the ellipses need to be limited between 0 and M_PI.
        Handle(Geom2d_TrimmedCurve) anArc1 = new Geom2d_TrimmedCurve(anEllipse1, 0, M_PI);
        Handle(Geom2d_TrimmedCurve) anArc2 = new Geom2d_TrimmedCurve(anEllipse2, 0, M_PI);
        
        // Defining a segment, a line limited by the first and the last point of one of the arcs
        gp_Pnt2d anEllipsePnt1 = anEllipse1->Value(0);
        gp_Pnt2d anEllipsePnt2;
        anEllipse1->D0(M_PI, anEllipsePnt2);        

        Handle(Geom2d_TrimmedCurve) aSegment = GCE2d_MakeSegment(anEllipsePnt1, anEllipsePnt2);

        // Compute the edges out of these curves
        TopoDS_Edge anEdge1OnSurf1 = BRepBuilderAPI_MakeEdge(anArc1, aCyl1);
        TopoDS_Edge anEdge2OnSurf1 = BRepBuilderAPI_MakeEdge(aSegment, aCyl1);
        TopoDS_Edge anEdge1OnSurf2 = BRepBuilderAPI_MakeEdge(anArc2, aCyl2);
        TopoDS_Edge anEdge2OnSurf2 = BRepBuilderAPI_MakeEdge(aSegment, aCyl2);        
        
        // create the two profiles of the threading, lying on each surface.
        TopoDS_Wire threadingWire1 = BRepBuilderAPI_MakeWire(anEdge1OnSurf1, anEdge2OnSurf1);
        TopoDS_Wire threadingWire2 = BRepBuilderAPI_MakeWire(anEdge1OnSurf2, anEdge2OnSurf2);
        
        // compute 3D curves for all the edges of a shape
        BRepLib::BuildCurves3d(threadingWire1);
        BRepLib::BuildCurves3d(threadingWire2);

        // Create the threading 
        BRepOffsetAPI_ThruSections aTool(Standard_True);
        aTool.AddWire(threadingWire1); aTool.AddWire(threadingWire2);
        aTool.CheckCompatibility(Standard_False);
        TopoDS_Shape myThreading = aTool.Shape();
        
        // ----- Building the Resulting Compound ------
        // build single shape from myBody and myThreading
        TopoDS_Compound aRes;
        BRep_Builder aBuilder;
        aBuilder.MakeCompound (aRes);
        aBuilder.Add (aRes, myBody);
        aBuilder.Add (aRes, myThreading);

        std::cout<<"\n\nOpenCascade test succeeded!\n\n"<<std::endl;
        
    } catch (std::exception& e) {
         cout << e.what() << \"\\n\";
    } catch (...) {
         cout << "Unknown exception caught" << \"\\n\";
    }

}       
''')
         Source.close()
             
         # Compile and run the test
         self.__makeTest__()
   