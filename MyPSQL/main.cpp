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

    //�����ļ�
    QTranslator translator;
    //bool loadTrans = translator.load(QApplication::applicationDirPath() + "//Translation_zh.ts");
    bool loadTrans = translator.load(QApplication::applicationDirPath() + "//Translation_zh.qm");
    QString oo = QApplication::applicationDirPath();
    app.installTranslator(&translator);

    //����tools.dll��֤������ĳ�ʼ��
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