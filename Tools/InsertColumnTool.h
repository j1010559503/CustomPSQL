#pragma once

#include <QStandardItem>
#include <QMessageBox>
#include <QSqlQuery>

#include "BaseTool.h"
#include "ToolFactory.h"
#include "BaseStyleWidget.h"
#include "GlobalManager.h"
#include "tools_global.h"
#include "MainWindow.h"
#include "ui_InsertColumnWidget.h"

class InsertColumnWidget;
/**
 * @brief ���ݱ�����й�����
 * ��ť�������С�����
 */
class TOOLS_EXPORT InsertColumnTool :
    public ButtonTool, public std::enable_shared_from_this<InsertColumnTool>
{
    Q_OBJECT
public:
    InsertColumnTool(const QString& text);
    ~InsertColumnTool();
    void handleEvent(QEvent* event) override;
    void clicked() override;
    void execute() override;
    void setIcon(QAction* action) override;

private:
    InsertColumnWidget* m_widget = nullptr;
    // ��ǰѡ�б���
    QString m_curTbName;
};

class TOOLS_EXPORT InsertColumnWidget : public BaseStyleWidget
{
    Q_OBJECT

public:
    InsertColumnWidget(QWidget* parent = nullptr);
    ~InsertColumnWidget();
    void initUI();
    void setTbName(const QString& name);
    void reset();
signals:
    void sendCertainClicked();
public slots:
    void certainClicked();
private:
    Ui::InsertColumnWidget ui;
    // ��ǰѡ�б���
    QString m_curTbName;
};
