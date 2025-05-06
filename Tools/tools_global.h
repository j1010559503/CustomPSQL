#pragma once

#include <QtCore/qglobal.h>
#include <QRegExpValidator>
#include <QVariantMap>
#include <QMetaType>
#include <QStandardItem>

#ifndef BUILD_STATIC
# if defined(TOOLS_LIB)
#  define TOOLS_EXPORT Q_DECL_EXPORT
# else
#  define TOOLS_EXPORT Q_DECL_IMPORT
# endif
#else
# define TOOLS_EXPORT
#endif

// 注册QStandardItem到 Qt 的元类型系统中
Q_DECLARE_METATYPE(QStandardItem*)

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