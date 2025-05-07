#pragma once

#include "BaseTool.h"
#include "ToolFactory.h"
#include "BaseStyleWidget.h"
#include "GlobalManager.h"
#include "ui_AddTableWidget.h"
#include "tools_global.h"
#include "MainWindow.h"
/**
 * @brief �ύ�޸���
 * �����������ύ�ݴ���޸�
 * ��û���κ��޸�ʱ��ť���ã���֮����
 */
class SubmitModifyTool :
    public ButtonTool, public std::enable_shared_from_this<SubmitModifyTool>
{
    Q_OBJECT
public:
    SubmitModifyTool(const QString& text);
    ~SubmitModifyTool();
    void handleEvent(QEvent* event) override;
    void clicked() override;
    void execute() override;
    void setIcon(QAction* action) override;
private:
    QAction* m_action = nullptr;
    // ������Ĭ�ϲ�����
    bool m_enable = false;
};
