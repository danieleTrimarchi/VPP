#include "PathUtils.h"
#include "VPPException.h"
#include "CoreFoundation/CoreFoundation.h"

// Ctor
PathUtils::PathUtils() {

	// Check: if this is not MACOSX, throw
	// to be extended in the future!

	char buffer[PATH_MAX+1];
	unsigned int bufsize= sizeof(buffer);
	if( _NSGetExecutablePath(buffer,&bufsize) == -1 )
		throw VPPException(HERE,"Buffer size exceeded by current requested executable path!");

	// Assign the path to the fs::path
	exePath_=buffer;
}

// Dtor
PathUtils::~PathUtils() {

}

// Get the path of the current executable which
// is executed
fs::path PathUtils::getExecutablePath() const {
	return exePath_.parent_path();
}

// Get the path of the current working directory
// This is the same as executablePath when running from
// the console, but it is not when running the UI.
// For a MACOSX-based app, the working dir is the location
// of the app boundle root.
fs::path PathUtils::getWorkingDirPath() const {
	//			exe				MacOsx				Content					app					workdir
	//return exePath_.parent_path().parent_path().parent_path().parent_path();

	CFBundleRef mainBundle = CFBundleGetMainBundle();
	CFURLRef resourcesURL = CFBundleCopyBundleURL(mainBundle);
	CFStringRef str = CFURLCopyFileSystemPath( resourcesURL, kCFURLPOSIXPathStyle );
	CFRelease(resourcesURL);
	char* path = new char[1024];

	CFStringGetCString(str,path,FILENAME_MAX,kCFStringEncodingASCII);
	CFRelease(str);

	fs::path appPath(path);
	return appPath.parent_path();

}
