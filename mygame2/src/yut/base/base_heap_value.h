#ifndef YUT_BASE_BASE_HEAP_VALUE_H
#define YUT_BASE_BASE_HEAP_VALUE_H

#include <yut/base/class_util.h>

namespace yut {
namespace base {

template <typename T>
class BaseHeapValue {
public:
    BaseHeapValue(const T &v) :
        heap_pos_(-1), value(v)
    {}
    virtual ~BaseHeapValue() {}

    virtual int getHeapPos() const { return heap_pos_; }
    virtual void setHeapPos(int heap_pos) { heap_pos_ = heap_pos; } 
    virtual void setValue(const T &v) { value = v; }
    virtual T &getValue() { return value; }
    virtual const T&getValue() const { return value; }

    virtual bool operator<(BaseHeapValue &rhv)
    {
        return value < rhv.value;
    }

protected:
    // YUT_NONCOPYABLE(BaseHeapValue)

    int heap_pos_;
    T value;
};

} // namespace base
} // namespace yut
#endif