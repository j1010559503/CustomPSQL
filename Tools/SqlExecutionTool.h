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
 * @brief ִ�й�����
 * ��ť��ִ�С����ߣ�
 * ����󵯳�sqlִ�д��ڣ��ٴε���ر�
 * ���ڶ��߳�����
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
 * @brief SQLִ���߳���
 * �ڵ������߳���ִ��SQL��ѯ
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
 * @brief SQLִ�д�����
 * �ṩSQL��������ִ�н���
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
