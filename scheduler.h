/*************************************************************************
    > File Name: scheduler.h
    > Author: hsz
    > Brief:
    > Created Time: Sun 27 Feb 2022 05:41:28 PM CST
 ************************************************************************/

#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include "fiber.h"
#include "thread.h"
#include <utils/string8.h>
#include <utils/mutex.h>
#include <memory>
#include <vector>
#include <list>
#include <atomic>

namespace eular {
class Scheduler
{
public:
    typedef std::shared_ptr<Scheduler> SP;

    /**
     * @brief 协程调度器
     * 
     * @param threads 线程数量
     * @param useCaller 是否包含用户调用线程
     * @param name 调度器名字
     */
    Scheduler(uint32_t threads = 1, bool useCaller = true, const std::string &name = "");
    virtual ~Scheduler();

    String8 getName() const { return mName; }
    static Scheduler* GetThis();
    static Fiber* GetMainFiber();

    void start();
    void stop();

    template<class FiberOrCb>
    void schedule(FiberOrCb fc, int th = -1)
    {
        bool needTickle = false;
        {
            AutoLock<Mutex> lock(mMutex);
            scheduleNoLock(fc, th);
        }
        if (needTickle) {
            tickle();
        }
    }

private:
    /**
     * @brief 协程信息结构体, 绑定哪个线程
     */
    struct FiberBindThread {
        Fiber::SP fiberPtr;         // 协程智能指针对象
        std::function<void()> cb;   // 协程执行函数
        int thread;                 // 内核线程ID

        FiberBindThread() : thread(-1) {}
        FiberBindThread(Fiber::SP sp, int th) : fiberPtr(sp), thread(th) {}
        FiberBindThread(Fiber::SP *sp, int th) : thread(th) { fiberPtr.swap(*sp); }
        FiberBindThread(std::function<void()> f, int th) : cb(f), thread(th) {}

        void reset()
        {
            fiberPtr = nullptr;
            cb = nullptr;
            thread = -1;
        }
    };

    template<class FiberOrCb>
    bool scheduleNoLock(FiberOrCb fc, int thread) {
        bool needTickle = mFibers.empty();
        FiberBindThread ft(fc, thread);
        if(ft.fiber || ft.cb) {
            mFibers.push_back(ft);
        }
        return needTickle;
    }
    

protected:
    void run();
    void setThis();
    void idle();
    virtual void tickle();


protected:
    std::vector<Thread::SP> mThreads;           // 线程数组
    std::vector<int>        mThreadIds;         // 线程id数组
    uint32_t                mThreadCount;       // 线程数量
    uint8_t                 mContainUserCaller; // 是否包含用户线程
    int                     mRootThread;        // userCaller为true时，为用户调用线程ID，false为-1
    bool                    mStoped;            // 是否停止

    std::atomic<uint32_t>   mActiveThreadCount = {0};
    std::atomic<uint32_t>   mIdleThreadCount = {0};

private:
    std::string             mName;              // 调度器名字
    eular::Mutex            mMutex;             // 任务队列锁
    Fiber::SP               mRootFiber;         // userCaller为true时有效
    std::list<FiberBindThread> mFibers;         // 待执行的协程队列
};

} // namespace eular

#endif // __SCHEDULER_H__
