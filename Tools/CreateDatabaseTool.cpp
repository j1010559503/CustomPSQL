#include "CreateDatabaseTool.h"

static bool registeredConnect = []()
{
	//创建工具类对象
	ToolFactory::registerTool("CreateDatabase", []() { return std::make_shared<CreateDatabaseTool>("CreateDatabase"); });
	return true;
}();

CreateDatabaseTool::CreateDatabaseTool(const QString& text) :ButtonTool(text)
{
	m_createDbWidget = new CreateDbWidget(GlobalManager::instance()->GetMainWindow());
}

CreateDatabaseTool::~CreateDatabaseTool()
{
}

void CreateDatabaseTool::handleEvent(QEvent* event)
{
}

void CreateDatabaseTool::clicked()
{
	m_createDbWidget->show();
}

void CreateDatabaseTool::execute()
{
	//注册工具，用于事件收发
	GlobalManager::instance()->RegisterTool(shared_from_this());
	connect(m_createDbWidget, &CreateDbWidget::certainCreate,
		[this](QString strJson) {
			GlobalManager::instance()->sendEvent(shared_from_this(), new CustomEvent(strJson));
			m_createDbWidget->hide();
		});
}

void CreateDatabaseTool::setIcon(QAction* action)
{
	QIcon openIcon = loadSvg(QString::fromUtf8("://new//prefix1//icon//CreateDatabase.svg"), QColor("#FFFFFF")); // 白色背景
	action->setIcon(openIcon);
}

CreateDbWidget::CreateDbWidget(QWidget* parent)
{
	initUI();
}

CreateDbWidget::~CreateDbWidget()
{
}

void CreateDbWidget::initUI()
{
	ui.setupUi(this);

	QHBoxLayout* layout = new QHBoxLayout();
	layout->setContentsMargins(1, 1, 1, 1);
	QPushButton* filedir = new QPushButton();
	filedir->setText("...");
	filedir->setMaximumWidth(30);
	filedir->setCursor(QCursor(Qt::ArrowCursor));
	//以右对齐的方式添加QWidget
	layout->addWidget(filedir, 0, Qt::AlignRight);
	ui.lineEdit_dbdir->setLayout(layout);

	connect(ui.pushButton_certain, SIGNAL(clicked()), this, SLOT(sendMessage()));
	connect(ui.pushButton_cancel, SIGNAL(clicked()), this, SLOT(hide()));
	connect(filedir, SIGNAL(clicked()), this, SLOT(showFileDialog()));
}

void CreateDbWidget::sendMessage()
{
	if (ui.lineEdit_dbname->text().isEmpty() || ui.lineEdit_dbdir->text().isEmpty() || ui.lineEdit_username->text().isEmpty() ||
		ui.lineEdit_password->text().isEmpty())
	{
		return;
	}
	QJsonObject jObj;
	jObj.insert("dbname", ui.lineEdit_dbname->text());
	jObj.insert("choosedir", ui.lineEdit_dbdir->text());
	jObj.insert("username", ui.lineEdit_username->text());
	jObj.insert("password", ui.lineEdit_password->text());

	QJsonDocument document;
	document.setObject(jObj);
	QByteArray byteArray = document.toJson(QJsonDocument::Compact);
	QString strJson(byteArray);
	emit certainCreate(strJson);
}

void CreateDbWidget::showFileDialog()
{
	QString dir = QFileDialog::getExistingDirectory();
	ui.lineEdit_dbdir->setText(dir);
}
