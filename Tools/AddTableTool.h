#pragma once

#include <QWidget>
#include <QString>
#include <QFileDialog>
#include <QJsonObject>
#include <QJsonDocument>
#include <QLineEdit>

#include "BaseTool.h"
#include "ToolFactory.h"
#include "BaseStyleWidget.h"
#include "GlobalManager.h"
#include "ui_AddTableWidget.h"
#include "tools_global.h"
#include "MainWindow.h"

class AddTableWidget;
/**
 * @brief 添加数据表工具类
 * 按钮“添加表”工具
 * 本工具根据窗口内容为当前数据库添加表
 */
class TOOLS_EXPORT AddTableTool :
    public ButtonTool, public std::enable_shared_from_this<AddTableTool>
{
    Q_OBJECT
public:
    AddTableTool(const QString& text);
    ~AddTableTool();
    void handleEvent(QEvent* event) override;
    void clicked() override;
    void execute() override;
    void setIcon(QAction* action) override;
private:
	AddTableWidget* m_addTableWidget = nullptr;
};
/**
 * @brief 添加数据表窗口
 */
class TOOLS_EXPORT AddTableWidget : public BaseStyleWidget
{
	Q_OBJECT

public:
	AddTableWidget(QWidget* parent = nullptr);
	~AddTableWidget();
	void initUI();
public slots:
	void certainClicked();
	//void onTypeChanged(int index);
signals:
	void certainAddTable(QString sqlStr);
private:
	Ui::AddTableWidgetClass ui;
	QLineEdit* m_tablename = nullptr;
	QLineEdit* m_primkey = nullptr;
	QLineEdit* m_primkeytype = nullptr;
	// 支持的数据类型
	QStringList m_sqliteDataTypes;
};
