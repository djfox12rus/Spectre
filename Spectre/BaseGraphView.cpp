#include "BaseGraphView.h"

#include <limits>
#include <vector>

#include <QPen>
#include <QVector>
#include <QLineF>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QRectF>
#include <QFont>
#include <QFontMetrics>
#include <QColor>

static constexpr double		epsilon			= std::numeric_limits<double>::epsilon() * 5;
static constexpr int		yCoordsOffset	= 35;
static constexpr int		xCoordsOffset	= 20;
static constexpr int		yTopOffset		= 20;
static constexpr int		xLeftOffset		= 10;
static constexpr int		fontSize		= 10;
static constexpr int		precisionLines	= 5;

static constexpr int		shadeOfGray		= 0xea;
static const QColor backColor(shadeOfGray, shadeOfGray, shadeOfGray);

static QFont f{ "Times", 10 };
static QFontMetrics fm{ f };

BaseGraphView::BaseGraphView(QWidget* parent)
	:QWidget(parent)
{
	setMouseTracking(true);
	setCursor(Qt::CrossCursor);
}

void BaseGraphView::setXLowBorder(double border)
{
	_xBorders.first = border;
	adjXGridStep();
	updateAndRepaint();
}

QPixmap BaseGraphView::pixmap() const
{
	return _temp;
}

void BaseGraphView::setXHighBorder(double border)
{
	_xBorders.second = border;
	adjXGridStep();
	updateAndRepaint();
}

void BaseGraphView::setYLowBorder(double border)
{
	_yBorders.first = border;
	adjYGridStep();
	updateAndRepaint();
}

void BaseGraphView::setYHighBorder(double border)
{
	_yBorders.second = border;
	adjYGridStep();
	updateAndRepaint();
}

void BaseGraphView::setXAxisName(const QString & name)
{
	_xAxisName = name;
}

void BaseGraphView::setYAxisName(const QString & name)
{
	_yAxisName = name;
}

void BaseGraphView::setShowCoordsValues(bool show)
{
	_showCoordsValues = show;
}

//void BaseGraphView::setXGridStep(double step)
//{
//	_xyGridSteps.first = step;
//	updateAndRepaint();
//}
//
//void BaseGraphView::setYGridStep(double step)
//{
//	_xyGridSteps.second = step;
//	updateAndRepaint();
//}

const std::pair<double, double>& BaseGraphView::xBorders()
{
	return _xBorders;
}

const std::pair<double, double>& BaseGraphView::yBorders()
{
	return _yBorders;
}

const std::pair<double, double>& BaseGraphView::xyGridSteps()
{
	return _xyGridSteps;
}

QPixmap BaseGraphView::templateGraph()
{
	return _templateGraph;
}

void BaseGraphView::redrawGraph()
{
	auto temp = updateGraph(_templateGraph);
	insertGraph(_graph, temp);
	repaint();
}

QPointF BaseGraphView::mapToGraph(QPointF point)
{
	if (point.x() < _xBorders.first && point.x() > _xBorders.second) return QPointF();
	if (point.y() < _yBorders.first && point.y() > _yBorders.second) return QPointF();
	
	const auto leftOffset = (point.x() - _xBorders.first) / (_xBorders.second - _xBorders.first);
	const auto topOffset = (_yBorders.second - point.y()) / (_yBorders.second - _yBorders.first);

	return QPointF(leftOffset * (_xGraphBorders.second - _xGraphBorders.first) + _xGraphBorders.first
				, topOffset * (_yGraphBorders.second - _yGraphBorders.first) + _yGraphBorders.first);
}

QPointF BaseGraphView::mapWdgtToGraph(QPointF point)
{
	return QPointF(point.x() - yCoordsOffset, point.y() - yTopOffset);
}

QPointF BaseGraphView::mapGraphToWdgt(QPointF point)
{
	return QPointF(point.x() + yCoordsOffset, point.y() + yTopOffset);
}

QPointF BaseGraphView::mapFromGraph(QPointF point)
{
	if (point.x() < _xGraphBorders.first && point.x() > _xGraphBorders.second) return QPointF();
	if (point.y() < _yGraphBorders.first && point.y() > _yGraphBorders.second) return QPointF();

	const auto leftOffset = (point.x() - _xGraphBorders.first) / (_xGraphBorders.second - _xGraphBorders.first);
	const auto topOffset = (_yGraphBorders.second - point.y()) / (_yGraphBorders.second - _yGraphBorders.first);

	return QPointF(leftOffset * (_xBorders.second - _xBorders.first) + _xBorders.first
				, topOffset * (_yBorders.second - _yBorders.first) + _yBorders.first);
}

QPainter & BaseGraphView::painter()
{
	return _painter;
}

void BaseGraphView::paintEvent(QPaintEvent * event)
{
	_painter.begin(this);
	_painter.drawPixmap(rect(), _temp);
	_painter.end();
}

