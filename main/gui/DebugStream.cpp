#include "DebugStream.h"

// Ctor
QDebugStream::QDebugStream(std::ostream &stream, QTextEdit* text_edit) : stream_(stream) {
	pLogWindow_ = text_edit;
	pOldBuf_ = stream.rdbuf();
	stream.rdbuf(this);
}

// Dtor
QDebugStream::~QDebugStream(){

	// output anything that is left
	if (!string_.empty())
		pLogWindow_->append(string_.c_str());

	stream_.rdbuf(pOldBuf_);
}

QDebugStream::int_type QDebugStream::overflow(int_type v) {

	if (v == '\n') {
		pLogWindow_->append(string_.c_str());
		string_.erase(string_.begin(), string_.end());

	} else
		string_ += v;

	return v;
}

std::streamsize QDebugStream::xsputn(const char *p, std::streamsize n) {

	string_.append(p, p + n);

	int pos = 0;
	while (pos != std::string::npos)
	{
		pos = string_.find('\n');
		if (pos != std::string::npos)
		{
			std::string tmp(string_.begin(), string_.begin() + pos);
			pLogWindow_->append(tmp.c_str());
			string_.erase(string_.begin(), string_.begin() + pos + 1);
		}
	}

	return n;
}

