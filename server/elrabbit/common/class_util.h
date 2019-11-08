#ifndef ELRABBIT_COMMMON_CLASS_UTIL_H
#define ELRABBIT_COMMMON_CLASS_UTIL_H

class Noncopyable {
    private:
        Noncopyable(const Noncopyable &) = delete;
        void operator=(const Noncopyable &) = delete;
        
    protected:
        Noncopyable() = default;
        ~Noncopyable() = default;
};

#define ELRABBIT_NONCOPYABLE(T) \
    T(const T&);                \
    T &operator=(const T&);     \

#define ELRABBIT_SINGLETON(T)   \
    public:                     \
        static T* getInstance() \
        {                       \
            static T obj;       \
            return &obj;        \
        }                       \
                                \
    private:                    \
        T();                    \
        ~T();                   \
        T(const T &);           \
        T &operator=(const T&); \

#endif // ELRABBIT_COMMMON_CLASS_UTIL_H