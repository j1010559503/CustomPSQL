#include "ThreadBase.h"

ThreadBase::ThreadBase(QObject* parent)
    : QThread(parent)
    , m_shouldStop(0)
{
}

ThreadBase::~ThreadBase()
{
    // 确保线程安全停止
    stopThread();

    // 等待线程结束
    wait();
}

void ThreadBase::startThread()
{
    // 如果线程已经在运行，则直接返回
    if (isRunning())
        return;

    // 重置停止标志
    m_shouldStop.storeRelease(0);

    // 启动线程
    start();
}

void ThreadBase::stopThread()
{
    // 设置停止标志
    m_shouldStop.storeRelease(1);

    // 唤醒等待的线程
    m_condition.wakeAll();

    // 等待线程结束
    if (!wait(3000)) {
        // 如果线程没有在3秒内结束，则终止它
        terminate();
        wait();
    }
}

bool ThreadBase::isRunning() const
{
    return QThread::isRunning() && !m_shouldStop.loadAcquire();
}

void ThreadBase::clearTasks()
{
    QMutexLocker locker(&m_mutex);
    m_taskQueue.clear();
}

int ThreadBase::taskCount() const
{
    QMutexLocker locker(&m_mutex);
    return m_taskQueue.size();
}

void ThreadBase::setPriority(QThread::Priority priority)
{
    QThread::setPriority(priority);
}

void ThreadBase::run()
{
    // 调用线程初始化
    threadInitialize();

    while (!m_shouldStop.loadAcquire()) 
    {
        std::function<void()> task;

        // 获取任务
        {
            QMutexLocker locker(&m_mutex);

            // 如果队列为空，等待新任务或停止信号
            while (m_taskQueue.isEmpty() && !m_shouldStop.loadAcquire()) 
            {
                m_condition.wait(&m_mutex);
            }

            // 再次检查停止标志
            if (m_shouldStop.loadAcquire())
                break;

            // 如果队列仍然为空，继续等待
            if (m_taskQueue.isEmpty())
                continue;

            // 获取队列中的第一个任务
            task = m_taskQueue.dequeue();

            // 如果队列为空，发出信号
            if (m_taskQueue.isEmpty())
                emit queueEmpty();
        }

        // 处理任务
        try 
        {
            processTask(task);
            emit taskCompleted();
        }
        catch (const std::exception& e) 
        {
            emit threadError(QString("Exception in thread: %1").arg(e.what()));
        }
        catch (...) 
        {
            emit threadError(QString("Unknown exception in thread"));
        }
    }

    // 调用线程清理
    threadCleanup();
}

void ThreadBase::processTask(std::function<void()> task)
{
    // 默认实现直接执行任务
    if (task) 
    {
        task();
    }
}

void ThreadBase::threadInitialize()
{
    // 默认实现为空
}

void ThreadBase::threadCleanup()
{
    // 默认实现为空
}