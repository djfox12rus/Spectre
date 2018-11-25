#include "SignalEditor.h"

#include <QToolBar>
#include <QMenuBar>
#include <QAction>
#include <QList>

#include "GraphEditor.h"
#include "GraphFunction.h"

static const QString titleStr = QStringLiteral("Графичексое задание сигнала");

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
	return {-5,5};
}

bool SignalEditor::isFunctionEmpty() const
{
	return _editor->currentLine().empty()|| _editor->currentLine().size() == 1;
}

void SignalEditor::init()
{
	_editor = new GraphEditor;

	setCentralWidget(_editor);

	auto actions = initActions();
	addToolBar(initToolBar(actions));
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

QMenuBar * SignalEditor::initMenuBar(const QList<QAction*>& acts)
{
	auto menuBar = new QMenuBar;
	
	auto menu = menuBar->addMenu(QStringLiteral("Сигнал"));

	menu->addActions(acts);

	return menuBar;
}

