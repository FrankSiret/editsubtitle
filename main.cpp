#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    QString arg0 = a.arguments().value(0, "");
    QString arg1 = a.arguments().value(1, "");

    a.setApplicationName("Edit Subtitle");
    a.setApplicationDisplayName("Edit Subtitle");
    a.setOrganizationName("FrankSiret");
    a.setOrganizationDomain("edsub.franksiret.project.qt.com");

    MainWindow w(nullptr, arg1);

    w.show();

    return a.exec();
}
