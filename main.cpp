#include "QtWidgetsApplication4.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	QtWidgetsApplication4 w;
	w.show();
	return a.exec();
}
