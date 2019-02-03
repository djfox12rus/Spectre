#include "Spectre.h"

#define _USE_MATH_DEFINES
#include <limits>
#include <math.h>

#include <QTabWidget>
#include <QToolBar>
#include <QMenuBar>
#include <QMenu>
#include <QVBoxLayout>
#include <QDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QMargins>
#include <QGridLayout>
#include <QFileDialog>
#include <QFileInfo>
#include <QImage>
#include <QCheckBox>
#include <QWheelEvent>

#include "GraphView.h"
#include "SignalEditor.h"
#include "SignalLibrary.h"

#include "FFT.h"
#include "FourierFunction.h"

static const QString titleStr = QStringLiteral("Спектральный анализ сигналов");

static const QString signalTabStr = QStringLiteral("Сигнал");
static const QString spectreTabStr = QStringLiteral("Спектр");

static const QString signalMenuStr = QStringLiteral("Сигнал");
static const QString paramsMenuStr = QStringLiteral("Параметры");

static const QString signalEditorStr = QStringLiteral("Задать графически");
static const QString signalLibStr = QStringLiteral("Библиотека сигналов");
static const QString signalSave = QStringLiteral("Сохранить как...");
static const QString exitStr = QStringLiteral("Выход");

static const QString singlePulseStr = QStringLiteral("Одиночный импульс");
static const QString periodicPulseStr = QStringLiteral("Периодическая последовательность импульсов");
static const QString singleRadioPulseStr = QStringLiteral("Одиночный радиоимпульс");
static const QString periodicRadioPulseStr = QStringLiteral("Периодическая последовательность радиоимпульсов");

static const QString signalParamsStr = QStringLiteral("Параметры сигнала");
static const QString showCoordsValuesStr = QStringLiteral("Показывать координаты курсора");

static const QString durationStr = QStringLiteral("Длительность импульса:");
static const QString periodStr = QStringLiteral("Период сигнала:");
static const QString freqStr = QStringLiteral("Частота несущей:");

static const QString acceptStr = QStringLiteral("Принять");
static const QString rejectStr = QStringLiteral("Отмена");

static const QString paramsDlgTT = QStringLiteral("Диалоговое окно задания параметров исследуемого сигнала");
static const QString paramsDlgWT = QStringLiteral("Диалоговое окно задания параметров исследуемого сигнала.\n"
"Позволяет выбрать тип сигнала и его свойтсва: длительность импульса/период и частоту несущей для радиосигналов.");

static constexpr std::pair<double, double> initialGraphRange = { -5, 5 };

Spectre::Spectre(QWidget *parent) 
	: QMainWindow(parent)
{
	init();

	setWindowTitle(titleStr);
	setWindowIcon(QPixmap(":/Spectre/Resources/signal_lib.png"));
}


void Spectre::openSignalEditor()
{
	if (_editor->isHidden())
	{
		_editor->show();
	}
	else
	{
		_editor->raise();
	}
}

void Spectre::wheelEvent(QWheelEvent * ev)
{
	auto angle = ev->angleDelta();
	angle /= 120;

	scrollX(angle.x());
	scrollY(angle.y());

	QMainWindow::wheelEvent(ev);
}

void Spectre::closeEvent(QCloseEvent * ev)
{
	if (_lib &&_lib->isVisible())
	{
		_lib->close();
	}
	if (_editor && _editor->isVisible())
	{
		_editor->close();
	}
}

void Spectre::openSignalLibrary()
{
	if (_lib->isHidden())
	{
		_lib->show();
	}
	else
	{
		_lib->raise();
	}
}

void Spectre::openParamsDialog()
{
	auto backupParams = _signalParams;
	emit updateParamsDlg();
	auto result = _signalParamsDlg->exec();
	if (result == QDialog::Accepted)
	{
		processSignalParams();
	}
	else
	{
		_signalParams = backupParams;
	}
}

void Spectre::reciveLibFunction()
{
	_initialSignal = _lib->currentFunction();
	//_initialRange = { -5,5 };
	_tau = _lib->currentTau();
	_initialSpectre = _lib->currentSpectre();
	//_initialSpectre = makeFourierFunction(_initialSignal, _tau);
	processSignalParams();
}

