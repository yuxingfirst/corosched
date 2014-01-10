coroshed
========
corosched是一个轻量级线程(协程)调度框架，主要特性包括：

1. 协程调度
2. 消息通信
3. 事件驱动
4. 多线程交替运行  

###主调度器
mastersched，使用队列管理所有等待运行的协程，按照FIFO次序将协程投入运行。

###并行调度器
parallelsched，scheduler的一种，运行在第二个线程中，接受从mastersched发送过来的协程，并将其投入运行。主要是运行那些含有阻塞操作的代码块。

###消息通信
message, 协程之间采用消息通信机制,类似于Erlang, 区别于Golang。每个message需要保存发送协程、接受协程的pid。  

###事件驱动
eventmanager  基于epoll实现事件驱动机制。  
因为整个框架主要是用于网络I/O, 所以实际上整个框架是受事件驱动的。当有事件(读、写、超时事件)发生时，才会spawn出协程，此时eventmanager会驱动mastersched运行。

###NEXT

1. 协程之间消息通信  
2. 针对协程封装一层socket操作
