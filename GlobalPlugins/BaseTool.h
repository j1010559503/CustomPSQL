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
 * @brief ���߻��࣬������
 * �������빤����
 */
class GLOBALPLUGINS_EXPORT BaseTool :public QObject
{
public:
	explicit BaseTool(const QString& name) :m_toolName(name) {};
    virtual ~BaseTool() = default;

	QString getName() { return m_toolName; }

    virtual void execute() = 0; // ���麯����ȷ���������ʵ��

	// �¼����մ���ӿڣ��������ʵ�֣����ڸ������߲��ͨ��
	virtual void handleEvent(QEvent* event) = 0; 
public:
	QString m_toolName;
};

/**
 * @brief ��ť���߻���
 * �̳б���Ĺ���ͨ���������Զ���ӵ������Ϸ�������
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