void Spectre::reciveEditorFunction()
{
	if (_editor->isFunctionEmpty())
	{
		_currentGraph->setFunction({});
		_currentSpectre->setFunction({});
	}
	else
	{
		_initialSignal = _editor->currentFunction();
		auto xRange = _editor->xRange();
		_tau = _editor->tau();


		//_tau = _lib->currentTau();
		//_initialRange = _editor->xRange();
		//TODO: FFT
		_initialSpectre = makeFourierFunction(_initialSignal, _tau);
	}
	processSignalParams();
}

void Spectre::saveAs()
{
	auto title = QStringLiteral("Сохранить ") + (_currentTab == SignalTab ? QStringLiteral("Cигнал") : QStringLiteral("Спектр"));
	auto defaultName = _currentTab == SignalTab ? QStringLiteral("Сигнал") : QStringLiteral("Спектр");
	auto fileName = QFileDialog::getSaveFileName(this, title, "/home/" + defaultName, QStringLiteral("Изображения ") + "(*.png)");

	if (fileName.isEmpty()) return;

	QImage image = (_currentTab == SignalTab ? _currentGraph->pixmap() : _currentSpectre->pixmap()).toImage();
	image.save(fileName);
}

void Spectre::init()
{

	_editor = initEditor();
	_lib = initLibrary();
	_signalParamsDlg = initSignalParams();

	setCentralWidget(initCentralWgt());

	const auto signalActs = initSignalActions();

	setMenuBar(initMenuBar(signalActs));
	addToolBar(initSignalToolBar(signalActs));

	setMinimumSize(1080, 600);
	showMaximized();
}

QTabWidget * Spectre::initCentralWgt()
{
	auto centralWgt = new QTabWidget;
	centralWgt->addTab(initSignalViewTab(), signalTabStr);
	centralWgt->addTab(initSpectreViewTab(), spectreTabStr);
	centralWgt->setContentsMargins(0,0,0,0);
	connect(centralWgt, &QTabWidget::currentChanged, this, [&current = _currentTab](int index)
	{
		current = CurrentTab(index);
	});
	return centralWgt;
}

QWidget * Spectre::initSignalViewTab()
{
	auto tab = new QWidget;

	_currentGraph = new GraphView;
	_currentGraph->setYAxisName(QStringLiteral("s(t), B"));
	_currentGraph->setXAxisName(QStringLiteral("t, c"));

	connect(this, &Spectre::setCoordsValueVisible, _currentGraph, &GraphView::setShowCoordsValues);

	auto layout = new QGridLayout;
	
	auto yHigh = new QDoubleSpinBox;
	yHigh->setRange(0.1, std::numeric_limits<double>::max());
	yHigh->setSingleStep(0.1);
	yHigh->setValue(1);
	yHigh->setMaximumWidth(55);
	yHigh->setSuffix(" B");
	connect(yHigh, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&graph = _currentGraph](double yH) 
	{
		graph->setYHighBorder(yH);
	});
	_signalBorders.yHigh = yHigh;

	auto yLow = new QDoubleSpinBox;
	yLow->setRange(-std::numeric_limits<double>::max(), 0);
	yLow->setValue(-1);
	yLow->setSingleStep(0.1);
	yLow->setMaximumWidth(55);
	yLow->setSuffix(" B");
	connect(yLow, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&graph = _currentGraph](double yL)
	{
		graph->setYLowBorder(yL);
	});
	_signalBorders.yLow = yLow;

	auto yControlsLayout = new QVBoxLayout;
	yControlsLayout->addWidget(yHigh);
	yControlsLayout->addStretch();
	yControlsLayout->addWidget(yLow);

	auto xHigh = new QDoubleSpinBox;
	xHigh->setRange(0.1, std::numeric_limits<double>::max());
	xHigh->setValue(5);
	xHigh->setSingleStep(0.1);
	xHigh->setMaximumWidth(60);
	xHigh->setSuffix(" c");
	connect(xHigh, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&graph = _currentGraph](double xH)
	{
		graph->setXHighBorder(xH);
	});
	_signalBorders.xHigh = xHigh;

	auto xLow = new QDoubleSpinBox;
	xLow->setRange(-std::numeric_limits<double>::max(), 0);
	xLow->setValue(-5);
	xLow->setSingleStep(0.1);
	xLow->setMaximumWidth(60);
	xLow->setSuffix(" c");
	connect(xLow, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&graph = _currentGraph](double xL)
	{
		graph->setXLowBorder(xL);
	});
	_signalBorders.xLow = xLow;

	auto xControlsLayout = new QHBoxLayout;
	xControlsLayout->addWidget(xLow);
	xControlsLayout->addStretch();
	xControlsLayout->addWidget(xHigh);

	layout->addLayout(yControlsLayout, 0, 0);
	layout->addWidget(_currentGraph, 0, 1);
	layout->addLayout(xControlsLayout, 1, 1);

	tab->setLayout(layout);
	return tab;
}

