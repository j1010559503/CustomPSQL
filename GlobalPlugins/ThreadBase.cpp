#include "ThreadBase.h"

ThreadBase::ThreadBase(QObject* parent)
    : QThread(parent)
    , m_shouldStop(0)
{
}

ThreadBase::~ThreadBase()
{
    // ȷ���̰߳�ȫֹͣ
    stopThread();

    // �ȴ��߳̽���
    wait();
}

void ThreadBase::startThread()
{
    // ����߳��Ѿ������У���ֱ�ӷ���
    if (isRunning())
        return;

    // ����ֹͣ��־
    m_shouldStop.storeRelease(0);

    // �����߳�
    start();
}

void ThreadBase::stopThread()
{
    // ����ֹͣ��־
    m_shouldStop.storeRelease(1);

    // ���ѵȴ����߳�
    m_condition.wakeAll();

    // �ȴ��߳̽���
    if (!wait(3000)) {
        // ����߳�û����3���ڽ���������ֹ��
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
    // �����̳߳�ʼ��
    threadInitialize();

    while (!m_shouldStop.loadAcquire()) 
    {
        std::function<void()> task;

        // ��ȡ����
        {
            QMutexLocker locker(&m_mutex);

            // �������Ϊ�գ��ȴ��������ֹͣ�ź�
            while (m_taskQueue.isEmpty() && !m_shouldStop.loadAcquire()) 
            {
                m_condition.wait(&m_mutex);
            }

            // �ٴμ��ֹͣ��־
            if (m_shouldStop.loadAcquire())
                break;

            // ���������ȻΪ�գ������ȴ�
            if (m_taskQueue.isEmpty())
                continue;

            // ��ȡ�����еĵ�һ������
            task = m_taskQueue.dequeue();

            // �������Ϊ�գ������ź�
            if (m_taskQueue.isEmpty())
                emit queueEmpty();
        }

        // ��������
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

    // �����߳�����
    threadCleanup();
}

void ThreadBase::processTask(std::function<void()> task)
{
    // Ĭ��ʵ��ֱ��ִ������
    if (task) 
    {
        task();
    }
}

void ThreadBase::threadInitialize()
{
    // Ĭ��ʵ��Ϊ��
}

void ThreadBase::threadCleanup()
{
    // Ĭ��ʵ��Ϊ��
}