#ifndef VPPEXCEPTION_H
#define VPPEXCEPTION_H

#include <exception>
#include <string>

using namespace std;

/// Define the macro here used to identify the function that throws
#define HERE __FILE__, __LINE__, __PRETTY_FUNCTION__

class VPPException : public std::exception {

	public:

		// Constructor
		VPPException(const char* inFile, int inLine, const char* inFunction, ostringstream message );

		// Constructor using c_str
		VPPException(const char* inFile, int inLine, const char* inFunction, const char* message );

		// Destructor
		virtual ~VPPException() throw();

		/// Output the error message
		virtual const char* what() const throw();

	protected:

		std::string msg_;

};

// This exception is a copy of VPPException. I need to have a different type to discriminate
// what is a non-convergence exception and continue on non-convergence error. It is exactly like
// a goto statement
class NonConvergedException : public std::exception {

	public:

		// Constructor
		NonConvergedException(const char* inFile, int inLine, const char* inFunction, const char* message );

		// Destructor
		virtual ~NonConvergedException() throw();

		/// Output the error message
		virtual const char* what() const throw();

	protected:

		std::string msg_;

};


/// This exception is thrown by the optimizer if it is not possible to
/// find a previous converged step, while computing an initial guess of
/// the solution. It is exactly like a goto statement used to stop guessing
/// the result
class NoPreviousConvergedException : public std::exception {

	public:

		// Constructor
		NoPreviousConvergedException(const char* inFile, int inLine, const char* inFunction, const char* message );

		// Destructor
		virtual ~NoPreviousConvergedException() throw();

		/// Output the error message
		virtual const char* what() const throw();

	protected:

		std::string msg_;

};



#endif