QWidget * Spectre::initSpectreViewTab()
{
	auto tab = new QWidget;

	_currentSpectre = new GraphView;
	_currentSpectre->setYLowBorder(0);
	_currentSpectre->setXLowBorder(-10);
	_currentSpectre->setXHighBorder(10);
	_currentSpectre->setYAxisName(QStringLiteral("|S(w)|, B/Гц"));
	_currentSpectre->setXAxisName(QStringLiteral("w, рад/c"));

	connect(this, &Spectre::setCoordsValueVisible, _currentSpectre, &GraphView::setShowCoordsValues);

	auto layout = new QGridLayout;

	auto yHigh = new QDoubleSpinBox;
	yHigh->setRange(0.1, std::numeric_limits<double>::max());
	yHigh->setSingleStep(0.1);
	yHigh->setValue(1);
	yHigh->setMaximumWidth(70);
	yHigh->setSuffix(QStringLiteral(" В/Гц"));
	connect(yHigh, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&graph = _currentSpectre](double yH)
	{
		graph->setYHighBorder(yH);
	});
	_spectreBorders.yHigh = yHigh;


#ifdef FFT_TEST
	auto yLow = new QDoubleSpinBox;
	yLow->setRange(-std::numeric_limits<double>::max(), -0);
	yLow->setValue(0);
	yLow->setSingleStep(0.1);
	yLow->setMaximumWidth(45);
	connect(yLow, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&graph = _currentSpectre](double yL)
	{
		graph->setYLowBorder(yL);
	});
	_spectreBorders.yLow = yLow;
#endif // FFT_TEST
	auto yControlsLayout = new QVBoxLayout;
	yControlsLayout->addWidget(yHigh);
	yControlsLayout->addStretch();
#ifdef FFT_TEST
	yControlsLayout->addWidget(yLow);
#endif // FFT_TEST
	auto xHigh = new QDoubleSpinBox;
	xHigh->setRange(-10 + 0.1, std::numeric_limits<double>::max());
	xHigh->setValue(10);
	xHigh->setSingleStep(0.1);
	xHigh->setMaximumWidth(85);
	xHigh->setSuffix(QStringLiteral(" рад/с"));
	_spectreBorders.xHigh = xHigh;

	auto xLow = new QDoubleSpinBox;
	xLow->setRange(-std::numeric_limits<double>::max(), 10 - 0.1);
	xLow->setValue(-10);
	xLow->setSingleStep(0.1);
	xLow->setMaximumWidth(85);
	xLow->setSuffix(QStringLiteral(" рад/с"));

	connect(xHigh, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&graph = _currentSpectre, xLow](double xH)
	{
		graph->setXHighBorder(xH);
		xLow->setRange(-std::numeric_limits<double>::max(), xH - 0.1);
	});
	connect(xLow, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&graph = _currentSpectre, xHigh](double xL)
	{
		graph->setXLowBorder(xL);
		xHigh->setRange(xL + 0.1, std::numeric_limits<double>::max());
	});
	_spectreBorders.xLow = xLow;

	auto xControlsLayout = new QHBoxLayout;
	xControlsLayout->addWidget(xLow);
	xControlsLayout->addStretch();
	xControlsLayout->addWidget(xHigh);

	layout->addLayout(yControlsLayout, 0, 0);
	layout->addWidget(_currentSpectre, 0, 1);
	layout->addLayout(xControlsLayout, 1, 1);

	tab->setLayout(layout);
	return tab;
}

