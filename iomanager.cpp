/*************************************************************************
    > File Name: iomanager.cpp
    > Author: hsz
    > Brief:
    > Created Time: Sat 05 Mar 2022 02:48:58 PM CST
 ************************************************************************/

#include "iomanager.h"
#include <log/log.h>
#include <sys/epoll.h>

#define LOG_TAG "IOManager"
#define EPOLL_MAX_SIZE 4096

namespace eular {

IOManager::IOManager(int threads, bool userCaller, String8 threadName) :
    mEpollFd(-1),
    Scheduler(threads, userCaller, threadName)
{
    mEpollFd = epoll_create(EPOLL_MAX_SIZE);
    LOG_ASSERT(mEpollFd > 0, "epoll_create error. [%d,%s]", errno, strerror(errno));

}

IOManager::~IOManager()
{

}


} // namespace eular
