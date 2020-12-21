#include "common/log.h"
#include <cstdio>
#include <stdlib.h>
#include <string>
#include <thread>
#include <yut/base/timer_heap.h>
#include <yut/base/timestamp.h>
#include <iostream>



int main(int argc, char *argv[])
{

    // init main log
    std::string main_log_file = "zjw-.%Y%m%d.log";
    if (common::log::LogManager::getInstance()->initMainLogger(main_log_file, true) == false) {
        ::fprintf(stderr, "init main log failed \n");
        return 1;
    }

    LOG_INFO("log start");
    ////////////////////////////////////////////////////

    // test timer
    

    std::thread timer_thread([]() {
    
        yut::base::TimerHeap timer_heap;

        yut::base::Timestamp now;
        now.setNow();

        timer_heap.addTimer(now, 1000, [](yut::base::TimerHeap::TimerId timer_id) {
            std::cout << "haha timer called:" << timer_id << std::endl;
        }, 3);

        for (;;) {
                  
            yut::base::Timestamp now;
            now.setNow();
            if (-1 == timer_heap.getNextTimeoutMillisecond(now)) {
                std::cout << "over" << std::endl;

                // std::this_thread::sleep_for(std::chrono::seconds(1));
                break;
            }

            timer_heap.checkTimeout(now);
        }
    });

    timer_thread.join();

    LOG_INFO("log end");

    return 0;
}