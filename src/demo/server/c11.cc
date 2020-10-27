#include <cstring>
#include <iostream>
#include <memory>
#include <unistd.h>

#include <lib/ele/base/concurrent_queue.h>
#include <lib/ele/base/dynamic_buffer.h>
#include <lib/ele/base/exception.h>
#include <lib/ele/base/object_pool.h>
#include <lib/ele/base/singleton.h>
#include <lib/ele/base/spin_lock.h>
#include <lib/ele/base/string_util.h>
#include <lib/ele/base/system.h>
#include <lib/ele/base/thread.h>
#include <lib/ele/base/thread_pool.h>
#include <lib/ele/base/timestamp.h>
#include <common/log.h>

using namespace std;

class STA
{
public:
    void say()
    {
        cout << "hallo p" << endl;
    }
};

class testSingleton
{
public:
    void sayHello()
    {
        cout << "testSingleton hello" << endl;
    }
};

class CB;

class CA
{
public:
    CA() { cout << "CA Constructor..." << endl; }
    ~CA() { cout << "CA Destructor ..." << endl; }

    weak_ptr<CB> pb;
};

class CB
{
public:
    CB() { cout << "CB Constructor..." << endl; }
    ~CB() { cout << "CB Destructor ..." << endl; }

    weak_ptr<CA> pa;
};

class Task
{
public:
    int mId;
    Task(int id) : mId(id)
    {
        cout << "Task::Constructor..." << endl;
    }

    ~Task()
    {
        cout << "Task::Destructor..." << endl;
    }
};

void testUniquePtr()
{
    std::unique_ptr<int> ptr1;

    if (!ptr1)
    {
        cout << "ptr1 is empty" << endl;
    }

    if (ptr1 == nullptr)
    {
        cout << "ptr1 is nullptr" << endl;
    }

    std::unique_ptr<Task> taskPtr(new Task(5));
    if (taskPtr != nullptr)
    {
        cout << "taskPtr is not empty" << endl;
    }

    cout << taskPtr->mId << endl;

    cout << "reset the taskPtr" << endl;

    taskPtr.reset();

    if (taskPtr == nullptr)
    {
        cout << "taskptr is nullptr" << endl;
    }

    std::unique_ptr<Task> taskPtr2(new Task(55));
    if (taskPtr2 != nullptr)
    {
        cout << "taskPtr2 is not empty" << endl;
    }

    {
        std::unique_ptr<Task> taskPtr3 = std::move(taskPtr2);
        if (taskPtr2 == nullptr)
        {
            cout << "taskPtr2 is empty" << endl;
        }
        if (taskPtr3 != nullptr)
        {
            cout << "taaskPtr3 is not empty" << endl;
            cout << taskPtr3->mId << endl;
        }
    }

    std::unique_ptr<Task> taskPtr4(new Task(66));
    if (taskPtr4 != nullptr)
    {
        cout << "taskPtr4 is  not empty" << endl;
    }

    Task *ptr = taskPtr4.release();

    if (taskPtr4 == nullptr)
    {
        cout << "taskPtr4 is empty" << endl;
    }

    cout << ptr->mId << endl;

    delete ptr;
}

// void castTest() {
//     char a = 'a';
//     int b = static_cast<char>(a);

//     double *c = new double;
//     void *d = static_cast<void*>(c);

//     int e = 10;
//     const int f = static_cast<const int>(e);

//     const int g = 11;
//     int h = static_cast<int>(g);
// }

template <typename T>
struct has_no_destroy
{
    template <typename C>
    static char test(decltype(&C::no_destroy));

    template <typename C>
    static int32_t test(...);

    const static bool value = sizeof(test<T>(0)) == 1;
};

struct B
{
    void no_destroy() {}
};

void threadFunc()
{
    cout << "threadFunc  w333" << endl;
}

