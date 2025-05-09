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

    // 启动线程
    void startThread();

    // 停止线程（安全停止）
    void stopThread();

    // 检查线程是否正在运行
    bool isRunning() const;

    // 添加任务到队列
    template<typename Func, typename... Args>
    void addTask(Func&& func, Args&&... args);

    // 清空任务队列
    void clearTasks();

    // 获取当前队列中的任务数量
    int taskCount() const;

    // 设置线程优先级
    void setPriority(QThread::Priority priority);

signals:
    // 任务完成信号
    void taskCompleted();

    // 任务队列为空信号
    void queueEmpty();

    // 线程错误信号
    void threadError(const QString& errorMessage);

protected:
    // 线程执行函数
    virtual void run() override;

    // 处理单个任务的虚函数，可由子类重写
    virtual void processTask(std::function<void()> task);

    // 线程初始化虚函数，可由子类重写
    virtual void threadInitialize();

    // 线程清理虚函数，可由子类重写
    virtual void threadCleanup();

private:
    // 任务队列
    QQueue<std::function<void()>> m_taskQueue;

    // 互斥锁保护任务队列
    mutable QMutex m_mutex;

    // 条件变量用于线程同步
    QWaitCondition m_condition;

    // 原子变量标记线程是否应该停止
    QAtomicInt m_shouldStop;
};

// 模板函数实现
template<typename Func, typename... Args>
void ThreadBase::addTask(Func&& func, Args&&... args)
{
    // 创建一个绑定了参数的函数对象
    auto boundFunc = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);

    // 将任务添加到队列
    QMutexLocker locker(&m_mutex);
    m_taskQueue.enqueue([boundFunc]() { boundFunc(); });

    // 唤醒等待的线程
    m_condition.wakeOne();
}