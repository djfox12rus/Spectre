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

void GraphView::setType(DrawType type)
{
	_type = type;
}

QPixmap GraphView::updateGraph(QPixmap temp)
{
	if (_func)
	{
		QPolygonF points;

		int steps = std::abs((xBorders().second - xBorders().first) / (xyGridSteps().first / precision));
		int toReserve = steps;
		if (_type == RadioSignal)
		{
			toReserve *= 2;
		}
		points.resize(toReserve);

		auto x_i = xBorders().first;
		for (int i = 0; i < steps; ++i, x_i += xyGridSteps().first / precision)
		{
			QPointF point{ x_i, _func(x_i) };
			points[i] = mapToGraph( point);
			if (_type == RadioSignal)
			{
				points[toReserve - i - 1] = mapToGraph({ x_i, -point.y() });
			}
		}

		std::vector<std::pair<QLineF, QPointF>> specteLines;
		if (_type == PeriodicSpectre)
		{
			if (xBorders().first <= 0.0 && xBorders().second > 0.0)
			{
				double y = _func(0) / 2;
				auto yPoint = mapToGraph({ 0.0, y });
				specteLines.push_back({ { {yPoint.x(), y > 0 ? yPoint.y() + pointRadius : yPoint.y() - pointRadius }, mapToGraph({0.0, 0.0}) }, yPoint });
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
			else
			{
				if (xBorders().first > 0)
				{
					double firstOmega = 0;
					while (firstOmega < xBorders().first) firstOmega += _omega;
					for (auto i = firstOmega; i < xBorders().second; i += _omega)
					{
						double y = _func(i);
						auto yPoint = mapToGraph({ i, y });
						specteLines.push_back({ { {yPoint.x(), y > 0 ? yPoint.y() + pointRadius : yPoint.y() - pointRadius }, mapToGraph({i, 0.0}) }, yPoint });
					}
				}
				else if (xBorders().second < 0)
				{
					double firstOmega = 0;
					while (firstOmega > xBorders().second) firstOmega -= _omega;
					for (auto i = firstOmega; i > xBorders().first; i -= _omega)
					{
						double y = _func(i);
						auto yPoint = mapToGraph({ i, y });
						specteLines.push_back({ { {yPoint.x(), y > 0 ? yPoint.y() + pointRadius : yPoint.y() - pointRadius }, mapToGraph({i, 0.0}) }, yPoint });
					}
				}
			}
		}

		auto& graphPainter = painter();

		graphPainter.begin(&temp);

		QPen p(Qt::red, 2.5, _type == Common ? Qt::SolidLine : Qt::DashLine);
		graphPainter.setPen(p);
		if (_type == RadioSignal)
		{
			graphPainter.setBrush({ "#ff5e5e" });
			graphPainter.drawPolygon(points);
		}
		else
		{
			graphPainter.drawPolyline(points);
		}

		if (_type == PeriodicSpectre)
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
