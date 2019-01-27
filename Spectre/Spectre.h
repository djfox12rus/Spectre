#pragma once

#include <functional>

#include <QtWidgets/QMainWindow>
#include <QList>

//#define FFT_TEST

class SignalEditor;
class SignalLibrary;
class QTabWidget;
class QDialog;
class QDoubleSpinBox;

class GraphView;

class Spectre : public QMainWindow
{
	Q_OBJECT

public:
	Spectre(QWidget *parent = nullptr);

signals:
	void		setCoordsValueVisible(bool show);
	void		updateParamsDlg();

protected:
	void		wheelEvent(QWheelEvent* ev) override;

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
		double		period{2};
		int			carrierFreq{ 1000 };
#ifdef FFT_TEST
		bool		useFFT{true};
#endif
	};

	struct SignalBorders
	{
		QDoubleSpinBox*	yHigh{};
		QDoubleSpinBox*	yLow{};
		QDoubleSpinBox*	xHigh{};
		QDoubleSpinBox*	xLow{};
	};
	
	struct SpectreBorders
	{
		QDoubleSpinBox*	yHigh{};
#ifdef FFT_TEST
		QDoubleSpinBox*	yLow{};
#endif
		QDoubleSpinBox*	xHigh{};
		QDoubleSpinBox*	xLow{};
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
	SignalBorders		_signalBorders;
	SpectreBorders		_spectreBorders;

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

	void				scrollX(int step);
	void				scrollY(int step);

	static std::function<double(double)> makeFourierFunction(std::function<double(double)>& initSignal, double tau);

	static double		updateValue(double initVal, int step);
};
