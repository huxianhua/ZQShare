#include "mainwindow.h"
#include <QApplication>

#include <QCoreApplication>
#include <QDir>

#include "ExampleStaticFileServing.h"

#include "ExampleRequestParsing.h"

#define REQUEST_PARS  ture


int main(int argc, char *argv[])
{
#if 0
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
#endif

#ifdef STATIC_FILE
    QCoreApplication app (argc, argv);
    new ExampleStaticFileServing (QDir::homePath (), &app);
    qDebug("QDir::homePath (): %s",QDir::homePath ().toUtf8().data());
    return app.exec ();
#endif

#ifdef REQUEST_PARS
    QCoreApplication app (argc, argv);
    new ExampleRequestParsing (&app);
    return app.exec ();
#endif

}
