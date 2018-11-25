#include "SignalLibrary.h"
#include "GraphView.h"

#define _USE_MATH_DEFINES
#include <limits>
#include <math.h>
//#include <corecrt_math_defines.h>

#include <QMenu>
#include <QBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QToolBar>
#include <QMenuBar>
#include <QAction>
#include <QList>
#include <QMargins>

static const QString titleStr = QStringLiteral("Библиотека сигналов");

SignalLibrary::SignalLibrary(QWidget * parent)
{
	init();
	setWindowTitle(titleStr);
	setWindowIcon(QPixmap(":/Spectre/Resources/black-library.png"));
}

void SignalLibrary::setFunctionType(int type)
{
	_currentType = FunctionTypes(type);
	updateFunction();

	emit setTauVisible(type >= Libsig1 && type <= Libsig8 || type == Libsig16 || type == Libsig32);
	emit setAVisible(type >= Libsig11 && type<= Libsig36 && type != Libsig32);
	emit setBVisible(type == Libsig18);

}

std::function<double(double)> SignalLibrary::currentFunction() const
{
	return _currenFunc;
}

std::function<double(double)> SignalLibrary::currentSpectre() const
{
	return currentSpectrePriv();
}

void SignalLibrary::init()
{
	_view = new GraphView;
	auto chooseToolBox = initChooseFuncToolBar();

	auto actions = initActions();

	setMenuBar(initMenuBar(actions));
	addToolBar(initToolBar(actions));
	addToolBar(initChooseFuncToolBar());

	setCentralWidget(_view);

	setFunctionType(0);

	setMinimumSize(1080, 600);
}

QMenuBar * SignalLibrary::initMenuBar()
{
	auto menuBar = new QMenuBar;

	auto chooseAct = menuBar->addAction(QStringLiteral("Выбрать сигнал..."));

	return menuBar;
}

QToolBar * SignalLibrary::initChooseFuncToolBar()
{
	auto mainLayout = new QVBoxLayout;

	auto chooseFuncBox = new QComboBox;
	for (int i = 0; i < LibsigCount; ++i)
	{
		chooseFuncBox->addItem(getSignalName(i));
	}

	connect(chooseFuncBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SignalLibrary::setFunctionType);

	mainLayout->addWidget(chooseFuncBox);
	
	auto tauSpinBox = new QDoubleSpinBox;
	tauSpinBox->setMaximumWidth(50);
	tauSpinBox->setRange(0.0, std::numeric_limits<double>::max());
	tauSpinBox->setSingleStep(0.01);
	tauSpinBox->setValue(_currentTau);
	connect(tauSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double tau) 
	{
		_currentTau = tau;
		updateFunction();
	});

	auto ASpinBox = new QDoubleSpinBox;
	ASpinBox->setMaximumWidth(50);
	ASpinBox->setRange(0.0, std::numeric_limits<double>::max());
	ASpinBox->setSingleStep(0.01);
	ASpinBox->setValue(_currentA);
	connect(ASpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double a)
	{
		_currentA = a;
		updateFunction();
	});

	auto BSpinBox = new QDoubleSpinBox;
	BSpinBox->setMaximumWidth(50);
	BSpinBox->setRange(0.0, std::numeric_limits<double>::max());
	BSpinBox->setSingleStep(0.01);
	BSpinBox->setValue(_currentB);
	connect(BSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double b)
	{
		_currentB = b;
		updateFunction();
	});

	auto tauLbl = new QLabel(QStringLiteral("Tau"));
	auto aLbl = new QLabel(QStringLiteral("A"));
	auto bLbl = new QLabel(QStringLiteral("B"));

	connect(this, &SignalLibrary::setTauVisible, tauLbl, &QLabel::setVisible);
	connect(this, &SignalLibrary::setAVisible, aLbl, &QLabel::setVisible);
	connect(this, &SignalLibrary::setBVisible, bLbl, &QLabel::setVisible);

	connect(this, &SignalLibrary::setTauVisible, tauSpinBox, &QDoubleSpinBox::setVisible);
	connect(this, &SignalLibrary::setAVisible, ASpinBox, &QDoubleSpinBox::setVisible);
	connect(this, &SignalLibrary::setBVisible, BSpinBox, &QDoubleSpinBox::setVisible);

	auto helperLayout = new QHBoxLayout;
	helperLayout->addWidget(chooseFuncBox);

	auto tempLayout = new QHBoxLayout;
	tempLayout->addWidget(tauLbl);
	tempLayout->addWidget(tauSpinBox);
	helperLayout->addLayout(tempLayout);

	tempLayout = new QHBoxLayout;
	tempLayout->addWidget(aLbl);
	tempLayout->addWidget(ASpinBox);
	helperLayout->addLayout(tempLayout);

	tempLayout = new QHBoxLayout;
	tempLayout->addWidget(bLbl);
	tempLayout->addWidget(BSpinBox);
	helperLayout->addLayout(tempLayout);

	auto helperWgt = new QWidget;
	helperWgt->setLayout(helperLayout);

	helperLayout->setContentsMargins(0, 0, 0, 0);

	auto box = new QToolBar;
	box->addWidget(helperWgt);

	connect(box, &QToolBar::orientationChanged, [helperLayout](Qt::Orientation orientation)
	{
		if (orientation == Qt::Vertical)
		{
			helperLayout->setDirection(QBoxLayout::TopToBottom);
		}
		else
		{
			helperLayout->setDirection(QBoxLayout::LeftToRight);
		}
	});

	return box;
}

