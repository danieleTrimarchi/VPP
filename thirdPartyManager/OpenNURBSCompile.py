from thirdPartyCompile import thirdPartyCompile
import os, shutil, sys, glob


class OpenNURBSCompile(thirdPartyCompile):
    
    # Ctor
    def __init__(self):
        
        # Simply call mother-class init
        super(OpenNURBSCompile,self).__init__()

        # Name of this third_party. 
        self.__name__="OpenNURBS"
        
        # Version of this third_party
        self.__version__="1.0"
        
        # Define the URL from which the src archive can be downloaded
        # Note that this should be the complete path of the file
        # to be downloaded (Including the filename!)
        self.__url__="https://github.com/mcneel/opennurbs/archive/master.zip"

        # Define the name of the archive downloadeed from the web. A priori this is 
        # simply the last entry of the url
        self.__srcArchiveName__="opennurbs-master"
        
        # Define the name of the folder extracted from the archive downloadeed from the web. 
        # A priori this is simply the srcArchiveName without the extension
        self.__srcDirName__="opennurbs-master"
        
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
        self.__buildInfo__["LIBS"] = ["opennurbs_public","opennurbs_public_zlib"] 
          
    # Compile this package    
    def __compile__(self,dest=None):
        
        # Decorate the mother class __package__ method
        super(OpenNURBSCompile,self).__compile__()
        
        # Run the XCode project opennurbs_public.xcodeproj with XCode. Use default settings
        self.__execute__("xcodebuild")

        # Also run the XCode project opennurbs_public_zlib.xcodeproj that lives in ./zlib
        os.chdir("zlib")
        self.__execute__("xcodebuild")
        
        # the build is performed in ./build and the static lib lives in 
        # ./build/Release/libopennurbs_public.a
                
    # Package the third party that was build   
    def __package__(self):
               
        # Decorate the mother class __package__ method. Do not make directories 
        # as we are handling this direcly here
        super(OpenNURBSCompile,self).__package__()
        
        # Go back to the build folder
        os.chdir( self.__thirdPartyBuildFolder__ )

        # Copy the libs to 'libPath'
        self.__copy__(os.path.join(self.__thirdPartyBuildFolder__,"build","Release","libopennurbs_public.a"), 
                      os.path.join(self.__buildInfo__["LIBPATH"][0],"libopennurbs_public.a"))
        self.__copy__(os.path.join(self.__thirdPartyBuildFolder__,"zlib","build","Release","libopennurbs_public_zlib.a"), 
                      os.path.join(self.__buildInfo__["LIBPATH"][0],"libopennurbs_public_zlib.a"))
        
        # Copy the haders to 'include'        
        for header in glob.glob(os.path.join(self.__thirdPartyBuildFolder__,"*.h")):
            self.__copy__(header,os.path.join(self.__buildInfo__["INCLUDEPATH"][0],os.path.basename(header)))       
        # Copy selected headers
        self.__copy__(os.path.join(self.__thirdPartyBuildFolder__,"example_userdata","example_ud.h"),
                      os.path.join(self.__buildInfo__["INCLUDEPATH"][0],"example_ud.h") )
        
        # Copy the docs
        self.__copy__(os.path.join(self.__thirdPartyBuildFolder__,"README.md"),
                      os.path.join(self.__buildInfo__["DOCPATH"][0],"README.md"))

     # Run a simple test to check that the compile was successiful
    def __test__(self):
 
         # Decorate the mother class __test__ method
         super(OpenNURBSCompile,self).__test__()
 
         # Copy the example file (thirdPartyManager/resources/my_surfaces.3dm)
         pathOfThisFile= os.path.dirname( os.path.realpath(__file__) )
         exampleFile= os.path.join(pathOfThisFile,"resources","my_surfaces.3dm")
         self.__copy__(exampleFile,"my_surfaces.3dm")
         
         # Write the test code - this direcly derives from the example given in the 
         # opennurbs source archive : example_read.cpp
         Source=open("main.cpp","w")
         Source.write('''
//  simplified version of : example_write.cpp / example_read.cpp
// 
//  Example program using the Rhino file IO toolkit.  The program writes
//     and reads back in a simple .3dm file with a NURBS surface

#include "opennurbs_public_examples.h"
#include "example_ud.h"
#include <iostream>

#define INTERNAL_INITIALIZE_MODEL(model) Internal_SetExampleModelProperties(model,OPENNURBS__FUNCTION__,__FILE__)


static void Internal_SetExampleModelProperties(
        ONX_Model& model,
        const char* function_name,
        const char* source_file_name
)
{
    const bool bHaveFunctionName = (nullptr != function_name && 0 != function_name[0]);
    if ( !bHaveFunctionName )
        function_name = "";

    const bool bHaveFileName = (nullptr != source_file_name && 0 != source_file_name[0]);
    if (!bHaveFileName)
        source_file_name = "";

    model.m_sStartSectionComments = "This was file created by OpenNURBS toolkit example code.";

    // set application information
    const ON_wString wide_function_name(function_name);
    const ON_wString wide_source_file_name(source_file_name);
    model.m_properties.m_Application.m_application_name
    = bHaveFunctionName
    ? ON_wString::FormatToString(L"OpenNURBS toolkit Example: %ls() function", static_cast<const wchar_t*>(wide_function_name))
    : ON_wString(L"OpenNURBS Examples");

    model.m_properties.m_Application.m_application_URL = L"http://www.opennurbs.org";
    model.m_properties.m_Application.m_application_details
    = bHaveFileName
    ? ON_wString::FormatToString(L"Opennurbs examples are in the file %ls.", static_cast<const wchar_t*>(wide_source_file_name))
    : ON_wString::FormatToString(L"Opennurbs examples are example_*.cpp files.");

    // some notes
    if (bHaveFunctionName && bHaveFileName)
    {
        model.m_properties.m_Notes.m_notes
        = ON_wString::FormatToString(
                L"This .3dm file was made with the OpenNURBS toolkit example function %s() defined in source code file %ls.",
                static_cast<const wchar_t*>(wide_function_name),
                static_cast<const wchar_t*>(wide_source_file_name));
        model.m_properties.m_Notes.m_bVisible = model.m_properties.m_Notes.m_notes.IsNotEmpty();
    }

    // set revision history information
    model.m_properties.m_RevisionHistory.NewRevision();
}

static bool Internal_WriteExampleModel(
        const ONX_Model& model,
        const wchar_t* filename,
        ON_TextLog& error_log ) {

    int version = 0;

    // writes model to archive
    return model.Write( filename, version, &error_log );
}

// This comes from example_write.cpp : write_surfaces_example
bool write(const ON_wString& fileName) {

    ONX_Model model;
    INTERNAL_INITIALIZE_MODEL(model);

    // The code between the comment bands has nothing to do with I/O.
    // It is simply an easy way to get a NURBS surface to write.
    const int bIsRational = false;
    const int dim = 3;
    const int u_degree = 2;
    const int v_degree = 3;
    const int u_cv_count = 3;
    const int v_cv_count = 5;

    // The knot vectors do NOT have the 2 superfluous knots
    // at the start and end of the knot vector.  If you are
    // coming from a system that has the 2 superfluous knots,
    // just ignore them when writing a 3dm file.
    double u_knot[ u_cv_count + u_degree - 1 ];
    double v_knot[ v_cv_count + v_degree - 1 ];

    // make up a quadratic knot vector with no interior knots
    u_knot[0] = u_knot[1] = 0.0;
    u_knot[2] = u_knot[3] = 1.0;

    // make up a cubic knot vector with one simple interior knot
    v_knot[0] = v_knot[1] = v_knot[2] = 0.0;
    v_knot[3] = 1.5;
    v_knot[4] = v_knot[5] = v_knot[6] = 2.0;

    // Rational control points can be in either homogeneous
    // or euclidean form. Non-rational control points do not
    // need to specify a weight.
    ON_3dPoint CV[u_cv_count][v_cv_count];

    int i, j;
    for ( i = 0; i < u_cv_count; i++ ) {
        for ( j = 0; j < v_cv_count; j++ ) {
            CV[i][j].x = i;
            CV[i][j].y = j;
            CV[i][j].z = i-j;
        }
    }

    // write a line on the default layer
    ON_NurbsSurface nurbs_surface( dim, bIsRational,
            u_degree+1, v_degree+1,
            u_cv_count, v_cv_count );

    for ( i = 0; i < nurbs_surface.KnotCount(0); i++ )
        nurbs_surface.SetKnot( 0, i, u_knot[i] );

    for ( j = 0; j < nurbs_surface.KnotCount(1); j++ )
        nurbs_surface.SetKnot( 1, j, v_knot[j] );

    for ( i = 0; i < nurbs_surface.CVCount(0); i++ ) {
        for ( j = 0; j < nurbs_surface.CVCount(1); j++ ) {
            nurbs_surface.SetCV( i, j, CV[i][j] );
        }
    }

    model.AddModelGeometryComponent(&nurbs_surface, nullptr);
    //   model.AddDefaultLayer(L"NURBS surface", ON_Color::UnsetColor);

    // errors printed to stdout
    ON_TextLog error_log;

    return Internal_WriteExampleModel(model, fileName, error_log);
}

void read(const ON_wString& fileName) {

    // Default dump is to stdout
    ON_TextLog dump;
    dump.SetIndentSize(2);

    // Placeholder for the model to be read
    ONX_Model model;

    dump.Print("\\nOpenNURBS Archive File:  %s ", &fileName );

    // File to be read
    FILE* archive_fp = ON::OpenFile( fileName, L"rb");
    if ( !archive_fp )
    {
        dump.Print("  Unable to open file.\\n" );
    }

    // create achive object from file pointer
    ON_BinaryFile archive( ON::archive_mode::read3dm, archive_fp );

    // read the contents of the file into "model"
    bool rc = model.Read( archive, &dump );

    // close the file
    ON::CloseFile( archive_fp );

    // print diagnostic
    if ( rc )
        dump.Print("Successfully read.\\n");
    else
        dump.Print("Errors during reading.\\n");

    // Try getting the surface from the file
    //    ON_NurbsSurface surf = model.

    // Iterate on the geom entities of this file
    // See https://developer.rhino3d.com/guides/opennurbs/reading-render-meshes/
    ONX_ModelComponentIterator it(model, ON_ModelComponent::Type::ModelGeometry);
    const ON_ModelComponent* model_component = nullptr;
    for (model_component = it.FirstComponent(); nullptr != model_component; model_component = it.NextComponent()) {
        const ON_ModelGeometryComponent* model_geometry = ON_ModelGeometryComponent::Cast(model_component);
        if(nullptr != model_geometry ){
            std::cout<<"-->> Model Component Name= "<<model_geometry->Name()<<std::endl;

            const ON_NurbsSurface* surf = ON_NurbsSurface::Cast(model_geometry->Geometry(nullptr));
            if (nullptr != surf)
            {
                std::cout<<"Found NURBS surface of dimension : "<< surf->Dimension()<<std::endl;
                std::cout<<"Found NURBS surface of u degree : "<< surf->Degree(0)<<std::endl;
                std::cout<<"Found NURBS surface of v degree : "<< surf->Degree(1)<<std::endl;
                std::cout<<"Found NURBS u knots : "<< surf->KnotCount(0)<<std::endl;
                std::cout<<"Found NURBS v knots : "<< surf->KnotCount(1)<<std::endl;
                std::cout<<"u knots values: "<<std::endl;
                for(int iKnot=0; iKnot<surf->KnotCount(0); iKnot++)
                    std::cout<<surf->Knot(0,iKnot)<<std::endl;
                std::cout<<"v knots values: "<<std::endl;
                for(int jKnot=0; jKnot<surf->KnotCount(1); jKnot++)
                    std::cout<<surf->Knot(1,jKnot)<<std::endl;
                // Rational control points...
                std::cout<<"Rational control pts: "<<std::endl;
                for(int i=0; i<surf->CVCount(0); i++)
                    for(int j=0; j<surf->CVCount(1); j++)
                        std::cout<<surf->CV(i,j)[0]<<" "<<surf->CV(i,j)[1]<<" "<<surf->CV(i,j)[2]<<std::endl;
            }

        }
    }

    // create a text dump of the model
    dump.PushIndent();
    model.Dump(dump);
    dump.PopIndent();
    dump.PopIndent();
}

int main( int argc, const char *argv[] ) {

    // Set the fileName to be used for I/O
    ON_wString fileName("my_surfaces_fromCode.3dm");

    write(fileName);

    read(fileName);


    return 0;
}
''')
         Source.close()
             
         # Compile and run the test
         self.__makeTest__()
   