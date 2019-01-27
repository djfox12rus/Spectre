#pragma once
#include "BaseGraphView.h"

#include <functional>

class GraphView :
	public BaseGraphView
{
public:
	enum DrawType
	{
		Common,
		PeriodicSpectre,
		RadioSignal
	};

	GraphView(QWidget* parent = nullptr);

	void	setFunction(std::function<double(double)> func, double omega = -1);

	void	setType(DrawType type);

protected:
	QPixmap updateGraph(QPixmap temp) final;

private:
	std::function<double(double)>		_func;
	double								_omega{-1};
	DrawType							_type{ Common };

};

