#pragma once

#include <QPolygonF>

class GraphFunction
{
public:
	GraphFunction(const QPolygonF& poliline);

	double operator()(double x) const;

private:
	QPolygonF	_baseLine;
};

