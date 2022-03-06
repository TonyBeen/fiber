/*************************************************************************
    > File Name: test_fiber.cc
    > Author: hsz
    > Brief:
    > Created Time: Wed 02 Mar 2022 08:29:36 AM CST
 ************************************************************************/

#include "fiber.h"
#include <log/log.h>
#include <iostream>

using namespace std;
using namespace eular;

#define LOG_TAG "test-fiber"

void test_fiber()
{
    LOGD("test_fiber begin");
    Fiber::Yeild2Hold();
    LOGD("test_fiber end");
}

const char *FiberState(Fiber::FiberState state)
{
    switch (state) {
    case Fiber::READY:
        return "READY";
    case Fiber::HOLD:
        return "HOLD";
    case Fiber::EXEC:
        return "EXEC";
    case Fiber::EXCEPT:
        return "EXCEPT";
    case Fiber::TERM:
        return "TERM";
    default:
        break;
    }

    return "UNKNOW";
}

int main(int argc, char **argv)
{
    Fiber::SP ptr = Fiber::GetThis();
    Fiber::SP fiber(new Fiber(test_fiber));

    LOG_ASSERT(fiber != nullptr, "");
    fiber->resume();
    LOGD("main get fiber state %s", FiberState(fiber->getState()));
    fiber->resume();     // 此处调用会使FiberEntry执行完毕，所以保持resume次数比Yeild操作多一即可
    return 0;
}
