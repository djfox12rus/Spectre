#include "SignalLibrary.h"
#include "GraphView.h"

static const QString titleStr = QStringLiteral("Библиотека сигналов");

SignalLibrary::SignalLibrary(QWidget * parent)
{
	init();
	setWindowTitle(titleStr);
	setWindowIcon(QPixmap(":/Spectre/Resources/black-library.png"));
}

void SignalLibrary::init()
{
	auto view = new GraphView;

	view->setFunction([](double x) {return x/10; });

	setCentralWidget(view);


	setMinimumSize(1080, 600);
}
