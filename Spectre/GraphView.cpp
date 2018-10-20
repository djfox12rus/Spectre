#include "GraphView.h"


#include <QPolygonF>
#include <QPen>

static constexpr double precision = 50;

GraphView::GraphView(QWidget * parent)
	:BaseGraphView(parent)
{
}

void GraphView::setFunction(std::function<double(double)> func)
{
	_func = func;
}

QPixmap GraphView::updateGraph(QPixmap temp)
{
	if (_func)
	{
		QPolygonF points;

		for (auto i = xBorders().first; i < xBorders().second; i += xyGridSteps().first / precision)
		{
			points.push_back(mapToGraph(QPointF(i, _func(i))));
		}

		auto& graphPainter = painter();

		graphPainter.begin(&temp);

		QPen p(Qt::red, 2);
		graphPainter.setPen(p);
		graphPainter.drawPolyline(points);

		graphPainter.end();

	}
	return temp;
}
