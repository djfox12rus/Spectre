#pragma once
#include "BaseGraphView.h"

#include <functional>

class GraphView :
	public BaseGraphView
{
public:
	GraphView(QWidget* parent = nullptr);

	void	setFunction(std::function<double(double)> func, double omega = -1);

protected:
	QPixmap updateGraph(QPixmap temp) override;

private:
	std::function<double(double)>		_func;
	double								_omega{-1};
};

