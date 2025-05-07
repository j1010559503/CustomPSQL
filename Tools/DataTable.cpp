#include "DataTable.h"

static bool registeredConnect = []()
{
	// 创建工具类对象
	ToolFactory::registerTool("DataTable", []() { return std::make_shared<DataTable>("DataTable"); });
	return true;
}();

DataTable::DataTable(const QString& name) :
	BaseTool(name)
{
    // 在应用程序开始时启用 SQL 调试
    qDebug() << "驱动程序支持的功能:" << GlobalManager::instance()->getActivedDatabase().driver()->hasFeature(QSqlDriver::Transactions);
    QLoggingCategory::setFilterRules("qt.sql.debug=true");
}

DataTable::~DataTable()
{
}

void DataTable::execute()
{
	// 注册工具，用于事件收发
	GlobalManager::instance()->RegisterTool(shared_from_this());

	m_dataTable = new QTableView(GlobalManager::instance()->GetMainWindow());
    // 设置水平表头的自动调整模式
    m_dataTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

	GlobalManager::instance()->GetMainWindow()->addDataTable(m_dataTable);
}

void DataTable::handleEvent(QEvent* event)
{
	CustomEvent* _event = dynamic_cast<CustomEvent*>(event);
	if (_event)
	{
        // 获取消息
        QString msg = _event->message();
		QString senderName = _event->getSender().get()->getName();
		if (senderName == "DirectoryTree" && msg == "selectedItem")
		{
            QStandardItem* extractedIndex = qvariant_cast<QStandardItem*>(_event->data()["item"]);
            // 点击item时设置当前显示表项
            GlobalManager::instance()->setActivedTbItem(extractedIndex);
            // 点击item更改时重新设置model
            m_dtModel = m_models[extractedIndex->data().toString()];
            m_dataTable->setModel(m_dtModel);
            showTable(extractedIndex);
		}
        else if (senderName == "DirectoryTree" && msg == "Connected")
        {
            // 连接成功后设置model,每一个QSqlTableModel对应一个数据库
            m_dtModel = new QSqlTableModel(m_dataTable, GlobalManager::instance()->getActivedDatabase());
            m_models[GlobalManager::instance()->getActivedConnectName()] = m_dtModel;
            // 编辑策略
            m_dataTable->setModel(m_dtModel);
            // 手动调用 submit() 时才提交
            m_dtModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
            connect(m_dtModel,&QSqlTableModel::dataChanged, [=](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles) {
                // 表单产生变化时记录变化
                GlobalManager::instance()->sendEvent(shared_from_this(), new CustomEvent("tableChanged"));
                //QMap<int,QVariant> data = m_dtModel->itemData(topLeft);
                //bool dirt = m_dtModel->isDirty(topLeft);
                //if (data[Qt::EditRole] == data[Qt::DisplayRole])
                //{
                //    m_dtModel->setData(topLeft, data[Qt::EditRole]);
                //}
                });
        }
        else if (senderName == "InsertColumn" && msg == "updateDataTable")
        {
            QStandardItem* extractedIndex = qvariant_cast<QStandardItem*>(_event->data()["tbname"]);
            showTable(extractedIndex);
            //m_dtModel->select();
        }
        else if (senderName == "AddRow" && msg == "updateDataTable")
        {
            QStandardItem* extractedIndex = qvariant_cast<QStandardItem*>(_event->data()["tbname"]);
            //showTable(extractedIndex);
            m_dtModel->select();
        }
        else if (senderName == "SubmitModify" && msg == "certainSubmit")
        {
            GlobalManager::instance()->getActivedDatabase().transaction();

            // 获取所有修改过的行和列
            QModelIndexList dirtyIndexes;
            for (int row = 0; row < m_dtModel->rowCount(); ++row) 
            {
                for (int col = 0; col < m_dtModel->columnCount(); ++col) 
                {
                    QModelIndex index = m_dtModel->index(row, col);
                    if (m_dtModel->isDirty(index)) 
                    {
                        dirtyIndexes.append(index);
                    }
                }
            }

            // 逐个提交修改过的单元格
            bool success = true;
            for (const QModelIndex& index : dirtyIndexes) 
            {
                QSqlRecord record = m_dtModel->record(index.row());
                QString fieldName = m_dtModel->headerData(index.column(), Qt::Horizontal).toString().split(" ").first();
                record.setValue(fieldName, m_dtModel->data(index, Qt::EditRole));
                if (!m_dtModel->setRecord(index.row(), record)) 
                {
                    success = false;
                    break;
                }
            }

            if (success && m_dtModel->submitAll()) 
            {
                GlobalManager::instance()->getActivedDatabase().commit();
            }
        }
	}
}

