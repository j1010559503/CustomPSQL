#include "BaseStyleWidget.h"

BaseStyleWidget::BaseStyleWidget(QWidget* parent)
    : QWidget(parent)
{
    setWindowFlag(Qt::Window);
    // ���ô����������Զ���䱳��ɫ
    setAutoFillBackground(true);
    // ���� QPalette �����ñ���ɫ
    QPalette palette = this->palette();
    QColor backgroundColor(MainBackgroundColor);
    palette.setColor(QPalette::Window, backgroundColor);
    setPalette(palette);

    // ��ȡ��Ļ�ֱ���
    QScreen* screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int screenWidth = screenGeometry.width();

    // �������ű������Ż�Ϊ�����գ�
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

    // ���������ڳߴ磨�Ż�Ϊ�����գ�
    int mainWindowWidth = 400 * scaleFactor; // 1200 �� 1800 �� 2400
    int mainWindowHeight = 300 * scaleFactor; // 700 �� 840 �� 1050
    setFixedSize(mainWindowWidth, mainWindowHeight);
    move((screenWidth - mainWindowWidth) / 2, (screenGeometry.height() - mainWindowHeight) / 2);

    // ����ȫ�������С��10px �� 12px �� 15px��
    QFont font;
    font.setPointSize(10 * scaleFactor);
    setFont(font);
    //setStyleSheet("background-color: #686de0;");
}
