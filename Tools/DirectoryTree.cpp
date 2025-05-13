#include "DirectoryTree.h"

static bool registeredConnect = []()
{
	// �������������
	ToolFactory::registerTool("DirectoryTree", []() { return std::make_shared<DirectoryTree>("DirectoryTree"); });
	return true;
}();

// ���ģ�����Ƿ������߼�Ϊ�ض��ַ�������
bool containsTopLevelItemWithText(QStandardItemModel* model, const QString& text) 
{
    int rowCount = model->rowCount();
    for (int i = 0; i < rowCount; ++i) 
    {
        QStandardItem* item = model->item(i);
        if (item && item->text() == text) 
        {
            return true;
        }
    }
    return false;
}

DirectoryTree::DirectoryTree(const QString& name) :BaseTool(name)
{
}

DirectoryTree::~DirectoryTree()
{
    m_directoryTree = nullptr;
    m_dtModel = nullptr;
}

void DirectoryTree::execute()
{
	// ע�Ṥ�ߣ������¼��շ�
	GlobalManager::instance().RegisterTool(this);
	m_directoryTree = new CustomTreeView(GlobalManager::instance().GetMainWindow());

    m_directoryTree->header()->setVisible(false);
    m_directoryTree->setIndentation(13);
    // ����˫���༭
    m_directoryTree->setEditTriggers(m_directoryTree->editTriggers() & ~QAbstractItemView::DoubleClicked);
    //һ��ѡ������
    m_directoryTree->setSelectionBehavior(QTreeView::SelectRows);
    //��ѡ�������������о���һ��ѡ����
    m_directoryTree->setSelectionMode(QTreeView::SingleSelection);

    // ���ø��ӹ�ϵ����֤�ڴ���ȷ�ͷ�
    m_dtModel = new QStandardItemModel(m_directoryTree);
    m_dtModel->setColumnCount(1);
    m_directoryTree->setModel(m_dtModel);
	GlobalManager::instance().GetMainWindow()->addDirectoryTree(m_directoryTree);

    //connect(m_directoryTree, SIGNAL(doubleClicked(const QModelIndex)), this, SLOT(showTable(const QModelIndex)));
    connect(m_directoryTree, SIGNAL(clicked(const QModelIndex)), this, SLOT(showTable(const QModelIndex)));
}

void DirectoryTree::handleEvent(QEvent* event)
{
	CustomEvent* _event = dynamic_cast<CustomEvent*>(event);
	if (_event)
	{
        QString ver = _event->message();
		QString senderName = _event->getSender()->getName();
		if (senderName == "connect")
		{
			connectDatabase(ver);
        }
        else if (senderName == "AddTable")
        {
            addDataTable(ver);
        }
	}
}

void DirectoryTree::connectDatabase(QString message)
{
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    QJsonObject jobj = doc.object();

    QString dbname = jobj.value("dbname").toString();
    QFileInfo fileInfo = QFileInfo(dbname);
    QString strippedName = fileInfo.baseName();

    QString connName;
    if (GlobalManager::instance().addDatabase("QSQLITE", dbname, connName) && !containsTopLevelItemWithText(m_dtModel, strippedName))
    {
        QStandardItem* database = new QStandardItem(strippedName);
        database->setData(connName);
        m_dtModel->appendRow(database);
        QStringList list = GlobalManager::instance().getDatabase(connName).tables();
        for (int i = 0; i < list.size(); i++)
        {
            QStandardItem* table = new QStandardItem(list[i]);
            //Ϊÿ������������������Ա��ں����������
            table->setData(connName);
            database->appendRow(table);
        }
        m_directoryTree->expand(database->index());
        // ���ü������ݿ�����
        GlobalManager::instance().setActivedConnectName(connName);
        GlobalManager::instance().sendEvent(this, new CustomEvent("Connected"));
    }
}

void DirectoryTree::addDataTable(QString sqlStr)
{
    QJsonDocument doc = QJsonDocument::fromJson(sqlStr.toUtf8());
    QJsonObject jobj = doc.object();
    QString tablename = jobj.value("tablename").toString();
    QString primkey = jobj.value("primkey").toString();
    QString primkeytype = jobj.value("primkeytype").toString();

    QSqlQuery query(GlobalManager::instance().getDatabase(GlobalManager::instance().getActivedConnectName()));
    QString creatTableStr = "CREATE TABLE " + tablename + " (" + primkey + " " + primkeytype + ")";

    query.prepare(creatTableStr);

    if (query.exec())
    {
        for (int i = 0; i < m_dtModel->rowCount(); ++i)
        {
            QStandardItem* item = m_dtModel->item(i, 0);

            QString connName = GlobalManager::instance().getActivedConnectName();

            QFileInfo fileInfo(GlobalManager::instance().getDatabase(connName).databaseName());
            // ��ȡ����ļ�����������׺��
            QString currDbName = fileInfo.baseName();

            if (item->text() == currDbName)
            {
                QStandardItem* table = new QStandardItem();
                table->setText(tablename);
                // �������ʱЯ��������
                table->setData(connName);
                item->appendRow(table);
                m_directoryTree->expand(item->index());
                break;
            }
        }
    }
}

void DirectoryTree::showTable(const QModelIndex index)
{
    QStandardItem* item = m_dtModel->itemFromIndex(index);
    QVariantMap param{ { "item", QVariant::fromValue(item) } };
    GlobalManager::instance().sendEvent(this, new CustomEvent("selectedItem", param));
    GlobalManager::instance().setActivedConnectName(item->data().toString());
    GlobalManager::instance().setActivedTbItem(item);
}
