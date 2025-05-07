#pragma once

#include <QStandardItemModel>
#include <QHeaderView>
#include <QSqlQuery>
#include <QStyledItemDelegate>
#include <QSqlRecord>
#include <QTableView>
#include <QSqlTableModel>
#include <QSqlField>
#include <QLoggingCategory>
#include <QSqlDriver>

#include "BaseTool.h"
#include "ToolFactory.h"
#include "BaseStyleWidget.h"
#include "GlobalManager.h"
#include "tools_global.h"
#include "MainWindow.h"

/**
 * @brief �Ҳ����ݱ���
 */
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

//class CustomTableModel : public QSqlTableModel {
//    Q_OBJECT
//
//public:
//    CustomTableModel(QObject* parent = nullptr, QSqlDatabase db = QSqlDatabase())
//        : QSqlTableModel(parent, db) {}
//
//    bool submitAll() override 
//    {
//        for (int row = 0; row < rowCount(); ++row) {
//            if (isDirty(index(row, 0))) { // Check if any field in the row is dirty
//                if (!submitRow(row)) {
//                    qDebug() << "Failed to submit row" << row;
//                    return false;
//                }
//            }
//        }
//        return true;
//    }
//
//protected:
//    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override {
//        if (!QSqlTableModel::setData(index, value, role))
//            return false;
//
//        emit dataChanged(index, index);
//        return true;
//    }
//};

class TOOLS_EXPORT ReadOnlyItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ReadOnlyItemDelegate();
    ~ReadOnlyItemDelegate();
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};
