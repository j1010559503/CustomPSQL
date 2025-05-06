#pragma once

#include "BaseTool.h"
#include "ToolFactory.h"
#include "BaseStyleWidget.h"

#include <QString>
#include <QWidget>
#include <QFileDialog>
#include <QJsonObject>
#include <QJsonDocument>
#include <QPushButton>
#include <QHBoxLayout>

#include "ui_ConnectWidget.h"
#include "GlobalManager.h"
#include "tools_global.h"
#include "MainWindow.h"

class ConnectWidget;

/**
 * @brief �������ݿ⹤����
 * ��ť�����ӡ�����
 */
class TOOLS_EXPORT ConnectTool :
	public ButtonTool, public std::enable_shared_from_this<ConnectTool>
{
	Q_OBJECT
public:
    ConnectTool(const QString& text);
    ~ConnectTool();
	void handleEvent(QEvent* event) override;
	void clicked() override;
	void execute() override;
	void setIcon(QAction* action) override;
	void reset();
public slots:
	void certainClicked(QString strjson);
private:
	ConnectWidget* m_connectWidget = nullptr;
};

//���ݿ����Ӵ���
class TOOLS_EXPORT ConnectWidget : public BaseStyleWidget
{
	Q_OBJECT

public:
	ConnectWidget(QWidget* parent = nullptr);
	~ConnectWidget();
	void initUI() override;
	void reset();
private:
	Ui::ConnectWidgetClass ui;

public slots:
	void certainClicked();
	void cancelClicked();
	void showDatabasePath();
signals:
	void certainConnect(QString message);
};