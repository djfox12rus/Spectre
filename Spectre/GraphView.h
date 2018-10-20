#pragma once
#include "BaseGraphView.h"

#include <functional>

class GraphView :
	public BaseGraphView
{
public:
	GraphView(QWidget* parent = nullptr);

	void	setFunction(std::function<double(double)> func);

protected:
	QPixmap updateGraph(QPixmap temp) override;

private:
	std::function<double(double)>		_func;
};