QList<QAction*> Spectre::initSignalActions()
{
	QList<QAction*> acts;

	acts.push_back(new QAction(QIcon(":/Spectre/Resources/edit.png"), signalEditorStr, this));
	acts.back()->setShortcut(Qt::CTRL + Qt::Key_N);
	connect(acts.back(), &QAction::triggered, this, &Spectre::openSignalEditor);

	acts.push_back(new QAction(QIcon(":/Spectre/Resources/black-library.png"), signalLibStr, this));
	acts.back()->setShortcut(Qt::CTRL + Qt::Key_L);
	connect(acts.back(), &QAction::triggered, this, &Spectre::openSignalLibrary);

	acts.push_back(new QAction(this));
	acts.back()->setSeparator(true);

	acts.push_back(new QAction(QIcon(":/Spectre/Resources/save-icon.png"), signalSave, this));
	acts.back()->setShortcuts(QKeySequence::SaveAs);
	connect(acts.back(), &QAction::triggered, this, &Spectre::saveAs);

	acts.push_back(new QAction(this));
	acts.back()->setSeparator(true);

	auto subMenu = new QMenu(QStringLiteral("Масштабировать по горизонтали"),this);
	connect(subMenu->menuAction(), &QAction::triggered, [this]() {scrollY(1); });
	connect(subMenu->addAction(QIcon(":/Spectre/Resources/large.png"), QStringLiteral("Увеличить")), &QAction::triggered, [this]() {scrollY(1); });
	subMenu->setDefaultAction(subMenu->actions().back());
	connect(subMenu->addAction(QIcon(":/Spectre/Resources/small.png"), QStringLiteral("Уменьшить")), &QAction::triggered, [this]() {scrollY(-1); });

	acts.push_back(subMenu->menuAction());
	acts.back()->setIcon(QIcon(":/Spectre/Resources/scale_vert.png"));

	subMenu = new QMenu(QStringLiteral("Масштабировать по вертикали"), this);
	connect(subMenu->menuAction(), &QAction::triggered, [this]() {scrollX(1); });
	connect(subMenu->addAction(QIcon(":/Spectre/Resources/large.png"), QStringLiteral("Увеличить")), &QAction::triggered, [this]() {scrollX(1); });
	subMenu->setDefaultAction(subMenu->actions().back());
	connect(subMenu->addAction(QIcon(":/Spectre/Resources/small.png"), QStringLiteral("Уменьшить")), &QAction::triggered, [this]() {scrollX(-1); });

	acts.push_back(subMenu->menuAction());
	acts.back()->setIcon(QIcon(":/Spectre/Resources/scale_hor.png"));

	acts.push_back(new QAction(this));
	acts.back()->setSeparator(true);

	acts.push_back(new QAction(QIcon(":/Spectre/Resources/exit.png"), exitStr, this));
	acts.back()->setShortcut(Qt::Key_Escape);
	connect(acts.back(), &QAction::triggered, this, &Spectre::close);

	return acts;
}

QToolBar * Spectre::initSignalToolBar(const QList<QAction*>& acts)
{
	auto toolBar = new QToolBar;
	toolBar->addActions(acts);
	return toolBar;
}

QMenuBar * Spectre::initMenuBar(const QList<QAction*>& signalActs)
{
	auto menuBar = new QMenuBar;

	auto signalMenu = menuBar->addMenu(signalMenuStr);

	signalMenu->addActions(signalActs);

	auto paramsMenu = menuBar->addMenu(paramsMenuStr);
	paramsMenu->addAction(signalParamsStr, this, &Spectre::openParamsDialog, Qt::CTRL + Qt::Key_P);
	auto act = paramsMenu->addAction(showCoordsValuesStr);
	act->setCheckable(true);
	act->setChecked(true);
	connect(act, &QAction::toggled, this, &Spectre::setCoordsValueVisible);

	return menuBar;
}

SignalEditor * Spectre::initEditor()
{
	auto edit = new SignalEditor(this);
	connect(edit, &SignalEditor::apply, this, &Spectre::reciveEditorFunction);
	connect(this, &Spectre::setCoordsValueVisible, edit, &SignalEditor::setCoordsValueVisible);
	return edit;
}

SignalLibrary * Spectre::initLibrary()
{
	auto lib = new SignalLibrary(this);
	connect(lib, &SignalLibrary::apply, this, &Spectre::reciveLibFunction);
	connect(this, &Spectre::setCoordsValueVisible, lib, &SignalLibrary::setCoordsValueVisible);
	return lib;
}

