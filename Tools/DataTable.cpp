#include "DataTable.h"

static bool registeredConnect = []()
{
	// �������������
	ToolFactory::registerTool("DataTable", []() { return std::make_shared<DataTable>("DataTable"); });
	return true;
}();

DataTable::DataTable(const QString& name) :
	BaseTool(name)
{
    // ��Ӧ�ó���ʼʱ���� SQL ����
    qDebug() << "��������֧�ֵĹ���:" << GlobalManager::instance()->getActivedDatabase().driver()->hasFeature(QSqlDriver::Transactions);
    QLoggingCategory::setFilterRules("qt.sql.debug=true");
}

DataTable::~DataTable()
{
}

void DataTable::execute()
{
	// ע�Ṥ�ߣ������¼��շ�
	GlobalManager::instance()->RegisterTool(shared_from_this());

	m_dataTable = new QTableView(GlobalManager::instance()->GetMainWindow());
    // ����ˮƽ��ͷ���Զ�����ģʽ
    m_dataTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

	GlobalManager::instance()->GetMainWindow()->addDataTable(m_dataTable);
}

void DataTable::handleEvent(QEvent* event)
{
	CustomEvent* _event = dynamic_cast<CustomEvent*>(event);
	if (_event)
	{
        // ��ȡ��Ϣ
        QString msg = _event->message();
		QString senderName = _event->getSender().get()->getName();
		if (senderName == "DirectoryTree" && msg == "selectedItem")
		{
            QStandardItem* extractedIndex = qvariant_cast<QStandardItem*>(_event->data()["item"]);
            // ���itemʱ���õ�ǰ��ʾ����
            GlobalManager::instance()->setActivedTbItem(extractedIndex);
            // ���item����ʱ��������model
            m_dtModel = m_models[extractedIndex->data().toString()];
            m_dataTable->setModel(m_dtModel);
            showTable(extractedIndex);
		}
        else if (senderName == "DirectoryTree" && msg == "Connected")
        {
            // ���ӳɹ�������model,ÿһ��QSqlTableModel��Ӧһ�����ݿ�
            m_dtModel = new QSqlTableModel(m_dataTable, GlobalManager::instance()->getActivedDatabase());
            m_models[GlobalManager::instance()->getActivedConnectName()] = m_dtModel;
            // �༭����
            m_dataTable->setModel(m_dtModel);
            // �ֶ����� submit() ʱ���ύ
            m_dtModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
            connect(m_dtModel,&QSqlTableModel::dataChanged, [=](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles) {
                // �������仯ʱ��¼�仯
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

            // ��ȡ�����޸Ĺ����к���
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

            // ����ύ�޸Ĺ��ĵ�Ԫ��
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
    // ���ñ���
    m_dtModel->setTable(m_selectedTableName);

    // ����һ��ӳ�䣬���ֶ��������͹�������
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

    // ��ȡ���ļ�¼��Ϣ
    QSqlRecord record = m_dtModel->record();
    for (int i = 0; i < record.count(); ++i)
    {
        QSqlField field = record.field(i);
        QString fieldName = field.name();
        QString fieldType = fieldTypes[fieldName];// ��ȡ�ֶ����͵�����
        // ���ñ�ͷΪ "�ֶ��� (����)"
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