QList<QAction*> SignalLibrary::initActions()
{
	QList<QAction*> acts;

	acts.push_back(new QAction(QIcon(":/Spectre/Resources/verification-mark.png"), QStringLiteral("Принять"), this));
	acts.back()->setShortcut(Qt::Key_Enter | Qt::Key_Return);
	connect(acts.back(), &QAction::triggered, this, &SignalLibrary::apply);
	connect(acts.back(), &QAction::triggered, this, &SignalLibrary::close);

	acts.push_back(new QAction(QIcon(":/Spectre/Resources/exit.png"), QStringLiteral("Закрыть"), this));
	acts.back()->setShortcut(Qt::Key_Escape);
	connect(acts.back(), &QAction::triggered, this, &SignalLibrary::close);

	return acts;
}

QToolBar * SignalLibrary::initToolBar(const QList<QAction*>& acts)
{
	auto toolBar = new QToolBar;
	toolBar->addActions(acts);

	return toolBar;
}

QMenuBar * SignalLibrary::initMenuBar(const QList<QAction*>& acts)
{
	auto menuBar = new QMenuBar;

	auto menu = menuBar->addMenu(QStringLiteral("Сигнал"));

	menu->addActions(acts);

	return menuBar;
}

QString SignalLibrary::getSignalName(int type)
{
	switch (FunctionTypes(type))
	{
	case SignalLibrary::Libsig1: return QStringLiteral("Сигнал 1");
		break;
	case SignalLibrary::Libsig2: return QStringLiteral("Сигнал 2");
		break;
	case SignalLibrary::Libsig3: return QStringLiteral("Сигнал 3");
		break;
	case SignalLibrary::Libsig4: return QStringLiteral("Сигнал 4");
		break;
	case SignalLibrary::Libsig5: return QStringLiteral("Сигнал 5");
		break;
	case SignalLibrary::Libsig6: return QStringLiteral("Сигнал 6");
		break;
	case SignalLibrary::Libsig7: return QStringLiteral("Сигнал 7");
		break;
	case SignalLibrary::Libsig8: return QStringLiteral("Сигнал 8");
		break;
	case SignalLibrary::SincPiX: return QStringLiteral("Сигнал 9");
		break;
	case SignalLibrary::Libsig10: return QStringLiteral("Сигнал 10");
		break;
	case SignalLibrary::Libsig11: return QStringLiteral("Сигнал 11");
		break;
	case SignalLibrary::Libsig12: return QStringLiteral("Сигнал 12");
		break;
	case SignalLibrary::Libsig13: return QStringLiteral("Сигнал 13");
		break;
	case SignalLibrary::Libsig14: return QStringLiteral("Сигнал 14");
		break;
	case SignalLibrary::Libsig15: return QStringLiteral("Сигнал 15");
		break;
	case SignalLibrary::Libsig16: return QStringLiteral("Сигнал 16");
		break;
	case SignalLibrary::Libsig17: return QStringLiteral("Сигнал 17");
		break;
	case SignalLibrary::Libsig18: return QStringLiteral("Сигнал 18");
		break;
	case SignalLibrary::Libsig19: return QStringLiteral("Сигнал 19");
		break;
	case SignalLibrary::Libsig20: return QStringLiteral("Сигнал 20");
		break;
	case SignalLibrary::Libsig21: return QStringLiteral("Сигнал 21");
		break;
	case SignalLibrary::Libsig22: return QStringLiteral("Сигнал 22");
		break;
	case SignalLibrary::Libsig23: return QStringLiteral("Сигнал 23");
		break;
	case SignalLibrary::Libsig24: return QStringLiteral("Сигнал 24");
		break;
	case SignalLibrary::Libsig25: return QStringLiteral("Сигнал 25");
		break;
	case SignalLibrary::Libsig26: return QStringLiteral("Сигнал 26");
		break;
	case SignalLibrary::Libsig27: return QStringLiteral("Сигнал 27");
		break;
	case SignalLibrary::Libsig28: return QStringLiteral("Сигнал 28");
		break;
	case SignalLibrary::Libsig29: return QStringLiteral("Сигнал 29");
		break;
	case SignalLibrary::Libsig30: return QStringLiteral("Сигнал 30");
		break;
	case SignalLibrary::Libsig31: return QStringLiteral("Сигнал 31");
		break;
	case SignalLibrary::Libsig32: return QStringLiteral("Сигнал 32");
		break;
	case SignalLibrary::Libsig33: return QStringLiteral("Сигнал 33");
		break;
	case SignalLibrary::Libsig34: return QStringLiteral("Сигнал 34");
		break;
	case SignalLibrary::Libsig35: return QStringLiteral("Сигнал 35");
		break;
	case SignalLibrary::Libsig36: return QStringLiteral("Сигнал 36");
		break;
	case SignalLibrary::LibsigCount:
		break;
	default:
		break;
	}
	return QString();
}

