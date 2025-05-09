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
    // 获取屏幕分辨率
    QScreen* screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int screenWidth = screenGeometry.width();

    // 计算缩放比例
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

    // 设置主窗口尺寸
    int mainWindowWidth = 1200 * scaleFactor; // 1200 → 1800 → 2400
    int mainWindowHeight = 700 * scaleFactor; // 700 → 840 → 1050
    setFixedSize(mainWindowWidth, mainWindowHeight);
    move((screenWidth - mainWindowWidth) / 2, (screenGeometry.height() - mainWindowHeight) / 2);

    // 设置全局字体大小（10px → 12px → 15px）
    QFont font;
    font.setPointSize(10 * scaleFactor);
    setFont(font);

    // 创建工具栏
    m_toolBar = new QToolBar(this);
    m_toolBar->setMovable(false);
    m_toolBar->setFloatable(false);
    int toolBarHeight = 80 * scaleFactor;
    m_toolBar->setFixedHeight(toolBarHeight);

    // 设置按钮图标大小
    m_toolBar->setIconSize(QSize(40 * scaleFactor, 40 * scaleFactor + 10 * scaleFactor));
    m_toolBar->setFont(font);

    // 创建主布局（垂直布局）
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(5 * scaleFactor, 5 * scaleFactor, 5 * scaleFactor, 5 * scaleFactor); // 边距
    m_mainLayout->setSpacing(8 * scaleFactor); // 控件间距

    // 确保工具栏按钮显示图标在上、文字在下
    m_toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    // 设置样式表以改变悬停效果
    m_toolBar->setStyleSheet(
        "QToolButton {"
        "   background-color: transparent;" // 背景透明
        "   border: 1px solid transparent;" // 默认状态有透明边框
        "   padding: 5px;"                // 设置内边距
        "}"
        "QToolButton:hover {"
        "   border: 1px solid #ADD8E6;"      // 悬停时淡蓝色边框
        "   background-color: transparent;" // 背景仍然透明
        "}"
    );

    // 将工具栏添加到布局顶部
    //m_mainLayout->addWidget(m_toolBar);
    addToolBar(m_toolBar);

    // 创建内容区域（中间部分)
    // 创建 QSplitter 并设置为水平方向
    m_splitter = new QSplitter(Qt::Horizontal, this);
    m_splitter->setStyleSheet(
        "QSplitter::handle {"
        "   border: 0px solid #aaaaaa;"
        "}"
    );

    // 将内容区域添加到主布局
    m_mainLayout->addWidget(m_splitter, 1); // 占据剩余空间
    setCentralWidget(m_splitter);

    // 设置主布局
    setLayout(m_mainLayout);
}

void MainWindow::addNewButton(const QString& text)
{
    QString cnName = getName(text);
    // 添加按钮
    QAction* btn_action = m_toolBar->addAction(cnName);

    ++m_colCount;

    for (const auto& tool : m_tools)
    {
        if (tool && tool->getName() == text)
        {
            //向下类型转换
            std::shared_ptr<ButtonTool> buttonTool = std::dynamic_pointer_cast<ButtonTool>(tool);
            if (buttonTool) 
            {
                connect(btn_action, &QAction::triggered, buttonTool.get(), &ButtonTool::getClicked);
                buttonTool->setIcon(btn_action);
            }
        }
    }

    // 每5个按钮换行一次
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

        // 设置初始大小（像素值）和动态比例
        m_splitter->setSizes({ 100, 500 }); // 初始比例 2:1
        m_splitter->setStretchFactor(0, 1); // 左侧伸缩因子为 2
        m_splitter->setStretchFactor(1, 1); // 右侧伸缩因子为 1
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
