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

	QPixmap	pixmap() const;

public slots:

	void	setXLowBorder(double border);
	void	setXHighBorder(double border);
	void	setYLowBorder(double border);
	void	setYHighBorder(double border);
	

	void	setXAxisName(const QString& name);
	void	setYAxisName(const QString& name);

	void	setShowCoordsValues(bool show);
	//void	setXGridStep(double step);
	//void	setYGridStep(double step);

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

	void	updateAndRepaint();

private:
	QPixmap			_graph;
	QPixmap			_temp;
	QPixmap			_templateGraph;
	QPainter		_painter;

	QString			_yAxisName;
	QString			_xAxisName;

	bool			_showCoordsValues{true};

	std::pair<double, double>	_xBorders {-5, 5};
	std::pair<double, double>	_yBorders {-1.0, 1.0};
	std::pair<double, double>	_xyGridSteps{0.5, 0.1};

	std::pair<double, double>	_xGraphBorders{ 0,0 };
	std::pair<double, double>	_yGraphBorders{ 0,0 };
	
	QPixmap			makeTemplatePix(QSize newSize);

	using			CoordsValues = std::vector<std::pair<double, double>>;
	std::tuple<QPixmap, QVector<QLineF>, QVector<QLineF>, CoordsValues, CoordsValues, QPointF>
					makeTemplateGraphPix(QSize graphSize);

	void			insertGraph(QPixmap& graph, QPixmap templateGraph);

	void			adjXGridStep();
	void			adjYGridStep();

	static inline QString adjCoord(double value, double eps);


};

