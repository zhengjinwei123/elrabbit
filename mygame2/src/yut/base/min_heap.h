#ifndef YUT_BASE_MIN_HEAP_H
#define YUT_BASE_MIN_HEAP_H

#include <vector>
#include <functional>
#include <yut/base/base_heap_value.h>

namespace yut {
namespace base {

// class BaseHeapValue;
template <typename T>
class MinHeap {
public:
    MinHeap() {
        heap_vector_.push_back(nullptr);
    }
    ~MinHeap() {

    }

    void reserve() {
        heap_vector_.reserve(heap_vector_.size() + 1);
    }

    void insert(BaseHeapValue<T> *v) {
        int cur_index = heap_vector_.size();
        heap_vector_.emplace_back(v);
        v->setHeapPos(cur_index);

        for (;;)
        {
            int parent_index = cur_index / 2;

            if (0 == parent_index) {
                break;
            }

            // smaller up and bigger down
            if (heap_vector_[parent_index] < heap_vector_[cur_index]) {
                break;
            }

            // replace position
            heap_vector_[parent_index]->setHeapPos(cur_index);
            heap_vector_[cur_index]->setHeapPos(parent_index);
            std::swap(heap_vector_[parent_index], heap_vector_[cur_index]);

            cur_index = parent_index;
        }
    }

    void erase(BaseHeapValue<T> *v) {
        int cur_index = v->getHeapPos();
        if (cur_index < 0) {
            return;
        }

        v->setHeapPos(-1);
        heap_vector_[cur_index] = heap_vector_.back();
        heap_vector_[cur_index]->setHeapPos(cur_index);
        heap_vector_.pop_back();

        for (;;)
        {
            int child_index = cur_index * 2;
            if (child_index >= (int)heap_vector_.size()) {
                break;
            }

            if (child_index + 1 < (int)heap_vector_.size() &&
                heap_vector_[cur_index + 1] < heap_vector_[cur_index]) {
                ++child_index;
            }

            if (heap_vector_[cur_index] < heap_vector_[child_index]) {
                break;
            }

            heap_vector_[cur_index]->setHeapPos(child_index);
            heap_vector_[child_index]->setHeapPos(cur_index);
            std::swap(heap_vector_[cur_index], heap_vector_[child_index]);

            cur_index = child_index;
        }
    }

    bool empty() const {
        return heap_vector_.size() <= 1;
    }

    BaseHeapValue<T> *top() const {
        if (heap_vector_.size() <= 1) {
            return nullptr;
        }
        return heap_vector_[1];
    }
private:
    std::vector<BaseHeapValue<T> *> heap_vector_;
};

} // namespace base
} // namespace yut

#endif