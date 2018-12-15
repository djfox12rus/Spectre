#include "GraphView.h"


#include <QPolygonF>
#include <QPen>

static constexpr double precision = 500;
static constexpr double pointRadius = 3;

GraphView::GraphView(QWidget * parent)
	:BaseGraphView(parent)
{
}

void GraphView::setFunction(std::function<double(double)> func, double omega)
{
	_func = func;
	_omega = omega;
	updateAndRepaint();
}

QPixmap GraphView::updateGraph(QPixmap temp)
{
	if (_func)
	{
		QPolygonF points;
		points.reserve((xBorders().second - xBorders().first)/(xyGridSteps().first / precision));

		for (auto i = xBorders().first; i < xBorders().second; i += xyGridSteps().first / precision)
		{
			points.push_back(mapToGraph(QPointF(i, _func(i))));
		}

		std::vector<std::pair<QLineF, QPointF>> specteLines;
		if (_omega > 0)
		{
			if (xBorders().first < 0.0 && xBorders().second > 0.0)
			{
				double y = _func(0) / 2;
				auto yPoint = mapToGraph({ 0.0, y });
				specteLines.push_back({ { {yPoint.x(), y > 0 ? yPoint.y() + pointRadius : yPoint.y() - pointRadius }, mapToGraph({0.0, 0.0}) }, yPoint });
			}
			for (auto i = _omega; i < xBorders().second; i += _omega)
			{
				double y = _func(i);
				auto yPoint = mapToGraph({ i, y });
				specteLines.push_back({ { {yPoint.x(), y > 0 ? yPoint.y() + pointRadius : yPoint.y() - pointRadius }, mapToGraph({i, 0.0}) }, yPoint });
			}
			for (auto i = -_omega; i > xBorders().first; i -= _omega)
			{
				double y = _func(i);
				auto yPoint = mapToGraph({ i, y });
				specteLines.push_back({ { {yPoint.x(), y > 0 ? yPoint.y() + pointRadius : yPoint.y() - pointRadius }, mapToGraph({i, 0.0}) }, yPoint });
			}
		}


		auto& graphPainter = painter();

		graphPainter.begin(&temp);

		QPen p(Qt::red, 2.5, _omega > 0? Qt::DashLine : Qt::SolidLine);
		graphPainter.setPen(p);
		graphPainter.drawPolyline(points);

		if (_omega > 0)
		{
			QPen p(Qt::blue, 2.5, Qt::SolidLine);
			graphPainter.setPen(p);
			
			for (const auto& line : specteLines)
			{
				graphPainter.drawLine(line.first);
				graphPainter.drawEllipse(line.second.x() - pointRadius, line.second.y() - pointRadius, pointRadius * 2, pointRadius * 2);
			}
		}

		graphPainter.end();

	}
	return temp;
}
