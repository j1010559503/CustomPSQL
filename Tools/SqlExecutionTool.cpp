#include "SqlExecutionTool.h"

static bool registeredConnect = []()
{
	//�������������
	ToolFactory::registerTool("SqlExecution", []() { return std::make_shared<SqlExecutionTool>("SqlExecution"); });
	return true;
}();

SqlExecutionTool::SqlExecutionTool(const QString& text) :ButtonTool(text)
{
    // ����SQLִ�д���
    m_sqlExecutionWidget = new SqlExecutionWidget();

    // ����ͣ������
    m_dockWidget = new QDockWidget(tr("SQL EXCUTE"), GlobalManager::instance().GetMainWindow());
    m_dockWidget->setWidget(m_sqlExecutionWidget);
    m_dockWidget->setAllowedAreas(Qt::RightDockWidgetArea);
    m_dockWidget->setFeatures(QDockWidget::DockWidgetClosable);

    // ��ӵ��������Ҳ�
    GlobalManager::instance().GetMainWindow()->addDockWidget(Qt::RightDockWidgetArea, m_dockWidget);
}

SqlExecutionTool::~SqlExecutionTool()
{
    m_dockWidget = nullptr;
    m_sqlExecutionWidget = nullptr;
}

void SqlExecutionTool::handleEvent(QEvent* event)
{
    CustomEvent* _event = dynamic_cast<CustomEvent*>(event);
    if (_event)
    {
        QString ver = _event->message();
        QString senderName = _event->getSender()->getName();
        if (senderName == "connect" || (senderName == "DirectoryTree" && ver == "selectedItem"))
        {
            m_sqlExecutionWidget->setDatabase(GlobalManager::instance().getActivedDatabase());
        }
    }
}

void SqlExecutionTool::clicked()
{
    if (m_isVisible)
    {
        m_dockWidget->hide();
        m_isVisible = false;
    }
    else
    {
        m_dockWidget->show();
        m_isVisible = true;
    }
}

void SqlExecutionTool::execute()
{
    //ע�Ṥ�ߣ������¼��շ�
    GlobalManager::instance().RegisterTool(this);
}

void SqlExecutionTool::setIcon(QAction* action)
{
	QIcon openIcon = loadSvg(QString::fromUtf8("://new//prefix1//icon//SqlExecution.svg"), QColor("#FFFFFF")); // ��ɫ����
	action->setIcon(openIcon);
}

// SQLִ���߳�ʵ��
SqlExecutionThread::SqlExecutionThread(QObject* parent)
    : QObject(parent)
{
}

void SqlExecutionThread::setQuery(const QString& query, const QSqlDatabase& db)
{
    m_query = query;
    m_db = QSqlDatabase::cloneDatabase(db, "SqlExecutionThread_" + QString::number(reinterpret_cast<quint64>(QThread::currentThreadId())));
}

void SqlExecutionThread::execute()
{
    if (!m_db.open()) 
    {
        emit error("Open DB Failed: " + m_db.lastError().text());
        emit finished();
        QString ju = m_db.driverName();
        QStringList ss = QSqlDatabase::drivers();
        return;
    }
    // ��ʱ�����������ִ�к�ʱ
    QElapsedTimer timer;
    timer.start();

    QSqlQuery query(m_db);
    if (!query.exec(m_query)) 
    {
        emit error("SQL excute error: " + query.lastError().text());
        m_db.close();
        emit finished();
        return;
    }

    // ��������ַ���
    QString result;

    // �����SELECT��ѯ����ʾ�����
    if (m_query.trimmed().toLower().startsWith("select")) 
    {
        int rowCount = 0;
        QStringList headers;
        QStringList rows;

        // ��ȡ����
        QSqlRecord record = query.record();
        for (int i = 0; i < record.count(); i++) 
        {
            headers << record.fieldName(i);
        }

        // ��ȡ������
        while (query.next()) 
        {
            rowCount++;
            QStringList fields;
            for (int i = 0; i < record.count(); i++) 
            {
                fields << query.value(i).toString();
            }
            rows << fields.join("\t");
        }

        result = tr("select excute success") + "\n";
        result += tr("return") + QString::number(rowCount) + tr(" row") + "\n";
        result += tr("excute time: ") + QString::number(timer.elapsed()) + " ms\n\n";

        // ��ӱ�ͷ
        result += headers.join("\t") + "\n";
        result += QString("-").repeated(result.length()) + "\n";

        // ���������
        result += rows.join("\n");
    }
    else 
    {
        // ���ڷ�SELECT��ѯ����ʾ��Ӱ�������
        result = tr("select excute success") + "\n";
        result += tr("influenced row") + QString::number(query.numRowsAffected()) + "\n";
        result += tr("excute time: ") + QString::number(timer.elapsed()) + " ms";
    }

    m_db.close();
    emit resultReady(result);
    emit finished();
}

