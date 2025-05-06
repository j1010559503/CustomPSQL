#pragma once

#include <QStandardItemModel>
#include <QHeaderView>
#include <QSqlQuery>
#include <QStyledItemDelegate>
#include <QSqlRecord>
#include <QTableView>
#include <QSqlTableModel>

#include "BaseTool.h"
#include "ToolFactory.h"
#include "BaseStyleWidget.h"
#include "GlobalManager.h"
#include "tools_global.h"
#include "MainWindow.h"

class TOOLS_EXPORT DataTable :
    public BaseTool, public std::enable_shared_from_this<DataTable>
{
    Q_OBJECT
public:
    DataTable(const QString& name);
    ~DataTable();
    void execute() override; // 纯虚函数，确保子类必须实现
    void handleEvent(QEvent* event) override; //事件接收处理
    void showTable(QStandardItem* table);
private:
    // 数据表
    QTableView* m_dataTable = nullptr;
    //QStandardItemModel* m_dtModel = nullptr;
    QSqlTableModel* m_dtModel = nullptr;
    // 当前选中的数据表
    QModelIndex m_selectedTable;
    // 点击时显示同行列顶部，左边高光item
    QStandardItem* m_topHighlightItem = nullptr;
    QStandardItem* m_leftHighlightItem = nullptr;
    // 数据表数据是否修改
    bool m_dtItemTextChanged = false;
    // 当前选中数据表名
    QString m_selectedTableName;
    // 当前选中数据表字段数
    int m_numSelectTableField = 0;
    // 表单初始化
    bool m_initTable = false;
    // 已连接数据库model
    QMap<QString, QSqlTableModel*> m_models;
};

class TOOLS_EXPORT ReadOnlyItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ReadOnlyItemDelegate();
    ~ReadOnlyItemDelegate();
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};
