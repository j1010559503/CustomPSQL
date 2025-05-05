#include "ConnectTool.h"

static bool registeredConnect = []()
{
	//创建工具类对象
	ToolFactory::registerTool("connect", []() { return std::make_shared<ConnectTool>("connect"); });
	return true;
}();

ConnectTool::ConnectTool(const QString& text) :ButtonTool(text)
{
	m_connectWidget = new ConnectWidget(GlobalManager::instance()->GetMainWindow());
}

ConnectTool::~ConnectTool()
{
}

void ConnectTool::handleEvent(QEvent* event)
{
	CustomEvent* _event = dynamic_cast<CustomEvent*>(event);
	if (_event)
	{
		QString ver = _event->message();
		QString senderName = _event->getSender().get()->getName();
		if (senderName == "DirectoryTree" && ver == "closeConnectWidget")
		{
			m_connectWidget->hide();
			m_connectWidget->reset();
		}
	}
}

void ConnectTool::clicked()
{
	m_connectWidget->show();
}

void ConnectTool::execute()
{
	//注册工具，用于事件收发
	GlobalManager::instance()->RegisterTool(shared_from_this());
	connect(m_connectWidget, &ConnectWidget::certainConnect,
		[this](QString strJson) {
			GlobalManager::instance()->sendEvent(shared_from_this(), new CustomEvent(strJson));
			m_connectWidget->hide();
		});
}

void ConnectTool::setIcon(QAction* action)
{
	QIcon openIcon = loadSvg(QString::fromUtf8("://new//prefix1//icon//connect.svg"), QColor("#FFFFFF")); // 白色背景
	action->setIcon(openIcon);
}

void ConnectTool::reset()
{

}

void ConnectTool::certainClicked(QString strjson)
{
	GlobalManager::instance()->sendEvent(shared_from_this(), new CustomEvent(strjson));
}

ConnectWidget::ConnectWidget(QWidget* parent)
	: BaseStyleWidget(parent)
{
	ui.setupUi(this);
	initUI();
}

ConnectWidget::~ConnectWidget()
{
}

void ConnectWidget::initUI()
{
	QHBoxLayout* layout = new QHBoxLayout();
	layout->setContentsMargins(1, 1, 1, 1);
	QPushButton* filedir = new QPushButton(this);
	filedir->setText("...");
	filedir->setMaximumWidth(30);
	filedir->setCursor(QCursor(Qt::ArrowCursor));
	//以右对齐的方式添加QWidget
	layout->addWidget(filedir, 0, Qt::AlignRight);
	ui.lineEdit_dbname->setLayout(layout);

	ui.lineEdit_username->setText("admin");
	ui.lineEdit_password->setText("admin");

	connect(ui.pushButton_certain, SIGNAL(clicked()), this, SLOT(certainClicked()));
	connect(ui.pushButton_cancel, SIGNAL(clicked()), this, SLOT(cancelClicked()));
	connect(filedir, SIGNAL(clicked()), this, SLOT(showDatabasePath()));
}

void ConnectWidget::reset()
{
	ui.lineEdit_dbname->setText("");
}

void ConnectWidget::cancelClicked()
{
	hide();
}

void ConnectWidget::showDatabasePath()
{
	QString dbfile = QFileDialog::getOpenFileName(NULL, "", ".", "*.db *.gpkg");
	ui.lineEdit_dbname->setText(dbfile);
}

void ConnectWidget::certainClicked()
{
	if (ui.lineEdit_dbname->text().isEmpty() || ui.lineEdit_username->text().isEmpty() ||
		ui.lineEdit_password->text().isEmpty())
	{
		return;
	}
	QJsonObject jObj;
	jObj.insert("dbname", ui.lineEdit_dbname->text());
	jObj.insert("username", ui.lineEdit_username->text());
	jObj.insert("password", ui.lineEdit_password->text());

	QJsonDocument document;
	document.setObject(jObj);
	QByteArray byteArray = document.toJson(QJsonDocument::Compact);
	QString strJson(byteArray);
	emit certainConnect(strJson);
}
