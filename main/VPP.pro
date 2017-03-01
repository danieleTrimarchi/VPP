#-------------------------------------------------
#  
# This file was generated by QtCreator and it is required - with 
# mainWindow.ui - in order to ask QMake to create a local XCode project
#
# qmake -spec macx-xcode VPP.pro 
#
#-------------------------------------------------

QT       += core gui network widgets datavisualization charts

greaterThan(QT_MAJOR_VERSION, 4): QT += core gui network widgets datavisualization charts

TARGET = VPP
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# This fills "Header Search Paths in XCode"
INCLUDEPATH +=	"/opt/local/include" \
		"/Users/dtrimarchi/third_party/eigen-3.2.7" \
		"/Users/dtrimarchi/third_party/SuiteSparse-4.5.2/UMFPACK/Include" \
		"/Users/dtrimarchi/third_party/SuiteSparse-4.5.2/SuiteSparse_config" \
		"/Users/dtrimarchi/third_party/SuiteSparse-4.5.2/AMD/Include" \
		"/Users/dtrimarchi/third_party/boost_1_60_0" \
		"/Users/dtrimarchi/third_party/nlopt-2.4.2/api" \
		"/Users/dtrimarchi/third_party/Ipopt-3.12.6/Ipopt/src/Interfaces" \
		"/Users/dtrimarchi/third_party/Ipopt-3.12.6/include/coin" \
		"/Users/dtrimarchi/third_party/opt/local/share/plplot5.11.1/examples/c++" \
		"/Users/dtrimarchi/third_party/opt/local/include/plplot" \
		"/Users/dtrimarchi/third_party/cppunit-1.13.2/build/include" \
		"/opt/local/share/plplot5.11.1/examples/c++" \
		"/opt/local/include/plplot"

# This fills Library Search Path in XCode
LIBPATH += 	"/System/Library/Frameworks/Accelerate.framework/Versions/Current/Frameworks/vecLib.framework/Versions/A" \
		"/opt/local/lib" \ 
		"/Users/dtrimarchi/third_party/SuiteSparse-4.5.2/UMFPACK/Lib" \
		"/Users/dtrimarchi/third_party/SuiteSparse-4.5.2/SuiteSparse_config" \ 
		"/Users/dtrimarchi/third_party/SuiteSparse-4.5.2/AMD/Lib" \ 
		"/Users/dtrimarchi/third_party/SuiteSparse-4.5.2/CHOLMOD/Lib" \
		"/Users/dtrimarchi/third_party/SuiteSparse-4.5.2/COLAMD/Lib" \
		"/Users/dtrimarchi/third_party/nlopt-2.4.2/libs" \
		"/Users/dtrimarchi/third_party/Ipopt-3.12.6/lib" \
		"/Users/dtrimarchi/third_party/cppunit-1.13.2/build/lib"



# Libs to be linked in
LIBS +=	-lipopt \
	-lumfpack \
	-lBLAS \
	-lamd \
	-lcolamd \
	-lcholmod \
	-lsuitesparseconfig \
	-lnlopt \
	-lplplot \
	-lplplotcxx

RESOURCES     = gui/VPP.qrc

SOURCES += main.cxx \
	gui/*.cpp \
	exceptions/*.cpp \ 
	core/*.cpp \
	io/*.cpp \
	solvers/*.cpp \
	plotting/*.cpp \
	results/*.cpp \
	utils/*.cpp \
	versioning/*.cpp 


HEADERS  +=	gui/*.h \
		exceptions/*.h \ 
		core/*.h \
		io/*.h \
		solvers/*.h \
		plotting/*.h \
		results/*.h \
		utils/*.h \
		versioning/*.h 

FORMS    += gui/MainWindow.ui

BUILD_DIR =  ../xCodeBuild
OBJECTS_DIR = ../xCodeBuild
MOC_DIR = ../xCodeBuild
RCC_DIR = ../xCodeBuild
UI_DIR = ../xCodeBuild 
DESTDIR = ../xCodeBuild


