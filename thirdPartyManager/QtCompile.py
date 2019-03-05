from thirdPartyCompile import thirdPartyCompile
import os, shutil, sys, glob, fnmatch
import multiprocessing

class qt5SconsTool(thirdPartyCompile):
    
    def __init__(self):
        
        # Simply call mother-class init
        super(qt5SconsTool,self).__init__()

        # Name of this third_party. 
        self.__name__="SCons_Qt5"

        # Version of this third_party
        self.__version__="1.0"

        # Define the URL from which IpOpt can be downloaded
        # Note that this should be the complete path of the file
        # to be downloaded (Including the filename!)
        self.__url__="https://bitbucket.org/dirkbaechle/scons_qt5/get/9caa36eb469d.zip"

        # Define the name of the archive downloadeed from the web. A priori this is 
        # simply the last entry of the url
        self.__srcArchiveName__="dirkbaechle-scons_qt5-9caa36eb469d.zip"
        
        # Define the name of the folder extracted from the archive downloadeed from the web. 
        # A priori this is simply the srcArchiveName without the extension
        self.__srcDirName__="dirkbaechle-scons_qt5-9caa36eb469d"

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
        self.__buildInfo__["LIBS"] = []

    def __compile__(self,dest=None):
        
        # Decorate the mother class __package__ method
        super(qt5SconsTool,self).__compile__()
        
        # I need to package here. This is akward and shows the 
        # limit of my framework, too bad. This is a requirement 
        # for Qt, and I need to have it set completely before 
        # running __compile__ on qt. 
        # Decorate the mother class __package__ method
        super(qt5SconsTool,self).__package__()
        
        # Now copy the content of the src folder to the folder: 
        # package_folder/site_tools/qt5
        self.__copytree__(self.__thirdPartyBuildFolder__,
                          os.path.join(self.__buildInfo__["INCLUDEPATH"][0],"site_tools","qt5"))
        
    def __package__(self):
        
        # All was already done in __compile__
        pass

    def __test__(self):
        
        # We do not test here, because I need qt that 
        # I do not have yet. The test will be performed 
        # while compiling the Qt test
        pass
        
