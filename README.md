# fiber
### 协程库

基于Linux的`ucontext`编写

Fiber是实现具体的协程逻辑；
Scheduler是协程调度器，内部维护着一个线城池，支持协程绑定到具体线程

thread是实现的线程逻辑，与std::thread类似

编写时继承Scheduler后再重写虚函数即可

##### 代码参考了[sylar](https://github.com/sylar-yin/sylar)，算是一个协程提取
