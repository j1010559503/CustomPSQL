#pragma once

#include <BaseTool.h>
#include <QSqlQuery>
#include <QMessageBox>

#include "BaseTool.h"
#include "ToolFactory.h"
#include "BaseStyleWidget.h"
#include "GlobalManager.h"
#include "ui_AddTableWidget.h"
#include "tools_global.h"
#include "MainWindow.h"

/**
 * @brief 添加行工具类
 * 按钮“添加行”工具
 */
class AddRowTool :
    public ButtonTool, public std::enable_shared_from_this<AddRowTool>
{
    Q_OBJECT
public:
    AddRowTool(const QString& text);
    ~AddRowTool();
    void handleEvent(QEvent* event) override;
    void clicked() override;
    void execute() override;
    void setIcon(QAction* action) override;
};

