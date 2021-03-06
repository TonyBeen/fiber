/*************************************************************************
    > File Name: thread.cpp
    > Author: hsz
    > Brief:
    > Created Time: Sun 27 Feb 2022 04:47:10 PM CST
 ************************************************************************/

#include "thread.h"
#include <utils/utils.h>
#include <utils/exception.h>
#include <log/log.h>

#define LOG_TAG "Thread"

namespace eular {

static thread_local Thread *gThread = nullptr;      // 当前线程
static thread_local eular::String8 gThreadName;     // 当前线程名字

Thread::Thread(std::function<void()> cb, const eular::String8 &threadName, uint32_t stackSize) :
    mThreadName(threadName.length() ? threadName : "Unknow"),
    mCb(cb),
    mShouldJoin(true),
    mSemaphore(0)
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    if (stackSize) {
        pthread_attr_setstacksize(&attr, stackSize);
    }

    int ret = pthread_create(&mTid, &attr, &Thread::entrance, this);
    pthread_attr_destroy(&attr);
    if (ret) {
        LOGE("pthread_create error. [%d,%s]", errno, strerror(errno));
        throw eular::Exception("pthread_create error");
    }
    mSemaphore.wait();
}

Thread::~Thread()
{
    if (mShouldJoin && mTid) {
        pthread_detach(mTid);
    }
}

void Thread::SetName(eular::String8 name)
{
    if (name.isEmpty()) {
        return;
    }
    if (gThread) {
        gThread->mThreadName = name;
    }
    gThreadName = name;
}

eular::String8 Thread::GetName()
{
    return gThreadName;
}

Thread *Thread::GetThis()
{
    return gThread;
}

void Thread::detach()
{
    if (mTid) {
        pthread_detach(mTid);
        mShouldJoin = false;
    }
}

void Thread::join()
{
    if (mShouldJoin && mTid) {
        int ret = pthread_join(mTid, nullptr);
        if (ret) {
            LOGE("pthread_join error. [%d,%s]", errno, strerror(errno));
            throw eular::Exception("pthread_join error");
        }
        mTid = 0;
    }
}

void *Thread::entrance(void *arg)
{
    LOG_ASSERT(arg, "arg never be null");
    Thread *th = static_cast<Thread *>(arg);
    gThread = th;
    gThreadName = th->mThreadName;
    gThread->mKernalTid = gettid();
    gThread->mSemaphore.post();

    pthread_setname_np(pthread_self(), th->mThreadName.substr(0, 15).c_str());

    std::function<void()> cb;
    cb.swap(th->mCb);

    cb();
    return nullptr;
}

} // namespace eular