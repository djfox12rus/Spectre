#include "GraphFunction.h"

#include "algorithm"

GraphFunction::GraphFunction(const QPolygonF & poliline)
	:_baseLine(poliline)
{
}

double GraphFunction::operator()(double x) const
{
	if (x < _baseLine.front().x() || x >= _baseLine.back().x())
	{
		return 0.0;
	}

	auto point = std::find_if(std::begin(_baseLine), std::end(_baseLine), [x](const QPointF& point)
	{
		return x <= point.x();
	});

	if (point == std::end(_baseLine) || point == std::begin(_baseLine)) return 0.0;
	auto prevPoint = point - 1;

	const double dx = (x - prevPoint->x()) / (point->x() - prevPoint->x());
	const double dy = (point->y() - prevPoint->y());
	return dx * dy + prevPoint->y();
}
