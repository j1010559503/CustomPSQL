#include "SubmitModifyTool.h"

static bool registeredConnect = []()
{
	//�������������
	ToolFactory::registerTool("SubmitModify", []() { return std::make_shared<SubmitModifyTool>("SubmitModify"); });
	return true;
}();

SubmitModifyTool::SubmitModifyTool(const QString& text) :ButtonTool(text)
{
}

SubmitModifyTool::~SubmitModifyTool()
{
}

void SubmitModifyTool::handleEvent(QEvent* event)
{
	CustomEvent* _event = dynamic_cast<CustomEvent*>(event);
	if (_event)
	{
		QString ver = _event->message();
		QString senderName = _event->getSender().get()->getName();
		if (senderName == "DataTable" && ver == "tableChanged")
		{
			// ���������޸ģ�ͼ���Ϊ��ɫ
			m_enable = true;
			QIcon openIcon = loadSvg(QString::fromUtf8("://new//prefix1//icon//SubmitModify.svg"), QColor("#FFFFFF")); // ��ɫ����
			if (m_action)
				m_action->setIcon(openIcon);
		}
	}
}

void SubmitModifyTool::clicked()
{
	if (m_enable)
	{
		GlobalManager::instance()->sendEvent(shared_from_this(), new CustomEvent("certainSubmit"));
		// �ύ�޸ĺ�ͼ������Ϊ��ɫ
		QIcon openIcon = loadSvg(QString::fromUtf8("://new//prefix1//icon//SubmitModify_f.svg"), QColor("#FFFFFF")); // ��ɫ����
		if (m_action)
			m_action->setIcon(openIcon);
		// ���ñ�־λ
		m_enable = false;
	}
}

void SubmitModifyTool::execute()
{
	// ע�Ṥ�ߣ������¼��շ�
	GlobalManager::instance()->RegisterTool(shared_from_this());
}

void SubmitModifyTool::setIcon(QAction* action)
{
	QIcon openIcon = loadSvg(QString::fromUtf8("://new//prefix1//icon//SubmitModify_f.svg"), QColor("#FFFFFF")); // ��ɫ����
	m_action = action;
	action->setIcon(openIcon);
}
