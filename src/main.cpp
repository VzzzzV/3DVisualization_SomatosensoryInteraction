#include "stdafx.h"
#include "QtVtk.h"
#include <QtWidgets/QApplication>
#include <vtkoutputwindow.h>
#include<windows.h>



int main(int argc, char *argv[])
{
	//CreateThread(NULL, 0, LEAP, NULL, 0, NULL);
	vtkOutputWindow::SetGlobalWarningDisplay(0);
	QApplication a(argc, argv);
	QtVtk w;
	w.show();
	return a.exec();
}
