# WEB_Server
<head>版本一</head><br>
<body>
使用了EPOLL+线程池，实现了Web-Server，
设计了一个比较简易的线程池，线程池中的线程数是固定的，下一步计划设计线程池为动态的，根据实际工作负担动态调整线程池中的线程数<br>
</body>
版本二<br>
实现了动态调整线程数的程序，中间遇到一个问题，之前一直没有解决，就是在删除线程的时候，一直删除不动，最终经过测试才发现，是因为在结束线程时，没有对互斥锁进行解锁操作
导致删除线程之后，其他线程都还不能抢到互斥锁，最终导致程序停滞，无法运行。<br>
测试了一下，目前存在问题，即使只有一个连接，任务队列中，会加入很多任务，目前还不清楚是哪里出了问题，<br>
版本三<br>
修复了上个版本存在的问题，重构的线程池的代码，上个版本的线程池中，使用数组实现了一个队列，来存放需要完成的任务，这个版本使用了C++中提供的queue，并且使用std::bind()函数来接收“任务”，上个版本是用一个结构体来接收的，将原有代码的一个文件分为了多个类。尽量让每个单独的功能形成一个类。这样代码更加简洁<br>
版本四<br>
对应代码为webserver_v1.3,增加了日志系统。对应代码还没贴上去，目前是一个简单的异步日志系统<br>
版本五<br>
目前正在做的，做一个登录界面，用户信息存放到mysql中......具体细节还没定，希望这个假期能加完这个功能。目前已经完成，集成mysql实现登陆功能
项目反思<br>
截至目前，自己之前对项目的设想也基本上全都实现了。但是其中有两个问题，对于mysql这块内容，我只会一些比较常用的指令，当问及如何进行优化时，不知道。下一步需要补齐这些基础知识。还有就是另外一个项目有点简单的。并没有什么有用的优化，或者说可以提出的创新点。是否考虑要重新做个项目还是咋滴。这两天再考虑考虑。
