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
static const QString periodicPulseStr = QStringLiteral("Периодический импульс");
static const QString singleRadioPulseStr = QStringLiteral("Одиночный радио-импульс");
static const QString periodicRadioPulseStr = QStringLiteral("Периодический радио-импульс");

static const QString signalParamsStr = QStringLiteral("Параметры сигнала");

static const QString durationStr = QStringLiteral("Длительность импульса:");
static const QString dutyCycleStr = QStringLiteral("Скважность сигнала:");

static const QString acceptStr = QStringLiteral("Принять");
static const QString rejectStr = QStringLiteral("Отмена");

static constexpr std::pair<double, double> initialGraphRange = { -5, 5 };

static constexpr int sampleCount = 2048;

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
	auto result = _signalParamsDlg->exec();
	if (result == QDialog::Accepted)
	{
		processSignalParams();
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
	auto title = QStringLiteral("Сохранить ") + (_currentTab == SignalTab ? QStringLiteral("сигнал") : QStringLiteral("Спектр"));
	auto defaultName = _currentTab == SignalTab ? QStringLiteral("Сигнал") : QStringLiteral("Спектр");
	auto fileName = QFileDialog::getSaveFileName(this, title, "/home/" + defaultName, QStringLiteral("Изображения ") + "(*.png *.jpg)");

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

	auto layout = new QGridLayout;
	
	auto yHigh = new QDoubleSpinBox;
	yHigh->setRange(0.1, std::numeric_limits<double>::max());
	yHigh->setSingleStep(0.1);
	yHigh->setValue(1);
	yHigh->setMaximumWidth(45);
	connect(yHigh, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&graph = _currentGraph](double yH) 
	{
		graph->setYHighBorder(yH);
	});

	auto yLow = new QDoubleSpinBox;
	yLow->setRange(-std::numeric_limits<double>::max(), 0);
	yLow->setValue(-1);
	yLow->setSingleStep(0.1);
	yLow->setMaximumWidth(45);
	connect(yLow, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&graph = _currentGraph](double yL)
	{
		graph->setYLowBorder(yL);
	});

	auto yControlsLayout = new QVBoxLayout;
	yControlsLayout->addWidget(yHigh);
	yControlsLayout->addStretch();
	yControlsLayout->addWidget(yLow);

	auto xHigh = new QDoubleSpinBox;
	xHigh->setRange(0.1, std::numeric_limits<double>::max());
	xHigh->setValue(5);
	xHigh->setSingleStep(0.1);
	xHigh->setMaximumWidth(45);
	connect(xHigh, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&graph = _currentGraph](double xH)
	{
		graph->setXHighBorder(xH);
	});

	auto xLow = new QDoubleSpinBox;
	xLow->setRange(-std::numeric_limits<double>::max(), 0);
	xLow->setValue(-5);
	xLow->setSingleStep(0.1);
	xLow->setMaximumWidth(45);
	connect(xLow, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&graph = _currentGraph](double xL)
	{
		graph->setXLowBorder(xL);
	});

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

	auto layout = new QGridLayout;

	auto yHigh = new QDoubleSpinBox;
	yHigh->setRange(0.1, std::numeric_limits<double>::max());
	yHigh->setSingleStep(0.1);
	yHigh->setValue(1);
	yHigh->setMaximumWidth(45);
	connect(yHigh, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&graph = _currentSpectre](double yH)
	{
		graph->setYHighBorder(yH);
	});

	auto yLow = new QDoubleSpinBox;
	yLow->setRange(-std::numeric_limits<double>::max(), -0);
	yLow->setValue(-1);
	yLow->setSingleStep(0.1);
	yLow->setMaximumWidth(45);
	connect(yLow, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&graph = _currentSpectre](double yL)
	{
		graph->setYLowBorder(yL);
	});

	auto yControlsLayout = new QVBoxLayout;
	yControlsLayout->addWidget(yHigh);
	yControlsLayout->addStretch();
	yControlsLayout->addWidget(yLow);

	auto xHigh = new QDoubleSpinBox;
	xHigh->setRange(0.1, std::numeric_limits<double>::max());
	xHigh->setValue(5);
	xHigh->setSingleStep(0.1);
	xHigh->setMaximumWidth(45);
	connect(xHigh, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&graph = _currentSpectre](double xH)
	{
		graph->setXHighBorder(xH);
	});

	auto xLow = new QDoubleSpinBox;
	xLow->setRange(-std::numeric_limits<double>::max(), -0);
	xLow->setValue(-5);
	xLow->setSingleStep(0.1);
	xLow->setMaximumWidth(45);
	connect(xLow, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&graph = _currentSpectre](double xL)
	{
		graph->setXLowBorder(xL);
	});

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

	return menuBar;
}

