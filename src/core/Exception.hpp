#pragma once

#include <exception>
#include "../common.h"

class Exception : public std::exception {
public:
	Exception() : std::exception() { }
	Exception(const char *message) : std::exception(message) { }
};
