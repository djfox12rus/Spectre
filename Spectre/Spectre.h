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

	void		saveAs();

private:

	enum SignalType
	{
		Pulse,
		PeriodicPulse,
		RadioPulse,
		PeriodicRadioPulse
	};

	enum CurrentTab
	{
		SignalTab,
		SpectreTab
	};

	struct SignalParams
	{
		SignalType	type{Pulse};
		double		duration{1};
		int			dutyCycle{5};
	};

	std::function<double(double)>	_initialSignal;
	std::function<double(double)>	_initialSpectre;
	//std::pair<double, double>		_initialRange;

	//BaseGraphView*		_view;
	SignalEditor*		_editor{};
	SignalLibrary*		_lib{};
	QDialog*			_signalParamsDlg{};
	SignalParams		_signalParams{};

	double				_tau{0};

	GraphView*			_currentGraph{};
	GraphView*			_currentSpectre{};

	CurrentTab			_currentTab{ SignalTab };

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

	static std::function<double(double)> makeFourierFunction(std::function<double(double)>& initSignal, double tau);
};
