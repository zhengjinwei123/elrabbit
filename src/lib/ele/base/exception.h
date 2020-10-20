#ifndef ELE_BASE_EXCEPTION_H
#define ELE_BASE_EXCEPTION_H

#include <exception>
#include <string>

namespace ele
{

    class Exception : public std::exception
    {
    private:
        std::string message_;
        std::string stack_;

    public:
        explicit Exception(const char *what);
        ~Exception() noexcept override = default;

        virtual const char *what() const throw();
        const char *stackTrace() const throw();
    };
} // namespace ele
#endif