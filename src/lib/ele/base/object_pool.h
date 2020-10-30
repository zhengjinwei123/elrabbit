#ifndef ELE_BASE_OBJECT_POOL_H
#define ELE_BASE_OBJECT_POOL_H

#include <functional>
#include <iostream>
#include <memory>
#include <vector>

#include "class_util.h"

namespace ele
{
    template <class T>
    class ObjectPool
    {
    public:
        ObjectPool() {}
        ~ObjectPool()
        {
            for (size_t i = 0; i < reused_object_list_.size(); ++i)
            {
                delete reused_object_list_[i];
            }
        }

        T *getObject()
        {
            if (!reused_object_list_.empty())
            {
                T *obj = reused_object_list_.back();
                reused_object_list_.pop_back();
                return obj;
            }
            return new T();
        }

        void returnObject(T *obj)
        {
            reused_object_list_.push_back(obj);
        }

    private:
        ELE_NONCOPYABLE(ObjectPool);

        std::vector<T *> reused_object_list_;
    };

    template <class T>
    class ObjectPoolUnique
    {
    public:
        using DeleterType = std::function<void(T *)>;
        using ObjectType = std::unique_ptr<T, DeleterType>;

    private:
        std::vector<std::unique_ptr<T>> pool_;

    private:
        void add(std::unique_ptr<T> obj)
        {
            pool_.emplace_back(std::move(obj));
        }

        ObjectType getObject()
        {
            ObjectType obj(pool_.back().release(), [this](T *t) {
                this->pool_.emplace_back(std::unique_ptr<T>(t));
            });
            pool_.pop_back();
            return std::move(obj);
        }

    public:
        ObjectType get()
        {
            if (pool_.empty())
            {
                add(std::unique_ptr<T>(new T()));
            }

            return getObject();
        }
    };
} // namespace ele

#endif // ELE_BASE_OBJECT_POOL_H