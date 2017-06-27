#ifndef QDEBUGSTREAM_H
#define QDEBUGSTREAM_H

#include <iostream>
#include <streambuf>
#include <string>

#include "qtextedit.h"

class QDebugStream : public std::basic_streambuf<char>
{
	public:

		/// Ctor
		QDebugStream(std::ostream &stream, QTextEdit* text_edit);

		/// Dtor
		~QDebugStream();

	protected:

		virtual QDebugStream::int_type overflow(int_type v);

		virtual std::streamsize xsputn(const char *p, std::streamsize n);

	private:

		std::ostream& stream_;
		std::streambuf* pOldBuf_;
		std::string string_;

		QTextEdit* pLogWindow_;
};

#endif
