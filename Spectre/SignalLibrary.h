#pragma once
#include <QtWidgets/QMainWindow>

#include <functional>

class QDialog;
class GraphView;

class SignalLibrary : public QMainWindow
{
	Q_OBJECT

public:
	SignalLibrary(QWidget* parent = nullptr);

	std::function<double(double)>	currentFunction() const;

signals:
	void	setTauVisible(bool visible);
	void	setAVisible(bool visible);
	void	setBVisible(bool visible);

	void	apply();

private slots:
	void	setFunctionType(int type);

private:
	enum FunctionTypes
	{
		Libsig1,
		Libsig2,
		Libsig3,
		Libsig4,
		Libsig5,
		Libsig6,
		Libsig7,
		Libsig8,
		Libsig9,
		Libsig10,
		Libsig11,
		Libsig12,
		Libsig13,
		Libsig14,
		Libsig15,
		Libsig16,
		Libsig17,
		Libsig18,
		Libsig19,
		Libsig20,
		Libsig21,
		Libsig22,
		Libsig23,
		Libsig24,
		Libsig25,
		Libsig26,
		Libsig27,
		Libsig28,
		Libsig29,
		Libsig30,
		Libsig31,
		Libsig32,
		Libsig33,
		Libsig34,
		Libsig35,
		Libsig36,
		LibsigCount
	};

	FunctionTypes	_currentType;
	double			_currentTau{1};
	double			_currentA{ 1 };
	double			_currentB{ 1 };
	GraphView*		_view;

	std::function<double(double)> _currenFunc;

	void		init();
	QMenuBar*	initMenuBar();
	QToolBar*	initChooseFuncToolBar();

	QList<QAction*>	initActions();
	QToolBar*		initToolBar(const QList<QAction*>& acts);
	QMenuBar*		initMenuBar(const QList<QAction*>& acts);

	QString		getSignalName(int type);
	void		updateFunction();

	static double libsig1(double t, double tau);			//1
	static double libsig2(double t, double tau);
	static double libsig3(double t, double tau);
	static double libsig4(double t, double tau);
	static double libsig5(double t, double tau);
	static double libsig6(double t, double tau);
	static double libsig7(double t, double tau);
	static double libsig8(double t, double tau);
	static double libsig9(double t);
	static double libsig10(double t);
	static double libsig11(double t, double a);
	static double libsig12(double t, double a);
	static double libsig13(double t, double a);
	static double libsig14(double t, double a);
	static double libsig15(double t, double a);
	static double libsig16(double t, double tau, double a);
	static double libsig17(double t, double a);
	static double libsig18(double t, double a, double b);
	static double libsig19(double t, double a);
	static double libsig20(double t, double a);
	static double libsig21(double t, double a);
	static double libsig22(double t, double a);
	static double libsig23(double t, double a);
	static double libsig24(double t, double a);
	static double libsig25(double t, double a);
	static double libsig26(double t, double a);
	static double libsig27(double t, double a);
	static double libsig28(double t, double a);
	static double libsig29(double t, double a);
	static double libsig30(double t, double a);
	static double libsig31(double t, double a);
	static double libsig32(double t, double tau);
	static double libsig33(double t, double a);
	static double libsig34(double t, double a);
	static double libsig35(double t, double a);
	static double libsig36(double t, double a);

};

