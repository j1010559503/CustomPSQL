#include "MainWindow.h"

const QString& getName(const QString& EnName)
{
    QString path = QApplication::applicationDirPath() + "//tools.ini";
    QSettings settings(path, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    QString cnName = settings.value(EnName).toString();
    return cnName;
}

MainWindow::MainWindow(QWidget* parent)
	:QMainWindow(parent), m_rowCount(0), m_colCount(0)
{
	initUI();
}

void MainWindow::addTool(const QString& toolName, ToolType type)
{
    std::shared_ptr<BaseTool> tool = ToolFactory::createTool(toolName);
    if (tool)
    {
        m_tools.push_back(std::move(tool));
        switch (type) 
        {
        case ToolType::ButtonTool:
            addNewButton(toolName);
            break;
        case ToolType::GeneralTool:
            break;
        case ToolType::DirectoryTree:
            //tool.get()->execute();
            break; 
        case ToolType::DataTable:
            break;
        default:
            break;
        }
    }
    return;
}

void MainWindow::executeAllTools() const
{
    for (const auto& tool : m_tools)
    {
        tool->execute();
    }
}

MainWindow::~MainWindow()
{
}

void MainWindow::initUI()
{
    // ��ȡ��Ļ�ֱ���
    QScreen* screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int screenWidth = screenGeometry.width();

    // �������ű���
    double scaleFactor = 1.0;
    if (screenWidth >= 3840) 
    {
        scaleFactor = 1.5; // 4K
    }
    else if (screenWidth >= 2560) 
    {
        scaleFactor = 1.2; // 2K
    }
    else if (screenWidth >= 1920) 
    {
        scaleFactor = 1.0; // 1K
    }

    // ���������ڳߴ�
    int mainWindowWidth = 1200 * scaleFactor; // 1200 �� 1800 �� 2400
    int mainWindowHeight = 700 * scaleFactor; // 700 �� 840 �� 1050
    setFixedSize(mainWindowWidth, mainWindowHeight);
    move((screenWidth - mainWindowWidth) / 2, (screenGeometry.height() - mainWindowHeight) / 2);

    // ����ȫ�������С��10px �� 12px �� 15px��
    QFont font;
    font.setPointSize(10 * scaleFactor);
    setFont(font);

    // ����������
    m_toolBar = new QToolBar(this);
    m_toolBar->setMovable(false);
    m_toolBar->setFloatable(false);
    int toolBarHeight = 80 * scaleFactor;
    m_toolBar->setFixedHeight(toolBarHeight);

    // ���ð�ťͼ���С
    m_toolBar->setIconSize(QSize(40 * scaleFactor, 40 * scaleFactor + 10 * scaleFactor));
    m_toolBar->setFont(font);

    // ���������֣���ֱ���֣�
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(5 * scaleFactor, 5 * scaleFactor, 5 * scaleFactor, 5 * scaleFactor); // �߾�
    m_mainLayout->setSpacing(8 * scaleFactor); // �ؼ����

    // ȷ����������ť��ʾͼ�����ϡ���������
    m_toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    // ������ʽ���Ըı���ͣЧ��
    m_toolBar->setStyleSheet(
        "QToolButton {"
        "   background-color: transparent;" // ����͸��
        "   border: 1px solid transparent;" // Ĭ��״̬��͸���߿�
        "   padding: 5px;"                // �����ڱ߾�
        "}"
        "QToolButton:hover {"
        "   border: 1px solid #ADD8E6;"      // ��ͣʱ����ɫ�߿�
        "   background-color: transparent;" // ������Ȼ͸��
        "}"
    );

    // ����������ӵ����ֶ���
    //m_mainLayout->addWidget(m_toolBar);
    addToolBar(m_toolBar);

    // �������������м䲿��)
    // ���� QSplitter ������Ϊˮƽ����
    m_splitter = new QSplitter(Qt::Horizontal, this);
    m_splitter->setStyleSheet(
        "QSplitter::handle {"
        "   border: 0px solid #aaaaaa;"
        "}"
    );

    // ������������ӵ�������
    m_mainLayout->addWidget(m_splitter, 1); // ռ��ʣ��ռ�
    setCentralWidget(m_splitter);

    // ����������
    setLayout(m_mainLayout);
}

void MainWindow::addNewButton(const QString& text)
{
    QString cnName = getName(text);
    // ��Ӱ�ť
    QAction* btn_action = m_toolBar->addAction(cnName);

    ++m_colCount;

    for (const auto& tool : m_tools)
    {
        if (tool && tool->getName() == text)
        {
            //��������ת��
            std::shared_ptr<ButtonTool> buttonTool = std::dynamic_pointer_cast<ButtonTool>(tool);
            if (buttonTool) 
            {
                connect(btn_action, &QAction::triggered, buttonTool.get(), &ButtonTool::getClicked);
                buttonTool->setIcon(btn_action);
            }
        }
    }

    // ÿ5����ť����һ��
    if (m_colCount % 5 == 0) 
    {
        ++m_rowCount;
        m_colCount = 0;
    }
}

void MainWindow::addDirectoryTree(QTreeView* c_treeview)
{
    if (c_treeview && !m_treeTable)
    {
        m_treeTable = c_treeview;
        m_splitter->addWidget(m_treeTable);
    }
}

void MainWindow::addDataTable(QTableView* c_tableview)
{
    if (c_tableview && !m_treeData)
    {
        m_treeData = c_tableview;
        m_splitter->addWidget(m_treeData);

        // ���ó�ʼ��С������ֵ���Ͷ�̬����
        m_splitter->setSizes({ 100, 500 }); // ��ʼ���� 2:1
        m_splitter->setStretchFactor(0, 1); // �����������Ϊ 2
        m_splitter->setStretchFactor(1, 1); // �Ҳ���������Ϊ 1
    }
}

const std::shared_ptr<BaseTool> MainWindow::findToolByName(const QString& name) const
{
    for (const auto& tool : m_tools) 
    {
        if (tool && tool->getName() == name) 
        {
            return tool;
        }
    }
    return nullptr;
}
