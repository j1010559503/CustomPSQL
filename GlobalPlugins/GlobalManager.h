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
 * @brief 全局管理类
 * 提供工具所需公共接口
 */
class GLOBALPLUGINS_EXPORT GlobalManager
{
private:
	GlobalManager();
	~GlobalManager();
	GlobalManager(const GlobalManager&) = delete;            // 禁止拷贝
	GlobalManager& operator=(const GlobalManager&) = delete; // 禁止赋值
public:
	static GlobalManager* instance();

	inline void SetMainWindow(MainWindow* m) { m_mainWindow = m; };

	inline MainWindow* GetMainWindow() { return m_mainWindow; };

    // 发送事件，用于工具间通信
    // 发送者，事件
	void sendEvent(std::shared_ptr<BaseTool> sender, CustomEvent* event);

    // 注册工具类，用于事件收发
	void RegisterTool(std::shared_ptr<BaseTool> tool);

    // 添加数据库连接，自动生成唯一的连接名称
    // 类型，数据库路径，接收连接名
    bool addDatabase(const QString& driver, const QString& databaseName, QString& outConnectionName); 
    
    // 移除数据库连接
    bool removeDatabase(const QString& connectionName);

    // 获取数据库连接
    QSqlDatabase getDatabase(const QString& connectionName);

    // 获取当前激活数据库连接
    QSqlDatabase getActivedDatabase();

    // 设置当前激活数据库连接名
    inline void setActivedConnectName(const QString& connectionName) { m_activedConnect = connectionName; };

    // 获取当前激活数据库连接名
    QString getActivedConnectName() { return m_activedConnect; };

    // 设置当前激活数据表项
    inline void setActivedTbItem(QStandardItem* name) { m_curActivedTb = name; };

    // 获取当前激活数据表项
    QStandardItem* getActivedTbItem() { return m_curActivedTb; };

    // 检查数据库连接是否打开
    bool isDatabaseOpen(const QString& connectionName);
    
    // 获取当前数据库连接数
    inline int getConnectCounts() { return m_databases.size(); };

    // 生成唯一的连接名称
    QString generateUniqueConnectionName();
    
private:
	static GlobalManager* s_instance;			// 使用GlobalManager*作为访问唯一对象的入口
	static QMutex* m_mutex;					    // 互斥锁
	MainWindow* m_mainWindow = nullptr;			// 主窗口
    QMap<QString, QSqlDatabase> m_databases;    // 包含所有数据库连接
    int connectionCounter = 0;                  // 连接计数
    QString m_activedConnect;                   // 当前激活数据库连接名
    QVector<QString> m_dbPaths;                 // 所有连接数据库路径
    QStandardItem* m_curActivedTb = nullptr;	// 当前激活表项
};