void BaseGraphView::resizeEvent(QResizeEvent * event)
{
	_graph = makeTemplatePix(event->size());
	_temp = _graph;

	QWidget::resizeEvent(event);
}

void BaseGraphView::mouseMoveEvent(QMouseEvent * event)
{
	auto pos = event->localPos();
	_temp = _graph;
	if (pos.x() >= yCoordsOffset && pos.y() < _graph.height() - xCoordsOffset && pos.y() > yTopOffset)
	{
		_painter.begin(&_temp);
		_painter.setPen(QPen(Qt::black, 0.5));

		QLineF horizontal;
		horizontal.setP1(QPointF(yCoordsOffset, pos.y()));
		horizontal.setP2(pos);
		_painter.drawLine(horizontal);
		QLineF vertical;
		vertical.setP1(QPointF(pos.x(), _temp.height() - xCoordsOffset));
		vertical.setP2(pos);
		_painter.drawLine(vertical);

		if (_showCoordsValues)
		{
			auto point = mapFromGraph(mapWdgtToGraph(pos));
			_painter.setFont(f);

			auto str =" " + QString::number(point.x(), 'f', 2) + ", " + QString::number(point.y(), 'f', 2);
			QRectF rect(QPointF{ pos.x() + 2, pos.y() - fm.height() - 3 }, QSizeF{double(fm.width(str)), double(fm.height())});

			_painter.setBrush(Qt::white);
			_painter.drawRect(rect);
			_painter.drawText(rect, str);

		}

		_painter.end();
	}
	repaint();
	QWidget::mouseMoveEvent(event);
}

QPixmap BaseGraphView::makeTemplatePix(QSize newSize)
{
	if (!newSize.isValid()) return QPixmap();
	QSize graphSize(newSize.width() - yCoordsOffset - xLeftOffset, newSize.height()- xCoordsOffset - yTopOffset);
	QVector<QLineF> verticalPrecGridLines;
	QVector<QLineF> horisontalPrecGridLines;
	CoordsValues xCoords;
	CoordsValues yCoords;
	QPointF origin;
	std::tie(_templateGraph, verticalPrecGridLines, horisontalPrecGridLines, xCoords, yCoords, origin) = makeTemplateGraphPix(graphSize);

 	auto templateGraph = updateGraph(_templateGraph);

	QPixmap out(newSize);
	out.fill(backColor);
	{
		insertGraph(out, templateGraph);

		_painter.begin(&out);

		QRectF templateRect(QPointF(yCoordsOffset, yTopOffset), templateGraph.size());

		_painter.setPen(QPen(Qt::black, 1));
		_painter.drawLines(verticalPrecGridLines);
		_painter.drawLines(horisontalPrecGridLines);

		
		_painter.setFont(f);

		const auto xCoordsTextOffset = newSize.height() - (xCoordsOffset - fontSize - 5);
		for (const auto& xCoord : xCoords)
		{
			_painter.drawText(QPointF(xCoord.second + yCoordsOffset - fm.width("0.00")/ 2, xCoordsTextOffset), adjCoord(xCoord.first, epsilon));
		}

		for (const auto& yCoord : yCoords)
		{
			_painter.drawText(QPointF(2, yCoord.second + fontSize/2 + yTopOffset), adjCoord(yCoord.first, epsilon));
		}

		_painter.drawText(QPointF(origin.x() + yCoordsOffset/2, yTopOffset - 3), _yAxisName);
		_painter.setBrush(Qt::white);
		QRect xAxisText{ QPoint(newSize.width() - xLeftOffset - fm.width(_xAxisName) - 5, origin.y() + yTopOffset - fm.height()), QSize {fm.width(_xAxisName) + 5, fm.height()} };
		_painter.drawRect(xAxisText);
		_painter.drawText(QPointF(newSize.width() - xLeftOffset - fm.width(_xAxisName) - 3, origin.y() + yTopOffset - 3), _xAxisName);

		_painter.end();
	}
	return out;
}

