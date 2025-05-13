#include "RollBackTool.h"

static bool registeredConnect = []()
{
	// 创建工具类对象
	ToolFactory::registerTool("RollBack", []() { return std::make_shared<RollBackTool>("RollBack"); });
	return true;
}();

RollBackTool::RollBackTool(const QString& text) :
	ButtonTool(text)
{
}

RollBackTool::~RollBackTool()
{
}

void RollBackTool::handleEvent(QEvent* event)
{
}

void RollBackTool::clicked()
{
}

void RollBackTool::execute()
{
	//注册工具，用于事件收发
	GlobalManager::instance().RegisterTool(this);
}

void RollBackTool::setIcon(QAction* action)
{
	QIcon openIcon = loadSvg(QString::fromUtf8("://new//prefix1//icon//RollBack_f.svg"), QColor("#FFFFFF")); // 白色背景
	action->setIcon(openIcon);
}

QList<RollBackTool::ChangeRecord> RollBackTool::collectChanges(QSqlTableModel* model)
{
    QList<ChangeRecord> changes;

    // 获取表名和主键列
    QString tableName = model->tableName();
    QString primaryKeyColumn;

    QSqlIndex primaryKey = model->primaryKey();

    // 假设第一列是主键
    primaryKeyColumn = model->headerData(primaryKey.count(), Qt::Horizontal).toString();

    for (int row = 0; row < model->rowCount(); ++row) 
    {
        if (model->isDirty(model->index(row, 0))) 
        {
            ChangeRecord record;
            record.tableName = tableName;
            record.primaryKeyColumn = primaryKeyColumn;
            record.primaryKeyValue = model->data(model->index(row, 0));

            // 收集所有列的数据
            for (int col = 0; col < model->columnCount(); ++col) 
            {
                QString columnName = model->headerData(col, Qt::Horizontal).toString();
                QVariant currentValue = model->data(model->index(row, col));
                QVariant originalValue = model->data(model->index(row, col), Qt::UserRole - 1); // 原始值

                record.newValues[columnName] = currentValue;
                record.oldValues[columnName] = originalValue;
            }

            changes.append(record);
        }
    }

    return changes;
}