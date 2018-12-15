#pragma once

#include <functional>

#include <QtWidgets/QMainWindow>

class GraphEditor;

class SignalEditor : public QMainWindow
{
	Q_OBJECT

public:
	SignalEditor(QWidget* parent = nullptr);

	std::function<double(double)>	currentFunction() const;
	std::pair<double, double>		xRange()const;
	double							tau()const;
	bool							isFunctionEmpty()const;

signals:
	void	apply();

private:
	GraphEditor *	_editor{};
	double			_tau{1.0};

	void				init();
	QList<QAction*>		initActions();
	QToolBar*			initToolBar(const QList<QAction*>& acts);
	QToolBar*			initTauBar();
	QMenuBar*			initMenuBar(const QList<QAction*>& acts);

	void				setEditorRange(double t);

};

