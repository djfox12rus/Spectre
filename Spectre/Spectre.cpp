#include "Spectre.h"

#include <limits>

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

#include "GraphView.h"
#include "SignalEditor.h"
#include "SignalLibrary.h"

static const QString titleStr = QStringLiteral("Спектральный анализ сигналов");

static const QString signalTabStr = QStringLiteral("Сигнал");
static const QString spectreTabStr = QStringLiteral("Спектр");

static const QString signalMenuStr = QStringLiteral("Сигнал");
static const QString paramsMenuStr = QStringLiteral("Параметры");

static const QString signalEditorStr = QStringLiteral("Задать графически");
static const QString signalLibStr = QStringLiteral("Библиотека сигналов");
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

Spectre::Spectre(QWidget *parent) 
	: QMainWindow(parent)
{
	init();

	setWindowTitle(titleStr);
	setWindowIcon(QPixmap(":/Spectre/Resources/signal_lib.png"));
}


void Spectre::openSignalEditor()
{
	_editor->showMaximized();
}

void Spectre::openSignalLibrary()
{
	_lib->showMaximized();
}

void Spectre::openParamsDialog()
{
	auto result = _signalParams->exec();
	if (result == QDialog::Accepted)
	{

	}
}

void Spectre::init()
{

	_editor = initEditor();
	_lib = initLibrary();
	_signalParams = initSignalParams();

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
	return centralWgt;
}

QWidget * Spectre::initSignalViewTab()
{
	auto tab = new QWidget;

	auto graph = new GraphView;

	auto layout = new QVBoxLayout;
	layout->addWidget(graph);

	tab->setLayout(layout);
	return tab;
}

QWidget * Spectre::initSpectreViewTab()
{
	auto tab = new QWidget;
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
	return edit;
}

SignalLibrary * Spectre::initLibrary()
{
	auto lib = new SignalLibrary(this);
	return lib;
}

QDialog * Spectre::initSignalParams()
{

	enum SignalType
	{
		Pulse,
		PeriodicPulse,
		RadioPulse,
		PeriodicRadioPulse
	};

	auto signalType = new QComboBox;
	signalType->addItem(singlePulseStr);
	signalType->addItem(periodicPulseStr);
	signalType->addItem(singleRadioPulseStr);
	signalType->addItem(periodicRadioPulseStr);
	signalType->setCurrentIndex(Pulse);

	auto paramLayout = new QHBoxLayout;

	auto durationLabel = new QLabel(durationStr);
	auto dutyCycleLabel = new QLabel(dutyCycleStr);
	dutyCycleLabel->setVisible(false);

	paramLayout->addWidget(durationLabel);
	paramLayout->addWidget(dutyCycleLabel);
	paramLayout->addStretch();

	auto duration = new QDoubleSpinBox;
	duration->setRange(std::numeric_limits<double>::epsilon(), std::numeric_limits<double>::max());
	duration->setValue(0.1);
	duration->setSingleStep(0.1);

	paramLayout->addWidget(duration);

	auto dutyCycle = new QSpinBox;
	dutyCycle->setRange(0, std::numeric_limits<int>::max());
	dutyCycle->setValue(5);
	dutyCycle->setSingleStep(1);
	dutyCycle->setVisible(false);

	paramLayout->addWidget(dutyCycle);

	connect(signalType, static_cast<void(QComboBox::*)(int)> (&QComboBox::currentIndexChanged), [durationLabel, dutyCycleLabel, duration, dutyCycle](int index)
	{
		if (index == Pulse || index == RadioPulse)
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
