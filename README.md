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
EventManager is an IO event-driven thread pool. The difference with a regular thread pool is that it listens to IO descriptors with epolling in a seperate thread. The epolling thread would submit IO-ready tasks to its internal thread pool. EventManager supports non-blocking IO.

##### Create an EventManager:
    EventManager event_manager(thread_pool_size);

##### Submit a task without IO listening is just as FixedThreadPool:
    event_manager.AddTask(Base::NewCallBack(func, args...));

##### Submit a task waiting for IO to be readable:
    event_manager.AddTaskWaitingReadable(fd, Base::NewCallBack(&func, args...);

##### Submit a task waiting for IO to be writable:
    event_manager.AddTaskWaitingWritable(fd, Base::NewCallBack(&func, args...);
