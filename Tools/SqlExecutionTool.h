#pragma once

#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSplitter>
#include <QApplication>
#include <QSqlError>
#include <QSqlRecord>
#include <QTime>
#include <QThread>
#include <QSqlQuery>
#include <QDockWidget>

#include "BaseTool.h"
#include "ToolFactory.h"
#include "BaseStyleWidget.h"
#include "GlobalManager.h"
#include "ui_AddTableWidget.h"
#include "tools_global.h"
#include "MainWindow.h"

class SqlExecutionWidget;
/**
 * @brief 执行工具类
 * 按钮“执行”工具，
 * 点击后弹出sql执行窗口，再次点击关闭
 * 基于多线程运行
 */
class SqlExecutionTool :
    public ButtonTool, public std::enable_shared_from_this<SqlExecutionTool>
{
    Q_OBJECT
public:
    SqlExecutionTool(const QString& text);
    ~SqlExecutionTool();
    void handleEvent(QEvent* event) override;
    void clicked() override;
    void execute() override;
    void setIcon(QAction* action) override;
private:
    QDockWidget* m_dockWidget = nullptr;
    SqlExecutionWidget* m_sqlExecutionWidget = nullptr;
    bool m_isVisible = false;
};

/**
 * @brief SQL执行线程类
 * 在单独的线程中执行SQL查询
 */
class SqlExecutionThread : public QObject
{
    Q_OBJECT

public:
    explicit SqlExecutionThread(QObject* parent = nullptr);
    void setQuery(const QString& query, const QSqlDatabase& db);

public slots:
    void execute();

signals:
    void resultReady(const QString& result);
    void error(const QString& errorMessage);
    void finished();

private:
    QString m_query;
    QSqlDatabase m_db;
};

/**
 * @brief SQL执行窗口类
 * 提供SQL语句输入和执行界面
 */
class SqlExecutionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SqlExecutionWidget(QWidget* parent = nullptr);
    ~SqlExecutionWidget();

    void setDatabase(const QSqlDatabase& db);

public slots:
    void executeQuery();
    void displayResult(const QString& result);
    void displayError(const QString& errorMessage);
    void queryFinished();

private:
    QTextEdit* m_sqlEditor;
    QTextEdit* m_resultDisplay;
    QPushButton* m_executeButton;
    QVBoxLayout* m_layout;
    QLabel* m_statusLabel;

    QSqlDatabase m_db;
    QThread m_workerThread;
    SqlExecutionThread* m_worker;

    bool m_isExecuting;
};
