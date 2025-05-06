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
}

DataTable::~DataTable()
{
}

void DataTable::execute()
{
	// ע�Ṥ�ߣ������¼��շ�
	GlobalManager::instance()->RegisterTool(shared_from_this());

	m_dataTable = new QTableView(GlobalManager::instance()->GetMainWindow());
	// �Ҳ��������
	m_dataTable->horizontalHeader()->hide();
	m_dataTable->verticalHeader()->hide();
	m_dataTable->verticalHeader()->setDefaultSectionSize(15);

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
            QStandardItem* extractedIndex = (QStandardItem*)_event->data()[0];
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
            m_dtModel->setEditStrategy(QSqlTableModel::OnManualSubmit);}
	}
}

void DataTable::showTable(QStandardItem* item)
{
    m_dtItemTextChanged = false;
    m_dtModel->clear();
    m_topHighlightItem = nullptr;
    m_leftHighlightItem = nullptr;
    QStandardItem* _item = item;
    m_selectedTableName = _item->text();
    // ���ñ���
    m_dtModel->setTable(m_selectedTableName);
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

