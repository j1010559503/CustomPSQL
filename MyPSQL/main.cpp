#include <QObject>
#include <QApplication>
#include <QThread>
#include <QTranslator>

#include "MainWindow.h"
#include "GlobalManager.h"
#include "EventManager.h"
#include "Init.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    //翻译文件
    QTranslator translator;
    //bool loadTrans = translator.load(QApplication::applicationDirPath() + "//Translation_zh.ts");
    bool loadTrans = translator.load(QApplication::applicationDirPath() + "//Translation_zh.qm");
    QString oo = QApplication::applicationDirPath();
    app.installTranslator(&translator);

    //加载tools.dll保证工具类的初始化
    InitializeTools();

    MainWindow w;
    GlobalManager::instance()->SetMainWindow(&w);

    w.addTool("connect", ToolType::ButtonTool);
    w.addTool("AddTable", ToolType::ButtonTool);
    w.addTool("CreateDatabase", ToolType::ButtonTool);

    w.addTool("DirectoryTree", ToolType::DirectoryTree);

    w.addTool("DataTable", ToolType::DataTable);

    w.executeAllTools();

    w.show();

    return app.exec();
}