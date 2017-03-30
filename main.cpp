#include "mainwindow.h"
#include "common.h"
#include "json_common.h"
#include <QApplication>
#include <QSplashScreen>
#include <QPixmap>
#include <QTimer>
#include <QDir>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // remove mouse cursor
    qApp->setOverrideCursor(Qt::BlankCursor);

    // check if interchange folder exists
    // if not, create it
    if(!(QDir((QString) JSON_FILEPATH).exists())) {
        QDir().mkpath((QString) JSON_FILEPATH);
    }

    // set-up splash screen
    QPixmap splashPixmap("./Images/splash.png");
    QSplashScreen splash(splashPixmap);
    splash.show();

    MainWindow w;

    // gets the stylesheet file as string
    QString l_strStyle = getStyleSheetAsString("stylesheet.qss");

    // uses qApp macro to get the QApplication pointer
    // and set the stylesheet application wide.
    qApp->setStyleSheet(l_strStyle);

    QTimer::singleShot(3500,&w, SLOT(show()));
    QTimer::singleShot(3500,&splash, SLOT(close()));

    //w.show();
    //splash.finish(&w);

    return a.exec();
}