QDialog * Spectre::initSignalParams()
{

	auto signalType = new QComboBox;
	signalType->addItem(singlePulseStr);
	signalType->addItem(periodicPulseStr);
	signalType->addItem(singleRadioPulseStr);
	signalType->addItem(periodicRadioPulseStr);
	signalType->setCurrentIndex(_signalParams.type);

	auto paramLayout = new QHBoxLayout;

	auto durationLabel = new QLabel(durationStr);
	auto periodLabel = new QLabel(periodStr);
	periodLabel->setVisible(false);

	paramLayout->addWidget(durationLabel);
	paramLayout->addWidget(periodLabel);
	paramLayout->addStretch();

	auto duration = new QDoubleSpinBox;
	duration->setRange(std::numeric_limits<double>::epsilon(), std::numeric_limits<double>::max());
	duration->setValue(_signalParams.duration);
	duration->setSingleStep(0.05);
	duration->setSuffix(" c");
	connect(duration, static_cast<void(QDoubleSpinBox::*)(double)> (&QDoubleSpinBox::valueChanged), [&signalParams = _signalParams](double d) {signalParams.duration = d; });

	paramLayout->addWidget(duration);

	auto period = new QDoubleSpinBox;
	period->setRange(2, std::numeric_limits<double>::max());
	period->setValue(_signalParams.period);
	period->setSingleStep(1);
	period->setVisible(false);
	period->setSuffix(" c");
	connect(period, static_cast<void(QDoubleSpinBox::*)(double)> (&QDoubleSpinBox::valueChanged), [&signalParams = _signalParams](double period) {signalParams.period = period; });

	paramLayout->addWidget(period);

	auto freqLayout = new QHBoxLayout;
	auto freqLabel = new QLabel(freqStr);
	freqLabel->setVisible(false);
	auto freqEdit = new QSpinBox;
	freqEdit->setRange(1, std::numeric_limits<int>::max());
	freqEdit->setValue(_signalParams.carrierFreq);
	freqEdit->setSingleStep(100);
	freqEdit->setVisible(false);
	freqEdit->setSuffix(QStringLiteral(" Гц"));
	freqEdit->setMinimumWidth(period->sizeHint().width());
	connect(freqEdit, static_cast<void(QSpinBox::*)(int)> (&QSpinBox::valueChanged), [&signalParams = _signalParams](int freq) {signalParams.carrierFreq = freq; });

	freqLayout->addWidget(freqLabel);
	freqLayout->addWidget(freqEdit);


	connect(signalType, static_cast<void(QComboBox::*)(int)> (&QComboBox::currentIndexChanged), [durationLabel, periodLabel, freqLabel, duration, period, freqEdit, &signalParams = _signalParams](int index)
	{
		auto prevType = signalParams.type;
		signalParams.type = SignalType(index);

		if (signalParams.type == Pulse || signalParams.type == RadioPulse)
		{
			periodLabel->setVisible(false);
			period->setVisible(false);
			durationLabel->setVisible(true);
			duration->setVisible(true);
		}
		else
		{
			durationLabel->setVisible(false);
			duration->setVisible(false);
			periodLabel->setVisible(true);
			period->setVisible(true);
		}

		if (signalParams.type == RadioPulse || signalParams.type == PeriodicRadioPulse)
		{
			freqLabel->setVisible(true);
			freqEdit->setVisible(true);
			if (prevType == Pulse || prevType == PeriodicPulse)
			{
				signalParams.changeRange = true;
			}
		}
		else
		{
			freqLabel->setVisible(false);
			freqEdit->setVisible(false);
			if (prevType == RadioPulse || prevType == PeriodicRadioPulse)
			{
				signalParams.changeRange = true;
			}
		}
	});

	auto acceptBtn = new QPushButton(acceptStr);
	auto rejectBtn = new QPushButton(rejectStr);

	auto buttonsLayout = new QHBoxLayout;
	buttonsLayout->addStretch();
	buttonsLayout->addWidget(acceptBtn);
	buttonsLayout->addWidget(rejectBtn);

#ifdef FFT_TEST
	auto fft_check = new QCheckBox(QStringLiteral("Использовать БПФ"));
	fft_check->setChecked(_signalParams.useFFT);
	connect(fft_check, &QCheckBox::toggled, [&signalParams = _signalParams](bool checked) {signalParams.useFFT = checked; });

#endif

	auto layout = new QVBoxLayout;
	layout->addWidget(signalType);
	layout->addLayout(paramLayout);
	layout->addLayout(freqLayout);
	layout->addStretch();
#ifdef FFT_TEST
	layout->addWidget(fft_check);
#endif
	layout->addLayout(buttonsLayout);

	auto paramsDialog = new QDialog(this);

	connect(this, &Spectre::updateParamsDlg, paramsDialog, [duration, period, freqEdit, signalType, &signalParams = _signalParams]() 
	{
		duration->setValue(signalParams.duration);
		period->setValue(signalParams.period);
		freqEdit->setValue(signalParams.carrierFreq);
		signalType->setCurrentIndex(signalParams.type);
	
	});

	connect(acceptBtn, &QPushButton::clicked, paramsDialog, &QDialog::accept);
	connect(rejectBtn, &QPushButton::clicked, paramsDialog, &QDialog::reject);
	paramsDialog->setLayout(layout);
	paramsDialog->setWindowTitle(signalParamsStr);

	paramsDialog->setToolTip(paramsDlgTT);
	paramsDialog->setWhatsThis(paramsDlgWT);

	return paramsDialog;
}