void SignalLibrary::updateFunction()
{
	switch (_currentType)
	{
	case SignalLibrary::Libsig1:
	{
		_currenFunc = [&tau = _currentTau](double x) -> double
		{
			return libsig1(x, tau);
		};
	}
	break;
	case SignalLibrary::Libsig2:
	{
		_currenFunc = [&tau = _currentTau](double x) -> double
		{
			return libsig2(x, tau);
		};
	}
	break;
	case SignalLibrary::Libsig3:
	{
		_currenFunc = [&tau = _currentTau](double x) -> double
		{
			return libsig3(x, tau);
		};
	}
	break;
	case SignalLibrary::Libsig4:
	{
		_currenFunc = [&tau = _currentTau](double x) -> double
		{
			return libsig4(x, tau);
		};
	}
	break;
	case SignalLibrary::Libsig5:
	{
		_currenFunc = [&tau = _currentTau](double x) -> double
		{
			return libsig5(x, tau);
		};
	}
	break;
	case SignalLibrary::Libsig6:
	{
		_currenFunc = [&tau = _currentTau](double x) -> double
		{
			return libsig6(x, tau);
		};
	}
	break;
	case SignalLibrary::Libsig7:
	{
		_currenFunc = [&tau = _currentTau](double x) -> double
		{
			return libsig7(x, tau);
		};
	}
	break;
	case SignalLibrary::Libsig8:
	{
		_currenFunc = [&tau = _currentTau](double x) -> double
		{
			return libsig8(x, tau);
		};
	}
	break;
	case SignalLibrary::SincPiX:
	{
		_currenFunc = &sincPiX;
	}
	break;
	case SignalLibrary::Libsig10:
	{
		_currenFunc = &libsig10;
	}
	break;
	case SignalLibrary::Libsig11:
	{
		_currenFunc = [&a = _currentA](double x) -> double
		{
			return libsig11(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig12:
	{
		_currenFunc = [&a = _currentA](double x) -> double
		{
			return libsig12(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig13:
	{
		_currenFunc = [&a = _currentA](double x) -> double
		{
			return libsig13(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig14:
	{
		_currenFunc = [&a = _currentA](double x) -> double
		{
			return libsig14(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig15:
	{
		_currenFunc = [&a = _currentA](double x) -> double
		{
			return libsig15(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig16:
	{
		_currenFunc = [&tau = _currentTau, &a = _currentA](double x) -> double
		{
			return libsig16(x, tau, a);
		};
	}
	break;
	case SignalLibrary::Libsig17:
	{
		_currenFunc = [&a = _currentA](double x) -> double
		{
			return libsig17(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig18:
	{
		_currenFunc = [&a = _currentA, &b = _currentB](double x) -> double
		{
			return libsig18(x, a, b);
		};
	}
	break;
	case SignalLibrary::Libsig19:
	{
		_currenFunc = [&a = _currentA](double x) -> double
		{
			return libsig19(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig20:
	{
		_currenFunc = [&a = _currentA](double x) -> double
		{
			return libsig20(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig21:
	{
		_currenFunc = [&a = _currentA](double x) -> double
		{
			return libsig21(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig22:
	{
		_currenFunc = [&a = _currentA](double x) -> double
		{
			return libsig22(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig23:
	{
		_currenFunc = [&a = _currentA](double x) -> double
		{
			return libsig23(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig24:
	{
		_currenFunc = [&a = _currentA](double x) -> double
		{
			return libsig24(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig25:
	{
		_currenFunc = [&a = _currentA](double x) -> double
		{
			return libsig25(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig26:
	{
		_currenFunc = [&a = _currentA](double x) -> double
		{
			return libsig26(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig27:
	{
		_currenFunc = [&a = _currentA](double x) -> double
		{
			return libsig27(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig28:
	{
		_currenFunc = [&a = _currentA](double x) -> double
		{
			return libsig28(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig29:
	{
		_currenFunc = [&a = _currentA](double x) -> double
		{
			return libsig29(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig30:
	{
		_currenFunc = [&a = _currentA](double x) -> double
		{
			return libsig30(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig31:
	{
		_currenFunc = [&a = _currentA](double x) -> double
		{
			return libsig31(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig32:
	{
		_currenFunc = [&tau = _currentTau](double x) -> double
		{
			return libsig32(x, tau);
		};
	}
	break;
	case SignalLibrary::Libsig33:
	{
		_currenFunc = [&a = _currentA](double x) -> double
		{
			return libsig33(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig34:
	{
		_currenFunc = [&a = _currentA](double x) -> double
		{
			return libsig34(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig35:
	{
		_currenFunc = [&a = _currentA](double x) -> double
		{
			return libsig35(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig36:
	{
		_currenFunc = [&a = _currentA](double x) -> double
		{
			return libsig36(x, a);
		};
	}
	break;
	default:
		break;
	}


	_view->setFunction(_currenFunc);

}

std::function<double(double)> SignalLibrary::currentSpectrePriv() const
{
	switch (_currentType)
	{
	case SignalLibrary::Libsig1:
	{
		return [&tau = _currentTau](double x) -> double
		{
			return spectr1(x, tau);
		};
	}
	break;
	case SignalLibrary::Libsig2:
	{
		return [&tau = _currentTau](double x) -> double
		{
			return spectr2(x, tau);
		};
	}
	break;
	case SignalLibrary::Libsig3:
	{
		return [&tau = _currentTau](double x) -> double
		{
			return spectr3(x, tau);
		};
	}
	break;
	case SignalLibrary::Libsig4:
	{
		return [&tau = _currentTau](double x) -> double
		{
			return spectr4(x, tau);
		};
	}
	break;
	case SignalLibrary::Libsig5:
	{
		return [&tau = _currentTau](double x) -> double
		{
			return spectr5(x, tau);
		};
	}
	break;
	case SignalLibrary::Libsig6:
	{
		return [&tau = _currentTau](double x) -> double
		{
			return spectr6(x, tau);
		};
	}
	break;
	case SignalLibrary::Libsig7:
	{
		return [&tau = _currentTau](double x) -> double
		{
			return spectr7(x, tau);
		};
	}
	break;
	case SignalLibrary::Libsig8:
	{
		return [&tau = _currentTau](double x) -> double
		{
			return spectr8(x, tau);
		};
	}
	break;
	case SignalLibrary::SincPiX:
	{
		return &spectr9;
	}
	break;
	case SignalLibrary::Libsig10:
	{
		return &spectr10;
	}
	break;
	case SignalLibrary::Libsig11:
	{
		return [&a = _currentA](double x) -> double
		{
			return spectr11(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig12:
	{
		return [&a = _currentA](double x) -> double
		{
			return spectr12(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig13:
	{
		return [&a = _currentA](double x) -> double
		{
			return spectr13(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig14:
	{
		return [&a = _currentA](double x) -> double
		{
			return spectr14(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig15:
	{
		return [&a = _currentA](double x) -> double
		{
			return spectr15(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig16:
	{
		return [&tau = _currentTau, &a = _currentA](double x) -> double
		{
			return spectr16(x, tau, a);
		};
	}
	break;
	case SignalLibrary::Libsig17:
	{
		return [&a = _currentA](double x) -> double
		{
			return spectr17(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig18:
	{
		return [&a = _currentA, &b = _currentB](double x) -> double
		{
			return spectr18(x, a, b);
		};
	}
	break;
	case SignalLibrary::Libsig19:
	{
		return [&a = _currentA](double x) -> double
		{
			return spectr19(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig20:
	{
		return [&a = _currentA](double x) -> double
		{
			return spectr20(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig21:
	{
		return [&a = _currentA](double x) -> double
		{
			return spectr21(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig22:
	{
		return [&a = _currentA](double x) -> double
		{
			return spectr22(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig23:
	{
		return [&a = _currentA](double x) -> double
		{
			return spectr23(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig24:
	{
		return [&a = _currentA](double x) -> double
		{
			return spectr24(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig25:
	{
		return [&a = _currentA](double x) -> double
		{
			return spectr25(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig26:
	{
		return [&a = _currentA](double x) -> double
		{
			return spectr26(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig27:
	{
		return [&a = _currentA](double x) -> double
		{
			return spectr27(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig28:
	{
		return [&a = _currentA](double x) -> double
		{
			return spectr28(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig29:
	{
		return [&a = _currentA](double x) -> double
		{
			return spectr29(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig30:
	{
		return [&a = _currentA](double x) -> double
		{
			return spectr30(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig31:
	{
		return [&a = _currentA](double x) -> double
		{
			return spectr31(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig32:
	{
		return [&tau = _currentTau](double x) -> double
		{
			return spectr32(x, tau);
		};
	}
	break;
	case SignalLibrary::Libsig33:
	{
		return [&a = _currentA](double x) -> double
		{
			return spectr33(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig34:
	{
		return [&a = _currentA](double x) -> double
		{
			return spectr34(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig35:
	{
		return [&a = _currentA](double x) -> double
		{
			return spectr35(x, a);
		};
	}
	break;
	case SignalLibrary::Libsig36:
	{
		return [&a = _currentA](double x) -> double
		{
			return spectr36(x, a);
		};
	}
	break;
	default:
		break;
	}
}


double SignalLibrary::libsig1(double t, double tau)
{
	const double x = t / tau;
	if ((x < -0.5) || (x > 0.5)) return 0.0;
	return 1.0;
}

double SignalLibrary::libsig2(double t, double tau)
{
	if (t / tau < 0.0) return libsig1(t, tau);
	return -libsig1(t, tau);
}

double SignalLibrary::libsig3(double t, double tau)
{
	double x = fabs(t / tau);
	if (x > 0.5) return 0;
	return 1.0 - 2.0 * x;
}

double SignalLibrary::libsig4(double t, double tau)
{
	double x = fabs(t / tau);
	if (x >= 0.5) return 0.0;
	if (x < 1.0 / 6.0) return 1.0;
	return 3.0 * (0.5 - x);
}

double SignalLibrary::libsig5(double t, double tau)
{
	double x = fabs(t / tau);
	if (x >= 0.5) return 0.0;
	x *= M_PI;
	while (x > 1.0e+15) x -= 2 * M_PI;
	return cos(x);
}

double SignalLibrary::libsig6(double t, double tau)
{
	const double y = libsig5(t, tau);
	return y * y;
}

double SignalLibrary::libsig7(double t, double tau)
{
	if ((fabs(t) / tau) > 0.5) return 0.0;
	t = 2.0 * M_PI * t / tau;
	while (t > 1.0e+15) t -= 2 * M_PI;
	return fabs(sin(t));
}

double SignalLibrary::libsig8(double t, double tau)
{
	const double y = libsig7(t, tau);
	return y*y;
}

double SignalLibrary::sincPiX(double t)
{
	if (t == 0.0) return 1.0;
	t *= M_PI;
	double x = t;
	while (x > 1.0e+15) x -= 2 * M_PI;
	return sin(x) / t;
}

double SignalLibrary::libsig10(double t)
{
	const double y = sincPiX(t);
	return y * y;
}

double SignalLibrary::libsig11(double t, double a)
{
	t *= a;
	if (t > 700.0) return 0.0;
	return 1.0 / cosh(t);
}

double SignalLibrary::libsig12(double t, double a)
{
	const double y = libsig11(t, a);
	return y * y;
}

double SignalLibrary::libsig13(double t, double a)
{
	return a * t * libsig11(t, a);
}

double SignalLibrary::libsig14(double t, double a)
{
	if (t == 0.0) return 1.0;
	t *= a;
	if (t > 700.0) return 0.0;
	return t / sinh(t);
}

double SignalLibrary::libsig15(double t, double a)
{
	if (t < 0.0) return 0.0;
	t *= a;
	return exp(-t);
}

double SignalLibrary::libsig16(double t, double tau, double a)
{
	if (t / tau > 1.0) return 0.0;
	return libsig15(t, a);
}

double SignalLibrary::libsig17(double t, double a)
{
	t = a * fabs(t);
	return exp(-t);
}

double SignalLibrary::libsig18(double t, double a, double b)
{
	if (t < 0.0) return 0.0;
	return exp(-t * a) - exp(-t * b);
}

double SignalLibrary::libsig19(double t, double a)
{
	if (t < 0.0) return 0.0;
	return t * exp(-t * a);
}

double SignalLibrary::libsig20(double t, double a)
{
	return libsig19(t, a) * t;
}

double SignalLibrary::libsig21(double t, double a)
{
	return libsig19(t, a) * t * t;
}

double SignalLibrary::libsig22(double t, double a)
{
	return libsig19(t, a) * t * t * t;
}

double SignalLibrary::libsig23(double t, double a)
{
	return exp(-a * t*t);
}

double SignalLibrary::libsig24(double t, double a)
{
	return libsig23(t, a) * t;
}

double SignalLibrary::libsig25(double t, double a)
{
	return libsig24(t, a) * t;
}

double SignalLibrary::libsig26(double t, double a)
{
	if (t < 0.0) return 0.0;
	t *= a;
	return (1.0 - t)*exp(-t);
}

double SignalLibrary::libsig27(double t, double a)
{
	if (t < 0.0) return 0.0;
	return t * (1.0 - 0.5*a*t)*exp(-a * t);
}

double SignalLibrary::libsig28(double t, double a)
{
	if (t < 0.0) return 0;
	return t * t*(1.0 - a * t / 3.0)* exp(-a * t);
}

double SignalLibrary::libsig29(double t, double a)
{
	if (t < 0.0) return 0.0;
	return t * t*t*(1.0 - a * t / 4.0)*exp(-a * t);
}

double SignalLibrary::libsig30(double t, double a)
{
	if (t < 0.0) return 0.0;
	return (1 + a * t)*exp(-a * t);
}

double SignalLibrary::libsig31(double t, double a)
{
	return libsig30(t, a)*t;
}

double SignalLibrary::libsig32(double t, double tau)
{
	double x = fabs(t / tau);
	if (x > 0.5) return 0.0;
	return 1.0 - 4.0 * x * x;
}

double SignalLibrary::libsig33(double t, double a)
{
	return a * a / (a*a + t * t);
}

double SignalLibrary::libsig34(double t, double a)
{
	return t / (a*a + t * t);
}

double SignalLibrary::libsig35(double t, double a)
{
	if (t < 0.0) return 0.0;
	return (1.0 - t)*exp(-a * t);
}

double SignalLibrary::libsig36(double t, double a)
{
	if (t < 0.0) return 0.0;
	return (0.5*t*t - 2.0*t + 1.0)*exp(-a * t);
}

double SignalLibrary::sinc(double x)
{
	if (x == 0.0) return 1.0;
	double z = x;
	while (z > 1.0e+15) z -= 2 * M_PI;
	return sin(z) / x;
}

double SignalLibrary::spectr1(double w, double tau)
{
	if (w == 0) return tau;
	w *= (0.5 * tau);
	return fabs(tau*sinc(w));
}

double SignalLibrary::spectr2(double w, double tau)
{
	if (w == 0.0) return 0.0;
	w /= 4.0;
	double u = w * tau;
	while (u > 1.0e+15) u -= 2.0*M_PI;
	return fabs(1.0 / w * sin(u) * sin(u));
}

double SignalLibrary::spectr3(double w, double tau)
{
	w /= 4.0;
	double u = w * tau;
	return fabs(0.5*tau * sinc(u) * sinc(u));
}

double SignalLibrary::spectr4(double w, double tau)
{
	double u = w * tau / 3.0;
	return fabs(2.0*tau / 3.0 * sinc(u)*sinc(u / 2.0));
}

double SignalLibrary::spectr5(double w, double tau)
{
	double z = 1.0 - w * w * tau*tau / (M_PI*M_PI);
	if (z == 0.0) return 2.0 * tau / M_PI;
	double u = w * tau / 2.0;
	while (u > 1.0e+15) u -= 2.0*M_PI;
	return fabs(2.0*tau / M_PI * cos(u) / z);
}

double SignalLibrary::spectr6(double w, double tau)
{
	double z = 1.0 - w * w * tau*tau / (4.0*M_PI*M_PI);
	if (z == 0.0) return tau / (4.0*M_PI);
	return fabs(0.5*tau*sinc(0.5*w*tau) / z);
}

double SignalLibrary::spectr7(double w, double tau)
{
	double z = 1.0 - w * w * tau*tau / (4.0*M_PI*M_PI);
	if (z == 0.0) return 0.0;
	w *= (0.25*tau);
	while (w > 1e+15) w -= 2.0 * M_PI;
	return fabs(2.0*tau / M_PI * cos(w)*cos(w) / z);
}

double SignalLibrary::spectr8(double w, double tau)
{
	double z = 1.0 - w * w * tau*tau / (16.0*M_PI*M_PI);
	if (z == 0.0) return 0.0;
	return fabs(0.5*tau*sinc(w*tau*0.5) / z);
}

double SignalLibrary::spectr9(double w)
{
	if (w < 0.0) return 0.0;
	if (w > 2.0 * M_PI) return 0.0;
	return 1.0;
}

double SignalLibrary::spectr10(double w)
{
	if ((w < 0) || (w > 2 * M_PI)) return 0.0;
	return 1.0 - fabs(w)*0.5 / M_PI;
}

double SignalLibrary::spectr11(double w, double a)
{
	w *= (M_PI / (2 * a));
	if (w > 700.0) return 0.0;
	return fabs(M_PI / (a*cosh(w)));
}

double SignalLibrary::spectr12(double w, double a)
{
	w *= M_PI / (2.0*a);
	double k = 2.0 / a;
	if (w == 0.0) return k;
	if (w > 700.0) return 0.0;
	return fabs(k * w / sinh(w));
}

double SignalLibrary::spectr13(double w, double a)
{
	w *= M_PI / (2.0 * a);
	if (w > 700.0) return 0.0;
	return fabs(M_PI*M_PI*0.5 / (a*a) * sinh(w) / (cosh(w)*cosh(w)));
}

double SignalLibrary::spectr14(double w, double a)
{
	double k = 0.5 * M_PI / a;
	w *= k;
	if (w > 700.0) return 0.0;
	return fabs(M_PI*k / (cosh(w)*cosh(w)));
}

double SignalLibrary::spectr15(double w, double a)
{
	return 1.0 / sqrt(a*a + w * w);
}

double SignalLibrary::spectr16(double w, double tau, double a)
{
	double u = w * tau;
	if (u > 700.0) return 0.0;
	return sqrt(1.0 + exp(-2.0*a*tau) - 2.0*exp(-a * tau)*cos(u)) / sqrt(a*a + w * w);
}

double SignalLibrary::spectr17(double w, double a)
{
	return 2.0*a / (a*a + w * w);
}

double SignalLibrary::spectr18(double w, double a, double b)
{
	return fabs((b - a) / (sqrt(a*a + w * w)*sqrt(b*b + w * w)));
}

double SignalLibrary::spectr19(double w, double a)
{
	return 1.0 / (a*a + w * w);
}

double SignalLibrary::spectr20(double w, double a)
{
	return 2.0*spectr19(w, a) / sqrt(a*a + w * w);
}

double SignalLibrary::spectr21(double w, double a)
{
	const double res = spectr19(w, a);
	return 6.0*res*res;
}

double SignalLibrary::spectr22(double w, double a)
{
	return 4.0*spectr21(w, a) / sqrt(a*a + w * w);
}

double SignalLibrary::spectr23(double w, double a)
{
	return sqrt(M_PI / a)*exp(-w * w / (4.0*a));
}

double SignalLibrary::spectr24(double w, double a)
{
	return w * 0.5 / a * spectr23(w, a);
}

double SignalLibrary::spectr25(double w, double a)
{
	return fabs(1 - w * w / (2 * a)) / (2 * a) * spectr23(w, a);
}

double SignalLibrary::spectr26(double w, double a)
{
	return spectr19(w, a)*fabs(w);
}

double SignalLibrary::spectr27(double w, double a)
{
	return 0.5*spectr20(w, a)*fabs(w);
}

double SignalLibrary::spectr28(double w, double a)
{
	return 2.0*spectr27(w, a) / sqrt(a*a + w * w);
}

double SignalLibrary::spectr29(double w, double a)
{
	return 3.0*spectr28(w, a) / sqrt(a*a + w * w);
}

double SignalLibrary::spectr30(double w, double a)
{
	return sqrt(4.0*a*a + w * w)*spectr19(w, a);
}

double SignalLibrary::spectr31(double w, double a)
{
	return sqrt((9.0*a*a + w * w) / (a*a + w * w))*spectr19(w, a);
}

double SignalLibrary::spectr32(double w, double tau)
{
	double w2 = w * w;
	if (w2 == 0.0) return 2.0 * tau / 3.0;
	double u = w * tau * 0.5;
	while (u > 1.0e+15) u -= 2.0 * M_PI;
	return fabs(8.0 / (tau*w2)*(sinc(w*tau*0.5) - cos(u)));
}

double SignalLibrary::spectr33(double w, double a)
{
	return a * spectr34(w, a);
}

double SignalLibrary::spectr34(double w, double a)
{
	return M_PI * exp(-a * fabs(w));
}

double SignalLibrary::spectr35(double w, double a)
{
	return sqrt((a - 1)*(a - 1) + w * w)*spectr19(w, a);
}

double SignalLibrary::spectr36(double w, double a)
{
	return ((a - 1)*(a - 1) + w * w)*0.5*spectr20(w, a);
}
