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
 * @brief ������ݱ�����
 * ��ť����ӱ�����
 * �����߸��ݴ�������Ϊ��ǰ���ݿ���ӱ�
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
 * @brief ������ݱ���
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
	// ֧�ֵ���������
	QStringList m_sqliteDataTypes;
};
