#-------------------------------------------------
# README dtrimarchi!
#  
# This file is required - with mainWindow.ui - in order for 
# QMake to create a local XCode project with : 
#
# qmake -spec macx-xcode VPP.pro 
# 
# Note that this will NOT set the paths : 
#
#     Per-Configuration Build Product Path
#     Per-Configuration Intermediate Build Files Path
#
# These are defined by the xCode variable SYMROOT
#
# with the effect of creating a local Debug/VPP.app folder
# This is unwanted because ideally we should not pollute 
# the content of 'main' with build outputs, that should be 
# sent to the xCodeBuild folder (See DEST_BIN) 
# By now the easiest possibility is to reset these two 
# variables by hand when regenerating the xCode project
# 
# One (dirty) possibility - when including the xcode project
# generation into a scons script, is to 'sed' VPP.xcodeproj
# and change the value of SYMROOT in VPP.xcodeproj/project.pbxproj
# 
#-------------------------------------------------
QT += core gui widgets datavisualization printsupport 

TARGET = VPP 

TEMPLATE = app 

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.

DEFINES += QT_DEPRECATED_WARNINGS

RESOURCES = gui/VPP.qrc 

FORMS += gui/MainWindow.ui 

BUILD_DIR =  ../xCodeBuild
OBJECTS_DIR = ../xCodeBuild
MOC_DIR = ../xCodeBuild
RCC_DIR = ../xCodeBuild
UI_DIR = ../xCodeBuild 
DESTDIR = ../xCodeBuild
SYMROOT= ../xCodeBuild

INCLUDEPATH += "/Users/dtrimarchi/third_party/nlopt-2.4.2/api" \ 
"/usr/local/Cellar/qt5/5.7.0/lib/QtCore.framework/Versions/Current/Headers" \ 
"/usr/local/Cellar/qt5/5.7.0/lib/QtWidgets.framework/Versions/Current/Headers" \ 
"/usr/local/Cellar/qt5/5.7.0/lib/QtGui.framework/Versions/Current/Headers" \ 
"/Users/dtrimarchi/third_party/eigen-3.2.7" \ 
"/Users/dtrimarchi/third_party/boost_1_60_0" \ 
"/Users/dtrimarchi/third_party/Ipopt-3.12.6/Ipopt/src/Interfaces" \ 
"/Users/dtrimarchi/third_party/Ipopt-3.12.6/include/coin"

LIBPATH += "/Users/dtrimarchi/third_party/nlopt-2.4.2/libs" \ 
"/usr/local/Cellar/qt5/5.7.0/lib" \ 
"/System/Library/Frameworks/Accelerate.framework/Versions/Current/Frameworks/vecLib.framework/Versions/A" \ 
"/opt/local/lib" \ 
"/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/System/Library/Frameworks" \ 
"/Users/dtrimarchi/third_party/boost_1_60_0/bin.v2/libs/system/build/darwin-4.2.1/release/link-static/threading-multi" \ 
"/Users/dtrimarchi/third_party/boost_1_60_0/bin.v2/libs/fileSystem/build/darwin-4.2.1/release/link-static/threading-multi" \ 
"/Users/dtrimarchi/third_party/Ipopt-3.12.6/lib"

LIBS += -lnlopt \ 
-lboost_system \ 
-lboost_filesystem \ 
-lipopt

SOURCES += main.cxx \ 
qcustomplot/*.cpp \ 
gui/*.cpp \ 
exceptions/*.cpp \ 
core/*.cpp \ 
io/*.cpp \ 
solvers/*.cpp \ 
results/*.cpp \ 
utils/*.cpp \ 
versioning/*.cpp

HEADERS +=qcustomplot/*.h \ 
gui/*.h \ 
exceptions/*.h \ 
core/*.h \ 
io/*.h \ 
solvers/*.h \ 
results/*.h \ 
utils/*.h \ 
versioning/*.h

