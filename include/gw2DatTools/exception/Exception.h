#pragma once

#include <exception>

namespace gw2::exception
{

class Exception: public std::exception
{
public:
    Exception(const char* iReason);
    virtual ~Exception();
};

}
