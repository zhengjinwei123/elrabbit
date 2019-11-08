#include "elrabbit/common/exception.h"
#include "elrabbit/common/current_thread.h"

namespace elrabbit {

namespace common {

Exception::Exception(const std::string& msg) :
    message_(std::move(msg)),
    stack_(current_thread::strackTrace(false))
{

}

} // namespace common
} // namespace elrabbit