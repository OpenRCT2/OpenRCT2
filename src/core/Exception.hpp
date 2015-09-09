#pragma once

#include <exception>
#include "../common.h"

class Exception : public std::exception {
public:
	Exception() : std::exception() { }
	Exception(const char *message) : std::exception() {
		_message = message;
	}
	virtual ~Exception() { }

	const char *what() const throw() override { return _message; }
	const char *GetMessage() const { return _message; }

private:
	const char *_message;
};