SignalEditor * Spectre::initEditor()
{
	auto edit = new SignalEditor(this);
	connect(edit, &SignalEditor::apply, this, &Spectre::reciveEditorFunction);
	return edit;
}

SignalLibrary * Spectre::initLibrary()
{
	auto lib = new SignalLibrary(this);
	connect(lib, &SignalLibrary::apply, this, &Spectre::reciveLibFunction);
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
	auto dutyCycleLabel = new QLabel(dutyCycleStr);
	dutyCycleLabel->setVisible(false);

	paramLayout->addWidget(durationLabel);
	paramLayout->addWidget(dutyCycleLabel);
	paramLayout->addStretch();

	auto duration = new QDoubleSpinBox;
	duration->setRange(std::numeric_limits<double>::epsilon(), std::numeric_limits<double>::max());
	duration->setValue(_signalParams.duration);
	duration->setSingleStep(0.05);
	connect(duration, static_cast<void(QDoubleSpinBox::*)(double)> (&QDoubleSpinBox::valueChanged), [&signalParams = _signalParams](double d) {signalParams.duration = d; });

	paramLayout->addWidget(duration);

	auto dutyCycle = new QSpinBox;
	dutyCycle->setRange(2, std::numeric_limits<int>::max());
	dutyCycle->setValue(_signalParams.dutyCycle);
	dutyCycle->setSingleStep(1);
	dutyCycle->setVisible(false);
	connect(dutyCycle, static_cast<void(QSpinBox::*)(int)> (&QSpinBox::valueChanged), [&signalParams = _signalParams](int duty) {signalParams.dutyCycle = duty; });

	paramLayout->addWidget(dutyCycle);

	connect(signalType, static_cast<void(QComboBox::*)(int)> (&QComboBox::currentIndexChanged), [durationLabel, dutyCycleLabel, duration, dutyCycle, &signalParams = _signalParams](int index)
	{
		signalParams.type = SignalType(index);

		if (signalParams.type == Pulse || signalParams.type == RadioPulse)
		{
			dutyCycleLabel->setVisible(false);
			dutyCycle->setVisible(false);
			durationLabel->setVisible(true);
			duration->setVisible(true);
		}
		else
		{
			durationLabel->setVisible(false);
			duration->setVisible(false);
			dutyCycleLabel->setVisible(true);
			dutyCycle->setVisible(true);
		}
	});
	
	auto acceptBtn = new QPushButton(acceptStr);
	auto rejectBtn = new QPushButton(rejectStr);

	auto buttonsLayout = new QHBoxLayout;
	buttonsLayout->addStretch();
	buttonsLayout->addWidget(acceptBtn);
	buttonsLayout->addWidget(rejectBtn);

	auto layout = new QVBoxLayout;
	layout->addWidget(signalType);
	layout->addLayout(paramLayout);
	layout->addLayout(buttonsLayout);

	auto paramsDialog = new QDialog(this);
	connect(acceptBtn, &QPushButton::clicked, paramsDialog, &QDialog::accept);
	connect(rejectBtn, &QPushButton::clicked, paramsDialog, &QDialog::reject);
	paramsDialog->setLayout(layout);
	paramsDialog->setWindowTitle(signalParamsStr);
	paramsDialog->setFixedSize(paramsDialog->sizeHint());
	return paramsDialog;
}

void Spectre::processSignalParams()
{
	if (!_initialSignal) return;
	switch (_signalParams.type)
	{
	case Spectre::Pulse:
	{
		auto pulsFunc = [&initFunc = _initialSignal, &duration = _signalParams.duration](double x) -> double
		{
			return initFunc(x / duration);
		};
		_currentGraph->setFunction(pulsFunc);
		
		auto pulsSpectr = [&initFunc = _initialSpectre, &duration = _signalParams.duration](double x) -> double
		{
			return initFunc(x * duration);
		};

		_currentSpectre->setFunction(pulsSpectr);
	}
	break;
	case Spectre::PeriodicPulse:
	{
		double period = _tau * _signalParams.dutyCycle;
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

		_currentGraph->setFunction(periodsFunc);
		_currentSpectre->setFunction(_initialSpectre, 2 * M_PI / period);
	}
		break;
	case Spectre::RadioPulse:
		break;
	case Spectre::PeriodicRadioPulse:
		break;
	default:
		break;
	}
}

std::function<double(double)> Spectre::makeFourierFunction(std::function<double(double)>& initSignal, double tau)
{
	std::pair<double, double> range{ -tau, tau };
	double step = 2 * tau / sampleCount;
	std::vector<double> samples;
	for (int i = 0;  i < sampleCount; ++i)
	{
		samples.push_back(initSignal(range.first + i* step));
	}

	return FourierFunction{FFT::lowFreqFilter( FFT::rotate( FFT::transform(samples))), tau};
}
