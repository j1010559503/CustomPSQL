#pragma once

#include <QSqlTableModel>
#include <QSqlIndex>

#include "BaseTool.h"
#include "ToolFactory.h"
#include "BaseStyleWidget.h"
#include "GlobalManager.h"
#include "ui_AddTableWidget.h"
#include "tools_global.h"
#include "MainWindow.h"

class RollBackTool :
    public ButtonTool, public std::enable_shared_from_this<RollBackTool>
{
    Q_OBJECT
public:
    RollBackTool(const QString& text);
    ~RollBackTool();
    void handleEvent(QEvent* event) override;
    void clicked() override;
    void execute() override;
    void setIcon(QAction* action) override;
private:
    // �洢�޸�ǰ�����ݣ����ڻ���
    struct ChangeRecord 
    {
        QString tableName;
        QString primaryKeyColumn;
        QVariant primaryKeyValue;
        QMap<QString, QVariant> oldValues;
        QMap<QString, QVariant> newValues;
        enum ChangeType { Insert, Update, Delete } type;
    };

    QList<ChangeRecord> m_changeHistory;
    QSqlDatabase m_db;

    // ���ɲ�ִ��SQL���
    bool executeUpdateSql(const ChangeRecord& record);
    bool executeInsertSql(const ChangeRecord& record);
    bool executeDeleteSql(const ChangeRecord& record);

    // �ռ�ģ���еĸ���
    QList<ChangeRecord> collectChanges(QSqlTableModel* model);
};

