from thirdPartyCompile import thirdPartyCompile
import os, shutil, sys, glob, fnmatch
import multiprocessing

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
        self.__url__="http://download.qt.io/official_releases/qt/5.11/5.11.2/single/qt-everywhere-src-5.11.2.tar.xz"

        # Define the name of the archive downloadeed from the web. A priori this is 
        # simply the last entry of the url
        self.__srcArchiveName__="qt-everywhere-src-5.11.2.tar.xz"
        
        # Define the name of the folder extracted from the archive downloadeed from the web. 
        # A priori this is simply the srcArchiveName without the extension
        self.__srcDirName__="qt-everywhere-src-5.11.2"
        
        # Override (specialize) the build folder 
        self.__thirdPartyBuildFolder__= os.path.join(self.__thirdPartyBuildFolder__,self.__srcDirName__)

        # Override (specialize) the package folder 
        self.__thirdPartyPkgFolder__= os.path.join(self.__thirdPartyPkgFolder__,self.__srcDirName__)

        # Instantiate no requirements - Boost only has Python for BoostPython, which we do not want        

        # Define the build info. Will use these to copy the components (includes, libs...) to 
        # the package folder
        self.__buildInfo__["LIBPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"lib")]
        self.__buildInfo__["DOCPATH"] = [os.path.join(self.__thirdPartyPkgFolder__,"doc")]
        self.__buildInfo__["INCLUDEPATH"] = [
                               os.path.join(self.getLibPath()[0],'QtCore.framework/Versions/Current/Headers'),
                               os.path.join(self.getLibPath()[0],'QtWidgets.framework/Versions/Current/Headers'),
                               os.path.join(self.getLibPath()[0],'QtGui.framework/Versions/Current/Headers')
                               ]
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
        
        # Patch the whole project: add the spec CONFIG += create_pc create_prl no_install_prl
        # to get the .pc files we need to build with SCons. See 
        #https://stackoverflow.com/questions/6823862/qmake-creating-a-pc-file    
#         for root, dirNames, fileNames in os.walk("."):                       
#             for fileName in fnmatch.filter(fileNames,"*.pro"):
#                 patch(r'(CONFIG \+\= )',r"\g<0> create_pc create_prl no_install_prl ",os.path.join(root, fileName))
        # Actually, is it enough to patch the src .pro file only? 
        patch(r'(CONFIG \+\= )',r"\g<0> create_pc create_prl no_install_prl ","qt.pro")                
        
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

        # Also make the documentation. I cannot get this to build properly but 
        # really this is not a big deal...
        #self.__execute__("make docs")
        #self.__execute__("make install_docs")
                          
    # Package the third party that was build   
    def __package__(self):
              
        # Decorate the mother class __package__ method
        super(QtCompile,self).__package__()

        # Install the compile code in the -prefix location : the __package__ folder
        self.__execute__("sudo make -j 1 install") 
                        
    # Run a simple test to check that the compile was successiful
    def __test__(self):

        # Decorate the mother class __test__ method
        super(QtCompile,self).__test__()

        # Write the qt example from NestedLayout
        Source=open("main.cpp","w")
        Source.write('''
#include <QtWidgets>
using std::cout;

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
env = Environment()
env['QT5DIR']={pkgFolder} 
env['ENV']['PKG_CONFIG_PATH'] = [ os.path.join(qtdir,'lib/pkgconfig') ]
env.Append( CPPPATH=["{includePath}"] )
env.Append( LIBPATH=["{libPath}"] )
env.Append( LIBS={libs} )
env.Program('qtTest', Glob('*.cpp') )        
'''.format(pkgFolder= self.__thirdPartyPkgFolder__,
           includePath= self.__buildInfo__["INCLUDEPATH"],
           libPath= self.__buildInfo__["LIBPATH"],
           libs= self.__buildInfo__["LIBS"]))
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
        #verify what this does!
        # Copy the lib to the dest folder
        for iLib in self.__buildInfo__["LIBS"]: 
            shutil.copyfile(self.getFullDynamicLibName(iLib), 
                            os.path.join(dst,self.getFullDynamicLibName(iLib)))
                    
        