#pragma once

#include <functional>

#include <QtWidgets/QMainWindow>
#include <QList>

class SignalEditor;
class SignalLibrary;
class QTabWidget;
class QDialog;

class GraphView;

class Spectre : public QMainWindow
{
	Q_OBJECT

public:
	Spectre(QWidget *parent = nullptr);

signals:

private slots:
	void		openSignalEditor();
	void		openSignalLibrary();
	void		openParamsDialog();

	void		reciveLibFunction();
	void		reciveEditorFunction();

private:

	enum SignalType
	{
		Pulse,
		PeriodicPulse,
		RadioPulse,
		PeriodicRadioPulse
	};

	struct SignalParams
	{
		SignalType	type{Pulse};
		double		duration{0.5};
		int			dutyCycle{5};
	};

	std::function<double(double)>	_initialSignal;
	std::function<double(double)>	_initialSpectre;
	std::pair<double, double>		_initialRange;

	//BaseGraphView*		_view;
	SignalEditor*		_editor{};
	SignalLibrary*		_lib{};
	QDialog*			_signalParamsDlg{};
	SignalParams		_signalParams{};

	GraphView*			_currentGraph{};
	GraphView*			_currentSpectre{};

	void				init();
	QTabWidget*			initCentralWgt();
	QWidget*			initSignalViewTab();
	QWidget*			initSpectreViewTab();
	QList<QAction*>		initSignalActions();
	QToolBar*			initSignalToolBar(const QList<QAction*>& acts);
	QMenuBar*			initMenuBar(const QList<QAction*>& acts);

	SignalEditor*		initEditor();
	SignalLibrary*		initLibrary();
	QDialog*			initSignalParams();

	void				processSignalParams();
};
