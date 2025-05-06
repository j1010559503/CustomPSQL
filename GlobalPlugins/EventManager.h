#pragma once

#include <QObject>
#include <QEvent>
#include <QApplication>
#include <QMutex>
#include <QMutexLocker>
#include <memory>

#include "BaseTool.h"
#include "globalplugins_global.h"
#include "config.h"

// 自定义事件类型
const QEvent::Type CustomEventType = static_cast<QEvent::Type>(QEvent::User + 1);

/**
 * @brief 自定义事件类
 * 用于工具互相通信
 */
class GLOBALPLUGINS_EXPORT CustomEvent : public QEvent
{
public:
    explicit CustomEvent(const QString& msg, void* data = nullptr)
        : QEvent(CustomEventType) 
    {
        m_msg = msg;
        if (data)
            m_params.append(data);
    }

    QVector<void*> data() const { return m_params; }

    QString message() const { return m_msg; }

    void addData(void* data)
    {
        if (!m_params.contains(data))
            m_params.append(data);
    }

    inline void setSender(std::shared_ptr<BaseTool> sender) { m_sender = sender; };

    inline std::shared_ptr<BaseTool> getSender() { return m_sender; };

private:
    QVector<void*> m_params;
    QString m_msg;
    std::shared_ptr<BaseTool> m_sender;
};

/**
 * @brief 事件管理器类，单例类
 * 事件管理分发
 */
class GLOBALPLUGINS_EXPORT EventManager : public QObject
{
    Q_OBJECT
public:
    // 获取单例实例
    static EventManager* instance();

    // 删除拷贝构造函数和赋值操作符以防止复制
    EventManager(const EventManager&) = delete;
    EventManager& operator=(const EventManager&) = delete;

    void subscribe(std::shared_ptr<BaseTool> handler)
    {
        m_handlers.append(handler);
    }

    void unsubscribe(std::shared_ptr<BaseTool> handler)
    {
        m_handlers.removeAll(handler);
    }

    bool registered(std::shared_ptr<BaseTool> handler)
    {
        if (handler && m_handlers.contains(handler))
            return true;
        else
            return false;
    }

    void sendEvent(const QString& msg, void* data)
    {
        CustomEvent _event(msg, data);
        for (auto handler : m_handlers)
        {
            QApplication::sendEvent(handler.get(), &_event);
        }
    }

protected:
    bool event(QEvent* event) override 
    {
        if (event->type() == CustomEventType) 
        {
            for (auto handler : m_handlers)
            {
                handler->handleEvent(event);
            }
            return true;
        }
        return QObject::event(event);
    }

private:
    EventManager(QObject* parent = nullptr) : QObject(parent) {}

    ~EventManager() 
    {
        //// 清理资源
        //qDeleteAll(m_handlers);
        m_handlers.clear();
    }

    QList<std::shared_ptr<BaseTool>> m_handlers;

    static EventManager* s_instance;
};