// SQLִ�д���ʵ��
SqlExecutionWidget::SqlExecutionWidget(QWidget* parent)
    : QWidget(parent)
    , m_isExecuting(false)
{
    // ����UI���
    m_sqlEditor = new QTextEdit(this);
    m_sqlEditor->setPlaceholderText(tr("input SQL here..."));
    m_sqlEditor->setMinimumHeight(100);

    m_resultDisplay = new QTextEdit(this);
    m_resultDisplay->setReadOnly(true);
    m_resultDisplay->setFont(QFont("Courier New", 10));

    m_executeButton = new QPushButton(tr("excute"), this);
    m_statusLabel = new QLabel(tr("ready"), this);

    // ��������
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(m_executeButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_statusLabel);

    QSplitter* splitter = new QSplitter(Qt::Vertical, this);
    splitter->addWidget(m_sqlEditor);
    splitter->addWidget(m_resultDisplay);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 3);

    mainLayout->addWidget(splitter);
    mainLayout->addLayout(buttonLayout);

    // ���������߳�
    m_worker = new SqlExecutionThread();
    m_worker->moveToThread(&m_workerThread);

    // �����źźͲ�
    connect(m_executeButton, &QPushButton::clicked, this, &SqlExecutionWidget::executeQuery);
    connect(m_worker, &SqlExecutionThread::resultReady, this, &SqlExecutionWidget::displayResult);
    connect(m_worker, &SqlExecutionThread::error, this, &SqlExecutionWidget::displayError);
    connect(m_worker, &SqlExecutionThread::finished, this, &SqlExecutionWidget::queryFinished);
    connect(&m_workerThread, &QThread::finished, m_worker, &QObject::deleteLater);

    // ���������߳�
    m_workerThread.start();
}

SqlExecutionWidget::~SqlExecutionWidget()
{
    m_sqlEditor = nullptr;
    m_resultDisplay = nullptr;
    m_executeButton = nullptr;
    m_layout = nullptr;
    m_statusLabel = nullptr;
    m_worker = nullptr;

    m_workerThread.quit();
    m_workerThread.wait();
}

void SqlExecutionWidget::setDatabase(const QSqlDatabase& db)
{
    m_db = db;
}

void SqlExecutionWidget::executeQuery()
{
    if (m_isExecuting) 
    {
        return;
    }

    QString sql = m_sqlEditor->toPlainText().trimmed();
    if (sql.isEmpty()) 
    {
        m_resultDisplay->setText(tr("Please input SQL statement"));
        return;
    }

    m_isExecuting = true;
    m_executeButton->setEnabled(false);
    m_statusLabel->setText(tr("excuting..."));
    m_resultDisplay->clear();

    // ׼����ִ�в�ѯ
    m_worker->setQuery(sql, m_db);
    QMetaObject::invokeMethod(m_worker, "execute", Qt::QueuedConnection);
}

void SqlExecutionWidget::displayResult(const QString& result)
{
    m_resultDisplay->setText(result);
}

void SqlExecutionWidget::displayError(const QString& errorMessage)
{
    m_resultDisplay->setText(tr("error: ") + errorMessage);
}

void SqlExecutionWidget::queryFinished()
{
    m_isExecuting = false;
    m_executeButton->setEnabled(true);
    m_statusLabel->setText(tr("ready"));
}