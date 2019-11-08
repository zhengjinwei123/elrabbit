#ifndef ELRABBIT_COMMON_EXCEPTION_H
#define ELRABBIT_COMMON_EXCEPTION_H

#include <exception>
#include <stdexcept>
#include <string>

namespace elrabbit {

namespace common {

class SystemErrorException : public std::runtime_error {
public:
    SystemErrorException(const std::string &what_arg) :
        std::runtime_error(what_arg) {}
};

class Exception: public std::exception {
public:
    Exception(const std::string &what);
    ~Exception() noexcept override = default;

    const char* what() const noexcept override
    {
        return message_.c_str();
    }

    const char* stackTrace() const noexcept
    {
        return stack_.c_str();
    }

private:
    std::string message_;
    std::string stack_;

}; // class Exception

} // namespace commmon
} // namespace elrabbit

#endif // ELRABBIT_COMMON_EXCEPTION_H