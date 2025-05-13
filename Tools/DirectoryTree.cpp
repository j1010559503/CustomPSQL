#include "DirectoryTree.h"

static bool registeredConnect = []()
{
	// 创建工具类对象
	ToolFactory::registerTool("DirectoryTree", []() { return std::make_shared<DirectoryTree>("DirectoryTree"); });
	return true;
}();

// 检查模型中是否存在最高级为特定字符串的项
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
	// 注册工具，用于事件收发
	GlobalManager::instance().RegisterTool(this);
	m_directoryTree = new CustomTreeView(GlobalManager::instance().GetMainWindow());

    m_directoryTree->header()->setVisible(false);
    m_directoryTree->setIndentation(13);
    // 禁用双击编辑
    m_directoryTree->setEditTriggers(m_directoryTree->editTriggers() & ~QAbstractItemView::DoubleClicked);
    //一次选中整行
    m_directoryTree->setSelectionBehavior(QTreeView::SelectRows);
    //单选，配合上面的整行就是一次选单行
    m_directoryTree->setSelectionMode(QTreeView::SingleSelection);

    // 设置父子关系，保证内存正确释放
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
            //为每个表项添加连接名，以便于后续表项操作
            table->setData(connName);
            database->appendRow(table);
        }
        m_directoryTree->expand(database->index());
        // 设置激活数据库连接
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
            // 获取最后文件名（包括后缀）
            QString currDbName = fileInfo.baseName();

            if (item->text() == currDbName)
            {
                QStandardItem* table = new QStandardItem();
                table->setText(tablename);
                // 添加新项时携带连接名
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
