#pragma once
#include "BaseGraphView.h"

#include <QPolygonF>

class GraphEditor : public BaseGraphView
{
	Q_OBJECT

public:
	GraphEditor(QWidget * parent = nullptr);

	const QPolygonF&	currentLine()const;
	void				setCurrentLine(const QPolygonF& line);

signals:
	void	saveAvailable();

public slots:
	void	discard();

protected:
	QPixmap	updateGraph(QPixmap temp) override;
	void	mouseMoveEvent(QMouseEvent* event) override;
	void	mousePressEvent(QMouseEvent* event) override;
	void	mouseReleaseEvent(QMouseEvent* event) override;

private:
	QPolygonF		_currentLine;

	bool			_mouseLeftPressed{ false };
	bool			_drawTempLine{ true };
	QPointF			_pos;

	bool			_saveAvail{ false };
};

