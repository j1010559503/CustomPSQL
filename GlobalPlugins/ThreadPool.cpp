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

// Task ʵ��
ThreadPool::Task::Task(std::function<void()> func, ThreadPool* pool)
    : m_function(func)
    , m_pool(pool)
{
}

void ThreadPool::Task::run()
{
    // ִ������
    if (m_function) 
    {
        m_function();
    }

    // ���ٻ�������
    int remainingTasks = m_pool->m_activeTaskCount.fetchAndSubRelaxed(1) - 1;

    // ������������ź�
    emit m_pool->taskCompleted();

    // ���û��ʣ�����񣬷���������������ź�
    if (remainingTasks == 0) 
    {
        emit m_pool->allTasksCompleted();
    }
}