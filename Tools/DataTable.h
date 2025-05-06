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
    void execute() override; // ���麯����ȷ���������ʵ��
    void handleEvent(QEvent* event) override; //�¼����մ���
    void showTable(QStandardItem* table);
private:
    // ���ݱ�
    QTableView* m_dataTable = nullptr;
    //QStandardItemModel* m_dtModel = nullptr;
    QSqlTableModel* m_dtModel = nullptr;
    // ��ǰѡ�е����ݱ�
    QModelIndex m_selectedTable;
    // ���ʱ��ʾͬ���ж�������߸߹�item
    QStandardItem* m_topHighlightItem = nullptr;
    QStandardItem* m_leftHighlightItem = nullptr;
    // ���ݱ������Ƿ��޸�
    bool m_dtItemTextChanged = false;
    // ��ǰѡ�����ݱ���
    QString m_selectedTableName;
    // ��ǰѡ�����ݱ��ֶ���
    int m_numSelectTableField = 0;
    // ����ʼ��
    bool m_initTable = false;
    // ���������ݿ�model
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
