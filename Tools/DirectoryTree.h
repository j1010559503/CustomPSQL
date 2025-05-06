#pragma once

#include <QTreeView>
#include <QStandardItemModel>
#include <QSqlDatabase>
#include <QHeaderView>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFileInfo>
#include <QSqlQuery>

#include "BaseTool.h"
#include "ToolFactory.h"
#include "BaseStyleWidget.h"
#include "GlobalManager.h"
#include "tools_global.h"
#include "MainWindow.h"

class CustomTreeView;
/**
 * @brief 左侧目录树类
 * 左侧展示当前连接数据库以及其表
 * 所有表项设置data为当前连接名
 */
class TOOLS_EXPORT DirectoryTree : public BaseTool, public std::enable_shared_from_this<DirectoryTree>
{
    Q_OBJECT
public:
    DirectoryTree(const QString& name);
    ~DirectoryTree();
    void execute() override; // 纯虚函数，确保子类必须实现
    void handleEvent(QEvent* event) override; //事件接收处理
    void connectDatabase(QString message);
    void addDataTable(QString sqlStr);

public slots:
    void showTable(const QModelIndex index);

private:
    CustomTreeView* m_directoryTree = nullptr;
    QStandardItemModel* m_dtModel = nullptr;
};

class TOOLS_EXPORT CustomTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit CustomTreeView(QWidget* parent = nullptr) : QTreeView(parent) {}
};