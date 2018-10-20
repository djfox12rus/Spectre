#pragma once

#include <QtWidgets/QMainWindow>
#include <QList>

class SignalEditor;
class SignalLibrary;
class QTabWidget;
class QDialog;

class Spectre : public QMainWindow
{
	Q_OBJECT

public:
	Spectre(QWidget *parent = nullptr);

private slots:
	void		openSignalEditor();
	void		openSignalLibrary();
	void		openParamsDialog();

private:

	//BaseGraphView*		_view;
	SignalEditor*		_editor{};
	SignalLibrary*		_lib{};
	QDialog*			_signalParams{};

	void				init();
	QTabWidget*			initCentralWgt();
	QWidget*			initSignalViewTab();
	QWidget*			initSpectreViewTab();
	QList<QAction*>		initSignalActions();
	QToolBar*			initSignalToolBar(const QList<QAction*>& acts);
	QMenuBar*			initMenuBar(const QList<QAction*>& acts);

	SignalEditor*		initEditor();
	SignalLibrary*		initLibrary();
	QDialog*			initSignalParams();
};
