#include "AddRowTool.h"

static bool registeredConnect = []()
{
	//创建工具类对象
	ToolFactory::registerTool("AddRow", []() { return std::make_shared<AddRowTool>("AddRow"); });
	return true;
}();

AddRowTool::AddRowTool(const QString& text) :ButtonTool(text)
{
}

AddRowTool::~AddRowTool()
{
}

void AddRowTool::handleEvent(QEvent* event)
{
}

void AddRowTool::clicked()
{
	if (!GlobalManager::instance()->getActivedDatabase().isValid())
	{
		QMessageBox::information(GlobalManager::instance()->GetMainWindow(), tr("Tips"), tr("NoConncet"), QMessageBox::Ok);
		return;
	}
	else if (!GlobalManager::instance()->getActivedTbItem())
	{
		QMessageBox::information(GlobalManager::instance()->GetMainWindow(), tr("Tips"), tr("NoChooseTable"), QMessageBox::Ok);
		return;
	}
	else
	{
		QString sql = QString("INSERT INTO %1 DEFAULT VALUES").arg(GlobalManager::instance()->getActivedTbItem()->text());
		QSqlQuery query(GlobalManager::instance()->getActivedDatabase());
		if (!query.exec(sql))
		{
			qDebug() << "Append row failed";

		}
		else
		{
			QVariantMap param{ { "tbname", QVariant::fromValue(GlobalManager::instance()->getActivedTbItem())} };
			GlobalManager::instance()->sendEvent(shared_from_this(), new CustomEvent("updateDataTable", param));
		}
	}
}

void AddRowTool::execute()
{
	//注册工具，用于事件收发
	GlobalManager::instance()->RegisterTool(shared_from_this());
}

void AddRowTool::setIcon(QAction* action)
{
	QIcon openIcon = loadSvg(QString::fromUtf8("://new//prefix1//icon//AddRow.svg"), QColor("#FFFFFF")); // 白色背景
	action->setIcon(openIcon);
}
