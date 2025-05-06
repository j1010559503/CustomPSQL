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

// �Զ����¼�����
const QEvent::Type CustomEventType = static_cast<QEvent::Type>(QEvent::User + 1);

/**
 * @brief �Զ����¼���
 * ���ڹ��߻���ͨ��
 * �������
 * (������Ϣ�ַ���msg��ȱʡ���� Ĭ��Ϊ��)
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

    inline void setSender(std::shared_ptr<BaseTool> sender) { m_sender = sender; };

    inline std::shared_ptr<BaseTool> getSender() { return m_sender; };

private:
    QString m_msg;
    std::shared_ptr<BaseTool> m_sender;
    QVariantMap m_parameters;
};

/**
 * @brief �¼��������࣬������
 * �¼�����ַ�
 */
class GLOBALPLUGINS_EXPORT EventManager : public QObject
{
    Q_OBJECT
public:
    // ��ȡ����ʵ��
    static EventManager* instance();

    // ɾ���������캯���͸�ֵ�������Է�ֹ����
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

    void sendEvent(const QString& msg, const QVariantMap& params = QVariantMap())
    {
        CustomEvent _event(msg, params);
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
        //// ������Դ
        //qDeleteAll(m_handlers);
        m_handlers.clear();
    }

    QList<std::shared_ptr<BaseTool>> m_handlers;

    static EventManager* s_instance;
};

