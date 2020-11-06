#ifndef ELE_EXCHANGE_BASE_STRUCT_H
#define ELE_EXCHANGE_BASE_STRUCT_H

#include <cstddef>
#include <string>

namespace ele
{
    namespace exchange
    {
        class BaseStruct
        {
            public:
                typedef BaseStruct *(*CreateFunc)();

                BaseStruct();
                virtual ~BaseStruct();

                virtual BaseStruct *clone() const = 0;

                virtual int encode(char *buffer, size_t size) const = 0;
                virtual int decode(const char *buffer, size_t size) = 0;

                virtual std::string dump() const = 0;
            
            protected:
                static std::string dumpBytes(const std::string &val);
        };
    }
}

#endif