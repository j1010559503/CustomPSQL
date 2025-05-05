#include "AddTableTool.h"

static bool registeredConnect = []()
{
	//�������������
	ToolFactory::registerTool("AddTable", []() { return std::make_shared<AddTableTool>("AddTable"); });
	return true;
}();

AddTableTool::AddTableTool(const QString& text) :ButtonTool(text)
{
	m_addTableWidget = new AddTableWidget(GlobalManager::instance()->GetMainWindow());
}

AddTableTool::~AddTableTool()
{
}

void AddTableTool::handleEvent(QEvent* event)
{
}

void AddTableTool::clicked()
{
	m_addTableWidget->show();
}

void AddTableTool::execute()
{
	//ע�Ṥ�ߣ������¼��շ�
	GlobalManager::instance()->RegisterTool(shared_from_this());
	connect(m_addTableWidget, &AddTableWidget::certainAddTable,
		[this](QString strJson) {
			GlobalManager::instance()->sendEvent(shared_from_this(), new CustomEvent(strJson));
			m_addTableWidget->hide();
		});
}

void AddTableTool::setIcon(QAction* action)
{
	QIcon openIcon = loadSvg(QString::fromUtf8("://new//prefix1//icon//AddTable.svg"), QColor("#FFFFFF")); // ��ɫ����
	action->setIcon(openIcon);
}

AddTableWidget::AddTableWidget(QWidget* parent)
{
	initUI();
}

AddTableWidget::~AddTableWidget()
{
}

void AddTableWidget::initUI()
{
	ui.setupUi(this);

	// ��� SQLite ֧�ֵ���������
	m_sqliteDataTypes << "INTEGER" << "REAL" << "TEXT" << "BLOB" << "NULL";
	// Ĭ��ѡ��int
	ui.lineEdit_primkey->setValidator(new SqlColumnNameValidator());

	// ������������ӵ� QComboBox
	ui.comboBox_keyTypes->addItems(m_sqliteDataTypes);

	//connect(ui.comboBox_keyTypes, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AddTableWidget::onTypeChanged);

	connect(ui.pushButton_certain, SIGNAL(clicked()), this, SLOT(certainClicked()));
	connect(ui.pushButton_cancel, &QPushButton::clicked, this, [&]() {hide(); });
}

//void AddTableWidget::onTypeChanged(int index)
//{
//	const QString currentType = m_sqliteDataTypes[index];
//	if (currentType == "INTEGER") 
//	{
//		ui.lineEdit_value->setValidator(new QIntValidator());
//	}
//	else if (currentType == "REAL") 
//	{
//		ui.lineEdit_value->setValidator(new QDoubleValidator());
//	}
//	else 
//	{
//		ui.lineEdit_value->setValidator(nullptr);
//	}
//}

void AddTableWidget::certainClicked()
{
	if (!ui.lineEdit_tablename->text().isEmpty() && !ui.lineEdit_primkey->text().isEmpty())
	{
		QJsonObject jObj;
		jObj.insert("tablename", ui.lineEdit_tablename->text());
		jObj.insert("primkey", ui.lineEdit_primkey->text());
		jObj.insert("primkeytype", ui.comboBox_keyTypes->currentText());

		QJsonDocument document;
		document.setObject(jObj);
		QByteArray byteArray = document.toJson(QJsonDocument::Compact);
		QString strJson(byteArray);

		emit certainAddTable(strJson);
	}
}
