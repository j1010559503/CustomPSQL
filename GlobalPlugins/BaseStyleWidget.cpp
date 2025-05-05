#include "BaseStyleWidget.h"

BaseStyleWidget::BaseStyleWidget(QWidget* parent)
    : QWidget(parent)
{
    setWindowFlag(Qt::Window);
    // 设置窗口属性以自动填充背景色
    setAutoFillBackground(true);
    // 创建 QPalette 并设置背景色
    QPalette palette = this->palette();
    QColor backgroundColor(MainBackgroundColor);
    palette.setColor(QPalette::Window, backgroundColor);
    setPalette(palette);

    // 获取屏幕分辨率
    QScreen* screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int screenWidth = screenGeometry.width();

    // 计算缩放比例（优化为更紧凑）
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

    // 设置主窗口尺寸（优化为更紧凑）
    int mainWindowWidth = 400 * scaleFactor; // 1200 → 1800 → 2400
    int mainWindowHeight = 300 * scaleFactor; // 700 → 840 → 1050
    setFixedSize(mainWindowWidth, mainWindowHeight);
    move((screenWidth - mainWindowWidth) / 2, (screenGeometry.height() - mainWindowHeight) / 2);

    // 设置全局字体大小（10px → 12px → 15px）
    QFont font;
    font.setPointSize(10 * scaleFactor);
    setFont(font);
    //setStyleSheet("background-color: #686de0;");
}
