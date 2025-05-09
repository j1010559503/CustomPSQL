#include "ThreadPool.h"

ThreadPool::ThreadPool(QObject* parent)
    : QObject(parent)
    , m_threadPool(new QThreadPool(this))
    , m_activeTaskCount(0)
{
}

ThreadPool::~ThreadPool()
{
    shutdown();
}

void ThreadPool::initialize(int threadCount)
{
    m_threadPool->setMaxThreadCount(threadCount);
}

void ThreadPool::shutdown()
{
    m_threadPool->clear();
    m_threadPool->waitForDone();
    m_activeTaskCount.storeRelease(0);
}

bool ThreadPool::waitForDone(int msTimeout)
{
    return m_threadPool->waitForDone(msTimeout);
}

int ThreadPool::threadCount() const
{
    return m_threadPool->maxThreadCount();
}

int ThreadPool::activeTaskCount() const
{
    return m_activeTaskCount.loadAcquire();
}

void ThreadPool::setMaxThreadCount(int maxThreadCount)
{
    m_threadPool->setMaxThreadCount(maxThreadCount);
}

// Task 实现
ThreadPool::Task::Task(std::function<void()> func, ThreadPool* pool)
    : m_function(func)
    , m_pool(pool)
{
}

void ThreadPool::Task::run()
{
    // 执行任务
    if (m_function) 
    {
        m_function();
    }

    // 减少活动任务计数
    int remainingTasks = m_pool->m_activeTaskCount.fetchAndSubRelaxed(1) - 1;

    // 发出任务完成信号
    emit m_pool->taskCompleted();

    // 如果没有剩余任务，发出所有任务完成信号
    if (remainingTasks == 0) 
    {
        emit m_pool->allTasksCompleted();
    }
}