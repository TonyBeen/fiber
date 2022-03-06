/*************************************************************************
    > File Name: iomanager.h
    > Author: hsz
    > Brief:
    > Created Time: Sat 05 Mar 2022 02:48:54 PM CST
 ************************************************************************/

#include "scheduler.h"
#include <utils/string8.h>

namespace eular {
class IOManager : public Scheduler
{
public:
    IOManager(int threads, bool userCaller, String8 threadName);
    virtual ~IOManager();

protected:
    virtual void idle() override;
    virtual void tickle() override;



private:
    int         mEpollFd;

};


} // namespace eular

