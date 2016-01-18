#ifndef VPPEXCEPTION_H
#define VPPEXCEPTION_H

#include <exception>
#include <string>

/// Define the macro here used to identify the function that throws
#define HERE __FILE__, __LINE__, __PRETTY_FUNCTION__

class VPPException : public std::exception {

	public:

		// Constructor
		VPPException(const char* inFile, int inLine, const char* inFunction, const char* message );

		// Destructor
		virtual ~VPPException() throw();

		/// Output the error message
		virtual const char* what() const throw();

	private:

		std::string msg;

};

#endif