void Spectre::processSignalParams()
{
	if (!_initialSignal) return;
	switch (_signalParams.type)
	{
	case Spectre::Pulse:
	case Spectre::RadioPulse: //fallthrough
	{
		auto pulsFunc = [&initFunc = _initialSignal, &duration = _signalParams.duration](double x) -> double
		{
			return initFunc(x / duration);
		};
		_currentGraph->setType(_signalParams.type == Spectre::RadioPulse ? GraphView::RadioSignal : GraphView::Common);
		_currentGraph->setFunction(pulsFunc);
		
		auto pulsSpectr = [&initFunc = _initialSpectre, &duration = _signalParams.duration](double x) -> double
		{
			return initFunc(x * duration);
		};

#ifdef FFT_TEST
		if (_signalParams.useFFT)
		{
			_currentSpectre->setFunction([initFunc = makeFourierFunction(_initialSignal, _signalParams.duration), &duration = _signalParams.duration](double x) -> double
			{
				return initFunc(x * duration);
			});
		}
		else
#endif // FFT_TEST

		_currentSpectre->setType(GraphView::Common);
		if (_signalParams.type == Spectre::RadioPulse)
		{
			int freq = _signalParams.carrierFreq;
			auto radioSpectre = [&initialSpectre = _initialSpectre, freq](double x) -> double
			{
				if (x > 0)
				{
					x -= freq;
				}
				else
				{
					x += freq;
				}
				return initialSpectre(x) / 2;
			};
			
			_currentSpectre->setFunction(radioSpectre);

			if (_signalParams.changeRange)
			{
				_signalParams.changeRange = false;
				_spectreBorders.xHigh->setValue(freq + 10);
				_spectreBorders.xLow->setValue(freq - 10);
			}
		}
		else
		{
			_currentSpectre->setFunction(pulsSpectr);

			if (_signalParams.changeRange)
			{
				_signalParams.changeRange = false;
				_spectreBorders.xLow->setValue(-10);
				_spectreBorders.xHigh->setValue(10);
			}
		}
	}
	break;
	case Spectre::PeriodicPulse:
	case Spectre::PeriodicRadioPulse:
	{
		double period = _signalParams.period;
		auto periodsFunc = [&initFunc = _initialSignal
			, tau = _tau
			, period = period]
			(double x) -> double
		{
			if (x < 0)
			{
				while (x < -period / 2)
				{
					x += period;
				}
			}
			else
			{
				while (x > period / 2)
				{
					x -= period;
				}
			}
			if (x < -tau / 2 && x > tau / 2) return 0.0;
			return initFunc(x);
		};
		_currentGraph->setType(_signalParams.type == Spectre::PeriodicRadioPulse ? GraphView::RadioSignal : GraphView::Common);
		_currentGraph->setFunction(periodsFunc);
#ifdef FFT_TEST
		if (_signalParams.useFFT)
		{
			_currentSpectre->setFunction([initFunc = makeFourierFunction(_initialSignal, _signalParams.duration), &duration = _signalParams.duration](double x) -> double
			{
				return initFunc(x * duration);
			}, 2 * M_PI / period);
		}
		else
#endif // FFT_TEST

		_currentSpectre->setType(GraphView::PeriodicSpectre);
		if (_signalParams.type == Spectre::PeriodicRadioPulse)
		{
			int freq = _signalParams.carrierFreq;
			auto periodicRadioSpectre = [&initialSpectre = _initialSpectre, freq](double x) -> double
			{
				if (x > 0)
				{
					x -= freq;
				}
				else
				{
					x += freq;
				}
				return initialSpectre(x) / 2;
			};
			_currentSpectre->setFunction(periodicRadioSpectre, 2 * M_PI / period);

			if (_signalParams.changeRange)
			{
				_signalParams.changeRange = false;
				_spectreBorders.xHigh->setValue(freq + 10);
				_spectreBorders.xLow->setValue(freq - 10);
			}
		}
		else
		{
			_currentSpectre->setFunction(_initialSpectre, 2 * M_PI / period);
			if (_signalParams.changeRange)
			{
				_signalParams.changeRange = false;
				_spectreBorders.xLow->setValue(-10);
				_spectreBorders.xHigh->setValue(10);
			}
		}
	}
		break;
	//case Spectre::RadioPulse:
	//{
	//}
	//	break;
	//case Spectre::PeriodicRadioPulse:
	//{
	//}
	//	break;
	default:
		break;
	}
}

