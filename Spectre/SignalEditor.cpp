#include "SignalEditor.h"

#include <limits>

#include <QToolBar>
#include <QMenuBar>
#include <QAction>
#include <QList>
#include <QDoubleSpinBox>
#include <QStyle>
#include <QFileDialog>
#include <QDataStream>
#include <QFile>
#include <QUuid>
#include <QMessageBox>

#include "GraphEditor.h"
#include "GraphFunction.h"

static const QString titleStr = QStringLiteral("Графичексое задание сигнала");

static const QUuid magic = "{4E933A4E-D857-4BD7-AF7C-CF5F5DD7504F}";

SignalEditor::SignalEditor(QWidget * parent)
	:QMainWindow(parent)
{
	init();
	setWindowTitle(titleStr);
	setWindowIcon(QPixmap(":/Spectre/Resources/edit.png"));
}

std::function<double(double)> SignalEditor::currentFunction() const
{
	return GraphFunction(_editor->currentLine());
}

std::pair<double, double> SignalEditor::xRange() const
{
	return {-_tau, _tau};
}

double SignalEditor::tau() const
{
	return _tau;
}

bool SignalEditor::isFunctionEmpty() const
{
	return _editor->currentLine().empty()|| _editor->currentLine().size() == 1;
}

void SignalEditor::saveHandler()
{
	auto filePath = QFileDialog::getSaveFileName(this, QStringLiteral("Сохранить произвольный импульс"), QStringLiteral("/home/Сигнал"), QStringLiteral("Файлы импульсов ") + "(*.pulse)");
	if (filePath.isEmpty()) return;
	QFile file{ filePath };
	if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		QDataStream stream(&file);
		stream << magic;
		stream << _editor->currentLine();
	}
	else
	{
		QMessageBox::critical(this, QStringLiteral("Ошибка файла"), QStringLiteral("Ошибка при создании файла."));
	}
}

void SignalEditor::loadHandler()
{
	auto filePath = QFileDialog::getOpenFileName(this, QStringLiteral("Загрузить произвольный импульс"), QStringLiteral("/home/"), QStringLiteral("Файлы импульсов ") + "(*.pulse)");
	if (filePath.isEmpty()) return;

	QFile file{ filePath };
	if (file.exists() && file.open(QIODevice::ReadOnly))
	{
		QDataStream stream(&file);
		QUuid id;
		stream >> id;
		if (id != magic)
		{
			QMessageBox::critical(this, QStringLiteral("Ошибка файла"), QStringLiteral("Невозможно загрузить сигнал. Файл данных имеет неверный формат."));
			return;
		}
		QPolygonF poliLine;
		stream >> poliLine;
		_editor->setCurrentLine(poliLine);
	}
	else
	{
		QMessageBox::critical(this, QStringLiteral("Ошибка файла"), QStringLiteral("Невозможно загрузить сигнал. Файл занят другим приложением или не существует."));
	}
}

void SignalEditor::init()
{
	_editor = new GraphEditor;
	connect(this, &SignalEditor::setCoordsValueVisible, _editor, &GraphEditor::setShowCoordsValues);
	setEditorRange(_tau);

	setCentralWidget(_editor);

	auto actions = initActions();
	addToolBar(initToolBar(actions));
	addToolBar(initTauBar());
	setMenuBar(initMenuBar(actions));

	//TODO: Add menus and stuff

	setMinimumSize(1080, 600);
}

QList<QAction*> SignalEditor::initActions()
{
	QList<QAction*> acts;

	acts.push_back(new QAction(QIcon(":/Spectre/Resources/verification-mark.png"), QStringLiteral("Принять"), this));
	acts.back()->setShortcut(Qt::Key_Enter | Qt::Key_Return);
	connect(acts.back(), &QAction::triggered, this, &SignalEditor::apply);
	connect(acts.back(), &QAction::triggered, this, &SignalEditor::close);

	acts.push_back(new QAction(QIcon(":/Spectre/Resources/cross.png"), QStringLiteral("Сбросить"), this));
	acts.back()->setShortcut(Qt::Key_Enter | Qt::Key_Return);
	connect(acts.back(), &QAction::triggered, _editor, &GraphEditor::discard);
	auto discardAct = acts.back();

	acts.push_back(new QAction(this));
	acts.back()->setSeparator(true);

	acts.push_back(new QAction(QIcon(":/Spectre/Resources/save-icon.png"), QStringLiteral("Сохранить"), this));
	acts.back()->setShortcuts(QKeySequence::SaveAs);
	acts.back()->setEnabled(false);
	connect(_editor, &GraphEditor::saveAvailable, [act = acts.back()]() {act->setEnabled(true); });
	connect(acts.back(), &QAction::triggered, this, &SignalEditor::saveHandler);
	connect(discardAct, &QAction::triggered, [act = acts.back()]() {act->setEnabled(false); });

	acts.push_back(new QAction(QIcon(":/Spectre/Resources/open.png"), QStringLiteral("Загрузить"), this));
	acts.back()->setShortcuts(QKeySequence::Open);
	connect(acts.back(), &QAction::triggered, this, &SignalEditor::loadHandler);

	acts.push_back(new QAction(this));
	acts.back()->setSeparator(true);

	acts.push_back(new QAction(QIcon(":/Spectre/Resources/exit.png"), QStringLiteral("Закрыть"), this));
	acts.back()->setShortcut(Qt::Key_Escape);
	connect(acts.back(), &QAction::triggered, this, &SignalEditor::close);

	return acts;
}

QToolBar * SignalEditor::initToolBar(const QList<QAction*>& acts)
{
	auto toolBar = new QToolBar;
	toolBar->addActions(acts);

	return toolBar;
}

QToolBar * SignalEditor::initTauBar()
{
	auto tauSpinBox = new QDoubleSpinBox;
	tauSpinBox->setRange(0.1, std::numeric_limits<double>::max());
	tauSpinBox->setValue(_tau);
	tauSpinBox->setSingleStep(0.1);

	connect(tauSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double b)
	{
		_tau = b;
		setEditorRange(_tau);
	});

	auto bar = new QToolBar;
	bar->addWidget(tauSpinBox);

	return bar;
}

QMenuBar * SignalEditor::initMenuBar(const QList<QAction*>& acts)
{
	auto menuBar = new QMenuBar;
	
	auto menu = menuBar->addMenu(QStringLiteral("Сигнал"));

	menu->addActions(acts);

	return menuBar;
}

void SignalEditor::setEditorRange(double t)
{
	_editor->setXLowBorder(-_tau);
	_editor->setXHighBorder(_tau);
}

