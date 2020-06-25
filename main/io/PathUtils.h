#ifndef PATH_UTILS
#define PATH_UTILS

#include <mach-o/dyld.h>
#include <limits.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

namespace fs= boost::filesystem;

/// Class used to retrieve the path of the executable
/// and of the app. This is a relatively delicate task
/// under MACOS when running the app, the path of which
/// is not univoquely defined. For example, try getting
/// the executable path with getcwd or with
/// boost::filesystem::current_path ends up in '/', which
/// is of no use. This will require some checking when
/// compiling for Windows / Linux
class PathUtils {

	public:

		/// Ctor
		PathUtils();

		/// Dtor
		~PathUtils();

		/// Get the path of the current executable which
		/// is executed
		fs::path getExecutablePath() const;

		/// Get the path of the current working directory
		/// This is the same as executablePath when running from
		/// the console, but it is not when running the UI.
		/// For a MACOSX-based app, the working dir is the location
		/// of the app boundle root.
		fs::path getWorkingDirPath() const;

	private:

		/// Path of the executable that is being actually run
		fs::path exePath_;

};

#endif
