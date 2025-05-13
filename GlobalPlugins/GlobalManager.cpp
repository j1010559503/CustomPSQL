#include "GlobalManager.h"

QMutex* GlobalManager::m_mutex = nullptr; // mutex�ĳ�ʼ��

GlobalManager::GlobalManager()
{
    m_mutex = new QMutex();
}

GlobalManager::~GlobalManager()
{
    // �ر����ݿ�
    for (auto it = m_databases.begin(); it != m_databases.end(); ++it)
    {
        it.value().close();
    }
    m_databases.clear();

    delete m_mutex;

    m_mutex = nullptr;
}

GlobalManager& GlobalManager::instance()
{
    static GlobalManager s_instance;
    return s_instance;
}

void GlobalManager::sendEvent(BaseTool* sender, CustomEvent* event)
{
    if (sender)
    {
        event->setSender(sender);
        QCoreApplication::sendEvent(&EventManager::instance(), event);
    }
}

void GlobalManager::RegisterTool(BaseTool* tool)
{
    if (tool)
    {
        EventManager::instance().subscribe(tool);
    }
}

bool GlobalManager::addDatabase(const QString& driver, const QString& databaseName, QString& outConnectionName)
{
    if (m_dbPaths.contains(databaseName))
        return false;
    QString connectionName = generateUniqueConnectionName();
    QSqlDatabase db = QSqlDatabase::addDatabase(driver, connectionName);
    bool ss = QSqlDatabase::isDriverAvailable(driver);
    db.setDatabaseName(databaseName);
    if (!db.open()) 
    {
        qDebug() << "�޷������ݿ�" << connectionName << ":" << db.lastError().text();
        return false;
    }

    m_dbPaths.append(databaseName);
    m_databases.insert(connectionName, db);
    outConnectionName = connectionName;
    qDebug() << "�ɹ����ӵ����ݿ�" << connectionName;
    return true;
}

bool GlobalManager::removeDatabase(const QString& connectionName)
{
    if (!m_databases.contains(connectionName))
    {
        qDebug() << "���ݿ�����" << connectionName << "������";
        return false;
    }

    QSqlDatabase db = m_databases.take(connectionName);
    db.close();
    qDebug() << "�رղ��Ƴ����ݿ�����" << connectionName;
    return true;
}

QSqlDatabase GlobalManager::getDatabase(const QString& connectionName)
{
    if (!m_databases.contains(connectionName))
    {
        qDebug() << "���ݿ�����" << connectionName << "������";
        return QSqlDatabase();
    }
    return m_databases.value(connectionName);
}

QSqlDatabase GlobalManager::getActivedDatabase()
{
    return m_databases.value(m_activedConnect);
}

bool GlobalManager::isDatabaseOpen(const QString& connectionName)
{
    if (!m_databases.contains(connectionName))
    {
        qDebug() << "���ݿ�����" << connectionName << "������";
        return false;
    }
    return m_databases.value(connectionName).isOpen();
}

QString GlobalManager::generateUniqueConnectionName()
{
    QString connectionName;
    do {
        connectionName = QString("connection%1").arg(++connectionCounter);
    } while (m_databases.contains(connectionName));
    return connectionName;
}

