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
}

int main(int argc, char **argv)
{
    Scheduler scheduler(2, true, "main");
    scheduler.start();

    scheduler.schedule(std::bind(&test));

    while (1) {
        printf("main sleep 1s\n");
        sleep(1);
    }

    return 0;
}
