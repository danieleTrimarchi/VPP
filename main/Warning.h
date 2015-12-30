#ifndef WARNING_H
#define WARNING_H

#include <string>
using namespace std;

/// Warning wrap class, simply used to force consistent
/// warning formatting
class Warning {

	public:

		/// Constructor
		Warning(string);

		/// Destructor
		~Warning();

	private:

		/// Disallow default constructor
		Warning();
};

#endif
