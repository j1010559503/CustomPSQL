#pragma once

#include <QWidget>
#include <QString>
#include <QFileDialog>
#include <QJsonObject>
#include <QJsonDocument>

#include "BaseTool.h"
#include "ToolFactory.h"
#include "BaseStyleWidget.h"

#include "ui_CreateDbWidget.h"
#include "GlobalManager.h"
#include "tools_global.h"
#include "MainWindow.h"

class CreateDbWidget;
class TOOLS_EXPORT CreateDatabaseTool :
    public ButtonTool, public std::enable_shared_from_this<CreateDatabaseTool>
{
    Q_OBJECT
public:
    CreateDatabaseTool(const QString& text);
    ~CreateDatabaseTool();
    void handleEvent(QEvent* event) override;
    void clicked() override;
    void execute() override;
	void setIcon(QAction* action) override;
private:
	CreateDbWidget* m_createDbWidget = nullptr;
};

class TOOLS_EXPORT CreateDbWidget : public BaseStyleWidget
{
	Q_OBJECT

public:
	CreateDbWidget(QWidget* parent = nullptr);
	~CreateDbWidget();
	void initUI() override;
private:
	Ui::CreateDbWidgetClass ui;
public slots:
	void sendMessage();
	void showFileDialog();
signals:
	void certainCreate(QString message);
};