void DataTable::showTable(QStandardItem* item)
{
    m_dtModel->clear();
    QStandardItem* _item = item;
    m_selectedTableName = _item->text();
    // 设置表名
    m_dtModel->setTable(m_selectedTableName);

    // 创建一个映射，将字段名与类型关联起来
    QMap<QString, QString> fieldTypes;
    QSqlQuery query(GlobalManager::instance()->getDatabase(_item->data().toString()));
    query.exec(QString("PRAGMA table_info (%1)").arg(m_selectedTableName));
    if (query.exec())
    {
        while (query.next()) 
        {
            QString fieldName = query.value("name").toString();
            QString fieldType = query.value("type").toString();
            fieldTypes.insert(fieldName, fieldType);
        }
    }

    // 获取表格的记录信息
    QSqlRecord record = m_dtModel->record();
    for (int i = 0; i < record.count(); ++i)
    {
        QSqlField field = record.field(i);
        QString fieldName = field.name();
        QString fieldType = fieldTypes[fieldName];// 获取字段类型的名称
        // 设置表头为 "字段名 (类型)"
        m_dtModel->setHeaderData(i, Qt::Horizontal, QString("%1 (%2)").arg(fieldName).arg(fieldType));
    }

    m_dtModel->select();

    //if (!_item->parent())
    //{
    //    return;
    //}
    //QSqlQuery query(GlobalManager::instance()->getDatabase(_item->data().toString()));
    //QString qStr = "SELECT * FROM " + _item->text();
    //query.prepare(qStr);
    //if (query.exec())
    //{
    //    int i = 0;
    //    int indexRow = 0;
    //    int indexColumn = 0;
    //    int secondRowIndex = 2;
    //    int max_typeCount = 0;
    //    bool second_start = false;
    //    QSet<QString> typeSet;
    //    bool first_record = true;
    //    while (query.next())
    //    {
    //        if (first_record)
    //        {
    //            for (int i = 0; i < query.record().count(); ++i)
    //            {
    //                //auto type = query.record().field(i).type();
    //                QString name = query.record().fieldName(i);
    //                QString value = query.record().value(i).toString();
    //                value = value.isEmpty() ? "(Null)" : value;
    //                if (!typeSet.contains(name))
    //                {
    //                    typeSet.insert(name);
    //                    QStandardItem* typeItem = new QStandardItem(name);
    //                    m_dtModel->setItem(0, i, typeItem);
    //                    QStandardItem* valueItem = new QStandardItem(value);
    //                    m_dtModel->setItem(1, i, valueItem);
    //                    m_dataTable->scrollTo(typeItem->index(), QAbstractItemView::PositionAtTop);
    //                }
    //            }
    //            m_numSelectTableField = query.record().count();
    //            first_record = false;
    //            indexRow = 2;
    //            m_dataTable->setItemDelegateForRow(0, new ReadOnlyItemDelegate());
    //        }
    //        else
    //        {
    //            for (int i = 0; i < query.record().count(); ++i)
    //            {
    //                //auto type = query.record().field(i).type();
    //                QString name = query.record().fieldName(i);
    //                QString value = query.record().value(i).toString();
    //                value = value.isEmpty() ? "(Null)" : value;
    //                if (typeSet.contains(name))
    //                {
    //                    QStandardItem* valueItem = new QStandardItem(value);
    //                    m_dtModel->setItem(indexRow, i, valueItem);
    //                }
    //            }
    //            ++indexRow;
    //            m_initTable = true;
    //        }
    //    }
    //}
    //m_dataTable->resizeColumnsToContents();
    //m_dtItemTextChanged = true;
}

ReadOnlyItemDelegate::ReadOnlyItemDelegate()
{
}

ReadOnlyItemDelegate::~ReadOnlyItemDelegate()
{
}

QWidget* ReadOnlyItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return nullptr;
}

