#include "BaseTool.h"

ButtonTool::ButtonTool(const QString& text) : BaseTool(text), m_text(text)
{

}

void ButtonTool::clicked()
{

}

void ButtonTool::setIcon(QAction* action)
{
}

void ButtonTool::getClicked()
{
	clicked();
}

QIcon loadSvg(const QString& path)
{
	QSvgRenderer renderer(path);
	QPixmap pixmap(renderer.defaultSize());
	QPainter painter(&pixmap);
	renderer.render(&painter);
	return QIcon(pixmap);
}

QIcon GLOBALPLUGINS_EXPORT loadSvg(const QString& path, const QColor& backgroundColor)
{
	QSvgRenderer renderer(path);
	QPixmap pixmap(renderer.defaultSize());
	QPainter painter(&pixmap);

	// ��䱳����ɫ
	painter.fillRect(pixmap.rect(), backgroundColor);

	// ��Ⱦ SVG ����
	renderer.render(&painter);

	return QIcon(pixmap);
}
