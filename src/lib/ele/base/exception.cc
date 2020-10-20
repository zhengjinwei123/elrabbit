#include "exception.h"

#include <cstdlib>
#include <cxxabi.h>
#include <execinfo.h>

namespace ele
{
    Exception::Exception(const char *what) : message_(what)
    {
        const int length = 200;
        void *buffer[length];

        int nptrs = ::backtrace(buffer, length);
        char **strs = ::backtrace_symbols(buffer, nptrs);
        if (strs)
        {
            for (int i = 0; i < nptrs; i++)
            {
                stack_.append(strs[i]);
                stack_.push_back('\n');
            }
            ::free(strs);
        }
    }

    const char *Exception::what() const throw()
    {
        return message_.c_str();
    }

    const char *Exception::stackTrace() const throw()
    {
        return stack_.c_str();
    }
} // namespace ele
