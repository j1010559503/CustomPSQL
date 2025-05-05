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
    //������Ӱ�ť����
    int m_rowCount;
    int m_colCount;
    //��ӹ����б�
    std::vector<std::shared_ptr<BaseTool>> m_tools;
    //����ӵİ�ť�б�
    QList<QPushButton*> m_buttons;
    //��ť���񲼾�
    QGridLayout* m_hBtnBox;
    //���������ݿ��б�
    QTreeView* m_treeTable = nullptr;
    //���ݱ�
    QTableView* m_treeData = nullptr;
    //��ӡ��Ϣ
    QLabel* m_console = nullptr;
    //������ݿ⣬�ұ����ݱ���
    QHBoxLayout* m_hTreeBox = nullptr;
    //��Ϣ����̨����
    QHBoxLayout* m_hConsoleBox = nullptr;
    // ����������
    QToolBar* m_toolBar = nullptr;
    // ���岼��
    QVBoxLayout* m_mainLayout = nullptr;
    // �������ݲ���
    QHBoxLayout* m_contentLayout = nullptr;
    QSplitter* m_splitter = nullptr;
};