int main(int argc, const char *argv[])
{

    // shared_ptr<CA> spa =  make_shared<CA>();
    // shared_ptr<CB> spb = make_shared<CB>();

    // spa->pb = spb;
    // spb->pa = spa;

    // testUniquePtr();

    // cout << typeid(uint32_t).name() << endl;
    // cout << typeid(std::string).name() << endl;

    // castTest();

    // int a = 1;
    // decltype(a) x1 = 100;

    // cout << x1 << endl;

    // cout << has_no_destroy<B>::value << endl;

    union
    {
        short s;
        char c[sizeof(short)];
    } un;

    un.s = 0x0102;

    if (sizeof(short) == 2)
    {
        if (un.c[0] == 1 && un.c[1] == 2)
        {
            cout << "big endian" << endl;
        }
        else if (un.c[0] == 2 && un.c[1] == 1)
        {
            cout << "little endian" << endl;
        }
        else
        {
            cout << sizeof(short) << endl;
        }
    }

    cout << un.s << "|" << un.c << endl;

    // std::string upper = ele::string_util::toUpper("zjw called");
    // cout << "upper:" << upper << endl;

    // ele::system::daemon(false, true);
    // ele::system::createPidFile("/mnt/hgfs/linuxcpp/demo1/c11pid");

    // while(true) {
    //     sleep(1000);
    //     cout << "hello" << endl;
    // }

    // ele::Singleton<testSingleton>::instance().sayHello();

    // ele::Thread new_thread(threadFunc, "mthread");
    // new_thread.start();
    // new_thread.join();

    // ele::ThreadPool thread_pool("ele_test_thread_pool");
    // thread_pool.start(3);

    // while(true)
    // {
    //     thread_pool.run(threadFunc);
    // }

    try
    {

        throw ele::Exception("哈哈哈");
    }
    catch (ele::Exception e)
    {
        cout << "what:" << e.what() << endl;
        cout << "strace:" << e.stackTrace() << endl;
    }

    //  int i = 0;
    // //below will call segmentfault
    // scanf("%d", i);
    // printf("%d", i);

    // ele::ObjectPoolUnique<STA> pPool;

    // pPool.add(std::unique_ptr<STA>(new STA()));
    // pPool.add(std::unique_ptr<STA>(new STA()));

    // {
    //     auto ptr = pPool.get();
    //     pPool.get();
    //     cout << "pool1 size:" << pPool.size() << endl;
    // }

    // cout << "pool2 size:" << pPool.size() << endl;

    // {
    //     pPool.get();
    //     pPool.get();
    //     cout << "pool3 size:" << pPool.size() << endl;
    // }

    // cout << "pool4 size:" << pPool.size() << endl;

    // auto b = pPool.get();

    ele::Timestamp now;
    now.setNow();

    cout << "tm:" << now.getSecond() << endl;
    cout << "tmm:" << now.getMilliSecond() << endl;

    ele::DynamicBuffer buf;
    buf.writeInt32(120);
    buf.writeInt32(10020);
    buf.writeInt16(255);

    uint32_t j = 0;
    uint32_t j1;
    uint16_t j2;

    buf.readInt32(j);
    buf.readInt32(j1);
    buf.readInt16(j2);

    cout << "j:" << j << endl;
    cout << "j1:" << j1 << endl;
    cout << "j2:" << j2 << endl;

    // ele::ThreadPool threadPool("thread pool 1");
    // threadPool.start(10);

    // ele::ConcurrentQueue<int> conqueue(10);

    // while (1)
    // {
    //     threadPool.run([&]() {

            

    //         int a = 1;
    //         conqueue.pop(a);
    //         conqueue.push(11010);


    //         cout << "hallo:" << a << ":" << endl;
    //     });
    // }

    // int a;
    // cin >> a;


    char strBuff[1024];
    ele::Timestamp now1;
    now1.setNow();
    ele::Timestamp::format(strBuff, sizeof(strBuff), "zjw_called_%Y-%m-%d %H:%M:%S", now1.getSecond());

    cout << "zjw:" << strBuff <<  endl;


    std::string main_log_file = "/mnt/hgfs/linuxcpp/log_zjw_.%Y%m%d.log";
    if (false == common::log::LogManager::getInstance()->initMainLogger(main_log_file, true)) {
        ::fprintf(stderr, "init main log failed \n");
        return 0;
    }

    ::fprintf(stderr, "init main log success \n");


    PLAIN_LOG_INFO("------------------------------\n");
    LOG_INFO("game-battle-server start");

    return 0;
}
