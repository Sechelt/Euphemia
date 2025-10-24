#include <QApplication>
#include <SDK.h>

#include "MainWindow.h"

int main( int argc, char *argv[] )
{
    QApplication a( argc, argv );

    QCoreApplication::setOrganizationName( CBD_COMPANY );
    QCoreApplication::setOrganizationDomain( CBD_DOMAIN );
    QCoreApplication::setApplicationName( SDK_NAME );
    QCoreApplication::setApplicationVersion( SDK_VER );

    MainWindow w;
    w.show();

    return a.exec();
}

