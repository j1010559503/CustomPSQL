#pragma once

#include "BaseStyleWidget.h"
#include "ToolFactory.h"
#include "config.h"

#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QTreeView>
#include <QTableView>
#include <QScreen>
#include <QToolBar>
#include <QSplitter>

#include "globalplugins_global.h"

class GLOBALPLUGINS_EXPORT MainWindow :
    public BaseStyleWidget
{
public:
    MainWindow(QWidget* parent = nullptr);

    void addTool(const QString& toolName, ToolType type);

    void executeAllTools() const;

    ~MainWindow();

    void initUI() override;

    void addNewButton(const QString& text); 

    void addDirectoryTree(QTreeView* c_treeview);

    void addDataTable(QTableView* c_tableview);

    const std::shared_ptr<BaseTool> findToolByName(const QString& name) const;

private:
    //用于添加按钮计数
    int m_rowCount;
    int m_colCount;
    //添加工具列表
    std::vector<std::shared_ptr<BaseTool>> m_tools;
    //已添加的按钮列表
    QList<QPushButton*> m_buttons;
    //按钮网格布局
    QGridLayout* m_hBtnBox;
    //已连接数据库列表
    QTreeView* m_treeTable = nullptr;
    //数据表
    QTableView* m_treeData = nullptr;
    //打印信息
    QLabel* m_console = nullptr;
    //左边数据库，右边数据表布局
    QHBoxLayout* m_hTreeBox = nullptr;
    //信息控制台布局
    QHBoxLayout* m_hConsoleBox = nullptr;
    // 顶部工具条
    QToolBar* m_toolBar = nullptr;
    // 总体布局
    QVBoxLayout* m_mainLayout = nullptr;
    // 主体内容布局
    QHBoxLayout* m_contentLayout = nullptr;
    QSplitter* m_splitter = nullptr;
};

