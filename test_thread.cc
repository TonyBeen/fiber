/*************************************************************************
    > File Name: test_thread.cc
    > Author: hsz
    > Brief:
    > Created Time: Sun 27 Feb 2022 05:30:38 PM CST
 ************************************************************************/

#include "thread.h"
#include <utils/utils.h>
#include <iostream>

using namespace std;

void test_thread()
{
    cout << __func__ << "() thread name: " << Thread::GetThis()->getName() << ", tid: " << gettid() << endl;
}

int main(int argc, char **argv)
{
    Thread th(std::bind(test_thread), "for test");

    th.detach();
    
    printf("main thread tid: %ld\n", gettid());
    for (int i = 0; i < 100 * 1000; ++i) {}
    return 0;
}
