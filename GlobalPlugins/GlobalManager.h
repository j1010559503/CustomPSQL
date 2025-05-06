#pragma once

#include <QMutex>
#include <QMutexLocker>
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QStandardItem>

//#include "MainWindow.h"
#include "EventManager.h"
#include "BaseTool.h"
#include "globalplugins_global.h"

class MainWindow;
/**
 * @brief ȫ�ֹ�����
 * �ṩ�������蹫���ӿ�
 */
class GLOBALPLUGINS_EXPORT GlobalManager
{
private:
	GlobalManager();
	~GlobalManager();
	GlobalManager(const GlobalManager&) = delete;            // ��ֹ����
	GlobalManager& operator=(const GlobalManager&) = delete; // ��ֹ��ֵ
public:
	static GlobalManager* instance();

	inline void SetMainWindow(MainWindow* m) { m_mainWindow = m; };

	inline MainWindow* GetMainWindow() { return m_mainWindow; };

    // �����¼������ڹ��߼�ͨ��
    // �����ߣ��¼�
	void sendEvent(std::shared_ptr<BaseTool> sender, CustomEvent* event);

    // ע�Ṥ���࣬�����¼��շ�
	void RegisterTool(std::shared_ptr<BaseTool> tool);

    // ������ݿ����ӣ��Զ�����Ψһ����������
    // ���ͣ����ݿ�·��������������
    bool addDatabase(const QString& driver, const QString& databaseName, QString& outConnectionName); 
    
    // �Ƴ����ݿ�����
    bool removeDatabase(const QString& connectionName);

    // ��ȡ���ݿ�����
    QSqlDatabase getDatabase(const QString& connectionName);

    // ��ȡ��ǰ�������ݿ�����
    QSqlDatabase getActivedDatabase();

    // ���õ�ǰ�������ݿ�������
    inline void setActivedConnectName(const QString& connectionName) { m_activedConnect = connectionName; };

    // ��ȡ��ǰ�������ݿ�������
    QString getActivedConnectName() { return m_activedConnect; };

    // ���õ�ǰ�������ݱ���
    inline void setActivedTbItem(QStandardItem* name) { m_curActivedTb = name; };

    // ��ȡ��ǰ�������ݱ���
    QStandardItem* getActivedTbItem() { return m_curActivedTb; };

    // ������ݿ������Ƿ��
    bool isDatabaseOpen(const QString& connectionName);
    
    // ��ȡ��ǰ���ݿ�������
    inline int getConnectCounts() { return m_databases.size(); };

    // ����Ψһ����������
    QString generateUniqueConnectionName();
    
private:
	static GlobalManager* s_instance;			// ʹ��GlobalManager*��Ϊ����Ψһ��������
	static QMutex* m_mutex;					    // ������
	MainWindow* m_mainWindow = nullptr;			// ������
    QMap<QString, QSqlDatabase> m_databases;    // �����������ݿ�����
    int connectionCounter = 0;                  // ���Ӽ���
    QString m_activedConnect;                   // ��ǰ�������ݿ�������
    QVector<QString> m_dbPaths;                 // �����������ݿ�·��
    QStandardItem* m_curActivedTb = nullptr;	// ��ǰ�������
};

