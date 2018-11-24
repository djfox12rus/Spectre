#include "GraphEditor.h"

#include <algorithm>
#include <iterator>

#include <QMouseEvent>
#include <QCursor>


GraphEditor::GraphEditor(QWidget * parent)
{
}

void GraphEditor::discard()
{
	_currentLine.clear();
	updateAndRepaint();
}

const QPolygonF & GraphEditor::currentLine() const
{
	return _currentLine;
}

QPixmap GraphEditor::updateGraph(QPixmap temp)
{
	auto& basePainter = painter();

	basePainter.begin(&temp);

	QPen p1(Qt::red, 2);
	basePainter.setPen(p1);

	QPolygonF line;
	std::transform(_currentLine.cbegin(), _currentLine.cend(), std::back_inserter(line), [this](const QPointF& point) 
	{
		return mapToGraph(point);
	});

	basePainter.drawPolyline(line);

	if (_drawTempLine && !_currentLine.empty())
	{
		QPen p2(Qt::red, 0.5);
		basePainter.setPen(p2);
		basePainter.drawLine(QLineF(line.back(), mapWdgtToGraph(_pos)));
	}

	basePainter.end();
	return temp;
}

void GraphEditor::mouseMoveEvent(QMouseEvent * event)
{
	if (_currentLine.empty())
	{
		_currentLine.push_back(QPointF(xBorders().first, 0));
	}

	_pos = event->localPos();
	const auto realPos = mapFromGraph(mapWdgtToGraph(_pos));
	if (realPos.x() < xBorders().second)
	{
		auto lastPos = mapGraphToWdgt(mapToGraph(_currentLine.back()));
		if (_mouseLeftPressed)
		{
			if (_pos.x() < lastPos.x())
			{
				auto newCursorPos = mapToGlobal(QPoint(lastPos.x(), _pos.y()));
				QCursor::setPos(newCursorPos);
				_currentLine.push_back(mapFromGraph(mapWdgtToGraph(QPointF(lastPos.x(), _pos.y()))));
			}
			else
			{
				_currentLine.push_back(mapFromGraph(mapWdgtToGraph(_pos)));
			}
		}

		if (_pos.x() >= lastPos.x())
		{
			_drawTempLine = true;
		}
		else
		{
			_drawTempLine = false;
		}
	}
	else
	{
		_drawTempLine = false;
	}

	redrawGraph();
	BaseGraphView::mouseMoveEvent(event);
}

void GraphEditor::mousePressEvent(QMouseEvent * event)
{
	if (event->button() == Qt::LeftButton)
	{
		if (mapFromGraph(mapWdgtToGraph(_pos)).x() < xBorders().second)
		{
			if (_currentLine.empty())
			{
				_currentLine.push_back(QPointF(xBorders().first, 0));
			}
			auto pos = event->localPos();
			auto lastPos = mapGraphToWdgt(mapToGraph(_currentLine.back()));
			if (pos.x() < lastPos.x())
			{
				auto newCursorPos = mapToGlobal(QPoint(lastPos.x(), pos.y()));
				QCursor::setPos(newCursorPos);
				_currentLine.push_back(mapFromGraph(mapWdgtToGraph(QPointF(lastPos.x(), pos.y()))));
			}

			else
			{
				_currentLine.push_back(mapFromGraph(mapWdgtToGraph(pos)));
			}

			_mouseLeftPressed = true;
			_drawTempLine = true;

			redrawGraph();
		}
		else
		{
			_drawTempLine = false;
		}
	}
	BaseGraphView::mousePressEvent(event);
}

void GraphEditor::mouseReleaseEvent(QMouseEvent * event)
{
	_mouseLeftPressed = false;
	redrawGraph();
	BaseGraphView::mousePressEvent(event);
}