std::tuple<QPixmap, QVector<QLineF>, QVector<QLineF>, BaseGraphView::CoordsValues, BaseGraphView::CoordsValues, QPointF> BaseGraphView::makeTemplateGraphPix(QSize graphSize)
{
	QPixmap templateGraph(graphSize); //TODO: adj size
	templateGraph.fill(Qt::white);

	const auto xSteps = (_xBorders.second - _xBorders.first) / _xyGridSteps.first;
	const auto ySteps = (_yBorders.second - _yBorders.first) / _xyGridSteps.second;

	const auto xPrecisionSteps = xSteps * precisionLines;
	const auto yPrecisionSteps = ySteps * precisionLines;

	const auto offsetX = (templateGraph.width() / xPrecisionSteps) / 2 + 1;
	const auto realXStep = (templateGraph.width() - offsetX * 2) / xSteps;
	const auto realXPrecStep = (templateGraph.width() - offsetX * 2) / (xSteps * precisionLines);

	_xGraphBorders.first = offsetX;

	std::vector<std::pair<double, double>> xCoords;
	QVector<QLineF> verticalGridLines;
	QVector<QLineF> verticalPrecGridLines;
	QLineF zeroX;
	bool zeroXDrawn = false;
	double j = offsetX;
	for (auto i = _xBorders.first; i <= _xBorders.second; i += _xyGridSteps.first, j += realXStep)
	{
		if (i < -epsilon || i > epsilon)
		{
			verticalGridLines.push_back(QLineF(j, 0, j, templateGraph.height()));
		}
		else
		{
			zeroX.setLine(j, 0, j, templateGraph.height());
			zeroXDrawn = true;
		}

		const auto topOffset = graphSize.height() + yTopOffset;
		for (auto k = 0; k < precisionLines; ++k)
		{
			const auto jOffset = j + k * realXPrecStep + yCoordsOffset;
			if (jOffset > graphSize.width() + yCoordsOffset) break;
			verticalPrecGridLines.push_back(QLineF(jOffset, topOffset, jOffset, topOffset + 2));
		}
		xCoords.emplace_back(i, j);
	}

	_xGraphBorders.second = j - (j > graphSize.width() ? realXStep : 0);

	const auto offsetY = (templateGraph.height() / yPrecisionSteps) / 2 + 1;
	const auto realYStep = (templateGraph.height() - offsetY * 2) / ySteps;
	const auto realYPrecStep = (templateGraph.height() - offsetY * 2) / (ySteps * precisionLines);

	_yGraphBorders.first = offsetY;

	std::vector<std::pair<double, double>> yCoords;
	QVector<QLineF> horisontalGridLines;
	QVector<QLineF> horisontalPrecGridLines;
	QLineF zeroY;
	bool zeroYDrawn = false;
	j = offsetY;
	for (auto i = _yBorders.second; i >= _yBorders.first; i -= _xyGridSteps.second, j += realYStep)
	{

		if (i < -epsilon || i > epsilon)
		{
			horisontalGridLines.push_back(QLineF(0, j, templateGraph.width(), j));
		}
		else
		{
			zeroY.setLine(0, j, templateGraph.width(), j);
			zeroYDrawn = true;
		}

		for (auto k = 0; k < precisionLines; ++k)
		{
			const auto jOffset = j + k * realYPrecStep + yTopOffset;
			const auto topOffset = graphSize.height() + yTopOffset;
			if (jOffset > topOffset) break;
			horisontalPrecGridLines.push_back(QLineF(yCoordsOffset - 2, jOffset, yCoordsOffset, jOffset));
		}
		yCoords.emplace_back(i, j);
	}
	
	_yGraphBorders.second = j - (j > graphSize.height()? realYStep : 0);

	if (!zeroXDrawn)
	{
		auto point = mapToGraph({});
		zeroX.setLine(point.x(), 0, point.x(), templateGraph.height());
		xCoords.emplace_back(point.x(), 0);
	}

	if (!zeroYDrawn)
	{
		auto point = mapToGraph({});
		zeroY.setLine(0, point.y(), templateGraph.width(), point.y());
		yCoords.emplace_back(0, point.y());
	}

	{
		_painter.begin(&templateGraph);

		_painter.setPen(QPen(Qt::black, 0.5, Qt::DashLine));
		_painter.drawLines(verticalGridLines);
		_painter.drawLines(horisontalGridLines);

		_painter.setPen(QPen(Qt::black, 1));
		if (zeroX.length() > 0)
		{
			_painter.drawLine(zeroX);
		}
		if (zeroY.length() > 0)
		{
			_painter.drawLine(zeroY);
		}

		_painter.end();
	}
	return { templateGraph, verticalPrecGridLines, horisontalPrecGridLines, xCoords, yCoords, mapToGraph({}) };
}

void BaseGraphView::insertGraph(QPixmap& graph, QPixmap templateGraph)
{
	_painter.begin(&graph);

	QRectF templateRect(QPointF(yCoordsOffset, yTopOffset), templateGraph.size());
	_painter.drawPixmap(templateRect, templateGraph, templateGraph.rect());
	_painter.setPen(QPen(Qt::black, 1));
	_painter.drawRect(templateRect);

	_painter.end();
}

void BaseGraphView::adjXGridStep()
{
	_xyGridSteps.first = (_xBorders.second - _xBorders.first) / 20;
}

void BaseGraphView::adjYGridStep()
{
	_xyGridSteps.second = (_yBorders.second - _yBorders.first) / 20;
}

void BaseGraphView::updateAndRepaint()
{
	_graph = makeTemplatePix(size());
	_temp = _graph;
	repaint();
}

inline QString BaseGraphView::adjCoord(double value, double eps)
{
	if (value < -eps || value > eps)
	{
		return QString("%1").arg(value, 4, 'f', 2);
	}
	else
	{
		return "0.00";
	}
}
