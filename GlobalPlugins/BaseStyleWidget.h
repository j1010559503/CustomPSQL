#pragma once

#include <QWidget>
#include <QGuiApplication>
#include <QScreen>

#include "globalplugins_global.h"
#include "config.h"

class GLOBALPLUGINS_EXPORT BaseStyleWidget :
    public QWidget
{
public:
    BaseStyleWidget(QWidget* parent = nullptr);
    virtual ~BaseStyleWidget() = default;
protected:
    virtual void initUI() = 0;
};

