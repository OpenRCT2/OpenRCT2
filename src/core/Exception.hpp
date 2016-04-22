#pragma once

#include "../common.h"

#include <exception>

class Exception : public std::exception
{
public:
    Exception() : Exception(nullptr) { }

    Exception(const char * message) : std::exception()
    {
        _message = message;
    }

    virtual ~Exception() { }

    const char * what()       const throw() override { return _message; }
    const char * GetMessage() const                  { return _message; }
    const char * GetMsg() const                      { return _message; }

private:
    const char * _message;
};
