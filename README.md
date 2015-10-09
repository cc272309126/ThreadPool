# A naive C++ ThreadPool

### ThreadPool - Basic thread pooling functionality.
See a tiny example in main.cpp.

##### Create a thread pool:
    Executors::FixedThreadPool pool(size);

##### Submit a task to thread pool:
    pool.AddTask(Base::NewCallBack(func, args...));

##### Stop the thread pool and wait for all tasks to finish:
    pool.Stop();
    pool.AwaitTermination();

### EventManager - An event-driven thread pool.
EventManager is an IO event-driven thread pool. The difference with a regular thread pool is that it listens to IO descriptors with epolling in a seperate thread. The epolling thread would submit IO-ready tasks to its internal thread pool. EventManager utilizes epoll(4) and supports non-blocking IO. 

##### Create an EventManager:
    EventManager event_manager(thread_pool_size);

##### Submit a task without IO monitoring same as a regular FixedThreadPool:
    event_manager.AddTask(Base::NewCallBack(func, args...));

##### Submit a task waiting for IO to be readable:
    event_manager.AddTaskWaitingReadable(fd, Base::NewCallBack(&func, args...);

##### Submit a task waiting for IO to be writable:
    event_manager.AddTaskWaitingWritable(fd, Base::NewCallBack(&func, args...);

##### Remove a awaiting task from event manager:
    event_manager.RemoveAwaitingTask(fd);

An tiny echo server example is provided which uses EventManager for multi-task handling. Server side is purely non-block socket programmed and uses state machine to maintain status of each connection. A simple client sends concurrent requests for load testing.