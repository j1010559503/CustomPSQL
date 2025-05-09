#pragma once

#include <QObject>
#include <QVector>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include <QAtomicInt>
#include <QThreadPool>
#include <QRunnable>
#include <functional>
#include <memory>

class ThreadPool : public QObject
{
    Q_OBJECT

public:
    explicit ThreadPool(QObject* parent = nullptr);
    ~ThreadPool();

    // ��ʼ���̳߳أ������߳�����
    void initialize(int threadCount = QThread::idealThreadCount());

    // �ر��̳߳�
    void shutdown();

    // ��������̳߳�
    template<typename Func, typename... Args>
    void addTask(Func&& func, Args&&... args);

    // �ȴ������������
    bool waitForDone(int msTimeout = -1);

    // ��ȡ�̳߳��е��߳�����
    int threadCount() const;

    // ��ȡ��ǰ�����е���������
    int activeTaskCount() const;

    // �����̳߳ص�����߳���
    void setMaxThreadCount(int maxThreadCount);

signals:
    // ������������ź�
    void allTasksCompleted();

    // ��������ź�
    void taskCompleted();

    // �̳߳ش����ź�
    void poolError(const QString& errorMessage);

private:
    // �����װ��
    class Task : public QRunnable
    {
    public:
        Task(std::function<void()> func, ThreadPool* pool);
        void run() override;

    private:
        std::function<void()> m_function;
        ThreadPool* m_pool;
    };

    // Qt�̳߳�
    QThreadPool* m_threadPool;

    // ��������
    QAtomicInt m_activeTaskCount;

    // ������
    mutable QMutex m_mutex;

    // ��������
    QWaitCondition m_condition;
};

// ģ�庯��ʵ��
template<typename Func, typename... Args>
void ThreadPool::addTask(Func&& func, Args&&... args)
{
    // ����һ�����˲����ĺ�������
    auto boundFunc = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);

    // ���ӻ�������
    m_activeTaskCount.fetchAndAddRelaxed(1);

    // ���������ύ���̳߳�
    Task* task = new Task([boundFunc, this]() {
        try {
            boundFunc();
        }
        catch (const std::exception& e) {
            emit poolError(QString("Exception in thread pool task: %1").arg(e.what()));
        }
        catch (...) {
            emit poolError(QString("Unknown exception in thread pool task"));
        }
        }, this);

    // �����Զ�ɾ��
    task->setAutoDelete(true);

    // ��������
    m_threadPool->start(task);
}

