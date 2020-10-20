#ifndef ELE_BASE_OBJECT_POOL_H
#define ELE_BASE_OBJECT_POOL_H

#include <functional>
#include <iostream>
#include <memory>
#include <vector>

namespace ele
{
    template <class T>
    class ObjectPoolUnique
    {
    public:
        using DeleteType = std::function<void(T *)>;
        using ObjectTye = std::unique_ptr<T, DeleteType>;

    private:
        std::vector<std::unique_ptr<T>> pool_;

    public:
        void add(std::unique_ptr<T> obj)
        {
            pool_.emplace_back(std::move(obj));
        }

        ObjectTye get()
        {
            if (pool_.empty())
            {
                throw std::logic_error("no more object");
            }

            ObjectTye obj(pool_.back().release(), [this](T *t) {
                std::cout << "destructor called" << std::endl;
                this->pool_.emplace_back(std::unique_ptr<T>(t));
            });

            pool_.pop_back();
            return std::move(obj);
        }

        bool empty() const
        {
            return pool_.empty();
        }

        size_t size() const
        {
            return pool_.size();
        }
    };

    template <class T>
    class ObjectPoolShared
    {
    public:
        using DeleteType = std::function<void(T *)>;
        using ObjectType = std::shared_ptr<T>;

    private:
        std::vector<std::unique_ptr<T>> pool_;

    public:
        void add(std::unique_ptr<T> obj)
        {
            pool_.emplace_back(std::move(obj));
        }

        ObjectType get()
        {
            if (pool_.empty())
            {
                throw std::logic_error("no more object");
            }

            ObjectType obj = pool_.back();
            ObjectType newObj = ObjectType(new T(*obj.get()), [this](T *t) {
                pool_.emplace_back(std::shared_ptr<T>(t));
            });

            pool_.pop_back();
            return std::move(obj);
        }

        bool empty() const
        {
            return pool_.empty();
        }

        size_t size() const
        {
            return pool_.size();
        }
    };
} // namespace ele

#endif // ELE_BASE_OBJECT_POOL_H