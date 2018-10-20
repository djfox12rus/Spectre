#pragma once

#include <utility>
#include <tuple>
#include <vector>
#include <QPixmap>
#include <QPainter>
#include <QWidget>
#include <QApplication>

class QPixpam;

class BaseGraphView : public QWidget
{
	Q_OBJECT
public:
	BaseGraphView(QWidget* parent = nullptr);


public slots:

	void	setXLowBordes(double border);
	void	setXHighBordes(double border);
	void	setYLowBordes(double border);
	void	setYHighBordes(double border);
	void	setXGridStep(double step);
	void	setYGridStep(double step);

protected:

	const std::pair<double, double>&	xBorders();
	const std::pair<double, double>&	yBorders();
	const std::pair<double, double>&	xyGridSteps();

	QPixmap								templateGraph();
	void								redrawGraph();

	virtual QPixmap	updateGraph(QPixmap temp) = 0;
	QPointF			mapToGraph(QPointF point);
	QPointF			mapWdgtToGraph(QPointF point);
	QPointF			mapGraphToWdgt(QPointF point);
	QPointF			mapFromGraph(QPointF point);
	QPainter&		painter();

	void	paintEvent(QPaintEvent* event) override;
	void	resizeEvent(QResizeEvent* event) override;
	void	mouseMoveEvent(QMouseEvent* event) override;

private:
	QPixmap			_graph;
	QPixmap			_temp;
	QPixmap			_templateGraph;
	QPainter		_painter;

	std::pair<double, double>	_xBorders {0.0, 10.0};
	std::pair<double, double>	_yBorders {-1.0, 1.0};
	std::pair<double, double>	_xyGridSteps{0.5, 0.1};

	std::pair<double, double>	_xGraphBorders{ 0,0 };
	std::pair<double, double>	_yGraphBorders{ 0,0 };
	
	QPixmap			makeTemplatePix(QSize newSize);

	using			CoordsValues = std::vector<std::pair<double, double>>;
	std::tuple<QPixmap, QVector<QLineF>, QVector<QLineF>, CoordsValues, CoordsValues>
					makeTemplateGraphPix(QSize graphSize);

	void			insertGraph(QPixmap& graph, QPixmap templateGraph);

	void			updateAndRepaint();

	static inline QString adjCoord(double value, double eps);


};

