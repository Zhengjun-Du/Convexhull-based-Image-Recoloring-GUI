
#include "mainwindow.h"
#include "openglwidget.h"
#include <QApplication>

#include <omp.h>

#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}
