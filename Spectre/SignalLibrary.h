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
	std::function<double(double)>	currentSpectre() const;

	double		currentTau() const;

signals:
	void	setTauVisible(bool visible);
	void	setAVisible(bool visible);
	void	setBVisible(bool visible);
	void	setFVisible(bool visible);
	void	apply();

	void	setCoordsValueVisible(bool show);

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
		SincPiX,
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
		Sin,
		LibsigCount
	};

	FunctionTypes	_currentType;
	double			_currentTau{1};
	double			_currentA{ 1 };
	double			_currentB{ 1 };
	int				_currentF{ 1 };
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

	std::function<double(double)>	currentSpectrePriv()const;

	static double libsig1(double t, double tau);			//1
	static double libsig2(double t, double tau);
	static double libsig3(double t, double tau);
	static double libsig4(double t, double tau);
	static double libsig5(double t, double tau);
	static double libsig6(double t, double tau);
	static double libsig7(double t, double tau);
	static double libsig8(double t, double tau);
	static double sincPiX(double t);
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
	static double libsin(double t, double f);

	static double sinc(double x);

	static double spectr1(double w, double tau);
	static double spectr2(double w, double tau);
	static double spectr3(double w, double tau);
	static double spectr4(double w, double tau);
	static double spectr5(double w, double tau);
	static double spectr6(double w, double tau);
	static double spectr7(double w, double tau);
	static double spectr8(double w, double tau);
	static double spectr9(double w);
	static double spectr10(double w);
	static double spectr11(double w, double a);
	static double spectr12(double w, double a);
	static double spectr13(double w, double a);
	static double spectr14(double w, double a);
	static double spectr15(double w, double a);
	static double spectr16(double w, double tau, double a);
	static double spectr17(double w, double a);
	static double spectr18(double w, double a, double b);
	static double spectr19(double w, double a);
	static double spectr20(double w, double a);
	static double spectr21(double w, double a);
	static double spectr22(double w, double a);
	static double spectr23(double w, double a);
	static double spectr24(double w, double a);
	static double spectr25(double w, double a);
	static double spectr26(double w, double a);
	static double spectr27(double w, double a);
	static double spectr28(double w, double a);
	static double spectr29(double w, double a);
	static double spectr30(double w, double a);
	static double spectr31(double w, double a);
	static double spectr32(double w, double tau);
	static double spectr33(double w, double a);
	static double spectr34(double w, double a);
	static double spectr35(double w, double a);
	static double spectr36(double w, double a);
	static double spectr_sin(double w, double f);
};