class QtCompile(thirdPartyCompile):
    
    # Ctor
    def __init__(self):
        
        # Simply call mother-class init
        super(QtCompile,self).__init__()

        # Name of this third_party. 
        self.__name__="Qt"
        
        # ----------------------------------------------------------------
        # /!\ WARNING : when changing version remember to als update the  
        #     .pc file updator in ../site_scons/qtPkgConfig/update.py
        # ----------------------------------------------------------------
        # Version of this third_party. 
        self.__version__="5.12.1"
        
        # Define the URL from which IpOpt can be downloaded
        # Note that this should be the complete path of the file
        # to be downloaded (Including the filename!)
        self.__url__="http://download.qt.io/official_releases/qt/5.12/5.12.1/single/qt-everywhere-src-5.12.1.tar.xz"

        # Define the name of the archive downloadeed from the web. A priori this is 
        # simply the last entry of the url
        self.__srcArchiveName__="qt-everywhere-src-5.12.1.tar.xz"
        
        # Define the name of the folder extracted from the archive downloadeed from the web. 
        # A priori this is simply the srcArchiveName without the extension
        self.__srcDirName__="qt-everywhere-src-5.12.1"
        
        # Override (specialize) the build folder 
        self.__thirdPartyBuildFolder__= os.path.join(self.__thirdPartyBuildFolder__,self.__srcDirName__)

        # Override (specialize) the package folder 
        self.__thirdPartyPkgFolder__= os.path.join(self.__thirdPartyPkgFolder__,self.__srcDirName__)

        # Instantiate the requirement: the scons qt5 build tool
        self.__requirements__.append( qt5SconsTool() )

        # Define the build info. Will use these to copy the components (includes, libs...) to 
        # the package folder
        self.__buildInfo__["LIBPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"lib")]
        self.__buildInfo__["DOCPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"doc")]
        self.__buildInfo__["INCLUDEPATH"] = [
                               os.path.join(self.__thirdPartyPkgFolder__,"include"),
                               os.path.join(self.getLibPath()[0],'QtCore.framework/Versions/Current/Headers'),
                               os.path.join(self.getLibPath()[0],'QtWidgets.framework/Versions/Current/Headers'),
                               os.path.join(self.getLibPath()[0],'QtGui.framework/Versions/Current/Headers')
                               ]
        self.__buildInfo__["BINPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"bin")]
        self.__buildInfo__["LIBS"] = ['QtCore',
                                      'QtGui',
                                      'QtWidgets',
                                      'QtDataVisualization', 
                                      'QtPrintSupport']
          
    # Compile this package    
    def __compile__(self,dest=None) : 

        # Decorate the mother class __package__ method
        super(QtCompile,self).__compile__()
         
        # Remove the qtWebEngine module that causes failure on macosx Sierra
        # See https://forum.qt.io/topic/94943/qt-5-11-2-fails-to-build-on-macos/4
        shutil.rmtree(os.path.join(self.__thirdPartyBuildFolder__,"qtwebengine"),
                      sys.exc_info())
         
        # Configure the build 
        self.__execute__("./configure "
                         "-prefix {pkg_folder} "         # Prefix
                         "-release "                     # Build release
                         "-no-debug-and-release "        # Do NOT build release AND debug
                         "-platform macx-clang QMAKE_APPLE_DEVICE_ARCHS=x86_64 " # See http://doc.qt.io/qt-5/osx.html
                         "-opensource -confirm-license " # Get the opensource licensing
                         "-no-dbus -no-gstreamer "
                         "-nomake tests -nomake examples".format(   # Do not bother with tests and examples 
                            pkg_folder=self.__thirdPartyPkgFolder__))
 
        # Modules to be compiled: 
        # TEST : Add qttools to get qtattributionsscanner
        # required for the target: make docs in the hope this
        # fixes the build. If this is the case, this is a bug 
        # in the qt build system...
        compileModules= ['qtbase','qtdatavis3d','qttools','qtlocation','qtdeclarative','qtvirtualkeyboard','qtdoc']
 
        # Multi-threaded compile for selected modules
        for iModule in compileModules:
            self.__execute__("make -j {} module-{}".format(multiprocessing.cpu_count(),iModule))    
                          
    # Package the third party that was build   
    def __package__(self):

        # Make sure to start in the build folder
        os.chdir(self.__thirdPartyBuildFolder__)
              
        # Decorate the mother class __package__ method
        super(QtCompile,self).__package__()

        # Install the compile code in the -prefix location : the __package__ folder
        self.__execute__("sudo make -j 1 install") 
         
        # Now package the .pc files required to compile with scons
        if not os.path.isdir(os.path.join(self.__thirdPartyPkgFolder__,"qtPkgConfig")):
            shutil.copytree("{VPPsrcDir}/site_scons/qtPkgConfig".format(VPPsrcDir=self.__VppSrcTreeFolder__), 
                            os.path.join(self.__thirdPartyPkgFolder__,"qtPkgConfig"))
         
        # Go to the pkgconfig folder
        os.chdir(os.path.join(self.__thirdPartyPkgFolder__,"qtPkgConfig"))
 
        # Update the .pc files with the current version
        self.__patch__("QtRoot = (.*)","QtRoot = \"{}\"".format(self.__thirdPartyPkgFolder__),
                       "update.py")
        self.__patch__("QtVersion = (.*)","QtVersion = \"{}\"".format(self.__version__),
                       "update.py")
        self.__execute__("python update.py")

        # Now build all the symbolic links to populate the 'include' dir as if this was not a 
        # build for frameworks.        
        for iFramework in glob.glob("{pkgLibPath}/*.framework".format(
                            pkgLibPath=os.path.join(self.__thirdPartyPkgFolder__,"lib"))): 
            os.symlink(os.path.join(iFramework,"Headers"), 
                       os.path.join(self.getIncludePath()[0],os.path.basename(iFramework).replace(".framework","")) )
        
        # Everything else is done. Now get back to the build directory and execute make docs
        # Note that this cannot be done before having a valid package, or some of the components wont be found
# Prerequisite: LLVM, see
# https://doc.qt.io/qt-5.11/qdoc-guide-clang.html
# Installing llvm : https://llvm.org/docs/GettingStarted.html#overview
#        os.chdir(self.__thirdPartyBuildFolder__)
#        self.__execute__("make docs")
#        self.__execute__("make install_docs")
        
    # Run a simple test to check that the compile was successiful
    def __test__(self):

        # Decorate the mother class __test__ method
        super(QtCompile,self).__test__()

        # Write the qt example from NestedLayout
        Source=open("main.cpp","w")
        Source.write('''
#include <QtWidgets>
#include <iostream>
using namespace std;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QWidget window;

    QLabel *queryLabel = new QLabel(
        QApplication::translate("nestedlayouts", "Query:")
    );

    QLineEdit *queryEdit = new QLineEdit();
    QTableView *resultView = new QTableView();

    QHBoxLayout* queryLayout = new QHBoxLayout();
    queryLayout->addWidget(queryLabel);
    queryLayout->addWidget(queryEdit);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(queryLayout);
    mainLayout->addWidget(resultView);
    window.setLayout(mainLayout);

    // Set up the model and configure the view...
//! [first part]

//! [set up the model]
    QStandardItemModel model;
    model.setHorizontalHeaderLabels(
        QStringList() << QApplication::translate("nestedlayouts", "Name")
                      << QApplication::translate("nestedlayouts", "Office"));

    QList<QStringList> rows = QList<QStringList>()
        << (QStringList() << "Verne Nilsen" << "123")
        << (QStringList() << "Carlos Tang" << "77")
        << (QStringList() << "Bronwyn Hawcroft" << "119");

    foreach (QStringList row, rows) {
        QList<QStandardItem *> items;
        foreach (QString text, row)
            items.append(new QStandardItem(text));
        model.appendRow(items);
    }

    resultView->setModel(&model);
    resultView->verticalHeader()->hide();
    resultView->horizontalHeader()->setStretchLastSection(true);
//! [set up the model]
//! [last part]
    window.setWindowTitle(
        QApplication::translate("nestedlayouts", "Nested layouts"));
    window.show();
    return app.exec();
}''')
        Source.close()
            
        # Write a SConstruct 
        Sconstruct=open("SConstruct","w")
        Sconstruct.write('''import os
env = Environment().Clone()
qtdir = \"{pkgFolder}\" 
env['QT5DIR']= qtdir
env['ENV']['PKG_CONFIG_PATH'] = [ os.path.join(qtdir,\"qtPkgConfig\") ]
env['ENV']['PATH'] += \":/opt/local/bin:{binPath}\"
env['CXXFLAGS'] +=  ['-std=c++11']

env.Append(  LINKFLAGS = \"-rpath {libPath}\" ) 

env.Tool('qt5')
env.EnableQt5Modules([
                      'QtGui',
                      'QtCore',
                      'QtNetwork',
                      'QtWidgets'
                      ])
QT_LIBS=[ ]
QT_INCS= {includePath}
QT_LIBPATH= \"{libPath}\"
env.Program('qtTest', Glob('*.cpp'),
            CPPPATH = QT_INCS,
            LIBS = QT_LIBS,
            LIBPATH = QT_LIBPATH )        
'''.format(pkgFolder= self.__thirdPartyPkgFolder__,
           binPath= self.__buildInfo__["BINPATH"][0],
           includePath= self.__buildInfo__["INCLUDEPATH"],
           libPath= self.__buildInfo__["LIBPATH"][0]))
        Sconstruct.close()
                  
        # Make a symbolic link to the qt5 scons build tool
        assert isinstance(self.__requirements__[0],qt5SconsTool)
        os.symlink(self.__requirements__[0].getIncludePath()[0], "site_scons")

        # Compile the example
        self.__execute__("scons -Q")
        
        # Run otool 
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
        #verify what this does!
        # Copy the lib to the dest folder
        for iLib in self.__buildInfo__["LIBS"]: 
            shutil.copyfile(self.getFullDynamicLibName(iLib), 
                            os.path.join(dst,self.getFullDynamicLibName(iLib)))
                    
        