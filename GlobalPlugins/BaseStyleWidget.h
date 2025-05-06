#pragma once

#include <QWidget>
#include <QGuiApplication>
#include <QScreen>

#include "globalplugins_global.h"
#include "config.h"

/**
 * @brief 窗口基类，抽象类
 * 包含一些基础设置，字号以及窗口尺寸等
 */
class GLOBALPLUGINS_EXPORT BaseStyleWidget :
    public QWidget
{
public:
    BaseStyleWidget(QWidget* parent = nullptr);
    virtual ~BaseStyleWidget() = default;
protected:
    virtual void initUI() = 0;
};

