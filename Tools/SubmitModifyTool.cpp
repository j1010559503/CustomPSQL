#include "SubmitModifyTool.h"

static bool registeredConnect = []()
{
	//创建工具类对象
	ToolFactory::registerTool("SubmitModify", []() { return std::make_shared<SubmitModifyTool>("SubmitModify"); });
	return true;
}();

SubmitModifyTool::SubmitModifyTool(const QString& text) :ButtonTool(text)
{
}

SubmitModifyTool::~SubmitModifyTool()
{
	m_action = nullptr;
}

void SubmitModifyTool::handleEvent(QEvent* event)
{
	CustomEvent* _event = dynamic_cast<CustomEvent*>(event);
	if (_event)
	{
		QString ver = _event->message();
		QString senderName = _event->getSender()->getName();
		if (senderName == "DataTable" && ver == "tableChanged")
		{
			// 表单发生了修改，图标变为绿色
			m_enable = true;
			QIcon openIcon = loadSvg(QString::fromUtf8("://new//prefix1//icon//SubmitModify.svg"), QColor("#FFFFFF")); // 白色背景
			if (m_action)
				m_action->setIcon(openIcon);
		}
	}
}

void SubmitModifyTool::clicked()
{
	if (m_enable)
	{
		GlobalManager::instance().sendEvent(this, new CustomEvent("certainSubmit"));
		// 提交修改后，图标设置为灰色
		QIcon openIcon = loadSvg(QString::fromUtf8("://new//prefix1//icon//SubmitModify_f.svg"), QColor("#FFFFFF")); // 白色背景
		if (m_action)
			m_action->setIcon(openIcon);
		// 重置标志位
		m_enable = false;
	}
}

void SubmitModifyTool::execute()
{
	// 注册工具，用于事件收发
	GlobalManager::instance().RegisterTool(this);
}

void SubmitModifyTool::setIcon(QAction* action)
{
	QIcon openIcon = loadSvg(QString::fromUtf8("://new//prefix1//icon//SubmitModify_f.svg"), QColor("#FFFFFF")); // 白色背景
	m_action = action;
	action->setIcon(openIcon);
}
