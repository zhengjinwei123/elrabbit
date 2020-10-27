#ifndef ELE_BASE_CLASS_UTIL_H
#define ELE_BASE_CLASS_UTIL_H


#define ELE_NONCOPYABLE(_T)                            \
    _T(const _T &);                                    \
    _T &operator=(const _T &);                         \

#define ELE_SINGLETON(_T)                              \
public:                                            \
        static _T *getInstance()                       \
        {                                              \
            static _T obj;                             \
            return &obj;                                \
        }                                              \
                                                       \
private:                                          \
        _T();                                         \
        ~_T();                                         \
        _T(const _T &);                               \
        _T &operator=(const _T &);                    \

#define ELE_PRECREATED_SINGLETON(_T)                  \
public:                                           \
        static _T *getInstance()                      \
        {                                             \
            static _T obj;                            \
            return &obj;                              \
        }                                             \
                                                      \
private:                                              \
        struct ObjectCreator {                        \
            ObjectCreator() { _T::getInstance(); }    \
        };                                            \
                                                      \
        static ObjectCreator oc;                      \
                                                      \
        _T();                                         \
        ~_T();                                        \
        _T(const _T &);                               \
        _T &operator=(const _T &);                    \

#define ELE_PRECREATED_SINGLETON_IMPL(_T)             \
    _T::ObjectCreator _T::oc;                         \

#endif