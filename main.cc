/*************************************************************************
    > File Name: main.cc
    > Author: hsz
    > Brief:
    > Created Time: Sat 05 Mar 2022 10:00:56 PM CST
 ************************************************************************/

#include "scheduler.h"
#include <utils/utils.h>

using namespace std;
using namespace eular;

void test()
{
    printf("%s() tid: %ld\n", __func__, gettid());
    printf("%s() fiber id %lu\n", __func__, Fiber::GetFiberID());
}

int main(int argc, char **argv)
{
    Scheduler scheduler(2, true, "main");
    scheduler.start();

    scheduler.schedule(std::bind(&test), gettid());

    printf("main fiber state: %d\n", Fiber::GetThis()->getState());

    Scheduler::GetMainFiber()->resume();

    return 0;
}
