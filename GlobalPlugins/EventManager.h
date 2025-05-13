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
 * 构造参数
 * (发送信息字符串msg，缺省参数 默认为空)
 */
class GLOBALPLUGINS_EXPORT CustomEvent : public QEvent
{
public:
    explicit CustomEvent(const QString& msg, const QVariantMap& params = QVariantMap())
        : QEvent(CustomEventType), m_parameters(params)
    {
        m_msg = msg;
    }

    ~CustomEvent()
    {
        m_parameters.clear();
    }

    QVariantMap data() const { return m_parameters; }

    QString message() const { return m_msg; }

    inline void setSender(BaseTool* sender) { m_sender = sender; };

    inline BaseTool* getSender() { return m_sender; };

private:
    QString m_msg;
    BaseTool* m_sender = nullptr;
    QVariantMap m_parameters;
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
    static EventManager& instance();

    // 删除拷贝构造函数和赋值操作符以防止复制
    EventManager(const EventManager&) = delete;
    EventManager& operator=(const EventManager&) = delete;

    void subscribe(BaseTool* handler)
    {
        m_handlers.append(handler);
    }

    void unsubscribe(BaseTool* handler)
    {
        m_handlers.removeAll(handler);
    }

    bool registered(BaseTool* handler)
    {
        if (handler && m_handlers.contains(handler))
            return true;
        else
            return false;
    }

    void sendEvent(const QString& msg, const QVariantMap& params = QVariantMap())
    {
        CustomEvent _event(msg, params);
        for (auto handler : m_handlers)
        {
            QApplication::sendEvent(handler, &_event);
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

    QList<BaseTool*> m_handlers;

    static EventManager* s_instance;
};

