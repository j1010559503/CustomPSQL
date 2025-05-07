#pragma once

#include "BaseTool.h"
#include "ToolFactory.h"
#include "BaseStyleWidget.h"
#include "GlobalManager.h"
#include "ui_AddTableWidget.h"
#include "tools_global.h"
#include "MainWindow.h"
/**
 * @brief 提交修改类
 * 本工具用于提交暂存的修改
 * 当没有任何修改时按钮禁用，反之启用
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
    // 本工具默认不启用
    bool m_enable = false;
};
