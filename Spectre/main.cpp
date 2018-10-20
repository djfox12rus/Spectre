#include "Spectre.h"
#include <QtWidgets/QApplication>
#include <QFileInfo>
#include <QDir>

int main(int argc, char *argv[])
{
	//if (argc > 0)
	//{	
	//	auto dirPath = QFileInfo(argv[0]).dir().path();
	//	QCoreApplication::addLibraryPath(dirPath);
	//}
	//QStringList paths = QCoreApplication::libraryPaths();
	////paths.append(".");
	////paths.append("imageformats");
	//paths.append("platforms");
	////paths.append("sqldrivers");
	//QCoreApplication::setLibraryPaths(paths);


	QApplication a(argc, argv);

	Spectre w;

	return a.exec();
}
