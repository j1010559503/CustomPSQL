#pragma once

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include <QAtomicInt>
#include <functional>
#include <memory>

class ThreadBase : public QThread
{
    Q_OBJECT

public:
    explicit ThreadBase(QObject* parent = nullptr);
    virtual ~ThreadBase();

    // �����߳�
    void startThread();

    // ֹͣ�̣߳���ȫֹͣ��
    void stopThread();

    // ����߳��Ƿ���������
    bool isRunning() const;

    // ������񵽶���
    template<typename Func, typename... Args>
    void addTask(Func&& func, Args&&... args);

    // ����������
    void clearTasks();

    // ��ȡ��ǰ�����е���������
    int taskCount() const;

    // �����߳����ȼ�
    void setPriority(QThread::Priority priority);

signals:
    // ��������ź�
    void taskCompleted();

    // �������Ϊ���ź�
    void queueEmpty();

    // �̴߳����ź�
    void threadError(const QString& errorMessage);

protected:
    // �߳�ִ�к���
    virtual void run() override;

    // ������������麯��������������д
    virtual void processTask(std::function<void()> task);

    // �̳߳�ʼ���麯��������������д
    virtual void threadInitialize();

    // �߳������麯��������������д
    virtual void threadCleanup();

private:
    // �������
    QQueue<std::function<void()>> m_taskQueue;

    // �����������������
    mutable QMutex m_mutex;

    // �������������߳�ͬ��
    QWaitCondition m_condition;

    // ԭ�ӱ�������߳��Ƿ�Ӧ��ֹͣ
    QAtomicInt m_shouldStop;
};

// ģ�庯��ʵ��
template<typename Func, typename... Args>
void ThreadBase::addTask(Func&& func, Args&&... args)
{
    // ����һ�����˲����ĺ�������
    auto boundFunc = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);

    // ��������ӵ�����
    QMutexLocker locker(&m_mutex);
    m_taskQueue.enqueue([boundFunc]() { boundFunc(); });

    // ���ѵȴ����߳�
    m_condition.wakeOne();
}