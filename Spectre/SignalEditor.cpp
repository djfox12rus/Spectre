#include "SignalEditor.h"

#include "GraphEditor.h"

static const QString titleStr = QStringLiteral("Графичексое задание сигнала");

SignalEditor::SignalEditor(QWidget * parent)
	:QMainWindow(parent)
{
	init();
	setWindowTitle(titleStr);
	setWindowIcon(QPixmap(":/Spectre/Resources/edit.png"));
}

void SignalEditor::init()
{
	_editor = new GraphEditor;

	setCentralWidget(_editor);
	//TODO: Add menus and stuff

	setMinimumSize(1080, 600);
}
