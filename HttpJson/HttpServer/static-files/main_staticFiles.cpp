
#include <QCoreApplication>
#include <QDir>

#include "ExampleStaticFileServing.h"

int static_main (int argc, char * argv []) {
    QCoreApplication app (argc, argv);
    new ExampleStaticFileServing (QDir::homePath (), &app);
    qDebug("QDir::homePath (): %s",QDir::homePath ().toUtf8().data());
    return app.exec ();
}

