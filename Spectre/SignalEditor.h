#pragma once
#include <QtWidgets/QMainWindow>

class BaseGraphView;

class SignalEditor : public QMainWindow
{
public:
	SignalEditor(QWidget* parent = nullptr);


private:
	BaseGraphView * _editor{};


	void init();
};

