#pragma once

#include <QString>
#include <QObject>
#include <QAction>
#include <QIcon>
#include <QSvgRenderer>
#include <QPixmap>
#include <QPainter>
#include <QColor>

#include <memory>
#include <iostream>

#include "globalplugins_global.h"

/**
 * @brief 工具基类，抽象类
 * 必须输入工具名
 */
class GLOBALPLUGINS_EXPORT BaseTool :public QObject
{
public:
	explicit BaseTool(const QString& name) :m_toolName(name) {};
    virtual ~BaseTool() = default;

	QString getName() { return m_toolName; }

    virtual void execute() = 0; // 纯虚函数，确保子类必须实现

	// 事件接收处理接口，子类必须实现，用于各个工具插件通信
	virtual void handleEvent(QEvent* event) = 0; 
public:
	QString m_toolName;
};

/**
 * @brief 按钮工具基类
 * 继承本类的工具通过工厂类自动添加到窗口上方工具栏
 */
class GLOBALPLUGINS_EXPORT ButtonTool : public BaseTool
{
public:
	explicit ButtonTool(const QString& text);
	virtual ~ButtonTool() = default;
	virtual void clicked();
	virtual void setIcon(QAction* action);
	inline QString text() { return m_text; };
protected:
	QString m_text;
public slots:
	void getClicked();
};

QIcon GLOBALPLUGINS_EXPORT loadSvg(const QString& path);

QIcon GLOBALPLUGINS_EXPORT loadSvg(const QString& path, const QColor& backgroundColor);
