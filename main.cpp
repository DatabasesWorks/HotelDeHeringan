#include "splash_screen.hpp"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SplashScreen main_splash( a );
    main_splash.ShowSplashScreen();
    /*

    */
    return a.exec();
}
