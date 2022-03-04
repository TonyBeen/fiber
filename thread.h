/*************************************************************************
    > File Name: thread.h
    > Author: hsz
    > Brief:
    > Created Time: Sun 27 Feb 2022 04:47:02 PM CST
 ************************************************************************/

#ifndef __THREAD_H__
#define __THREAD_H__

#include <utils/mutex.h>
#include <pthread.h>
#include <stdint.h>
#include <string>
#include <functional>
#include <memory>

class Thread
{
public:
    typedef std::shared_ptr<Thread> SP;
    Thread(std::function<void()> cb, const std::string &threadName = "", uint32_t stackSize = 0);
    ~Thread();

    static void         SetName(std::string name);
    static std::string  GetName();
    static Thread *     GetThis();
    std::string         getName() const { return mName; }
    pid_t               getTid() const { return mKernalTid; };

    void detach();
    void join();

protected:
    static void *entrance(void *arg);

private:
    pid_t                   mKernalTid;     // 内核tid
    pthread_t               mTid;           // pthread线程ID
    std::string             mName;
    std::function<void()>   mCb;            // 线程执行函数
    uint8_t                 mShouldJoin;    // 1为由用户回收线程，0为自动回收
    eular::Sem              mSemaphore;     // 等待mKernalTid赋值
};

#endif // __THREAD_H__
