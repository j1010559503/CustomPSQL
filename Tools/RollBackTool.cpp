#include "RollBackTool.h"

static bool registeredConnect = []()
{
	// �������������
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
	//ע�Ṥ�ߣ������¼��շ�
	GlobalManager::instance().RegisterTool(this);
}

void RollBackTool::setIcon(QAction* action)
{
	QIcon openIcon = loadSvg(QString::fromUtf8("://new//prefix1//icon//RollBack_f.svg"), QColor("#FFFFFF")); // ��ɫ����
	action->setIcon(openIcon);
}

QList<RollBackTool::ChangeRecord> RollBackTool::collectChanges(QSqlTableModel* model)
{
    QList<ChangeRecord> changes;

    // ��ȡ������������
    QString tableName = model->tableName();
    QString primaryKeyColumn;

    QSqlIndex primaryKey = model->primaryKey();

    // �����һ��������
    primaryKeyColumn = model->headerData(primaryKey.count(), Qt::Horizontal).toString();

    for (int row = 0; row < model->rowCount(); ++row) 
    {
        if (model->isDirty(model->index(row, 0))) 
        {
            ChangeRecord record;
            record.tableName = tableName;
            record.primaryKeyColumn = primaryKeyColumn;
            record.primaryKeyValue = model->data(model->index(row, 0));

            // �ռ������е�����
            for (int col = 0; col < model->columnCount(); ++col) 
            {
                QString columnName = model->headerData(col, Qt::Horizontal).toString();
                QVariant currentValue = model->data(model->index(row, col));
                QVariant originalValue = model->data(model->index(row, col), Qt::UserRole - 1); // ԭʼֵ

                record.newValues[columnName] = currentValue;
                record.oldValues[columnName] = originalValue;
            }

            changes.append(record);
        }
    }

    return changes;
}