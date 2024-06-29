#ifndef GW2DATTOOLS_EXCEPTION_EXCEPTION_H
#define GW2DATTOOLS_EXCEPTION_EXCEPTION_H

#include <exception>

namespace gw2dt
{
namespace exception
{

class Exception: public std::exception
{
public:
    Exception(const char* iReason);
    virtual ~Exception();
};

}
}

#endif // GW2DATTOOLS_EXCEPTION_EXCEPTION_H
