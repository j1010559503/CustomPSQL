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
/**
 * @brief 自定义Validator限制
 * 用于限制添加时输入的主键名
 */
class TOOLS_EXPORT SqlColumnNameValidator : public QRegExpValidator
{
public:
    SqlColumnNameValidator(QObject* parent = nullptr) : QRegExpValidator(parent) 
    {
        // 定义 SQL 关键字列表
        m_sqlKeywords << "ABORT" << "ACTION" << "ADD" << "AFTER" << "ALL" << "ALTER" << "ANALYZE" << "AND" << "AS" << "ASC"
            << "ATTACH" << "AUTOINCREMENT" << "BEFORE" << "BEGIN" << "BETWEEN" << "BY" << "CASCADE" << "CASE" << "CAST"
            << "CHECK" << "COLLATE" << "COLUMN" << "COMMIT" << "CONFLICT" << "CONSTRAINT" << "CREATE" << "CROSS"
            << "CURRENT_DATE" << "CURRENT_TIME" << "CURRENT_TIMESTAMP" << "DATABASE" << "DEFAULT" << "DEFERRABLE"
            << "DEFERRED" << "DELETE" << "DETACH" << "DISTINCT" << "DROP" << "EACH" << "ELSE" << "END" << "ESCAPE"
            << "EXCEPT" << "EXCLUSIVE" << "EXISTS" << "EXPLAIN" << "FAIL" << "FOR" << "FOREIGN" << "FROM" << "FULL"
            << "GLOB" << "GROUP" << "HAVING" << "IF" << "IGNORE" << "IMMEDIATE" << "IN" << "INDEX" << "INDEXED"
            << "INITIALLY" << "INNER" << "INSERT" << "INSTEAD" << "INTERSECT" << "INTO" << "IS" << "ISNULL"
            << "JOIN" << "KEY" << "LEFT" << "LIKE" << "LIMIT" << "MATCH" << "NATURAL" << "NO" << "NOT" << "NOTNULL"
            << "NULL" << "OF" << "OFFSET" << "ON" << "OR" << "ORDER" << "OUTER" << "PLAN" << "PRAGMA" << "PRIMARY"
            << "QUERY" << "RAISE" << "RECURSIVE" << "REFERENCES" << "REGEXP" << "REINDEX" << "RELEASE" << "RENAME"
            << "REPLACE" << "RESTRICT" << "RIGHT" << "ROLLBACK" << "ROW" << "SAVEPOINT" << "SELECT" << "SET"
            << "TABLE" << "TEMP" << "TEMPORARY" << "THEN" << "TO" << "TRANSACTION" << "TRIGGER" << "UNION"
            << "UNIQUE" << "UPDATE" << "USING" << "VACUUM" << "VALUES" << "VIEW" << "VIRTUAL" << "WHEN" << "WHERE"
            << "WITH" << "WITHOUT";

        // 正则表达式：以字母开头，后跟字母、数字或下划线
        QRegExp regExp("^[a-zA-Z][a-zA-Z0-9_]*$");
        setRegExp(regExp);
    }

    QValidator::State validate(QString& input, int& pos) const override 
    {
        QValidator::State state = QRegExpValidator::validate(input, pos);

        if (state == QValidator::Acceptable && isSqlKeyword(input)) 
        {
            return QValidator::Invalid;
        }

        return state;
    }

private:
    QStringList m_sqlKeywords;

    bool isSqlKeyword(const QString& word) const 
    {
        return m_sqlKeywords.contains(word.toUpper());
    }
};