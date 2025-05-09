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

    // 初始化线程池，设置线程数量
    void initialize(int threadCount = QThread::idealThreadCount());

    // 关闭线程池
    void shutdown();

    // 添加任务到线程池
    template<typename Func, typename... Args>
    void addTask(Func&& func, Args&&... args);

    // 等待所有任务完成
    bool waitForDone(int msTimeout = -1);

    // 获取线程池中的线程数量
    int threadCount() const;

    // 获取当前队列中的任务数量
    int activeTaskCount() const;

    // 设置线程池的最大线程数
    void setMaxThreadCount(int maxThreadCount);

signals:
    // 所有任务完成信号
    void allTasksCompleted();

    // 任务完成信号
    void taskCompleted();

    // 线程池错误信号
    void poolError(const QString& errorMessage);

private:
    // 任务包装类
    class Task : public QRunnable
    {
    public:
        Task(std::function<void()> func, ThreadPool* pool);
        void run() override;

    private:
        std::function<void()> m_function;
        ThreadPool* m_pool;
    };

    // Qt线程池
    QThreadPool* m_threadPool;

    // 活动任务计数
    QAtomicInt m_activeTaskCount;

    // 互斥锁
    mutable QMutex m_mutex;

    // 条件变量
    QWaitCondition m_condition;
};

// 模板函数实现
template<typename Func, typename... Args>
void ThreadPool::addTask(Func&& func, Args&&... args)
{
    // 创建一个绑定了参数的函数对象
    auto boundFunc = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);

    // 增加活动任务计数
    m_activeTaskCount.fetchAndAddRelaxed(1);

    // 创建任务并提交到线程池
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

    // 设置自动删除
    task->setAutoDelete(true);

    // 启动任务
    m_threadPool->start(task);
}