void Spectre::scrollX(int step)
{
	if (!step) return;
	if (_currentTab == SignalTab)
	{
		auto valLow = _signalBorders.xLow->value();
		auto valHigh = _signalBorders.xHigh->value();
		
		for (int i = 0; i < std::abs(step); ++i)
		{

			valLow = updateValue(valLow, step / std::abs(step));
			valHigh = updateValue(valHigh, step / std::abs(step));
		}

		_signalBorders.xLow->setValue(valLow);
		_signalBorders.xHigh->setValue(valHigh);
	}
	else
	{
		auto valLow = _spectreBorders.xLow->value();
		auto valHigh = _spectreBorders.xHigh->value();

		auto aver = valLow + (valHigh - valLow) / 2;
		valLow -= aver;
		valHigh -= aver;

		for (int i = 0; i < std::abs(step); ++i)
		{
			valLow = updateValue(valLow, step / std::abs(step));
			valHigh = updateValue(valHigh, step / std::abs(step));
		}

		_spectreBorders.xLow->setValue(valLow + aver);
		_spectreBorders.xHigh->setValue(valHigh + aver);
	}
}

void Spectre::scrollY(int step)
{
	if (!step) return;
	if (_currentTab == SignalTab)
	{
		auto valLow = _signalBorders.yLow->value();
		auto valHigh = _signalBorders.yHigh->value();

		for (int i = 0; i < std::abs(step); ++i)
		{
			valLow = updateValue(valLow, step / std::abs(step));
			valHigh = updateValue(valHigh, step / std::abs(step));
		}

		_signalBorders.yLow->setValue(valLow);
		_signalBorders.yHigh->setValue(valHigh);
	}
	else
	{
#ifdef FFT_TEST
		auto valLow = _spectreBorders.yLow->value();
#endif
		auto valHigh = _spectreBorders.yHigh->value();

		for (int i = 0; i < std::abs(step); ++i)
		{
#ifdef FFT_TEST
			valLow = updateValue(valLow, step / std::abs(step));
#endif
			valHigh = updateValue(valHigh, step / std::abs(step));
		}

#ifdef FFT_TEST
		_spectreBorders.yLow->setValue(valLow);
#endif
		_spectreBorders.yHigh->setValue(valHigh);
	}
}

std::function<double(double)> Spectre::makeFourierFunction(std::function<double(double)>& initSignal, double tau)
{
	tau *= 100;
	std::pair<double, double> range{ -tau, tau };
	double step = 2 * tau / FFT::sampleCount;
	FFT::ComplexSignal samples;
	for (int i = 0;  i < FFT::sampleCount; ++i)
	{
		samples.push_back(initSignal(range.first + i* step));
	}
	
	FFT::fft(std::begin(samples), FFT::sampleCount);

	return FourierFunction{FFT::amplitudeSpectre(FFT::rotate(samples), tau), tau, FFT::sampleCount / 4};
}

double Spectre::updateValue(double initVal, int step)
{
	if (initVal > 100) return initVal + step*100;
	if (initVal < -100) return initVal - step * 100;

	return initVal * (step > 0 ? 2. : 0.5);
}
