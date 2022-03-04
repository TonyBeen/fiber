/*************************************************************************
    > File Name: scheduler.cpp
    > Author: hsz
    > Brief:
    > Created Time: Sun 27 Feb 2022 05:41:32 PM CST
 ************************************************************************/

#include "scheduler.h"
#include <utils/utils.h>
#include <log/log.h>


#define LOG_TAG "scheduler"

namespace eular {

static thread_local Scheduler *gScheduler = nullptr;    // 线程调度器
static thread_local Fiber *gMainFiber = nullptr;        // 线程的主协程

Scheduler::Scheduler(uint32_t threads, bool useCaller, const std::string &name) :
    mContainUserCaller(useCaller),
    mName(name),
    mStoped(true)
{
    LOG_ASSERT(threads > 0, "%s %s:%s() Invalid Param", __FILE__, __LINE__, __func__);
    if (useCaller) {
        Fiber::GetThis();
        --threads;

        gScheduler = this;
        mRootFiber.reset(new Fiber(std::bind(&Scheduler::run, this)));
        Thread::SetName(name);

        mThreadIds.push_back(gettid());
        gMainFiber = mRootFiber.get();
        mRootThread = gettid();
    } else {
        mRootThread = -1;
    }

    mThreadCount = threads;
}

Scheduler::~Scheduler()
{

}


Scheduler *Scheduler::GetThis()
{
    return gScheduler;
}

Fiber *Scheduler::GetMainFiber()
{
    return gMainFiber;
}


void Scheduler::start()
{
    AutoLock<Mutex> lock(mMutex);
    if (!mStoped) {
        return;
    }

    mStoped = false;
    LOG_ASSERT(mThreads.empty(), "should be empty before start()");
    mThreads.resize(mThreadCount);
    for (size_t i = 0; i < mThreadCount; ++i) {
        mThreads[i].reset(new Thread(std::bind(&Scheduler::run, this), mName + "_" + std::to_string(i)));
        mThreadIds.push_back(mThreads[i]->getTid());
        LOGD("thread [%s:%d] start", mThreads[i]->getName().c_str(), mThreads[i]->getTid());
    }
}

void Scheduler::stop()
{

}

void Scheduler::run()
{
    LOGI("Scheduler::run() in %s:%d", Thread::GetName().c_str(), gettid());
    // hook_enable();
    setThis();
    if (gettid() != mRootThread) {
        gMainFiber = Fiber::GetThis().get();
    }
    Fiber::SP idleFiber(new Fiber(std::bind(&Scheduler::idle, this)));
    Fiber::SP cbFiber;

    FiberBindThread ft;
    while (true) {
        ft.reset();
        bool needTickle = false;
        bool isActive = false;
        {
            AutoLock<Mutex> lock(mMutex);
            auto it = mFiberQueue.begin();
            while (it != mFiberQueue.end()) {
                if (it->thread != -1 && it->thread != gettid()) {   // 不满足线程ID一致的条件
                    ++it;
                    needTickle = true;
                    continue;
                }

                LOG_ASSERT(it->fiberPtr || it->cb, "task can not be null");
                if (it->fiberPtr && it->fiberPtr->getState() == Fiber::EXEC) {
                    ++it;
                    continue;
                }

                // 找到可执行的任务
                ft = *it;
                mFiberQueue.erase(it++);
                ++mActiveThreadCount;
                isActive = true;
                break;
            }
            needTickle |= it != mFiberQueue.end();
        }

        if (needTickle) {
            tickle();
        }

        if (ft.fiberPtr && (ft.fiberPtr->getState() != Fiber::EXEC && ft.fiberPtr->getState() != Fiber::EXCEPT)) {
            ft.fiberPtr->resume();
            --mActiveThreadCount;
            if (ft.fiberPtr->getState() == Fiber::READY) {  // 用户主动设置协程状态为REDAY
                schedule(ft.fiberPtr);
            } else if (ft.fiberPtr->getState() != Fiber::TERM &&
                       ft.fiberPtr->getState() != Fiber::EXCEPT) {
                ft.fiberPtr->mState = Fiber::HOLD;
            }
            ft.reset();
        } else if (ft.cb) {
            if (cbFiber) {
                cbFiber->reset(ft.cb);
            } else {
                cbFiber.reset(new Fiber(ft.cb));
                LOG_ASSERT(cbFiber != nullptr, "");
            }
        }
        ft.reset();
        cbFiber->resume();
        --mActiveThreadCount;
        if (cbFiber->getState() == Fiber::READY) {
            schedule(cbFiber);
            cbFiber.reset();
        } else if (cbFiber->getState() == Fiber::EXCEPT ||
                   cbFiber->getState() == Fiber::TERM) {
            cbFiber->reset(nullptr);
        }
    }
}

void Scheduler::setThis()
{
    gScheduler = this;
}

void Scheduler::idle()
{

}

} // namespace eular
