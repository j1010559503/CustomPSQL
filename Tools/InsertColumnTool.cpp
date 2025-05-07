#include "InsertColumnTool.h"

static bool registeredConnect = []()
{
	//创建工具类对象
	ToolFactory::registerTool("InsertColumn", []() { return std::make_shared<InsertColumnTool>("InsertColumn"); });
	return true;
}();

InsertColumnTool::InsertColumnTool(const QString& text) :ButtonTool(text)
{
	m_widget = new InsertColumnWidget(GlobalManager::instance()->GetMainWindow());
}

InsertColumnTool::~InsertColumnTool()
{
}

void InsertColumnTool::handleEvent(QEvent* event)
{
	CustomEvent* _event = dynamic_cast<CustomEvent*>(event);
	if (_event)
	{
		// 获取消息
		QString msg = _event->message();
		QString senderName = _event->getSender().get()->getName();
		if (senderName == "DirectoryTree" && msg == "selectedItem")
		{
			// 当前选中表项
			QStandardItem* extractedIndex = qvariant_cast<QStandardItem*>(_event->data()["item"]);
			m_widget->setTbName(extractedIndex->text());
			m_curTbName = extractedIndex->text();
		}
	}
}

void InsertColumnTool::clicked()
{
	if (!GlobalManager::instance()->getActivedDatabase().isValid())
	{
		QMessageBox::information(m_widget, tr("Tips"), tr("NoConncet"), QMessageBox::Ok);
		return;
	}
	if (m_curTbName.isEmpty())
	{
		QMessageBox::information(m_widget, tr("Tips"), tr("NoChooseTable"), QMessageBox::Ok);
		return;
	}
	else
	{
		m_widget->reset();
		m_widget->setWindowModality(Qt::ApplicationModal); // 设置为应用程序模态
		m_widget->show();
	}
}

void InsertColumnTool::execute()
{
	//注册工具，用于事件收发
	GlobalManager::instance()->RegisterTool(shared_from_this());
	connect(m_widget, &InsertColumnWidget::sendCertainClicked,
		[&]() {
			QVariantMap param{ { "tbname", QVariant::fromValue(GlobalManager::instance()->getActivedTbItem())} };
			GlobalManager::instance()->sendEvent(shared_from_this(), new CustomEvent("updateDataTable", param));
		});
}

void InsertColumnTool::setIcon(QAction* action)
{
	QIcon openIcon = loadSvg(QString::fromUtf8("://new//prefix1//icon//InsertColumn.svg"), QColor("#FFFFFF")); // 白色背景
	action->setIcon(openIcon);
}

InsertColumnWidget::InsertColumnWidget(QWidget* parent)
{
	initUI();
}

InsertColumnWidget::~InsertColumnWidget()
{
}

void InsertColumnWidget::initUI()
{
	ui.setupUi(this);
	ui.label_colName->setText(tr("columnName:"));
	ui.label_defaultVal->setText(tr("defaultvalue:"));
	ui.label_tbName->setText(tr("tableName:"));
	ui.label_constraint->setText(tr("constraint:"));
	ui.label_type->setText(tr("type:"));
	ui.pushButton_certain->setText(tr("certain"));
	ui.pushButton_cancel->setText(tr("cancel"));

	ui.lineEdit_colName->setValidator(new SqlColumnNameValidator());
	ui.lineEdit_tbName->setText(m_curTbName);

	ui.comboBox_type->addItems({ "INTEGER", "TEXT", "REAL", "BLOB" });
	ui.comboBox_constraint->addItems({ "NOT NULL", "NULL" });

	connect(ui.pushButton_cancel, &QPushButton::clicked, this, [this]() {this->hide(); });
	connect(ui.pushButton_certain, &QPushButton::clicked, this, &InsertColumnWidget::certainClicked);
	connect(ui.comboBox_constraint, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [&](int idx) {
		if (idx == 1)
			ui.lineEdit_defaultVal->setDisabled(true);
		else 
			ui.lineEdit_defaultVal->setDisabled(false);
		});
}

void InsertColumnWidget::setTbName(const QString& name)
{
	m_curTbName = name;
	ui.lineEdit_tbName->setDisabled(true);
	ui.lineEdit_tbName->setText(m_curTbName);
}

void InsertColumnWidget::reset()
{
	ui.lineEdit_tbName->setText(m_curTbName);

	ui.lineEdit_defaultVal->setText("");
	ui.lineEdit_colName->setText("");

	ui.comboBox_type->setCurrentIndex(0);
	ui.comboBox_constraint->setCurrentIndex(0);

	ui.lineEdit_defaultVal->setDisabled(false);
}

void InsertColumnWidget::certainClicked()
{
	QString tableName = ui.lineEdit_tbName->text();
	QString columnName = ui.lineEdit_colName->text();
	QString columnType = ui.comboBox_type->currentText();
	QString constraints = ui.comboBox_constraint->currentText();
	QString defaultValue = ui.lineEdit_defaultVal->text();

	if (constraints == "NOT NULL" && defaultValue.isEmpty())
	{
		QMessageBox::information(GlobalManager::instance()->GetMainWindow(), tr("Tips"), tr("defaultValueEmpty"), QMessageBox::Ok);
	}
	else
	{
		QString sql = QString("ALTER TABLE %1 ADD COLUMN %2 %3 %4").arg(tableName).arg(columnName).arg(columnType).arg(constraints);
		if (!defaultValue.isEmpty())
		{
			sql += " DEFAULT " + defaultValue;
		}

		QSqlQuery query(GlobalManager::instance()->getActivedDatabase());
		if (!query.exec(sql))
		{
			QMessageBox::critical(this, "Error", "Failed to add column: " + query.lastError().text());
		}
		else {
			QMessageBox::information(this, "Success", "Column added successfully.");
			emit sendCertainClicked();
		}
		this->hide();
	}
}
