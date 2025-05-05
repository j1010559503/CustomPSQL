#pragma once

#include <unordered_map>
#include <memory>
#include <functional>
#include <QString>
#include <QSettings>
#include <QApplication>

#include"BaseTool.h"
#include "globalplugins_global.h"

class GLOBALPLUGINS_EXPORT ToolFactory
{
public:
    using CreatorFunction = std::function<std::shared_ptr<BaseTool>()>;

    static void registerTool(const QString& toolName, CreatorFunction creator) 
    {
        getToolCreators()[toolName] = creator;
    }

    static std::shared_ptr<BaseTool> createTool(const QString& toolName)
    {
        auto it = getToolCreators().find(toolName);
        if (it != getToolCreators().end())
        {
            return it->second();
        }
        return nullptr;
    }

private:
    //�������б�
    // ��ȡ��̬�ֲ����� s_toolCreators
    static std::unordered_map<QString, CreatorFunction>& getToolCreators()
    {
        static std::unordered_map<QString, CreatorFunction> s_toolCreators;
        return s_toolCreators;
    }
};

